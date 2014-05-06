#include <unistd.h>
static int c_close(int fd)
{
    return close(fd);
}
