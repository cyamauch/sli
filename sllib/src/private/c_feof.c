#include <stdio.h>
static int c_feof(void *stream)
{
    return feof((FILE *)stream);
}
