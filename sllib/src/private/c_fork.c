#include <sys/types.h>
#include <unistd.h>
static pid_t c_fork(void)
{
    return fork();
}
