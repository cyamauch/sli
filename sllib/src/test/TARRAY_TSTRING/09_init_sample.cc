/**
  example code
  tarray_tstringクラス:tarray_tstring &init()
		       tarray_tstring &init(const tarray_tstring &obj)
                       文字列配列の完全初期化
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    const char *tree[] = {"pine", "willow", NULL};
    tarray_tstring my_treeArr = tree;

    /* my_treeArrで初期化 */
    my_arr.init(my_treeArr); 
    /* 表示 */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    /* 完全初期化 */
    my_arr.init(); 
    /* 表示 */
    sio.printf("my_arr.length = [%zu]\n",  my_arr.length());

    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("my_arr.length[%zu] = [%zu]\n", i, my_arr.length(i));
    }

    return 0;
}

