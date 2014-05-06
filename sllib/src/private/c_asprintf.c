/* -*- Mode: C ; Coding: euc-japan -*- */
/* Time-stamp: <2007-06-03 20:10:22 cyamauch> */

#include "c_vasprintf.c"

static int c_asprintf(char **strp, const char *format, ... )
{
    int ret;
    va_list ap;
    va_start(ap,format);
    ret = c_vasprintf(strp,format,ap);
    va_end(ap);
    return ret;
}

