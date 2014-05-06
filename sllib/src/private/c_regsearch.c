#include <sys/types.h>
#include <regex.h>
static int c_regsearch( const void *preg, const char *string,
			bool notbol, bool noteol,
			ssize_t *pos, size_t *len )
{
    int status;
    regmatch_t pmatch[1];
    int eflags = 0;
    if ( notbol == true ) eflags |= REG_NOTBOL;
    if ( noteol == true ) eflags |= REG_NOTEOL;
    status = regexec((regex_t *)preg, string, 1, pmatch, eflags);
    if ( status == 0 ) {	/* found */
	*pos = pmatch[0].rm_so;
	if ( 0 <= pmatch[0].rm_so ) *len = pmatch[0].rm_eo - pmatch[0].rm_so;
	else *len = 0;
    }
    else {
	*pos = -1;
	*len = 0;
    }
    return status;
}
