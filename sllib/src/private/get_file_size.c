#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static long long get_file_size(int filedes)
{
    struct stat st;
    if ( fstat(filedes, &st) < 0 ) return -1;
    else return st.st_size;
}
