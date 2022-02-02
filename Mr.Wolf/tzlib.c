/* Find TZ using timezone.library */
#ifdef __amigaos4__
#include <proto/timezone.h>
#include "tzlib.h"
#include "error.h"

static int tzlib_tz(void)
{
	uint32 offset = 0;

	GetTimezoneAttrs(NULL, TZA_UTCOffset, &offset, TAG_DONE);

	return offset;
}

void tzlib_register(struct module_functions *funcs)
{
	funcs->tz = tzlib_tz;
}

#endif
