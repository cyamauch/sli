#include <stdio.h>
#include <stdarg.h>
static int c_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    int ret;
    va_start(ap, format);
    ret = vsnprintf(str,size,format,ap);
    va_end(ap);
    return ret;
}
