/* -*- Mode: C ; Coding: euc-japan -*- */
/* Time-stamp: <2008-07-07 02:57:22 cyamauch> */

#include <string.h>

static void rm_side_spaces( const char *str, const char *spaces,
			    const char **ret_str_begin, size_t *ret_str_len )
{
    size_t len_str;
    const char *p0;
    const char *p1;
    int i;

    if ( str == NULL ) {
	if ( ret_str_begin != NULL ) *ret_str_begin = NULL;
	if ( ret_str_len != NULL ) *ret_str_len = 0;
	return;
    }

    if ( spaces == NULL ) spaces="";
    len_str = strlen(str);
    for ( p0=str ;  ; p0++ ) {	/* skipping ' ' */
	for ( i=0 ; spaces[i] != '\0' ; i++ ) {
	    if ( *p0 == spaces[i] ) break;
	}
	if ( spaces[i] == '\0' ) break;
    }
    for ( p1=str+len_str ; p0 < p1 ; p1-- ) {
	if ( *p1 != '\0' ) {
	    for ( i=0 ; spaces[i] != '\0' ; i++ ) {
		if ( *p1 == spaces[i] ) break;
	    }
	    if ( spaces[i] == '\0' ) break;
	}
    }
    if ( ret_str_begin != NULL && ret_str_len != NULL ) {
	if ( *p0 == '\0' ) {
	    *ret_str_begin = p0;
	    *ret_str_len = 0;
	}
	else {
	    *ret_str_begin = p0;
	    *ret_str_len = p1 - p0 + 1;
	}
    }
    return;
}

