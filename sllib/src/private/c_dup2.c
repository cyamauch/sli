#include <unistd.h>
static int c_dup2(int oldfd, int newfd)
{
    return dup2(oldfd,newfd);
}
