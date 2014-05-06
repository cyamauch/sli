#include <stdio.h>
static int c_fputs(const char *s, void *stream)
{
    return fputs(s,(FILE *)stream);
}
