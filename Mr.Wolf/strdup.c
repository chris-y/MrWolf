#include <proto/exec.h>

#include <string.h>

#include "mrwolf.h"
#include "strdup.h"

char *strdup_vec(const char *s)
{
	BOOL cmq = FALSE;
	size_t len = strlen (s) + 1;
#ifdef __amigaos4__
	if(len % 4 == 0) cmq = TRUE;
#endif
	char *result = (char*) AllocVec(len, MEMF_PRIVATE);
	if (result == (char*) 0)
		return (char*) 0;
		
	if(cmq) {
		CopyMemQuick(s, result, len);
	} else {
		CopyMem(s, result, len);
	}
	return result;
}
