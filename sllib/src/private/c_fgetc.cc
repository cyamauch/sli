#include <stdio.h>
inline static int c_fgetc(void *stream)
{
    return fgetc((FILE *)stream);
}
