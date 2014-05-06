/* -*- Mode: C ; Coding: euc-japan -*- */
/* Time-stamp: <2007-07-24 14:44:33 cyamauch> */

#include <string.h>

static void suffix2filetype( const char *path, bool *r_gzip, bool *r_bzip2 )
{
    const char *dot_ptr;
    bool is_gzip = false;
    bool is_bzip2 = false;
    if ( path != NULL ) {
	dot_ptr = strrchr(path,'.');
	if ( dot_ptr != NULL ) {
	    if ( strcmp(dot_ptr,".gz") == 0 ) is_gzip = true;
	    else if ( strcmp(dot_ptr,".GZ") == 0 ) is_gzip = true;
	    else if ( strcmp(dot_ptr,".z") == 0 ) is_gzip = true;
	    else if ( strcmp(dot_ptr,".Z") == 0 ) is_gzip = true;
	    else if ( strcmp(dot_ptr,".bz2") == 0 ) is_bzip2 = true;
	    else if ( strcmp(dot_ptr,".BZ2") == 0 ) is_bzip2 = true;
	    else if ( strcmp(dot_ptr,".bz") == 0 ) is_bzip2 = true;
	    else if ( strcmp(dot_ptr,".BZ") == 0 ) is_bzip2 = true;
	}
    }
    *r_gzip  = is_gzip;
    *r_bzip2 = is_bzip2;
}

