#include <stdio.h>
static void *c_fdopen(int fildes, const char *mode)
{
    return (void *)fdopen(fildes,mode);
}
