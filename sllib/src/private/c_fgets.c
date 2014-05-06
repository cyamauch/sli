#include <stdio.h>
static char *c_fgets(char *s, int size, void *stream)
{
    return fgets(s,size,(FILE *)stream);
}
