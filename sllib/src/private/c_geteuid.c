#include <unistd.h>
static uid_t c_geteuid(void)
{
    return geteuid();
}
