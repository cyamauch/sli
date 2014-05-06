#include <stdlib.h>
inline static unsigned long int c_strtoul(const char *nptr, char **endptr, int base)
{
    return strtoul(nptr, endptr, base);
}
