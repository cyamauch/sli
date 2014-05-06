#include <stdio.h>
static void *c_fopen(const char *path, const char *mode)
{
    return (void *)fopen(path,mode);
}
