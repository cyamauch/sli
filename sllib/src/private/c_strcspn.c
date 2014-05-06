#include <string.h>
static size_t c_strcspn(const char *s, const char *reject)
{
    return strcspn(s,reject);
}
