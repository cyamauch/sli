#include <stdio.h>
static int c_fclose(void *stream)
{
    return fclose((FILE *)stream);
}
