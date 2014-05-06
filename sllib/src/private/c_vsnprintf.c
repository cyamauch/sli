#include <stdio.h>
#include <stdarg.h>
static int c_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    return vsnprintf(str,size,format,ap);
}
