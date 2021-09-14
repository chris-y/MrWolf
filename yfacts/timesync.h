/* Sync time using timesync.library */
#ifndef TIMESYNC_H
#define TIMESYNC_H 1

#ifdef __amigaos4__
int timesync_sync(char *server, int port, BOOL savesys, BOOL savebc);
char *timesync_default_server(void);
void timesync_show_error(int error, BOOL cli);
#endif

#endif
