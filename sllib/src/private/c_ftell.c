#include <stdio.h>
static long c_ftell(void *stream)
{
    return ftell((FILE *)stream);
}
