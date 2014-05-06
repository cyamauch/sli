#include <sli/stdstreamio.h>
#include <sli/tstring.h>
using namespace sli;

/**
 * @file   string_regexp.cc
 * @brief  文字列に対してPOSIX拡張正規表現を使ってみた例
 */

/*
 * Examples for regular expression to handle strings
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;				/* object for stdin, stdout */
    

    /*
     * test of simple regexp match
     */

    tstring my_str0, my_str1;          /* string object */
    ssize_t matched_pos;               /* variable for matched position */
    size_t matched_len;                /* variable for matched string length */

    my_str0.assign("X68030");          /* target for tests */
    my_str1.assign("FM-TOWNS");

    /* try match */
    matched_pos = my_str0.regmatch("^X.*",&matched_len);

    /* display result */
    if ( 0 <= matched_pos ) {
        sio.printf("str0: OK! pos=%zd len=%zu\n",matched_pos,matched_len);
    }
    else {
	sio.printf("str0: NG!\n");
    }

    /* try match */
    matched_pos = my_str1.regmatch("^X.*",&matched_len);

    /* display result */
    if ( 0 <= matched_pos ) {
        sio.printf("str1: OK! pos=%zd len=%zu\n",matched_pos,matched_len);
    }
    else {
	sio.printf("str1: NG!\n");
    }

    sio.printf("\n");


    /*
     * extract a string using regexp match
     */

    tstring my_url = "http://darts.isas.jaxa.jp/foo/";
    sio.printf("my_url = '%s'\n",my_url.cstr());

    my_url.regreplace("([a-z]+://)([^/]+)(.*)", "\\2", false);
    sio.printf("hostname = '%s'\n", my_url.cstr());

    sio.printf("\n");


    /*
     * erase elements of a CSV string
     */

    tstring my_str = "X1turboZIII  ,  Z80A, MB89321B ,";
    sio.printf("my_str = '%s'\n", my_str.cstr());

    /* replace using regexp */
    my_str.regreplace("[^,]", " ", true);
    sio.printf("result = '%s'\n", my_str.cstr());

    return 0;
}
