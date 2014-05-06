#include <stdio.h>
#include <stdarg.h>
static int c_sprintf(char *str, const char *format, ...)
{
    va_list ap;
    int ret;
    va_start(ap, format);
    ret = vsprintf(str,format,ap);
    va_end(ap);
    return ret;
}
