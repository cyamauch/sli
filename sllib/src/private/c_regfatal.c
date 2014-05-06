#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
static bool c_regfatal(int status)
{
    if ( status == REG_ESPACE ) return true;
    else return false;
}

