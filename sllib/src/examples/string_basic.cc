#include <sli/stdstreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   string_basic.cc
 * @brief  文字列の扱い方の基本を示したコード
 */

/*
 * Basic examples for handling strings
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;				/* object for stdin, stdout */
    tstring my_str;				/* string object */
    size_t i;
    
    my_str = "Hello World";			/* substitute */

    sio.printf("[0] my_str = '%s'\n", my_str.cstr());


    my_str[13] = '!';				/* put a char one by one */
    my_str[14] = '!';

    sio.printf("[1] my_str = '%s'\n", my_str.cstr());


    for ( i=0 ; i < my_str.length() ; i++ ) {	/* display chars one by one */
	sio.printf("[%c]",my_str.cchr(i));
    }
    sio.printf("\n");


    my_str.printf("%s %s", "Hello", "World");		/* overwirte */
    my_str.appendf(" %s", "!!");			/* append   */

    sio.printf("[2] my_str = '%s'\n", my_str.cstr());
 

    return 0;
}
