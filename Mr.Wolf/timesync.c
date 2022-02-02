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
	if(err == TSERR_FAIL) return ERR_CONN;
	return ERR_UNKNOWN;
}

static char *timesync_default_server(void)
{
	return strdup("PREFS\0");	
}


void timesync_register(struct module_functions *funcs)
{
	funcs->sync = timesync_sync;
	funcs->default_server = timesync_default_server;
	funcs->cleanup = NULL;
}

#endif
