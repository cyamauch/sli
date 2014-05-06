#include <string.h>
static int c_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1,s2,n);
}
