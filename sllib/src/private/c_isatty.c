#include <unistd.h>
static int c_isatty(int desc)
{
    return isatty(desc);
}
