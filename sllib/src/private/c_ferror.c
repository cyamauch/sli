#include <stdio.h>
static int c_ferror(void *stream)
{
    return ferror((FILE *)stream);
}
