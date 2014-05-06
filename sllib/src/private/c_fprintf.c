#include <stdio.h>
#include <stdarg.h>
static int c_fprintf(void *stream, const char *format, ...)
{
    va_list ap;
    int ret;
    va_start(ap, format);
    ret = vfprintf((FILE *)stream,format,ap);
    va_end(ap);
    return ret;
}
