#include <stdio.h>
static void c_clearerr(void *stream)
{
    clearerr((FILE *)stream);
    return;
}
