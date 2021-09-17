/* Sync time using timesync.library */
#ifndef TIMESYNC_H
#define TIMESYNC_H 1
#include "module.h"

#ifdef __amigaos4__
void timesync_register(struct module_functions *funcs);
#endif

#endif
