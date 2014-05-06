/**
  example code
  tarray_tstringクラス:size_t length() const
		       size_t length( size_t index ) const
                       文字列配列の長さ(個数)，又は文字列の長さ
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    my_arr.at(0).printf("Hello");
    my_arr.at(1).printf("Hoge");

    //my_arrの長さを表示
    sio.printf("my_arr length = %zu\n",my_arr.length());
    //my_arrの各要素の文字列長を表示
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr index%zu length = %zu\n",i, my_arr.length(i));
        //sio.printf("index %d ... [%d]\n",i, my_arr.length(i));
    }

    return 0;
}

