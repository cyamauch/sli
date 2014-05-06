/**
  example code
  tarray_tstringクラス:const char *const *cstrarray() const
                       文字列のポインタ配列のアドレス
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    /* オブジェクトの初期化 */
    const char *tmp[] = {"linux", "windows", "mac", NULL};
    tarray_tstring my_arr;
    my_arr = tmp;

    /* 文字列のポインタ配列のアドレスを取得する */
    const char *const *ptr = my_arr.cstrarray();
    if ( ptr != NULL ) {
        int i;
        for ( i=0 ; ptr[i] != NULL ; i++ ) {
            sio.printf("%d ... [%s]\n", i, ptr[i]);
        }
    }
    return 0;
}

