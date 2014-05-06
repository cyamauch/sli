#include <string.h>
inline static void *c_memset(void *s, int c, size_t n)
{
    return memset(s,c,n);
}
