#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
static int c_regsearchx( const void *preg, const char *string, 
			 bool notbol, bool noteol,
			 size_t n_max,
			 size_t pos[], size_t len[], size_t *n_ret )
{
    int status;
    regmatch_t *pmatch = NULL;
    int eflags = 0;
    if ( notbol == true ) eflags |= REG_NOTBOL;
    if ( noteol == true ) eflags |= REG_NOTEOL;
    pmatch = (regmatch_t *)malloc(sizeof(*pmatch) * n_max);
    if ( pmatch == NULL ) {
	*n_ret = 0;
	return REG_ESPACE;
    }
    status = regexec((regex_t *)preg, string, n_max, pmatch, eflags);
    if ( status == 0 ) {	/* found */
	size_t i;
	for ( i=0 ; i < n_max ; i++ ) {
	    pos[i] = pmatch[i].rm_so;
	    if ( 0 <= pmatch[i].rm_so ) {
		len[i] = pmatch[i].rm_eo - pmatch[i].rm_so;
	    }
	    else {
		break;
	    }
	}
	*n_ret = i;
    }
    else {
	*n_ret = 0;
    }
    if ( pmatch != NULL ) free(pmatch);
    return status;
}
