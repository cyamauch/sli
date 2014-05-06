/**
  example code
  tarray_tstringクラス:tarray_tstring &operator=(const tarray_tstring &obj)
		       const char *const *operator=(const char *const *elements)
                       文字列を代入
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *menu[] = {"rice ball", "sushi", "tofu", NULL};
    tarray_tstring my_arr;
    my_arr = menu;          /* '='は tarray_tstringクラスの演算子 */

    /* 表示 */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    return 0;
}

