/**
  example code
  tarray_tstringクラス:tarray_tstring &clean(const char *str = "")
		       tarray_tstring &clean(const tstring &str)
                       既存の文字列配列全体を任意の文字列でパディング
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    //new sample
    const char *tree[] = {"katsura", "torreya", NULL};
    tarray_tstring my_arr = tree;

    //"paulownia"でパディング
    my_arr.clean("paulownia");
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }

    //お掃除
    my_arr.clean();
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }

    return 0;
}

