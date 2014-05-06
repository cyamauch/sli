#include <sli/stdstreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   string_bracket.cc
 * @brief  文字列の括弧に囲まれた位置と長さを調べる例
 */

/*
 * Examples for searching bracket pair in strings
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;				/* object for stdin, stdout */
    /*             0123456789012345678901234567890 */
    tstring str = "hypot( creal(v), cimag(v) )";
    size_t span;

    sio.printf("target = '%s'\n",str.cstr());

    ssize_t pos = str.find_quoted("()", '\\', &span);
    sio.printf("pos = %zd  span = %zu\n", pos, span);

    return 0;
}
