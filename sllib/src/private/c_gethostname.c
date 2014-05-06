#include <unistd.h>
static int c_gethostname(char *name, size_t len)
{
    return gethostname(name, len);
}
