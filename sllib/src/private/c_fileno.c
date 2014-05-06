#include <stdio.h>

static int c_fileno(void *stream)
{
    return fileno((FILE *)stream);
}
