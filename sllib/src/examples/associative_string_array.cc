#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
using namespace sli;

/**
 * @file   associative_string_array.cc
 * @brief  文字列の連想配列の扱い方を示したコード
 */

/*
 * Example of associative string array
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    asarray_tstring my_arr;				/* array object */
    size_t i;

    my_arr["JR-EAST"] = "SUICA";
    my_arr["JR-CENTRAL"] = "TOICA";
    my_arr["JR-WEST"] = "ICOCA";

    /* display result */
    for ( i=0 ; i < my_arr.length() ; i++ ) {
        const char *key = my_arr.key(i);
        sio.printf("%s: [%s]\n", key, my_arr[key].cstr());
    }

    my_arr.dprint();

    /* perform insert */
    my_arr.insert("JR-EAST", "JR-HOKKAIDO","KITACA");
    my_arr.dprint();

    /* perform "tolower" to all elements */
    my_arr.tolower();
    my_arr.dprint();

    return 0;
}
