#include <string.h>
static char *c_strpbrk(const char *s, const char *accept)
{
    return (char *)strpbrk(s,accept);
}
