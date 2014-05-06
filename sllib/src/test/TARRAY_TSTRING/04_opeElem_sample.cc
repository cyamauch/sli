/**
  example code
  tarray_tstringクラス:tstring &operator[]( size_t index )
                       添え字の要素を参照
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[] = {"hawthorn", "oak", NULL};
    tarray_tstring my_arr = tree;
    /* 2番目の要素に"camellia"を代入 */
    my_arr[2] = "camellia";

    /* 表示 */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    //サイズは自動拡張され、エラーにならない
    my_arr[10] = "camellia";        //正常
    /* 表示 */
    sio.printf("********************************\n");
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr[i].cstr());
    }

    tarray_tstring my_arr3(my_arr);
    //サイズは自動拡張され、エラーにならない
    my_arr3[20] = "camellia";        //正常
    for ( size_t i=0 ; i < my_arr3.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr3[i].cstr());
    }

    return 0;
}

