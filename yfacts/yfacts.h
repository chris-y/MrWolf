#ifndef YFACTS_H
#define YFACTS_H 1

/* For some reason these are defined differently on OS3.2 and OS4,
   so we just define our own here */

#include <exec/io.h>

struct TV_compat /* TimeVal */
{
    long Seconds;
    long Microseconds;
};

struct TR_compat /* TimeRequest */
{
    struct IORequest Request;
    struct TV_compat   Time;
};

void set_sys_time(struct TV_compat *tv);
void set_bc_time(struct TV_compat *tv);
char *strdup(const char *s);

#endif
