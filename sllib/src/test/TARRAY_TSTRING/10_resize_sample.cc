/**
  example code
  tarray_tstringクラス:tarray_tstring &resize( size_t new_num_elements )
                       文字列配列の長さを変更
*/#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[] = {"andromeda", "yew", "Japanese pagoda tree",NULL};
    tarray_tstring my_arr = tree;
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }

    //サイズ拡張
    sio.printf("\n enhanced\n");
    my_arr.resize(4);
    sio.printf("my_arr after enhanced = %zu\n", my_arr.length());
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }

    //サイズ縮小
    sio.printf("\n reduced\n");
    my_arr.resize(2);
    sio.printf("my_arr after reduced = %zu\n", my_arr.length());
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }


    return 0;
}

