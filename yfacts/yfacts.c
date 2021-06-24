/* yoctofacts 
gcc -Os -N -o yfacts yfacts.c -lauto
strip yfacts
strip -R.comment yfacts
*/

#define __USE_INLINE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <proto/commodities.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/timesync.h>
#include <proto/timezone.h>
#include <proto/wb.h>

#include <workbench/startup.h>

#include "yFacts_rev.h"

const char __attribute__((used)) *version = VERSTAG;

/* Global config */
int poll = 720;
int port = 123;
char *server = NULL;
BOOL savebc = FALSE;
BOOL savesys = FALSE;
const char *default_server = "PREFS\0";
int panic_warn = 30;
int firstsync_delay = 0;

/* Global vars */
struct MsgPort *msgport;
struct TimeRequest *tioreq;
CxObj *broker;
struct MsgPort *broker_mp;
int panic_mode = 0;

struct NewBroker newbroker = {
	NB_VERSION,
	"YoctoFacts",
	VERS,
	"Minimalistic clock synchronisation",
	NBU_UNIQUE | NBU_NOTIFY, /* unique*/
	0, /* flags */
	0, /*pri*/
	0, /*msgport*/
	0
};

void show_error(int error, BOOL cli)
{
	switch(error) {
		case TSERR_FAIL:
			if(cli) printf("Cannot connect to server\n");
		break;

		default:
			if(cli) printf("yfacts returned error %ld\n", error);
		break;
	}
}

int timesync(void)
{
	int err;
//	int32 offset = 0;
//	struct timeval timediff;

/*
	err = GetTimezoneAttrs(NULL,
				TZA_UTCOffsetDST, &offset,
				TAG_DONE);
*/

//DebugPrintF("yFacts: %s:%ld sys:%ld bc:%ld\n", server, port, savesys, savebc);

	err = RemoteSync(RS_SERVER, server,
					RS_PORT, port,
					RS_SETSYSTIME, savesys,
					RS_SAVETIME, savebc,
//					RS_UTCOFFSET, - (offset * 60),
//					RS_DIFFTIME, &timediff,
					TAG_DONE);

//printf("%ld, %ld\n", timediff.tv_sec, timediff.tv_usec);

	return err;
}

void killpoll()
{
	if(CheckIO((struct IORequest *)tioreq)==0) {
    	AbortIO((struct IORequest *)tioreq);
    	WaitIO((struct IORequest *)tioreq);
	}
}

void startpoll(int time)
{
    tioreq->Request.io_Command=TR_ADDREQUEST;
    tioreq->Time.Seconds = time;
    tioreq->Time.Microseconds = 0L;
    SendIO((struct IORequest *)tioreq);
}

/* Sync and start polling, returns FALSE if no new poll set up */
BOOL timesync_poll()
{
	if (firstsync_delay) {
		startpoll(firstsync_delay);
		firstsync_delay = 0;
		return TRUE;
	}

	if (timesync() == TSERR_NONE) {
		panic_mode = 0;

		if(poll > 0) {
			startpoll(poll * 60);
		} else {
			return FALSE;
		}
	} else {
		if(panic_mode == panic_warn) {
			TimedDosRequesterTags(
				TDR_ImageType, TDRIMAGE_INFO,
				TDR_TitleString, "YoctoFacts",
				TDR_FormatString, "Clock synchronisation has failed above set threshold",
				TDR_GadgetString, "OK",
				TDR_Inactive, TRUE,
				TAG_DONE);
		}
		panic_mode++;
		startpoll(60);
	}
	return TRUE;
}

void main_loop()
{
	struct Message *msg;
	ULONG sigrcvd, msgid, msgtype;
	ULONG pollsig = 1L << msgport->mp_SigBit;
	ULONG cxsigflag = 1L << broker_mp->mp_SigBit;
	BOOL running = TRUE;

	while(running) {
		sigrcvd = Wait(pollsig | cxsigflag | SIGBREAKF_CTRL_C);

		if(sigrcvd & pollsig) {
			while(msg = GetMsg(msgport)) {
				// ReplyMsg(msg);
				running = timesync_poll();
			}
		}

		if(sigrcvd & cxsigflag) {
			while(msg = GetMsg(broker_mp)) {
				msgid = CxMsgID((CxMsg *)msg);
				msgtype = CxMsgType((CxMsg *)msg);
				ReplyMsg(msg);

				switch(msgtype) {
					case CXM_COMMAND:
						switch(msgid) {
							case CXCMD_DISABLE:
								killpoll();
								ActivateCxObj(broker,0L);
							break;
							case CXCMD_ENABLE:
								running = timesync_poll();
								ActivateCxObj(broker,1L);
							break;
							case CXCMD_KILL:
							case CXCMD_UNIQUE:
								running = FALSE;
							break;
						}
					break;
					default:
					break;
				}
			}
		}

		if(sigrcvd & SIGBREAKF_CTRL_C)
		{
			running = FALSE;
		}
	}
}

void gettooltypes(struct WBArg *wbarg)
{
	struct DiskObject *dobj;
	STRPTR *toolarray;
	char *s;

	if((*wbarg->wa_Name) && (dobj=GetDiskObject(wbarg->wa_Name))) {
		toolarray = (STRPTR *)dobj->do_ToolTypes;
		if(s = (char *)FindToolType(toolarray,"SERVER")) server = strdup(s);
		if(s = (char *)FindToolType(toolarray,"POLL")) poll = atoi(s);
		if(s = (char *)FindToolType(toolarray,"PORT")) port = atoi(s);
		if(s = (char *)FindToolType(toolarray,"DELAY")) firstsync_delay = atoi(s);
		if(s = (char *)FindToolType(toolarray,"WARN")) panic_warn = atoi(s);
		if(s = (char *)FindToolType(toolarray,"SAVE")) {
			if(MatchToolValue(s,"BATTCLOCK")) {
				savebc=TRUE;
			}
			if(MatchToolValue(s,"SYSTEM")) {
				savesys=TRUE;
			}
		}
		FreeDiskObject(dobj);
	}
}


BOOL startcx(void)
{
	if(broker_mp = CreateMsgPort()) {
		newbroker.nb_Port = broker_mp;
		if(broker = CxBroker(&newbroker,NULL)) {
			ActivateCxObj(broker,1L);
		} else {
			return FALSE;
		}
	}
	return TRUE;
}

void wbcleanup(void)
{
	struct Message *msg;

	if(broker) DeleteCxObj(broker);
	if(broker_mp)
	{
		while(msg = GetMsg(broker_mp))
			ReplyMsg(msg);
		DeletePort(broker_mp);
	}

	if(tioreq)
	{
		while(msg = GetMsg(msgport)) {
        	// ReplyMsg(msg);
		}
		if(CheckIO((struct IORequest *)tioreq)==0)
			{
		    AbortIO((struct IORequest *)tioreq);
		    WaitIO((struct IORequest *)tioreq);
			}

		CloseDevice((struct IORequest *) tioreq);
		DeleteIORequest((struct IORequest *)tioreq);
		DeleteMsgPort(msgport);
	}
}

int main(int argc, char **argv)
{
	struct WBStartup *WBenchMsg;
	struct WBArg *wbarg;
	LONG i;
	LONG olddir =-1;
	BOOL quiet = TRUE;
	int err;
	int rc = 0;
	LONG rarray[] = {0, 0, 0, 0};
	struct RDArgs *args;
	STRPTR template = "SERVER,PORT/N,SAVE/S,QUIET/S";

	enum
	{
		A_SERVER,
		A_PORT,
		A_SAVE,
		A_QUIET
	};

	if(argc != 0) {
		// cli startup
		quiet = FALSE;
		savesys = TRUE;

		args = ReadArgs(template,rarray,NULL);

		if(args) {
			if(rarray[A_SERVER]) {
				server = strdup((char *)rarray[A_SERVER]);
			} else {
				server = strdup(default_server);
			}

			if(rarray[A_PORT])
				port = *(ULONG *)rarray[A_PORT];

			if(rarray[A_SAVE])
				savebc = TRUE;

			if(rarray[A_QUIET])
				quiet = TRUE;

			FreeArgs(args);
		} else {
			rc = 10;
		}

		if(quiet == FALSE) {
			printf("YoctoFacts minimalistic clock synchronisation\n");
			printf("http://www.unsatisfactorysoftware.co.uk/facts\n");
		}

		if(rc == 10) {
			printf("Required argument missing\n");
		} else {
			if((err = timesync()) == 0) {
				if(quiet == FALSE) printf("Clock synchronised\n");
			} else {
				if(quiet == FALSE) show_error(err, TRUE);
				rc = 5;
			}
		}
	} else {
		WBenchMsg = (struct WBStartup *)argv;
		for(i=0,wbarg=WBenchMsg->sm_ArgList;i<WBenchMsg->sm_NumArgs;i++,wbarg++) {
			olddir =-1;
			if((wbarg->wa_Lock)&&(*wbarg->wa_Name))
				olddir = SetCurrentDir(wbarg->wa_Lock);

			gettooltypes(wbarg);

			if(olddir !=-1) SetCurrentDir(olddir);
		}

		if(startcx()) {
			msgport = CreateMsgPort();
			tioreq= (struct TimeRequest *)CreateIORequest(msgport,sizeof(struct MsgPort));
			OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest *)tioreq,0);

			if(server == NULL) server = strdup(default_server);

			if(timesync_poll()) {
				main_loop();
			}

			wbcleanup();
		}
	}

	if(server) free(server);

	return rc;
}
