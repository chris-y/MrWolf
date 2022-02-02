/* Mr.Wolf
 * (c) 2013, 2022 Chris Young / Unsatisfactory Software
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <proto/battclock.h>
#include <proto/commodities.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/timer.h>
#include <proto/wb.h>

#ifndef __amigaos4__
#include <clib/alib_protos.h>
#endif

#include <workbench/startup.h>

#include "module.h"
#include "error.h"
#include "mrwolf.h"
#include "Mr.Wolf_rev.h"

#ifndef __amigaos4__
#define TSERR_NONE 0
#define TSERR_FAIL 1
#define SetCurrentDir CurrentDir
#endif

enum {
	TSM_DEBUG = 0,
	TSM_LIB,
	TSM_SNTP
};

enum {
	TZM_NONE = 0,
	TZM_LIB = 1
};


#ifdef __VBCC__
#define UNUSED

char *strdup(const char *s)
{
  size_t len = strlen (s) + 1;
  char *result = (char*) malloc (len);
  if (result == (char*) 0)
    return (char*) 0;
  return (char*) memcpy (result, s, len);
}

#else
#define UNUSED __attribute__((unused))
#endif

const char UNUSED *version = VERSTAG;

/* Global config */
static struct module_functions funcs;
static int mode = TSM_SNTP;
static int poll = 720;
static int port = 123;
static char *server = NULL;
static BOOL savebc = FALSE;
static BOOL savesys = FALSE;
static int panic_warn = 30;
static int firstsync_delay = 0;

static int tzmode = TZM_LIB;

/* Global vars */
static struct MsgPort *msgport;
static struct TR_compat *tioreq;
static CxObj *broker;
static struct MsgPort *broker_mp;
static int panic_mode = 0;

#ifndef __amigaos4__
struct Device *TimerBase = NULL;
#endif

static struct NewBroker newbroker = {
	NB_VERSION,
	"Mr. Wolf",
	VERS,
	"Minimalistic clock synchronisation",
	NBU_UNIQUE | NBU_NOTIFY, /* unique*/
	0, /* flags */
	0, /*pri*/
	0, /*msgport*/
	0
};

static inline void show_error(int error, BOOL cli)
{
	if(error == ERR_OK) return;
	
	switch(error) {
		case ERR_CONN:
			if(cli) printf("Cannot connect to server\n");
		break;
		case ERR_LIB:
			if(cli) printf("Cannot open required library\n");
		break;
		case ERR_KOD:
			if(cli) printf("Kiss of Death\nPlease use a different server or try again if using the NTP pool\n");
		break;
		default:
			if(cli) printf("Mr.Wolf returned error %ld\n", error);
		break;
	}
}

void set_sys_time(struct TV_compat *tv)
{
	tioreq->Request.io_Command = TR_SETSYSTIME;
	tioreq->Time.Seconds = tv->Seconds;
	tioreq->Time.Microseconds = tv->Microseconds;

	DoIO(tioreq);
}

void set_bc_time(struct TV_compat *tv)
{
#ifdef __amigaos4__
	struct BattClockIFace *IBattClock = NULL;
#endif

	struct Library *BattClockBase = OpenResource("battclock.resource");

	if(BattClockBase) {
#ifdef __amigaos4__
		IBattClock = (struct BattClockIFace *)GetInterface((struct Library *)BattClockBase, "main", 1, NULL);
		if(IBattClock) {
#endif

			WriteBattClock(tv->Seconds);
#ifdef __amigaos4__
			DropInterface(IBattClock);
		}
#endif
	}
}

static inline int timesync(void)
{
	return funcs.sync(server, port, savesys, savebc);
}

static inline char *default_server(void)
{
	return funcs.default_server();
}

/* If tv is not NULL will adjust to UTC
 * Returns offset in seconds West of GMT
 */
int tz_offset(struct TV_compat *tv)
{
	struct TV_compat tz;
	int offset = funcs.tz() * 60;

	if((tv == NULL) || (offset == 0)) return offset;

	if(offset < 0) {
		tz.Seconds = abs(offset);
		tz.Microseconds = 0;

		SubTime(tv, &tz);
	} else {
		tz.Seconds = offset;
		tz.Microseconds = 0;

		AddTime(tv, &tz);
	}
	return offset;
}

static inline void cleanup(void)
{
	struct Message *msg;

	if(server) free(server);
	if(funcs.cleanup) funcs.cleanup();

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

#ifdef __amigaos4__
		if(ITimer) DropInterface((struct Interface *)ITimer);
#endif
		CloseDevice((struct IORequest *) tioreq);
		DeleteIORequest((struct IORequest *)tioreq);
		DeleteMsgPort(msgport);
	}

	return;
}

static int tz_dummy(void)
{
	return 0;
}

static void register_funcs(void)
{
	switch(mode) {
#ifdef __amigaos4__
		case TSM_LIB:
			timesync_register(&funcs);
		break;
#endif
		case TSM_SNTP:
			sntp_register(&funcs);
		break;
		default:
			debug_register(&funcs);
		break;
	}

	switch(tzmode) {
		case TZM_LIB:
			tzlib_register(&funcs);
		break;
		default:
			funcs.tz = tz_dummy;
		break;
	}
	
	if(server == NULL) server = default_server();
}

static void killpoll()
{
	if(CheckIO((struct IORequest *)tioreq)==0) {
		AbortIO((struct IORequest *)tioreq);
		WaitIO((struct IORequest *)tioreq);
	}
}

static void startpoll(int time)
{
    if(time < (15 * 60)) time = (15 * 60);

    tioreq->Request.io_Command=TR_ADDREQUEST;
    tioreq->Time.Seconds = time;
    tioreq->Time.Microseconds = 0L;
    SendIO((struct IORequest *)tioreq);
}

/* Sync and start polling, returns FALSE if no new poll set up */
static BOOL timesync_poll()
{
	if (firstsync_delay) {
		startpoll(firstsync_delay);
		firstsync_delay = 0;
		return TRUE;
	}

	if (timesync() == ERR_OK) {
		panic_mode = 0;

		if(poll > 0) {
			startpoll(poll * 60);
		} else {
			return FALSE;
		}
	} else {
		if(panic_mode == panic_warn) {
#ifdef __amigaos4__
			TimedDosRequesterTags(
				TDR_ImageType, TDRIMAGE_INFO,
				TDR_TitleString, "Mr. Wolf",
				TDR_FormatString, "Clock synchronisation has failed above set threshold",
				TDR_GadgetString, "OK",
				TDR_Inactive, TRUE,
				TAG_DONE);
#else
#warning TODO
#endif
		}
		panic_mode++;
		startpoll(60);
	}
	return TRUE;
}

static void main_loop()
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

static void gettooltypes(struct WBArg *wbarg)
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
		if(s = (char *)FindToolType(toolarray,"MODE")) {
			if(MatchToolValue(s,"LIBRARY")) {
				mode=TSM_LIB;
			}
			else if(MatchToolValue(s,"SNTP")) {
				mode=TSM_SNTP;
			}
			else if(MatchToolValue(s,"DEBUG")) {
				mode=TSM_DEBUG;
			}
		}
		if(s = (char *)FindToolType(toolarray,"TZMODE")) {
			if(MatchToolValue(s,"NONE")) {
				mode=TZM_NONE;
			} else if(MatchToolValue(s,"LIBRARY")) {
				mode=TZM_LIB;
			}
		}
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


static BOOL startcx(void)
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

static void wbcleanup(void)
{
	struct Message *msg;

	if(broker) DeleteCxObj(broker);
	if(broker_mp)
	{
		while(msg = GetMsg(broker_mp))
			ReplyMsg(msg);
		DeletePort(broker_mp);
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
	LONG rarray[] = {0, 0, 0, 0, 0};
	struct RDArgs *args;
	STRPTR template = "SERVER,PORT/N,USE/S,SAVE/S,QUIET/S";

	enum
	{
		A_SERVER,
		A_PORT,
		A_USE,
		A_SAVE,
		A_QUIET
	};

	msgport = CreateMsgPort();
	tioreq = (struct TimeRequest *)CreateIORequest(msgport,sizeof(struct MsgPort));
	OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest *)tioreq,0);

	TimerBase = (struct Device *)tioreq->Request.io_Device;
#ifdef __amigaos4__
	ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
#endif

	if(argc != 0) {
		// cli startup
		quiet = FALSE;

		args = ReadArgs(template,rarray,NULL);

		if(args) {
			if(rarray[A_SERVER]) {
				server = strdup((char *)rarray[A_SERVER]);
			}

			if(rarray[A_PORT])
				port = *(ULONG *)rarray[A_PORT];

			if(rarray[A_USE])
				savesys = TRUE;

			if(rarray[A_SAVE])
				savebc = TRUE;

			if(rarray[A_QUIET])
				quiet = TRUE;

			FreeArgs(args);
		} else {
			rc = 10;
		}

		if(quiet == FALSE) {
			printf("Mr. Wolf clock synchronisation\n");
			printf("https://www.unsatisfactorysoftware.co.uk/\n");
		}

		if(rc == 10) {
			printf("Required argument missing\n");
		} else {
			register_funcs();
			
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
			register_funcs();

			if(timesync_poll()) {
				main_loop();
			}

			wbcleanup();
		}
	}

	cleanup();

	return rc;
}
