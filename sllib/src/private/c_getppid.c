#include <sys/types.h>
#include <unistd.h>
static pid_t c_getppid(void)
{
    return getppid();
}
