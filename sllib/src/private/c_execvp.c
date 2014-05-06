#include <unistd.h>
static int c_execvp(const char *file, char *const argv[])
{
    return execvp(file,argv);
}
