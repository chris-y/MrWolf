/* debug module */
#include "debug.h"
#include "error.h"
#include "mrwolf.h"

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

static void debug_cleanup(void)
{
	printf("cleanup\n");
}

void debug_register(struct module_functions *funcs)
{
	funcs->sync = debug_sync;
	funcs->default_server = debug_default_server;
	funcs->cleanup = debug_cleanup;
}


