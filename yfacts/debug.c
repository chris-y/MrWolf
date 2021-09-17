/* debug module */
#include "debug.h"
#include "error.h"

#include <string.h>
#include <stdio.h>


static int debug_sync(char *server, int port, BOOL savesys, BOOL savebc)
{
	printf("sync %s:%d\n", server, port);
	return ERR_OK;
}

static char *debug_default_server(void)
{
	printf("default server\n");
	return strdup("DEBUG\0");	
}

static void debug_show_error(int error, BOOL cli)
{
	printf("show error %d\n", error);
}

void debug_register(struct module_functions *funcs)
{
	funcs->sync = debug_sync;
	funcs->default_server = debug_default_server;
	funcs->show_error = debug_show_error;
}

