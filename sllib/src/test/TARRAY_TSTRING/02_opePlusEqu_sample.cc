/**
  example code
  tarray_tstringクラス:tarray_tstring &operator+=(const tarray_tstring &obj)
		       const char *const *operator+=(const char *const *elements)
                       文字列配列の追加
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[] = {"ginkgo", "Japanese apricot", "maple", NULL};
    tarray_tstring my_arr = tree;

    const char *addTree[] = {"oak", "cherry tree", NULL};
    my_arr += addTree;

    /* 表示 */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    return 0;
}

