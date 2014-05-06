#include <stdio.h>
static size_t c_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fread(ptr,size,nmemb,(FILE *)stream);
}
