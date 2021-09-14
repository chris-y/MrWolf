/* Sync time using timesync.library */
#ifdef __amigaos4__
#include <proto/timesync.h>
#include <string.h>
#include <stdio.h>
#include "error.h"

int timesync_sync(char *server, int port, BOOL savesys, BOOL savebc)
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

const char *timesync_default_server(void)
{
	return strdup("PREFS\0");	
}

void timesync_show_error(int error, BOOL cli)
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

#endif
