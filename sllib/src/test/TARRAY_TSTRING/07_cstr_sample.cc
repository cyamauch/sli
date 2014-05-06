/**
  example code
  tarray_tstringクラス:const char *c_str( size_t index ) const
		       const char *cstr( size_t index ) const
                       文字列配列要素の先頭アドレス
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

    /* 表示 */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    /* 配列長を超えての参照はnullが返る */
    sio.printf("[%s]\n", my_arr.cstr(3));

    return 0;
}

