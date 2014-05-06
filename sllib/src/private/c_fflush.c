#include <stdio.h>
static int c_fflush(void *stream)
{
    return fflush((FILE *)stream);
}
