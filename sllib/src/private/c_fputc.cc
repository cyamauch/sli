#include <stdio.h>
inline static int c_fputc(int c, void *stream)
{
    return fputc(c,(FILE *)stream);
}
