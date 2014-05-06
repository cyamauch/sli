#include <unistd.h>
static int c_dup(int oldfd)
{
    return dup(oldfd);
}
