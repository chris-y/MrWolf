/* Sync time using timesync.library */
#ifdef __amigaos4__
#include <proto/timesync.h>
#include <string.h>
#include <stdio.h>
#include "timesync.h"
#include "error.h"

static int timesync_sync(char *server, int port, BOOL savesys, BOOL savebc)
{
	int err;

	err = RemoteSync(RS_SERVER, server,
					RS_PORT, port,
					RS_SETSYSTIME, savesys,
					RS_SAVETIME, savebc,
					TAG_DONE);

	if(err == TSERR_NONE) return ERR_OK;
		else return ERR_UNKNOWN;
}

static char *timesync_default_server(void)
{
	return strdup("PREFS\0");	
}

static void timesync_show_error(int error, BOOL cli)
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

void timesync_register(struct module_functions *funcs)
{
	funcs->sync = timesync_sync;
	funcs->default_server = timesync_default_server;
	funcs->show_error = timesync_show_error;
}

#endif
