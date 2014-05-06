#include <sli/tarray_tstring.h>
using namespace sli;

/**
 * @file   string_array_basic.cc
 * @brief  文字列配列の扱い方の基本を示したコード
 */

/*
 * Basic examples of string array
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    const char *group1[] = {"sakura", "mizuho", NULL};
    tarray_tstring my_arr = group1;		/* string array object */
    size_t i;

    my_arr[2] = "fuji";
    my_arr[3] = "hayabusa";

    for ( i=0 ; i < my_arr.length() ; i++ ) {                   /* 表示 */
        sio.printf("%zu: [%s]\n", i, my_arr[i].cstr());
    }

    my_arr.dprint();

    return 0;
}
