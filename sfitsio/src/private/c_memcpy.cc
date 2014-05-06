#include <string.h>
inline static void *c_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest,src,n);
}
