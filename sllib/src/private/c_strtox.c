#include <stdlib.h>
static double c_strtod(const char *nptr, char **endptr)
{
    return strtod(nptr,endptr);
}
static long c_strtol(const char *nptr, char **endptr, int base)
{
    return strtol(nptr,endptr,base);
}
static long long c_strtoll(const char *nptr, char **endptr, int base)
{
    return strtoll(nptr,endptr,base);
}
static unsigned long c_strtoul(const char *nptr, char **endptr, int base)
{
    return strtoul(nptr,endptr,base);
}
static unsigned long long c_strtoull(const char *nptr, char **endptr, int base)
{
    return strtoull(nptr,endptr,base);
}
