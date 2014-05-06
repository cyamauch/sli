#include <stdio.h>
#include <stdarg.h>
static int c_vsscanf(const char *str, const char *format, va_list ap)
{
    return vsscanf(str,format,ap);
}
