#include <unistd.h>
static int c_pipe(int filedes[])
{
    return pipe(filedes);
}
