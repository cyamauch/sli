#include <stdio.h>
static int c_rewind(void *stream)
{
    return fseek((FILE *)stream,0L,SEEK_SET);
}
