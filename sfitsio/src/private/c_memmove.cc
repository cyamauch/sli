#include <string.h>
inline static void *c_memmove(void *dest, const void *src, size_t n)
{
    return memmove(dest,src,n);
}
