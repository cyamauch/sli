#include <fnmatch.h>
static int c_strmatch(const char *pat, const char *str)
{
    return fnmatch(pat,str,0);
}
static int c_fnmatch(const char *pat, const char *str)
{
    return fnmatch(pat,str,FNM_PERIOD);
}
static int c_pnmatch(const char *pat, const char *str)
{
    return fnmatch(pat,str,FNM_PATHNAME | FNM_PERIOD);
}
