#include <stdlib.h>
inline static long int c_strtol(const char *nptr, char **endptr, int base)
{
    return strtol(nptr, endptr, base);
}
