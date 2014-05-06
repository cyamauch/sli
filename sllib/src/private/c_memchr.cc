#include <string.h>
inline static void *c_memchr(const void *s, int c, size_t n)
{
    return (void *)memchr(s,c,n);
}
