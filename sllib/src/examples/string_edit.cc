#include <sli/stdstreamio.h>
#include <sli/tstring.h>
#include <unistd.h>
using namespace sli;

/**
 * @file   string_edit.cc
 * @brief  文字列の編集の例を示したコード
 */

/*
 * Examples for editing strings
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;				/* object for stdin, stdout */
    tstring my_str;				/* string object */

    /*
     * Hello World
     */
    
    my_str = "  Hello World ";
    sio.printf("before:          '%s'\n", my_str.cstr());

    my_str.trim();
    sio.printf("after trim():    '%s'\n", my_str.cstr());

    if ( my_str == "hello world" ) sio.printf("compare...OK\n");
    else sio.printf("compare...NG\n");

    my_str.tolower();
    sio.printf("after tolower(): '%s'\n", my_str.cstr());

    if ( my_str == "hello world" ) sio.printf("compare...OK\n");
    else sio.printf("compare...NG\n");

    my_str.insertf(0, "pid=%ld ", (long)getpid());
    sio.printf("after insertf(): '%s'\n", my_str.cstr());

    sio.printf("\n");


    /*
     * a CSV string
     */

    my_str = "X1turboZIII  ,  Z80A, MB89321B ,";

    sio.printf("my_str = '%s'\n", my_str.cstr());

    /* strreplace() performs simple replacement */
    my_str.strreplace(",", ";", true);
    sio.printf("result = '%s'\n", my_str.cstr());


    return 0;
}
