#include <stdio.h>
static int c_remove(const char *pathname)
{
    return remove(pathname);
}
