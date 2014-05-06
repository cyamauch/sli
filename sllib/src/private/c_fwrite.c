#include <stdio.h>
static size_t c_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr,size,nmemb,(FILE *)stream);
}
