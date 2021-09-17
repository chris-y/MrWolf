#ifndef MODULE_H
#define MODULE_H 1
#include <exec/types.h>
struct module_functions {
	int (*sync)(char *server, int port, BOOL savesys, BOOL savebc);
	char *(*default_server)(void);
};

/* register function headers */
#include "debug.h"
#include "timesync.h"


#endif
