/* -*- Mode: C ; Coding: euc-japan -*- */
/* Time-stamp: <2008-07-07 03:24:07 cyamauch> */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static int c_vasprintf(char **strp, const char *fmt, va_list ap)
{
    int return_val = -1;
    va_list aq;
    int nn;
    char buf128[128];
    char *tmp_ptr;

    va_copy(aq, ap);

    nn = vsnprintf(buf128,128,fmt,ap);
    if ( nn < 0 ) {
	fprintf(stderr,"c_vasprintf(): [FATAL ERROR] vsnprintf() failed\n");
	goto quit;
    }
    tmp_ptr = (char *)malloc(nn+1);
    if ( tmp_ptr == NULL ) {
	fprintf(stderr,"c_vasprintf(): [FATAL ERROR] malloc() failed\n");
	goto quit;
    }
    if ( nn+1 <= 128 ) memcpy(tmp_ptr,buf128,nn+1);
    else {
	nn = vsnprintf(tmp_ptr,nn+1,fmt,aq);
	if ( nn < 0 ) {
	    fprintf(stderr,
		    "c_vasprintf(): [FATAL ERROR] vsnprintf() failed\n");
	    free(tmp_ptr);
	    goto quit;
	}
    }
    
    *strp = tmp_ptr;
    return_val = nn;

 quit:
    va_end(aq);
    return return_val;
}

