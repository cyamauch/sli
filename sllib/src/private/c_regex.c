#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
static int c_regcomp(void **preg_ptr, const char *regex)
{
    int status;
    regex_t *p = (regex_t *)malloc(sizeof(regex_t));
    if ( p == NULL ) {
	*preg_ptr = NULL;
	return REG_ESPACE;
    }
    status = regcomp(p, regex, REG_EXTENDED);
    *preg_ptr = (void *)p;
    return status;
}
static size_t c_regerror(int errcode, const void *preg, char *msg, size_t msz )
{
    return regerror(errcode,(const regex_t *)preg, msg, msz);
}
static void c_regfree(void *preg)
{
    if ( preg != NULL ) {
	regfree((regex_t *)preg);
	free(preg);
    }
}
