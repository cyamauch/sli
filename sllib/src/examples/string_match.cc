#include <sli/stdstreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   string_match.cc
 * @brief  文字列のシェル的なマッチングの例を示したコード
 */

/*
 * Examples of match strings
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;				/* object for stdin, stdout */
    tstring my_str0, my_str1;			/* string object */

    my_str0.assign("X68030");			/* target strings */
    my_str1.assign("FM-TOWNS");

    sio.printf("str0 = '%s'\n",my_str0.cstr());
    sio.printf("str1 = '%s'\n",my_str1.cstr());


    /*
     * test matching
     */
    if ( my_str0.strmatch("X*") == 0 ) sio.printf("str0: OK!\n");
    else sio.printf("str0: NG!\n");
    if ( my_str1.strmatch("X*") == 0 ) sio.printf("str1: OK!\n");
    else sio.printf("str1: NG!\n");


    return 0;
}
