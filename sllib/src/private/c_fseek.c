#include <stdio.h>
static int c_fseek(void *stream, long offset, int whence)
{
    return fseek((FILE *)stream,offset,whence);
}
