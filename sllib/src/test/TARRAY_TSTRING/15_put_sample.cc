/**
  example code
  tarray_tstringクラス:tarray_tstring &put( size_t index, const char *str, size_t n )
		       tarray_tstring &put( size_t index, const tstring &str, size_t n )
		       tarray_tstring &putf( size_t index, size_t n, const char *format, ... )
		       tarray_tstring &vputf( size_t index, size_t n, const char *format, va_list ap )
                       任意の要素位置へ文字列をセット
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr; 

    //#1 &put( size_t index, const char *str, size_t n  )
    my_arr.put(1, "elm", 2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //他のメンバ関数のサンプル
    tarray_tstring hogeArr; 
    tarray_tstring piyoArr;
    tarray_tstring penArr;

    //#3 &putf( size_t index, size_t n, const char *format, ... )
    hogeArr.putf(0, 2, "*** %s ***","hoge");
    sio.printf("putf( size_t index, size_t n, const char *format, ...)サンプル\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    return 0;

}

