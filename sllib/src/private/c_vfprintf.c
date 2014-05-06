#include <stdio.h>
#include <stdarg.h>
static int c_vfprintf(void  *stream, const char *format, va_list ap)
{
    return vfprintf((FILE *)stream,format,ap);
}
