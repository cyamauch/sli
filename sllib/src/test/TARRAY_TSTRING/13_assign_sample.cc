/**
  example code
  tarray_tstringクラス:tarray_tstring &assign( const char *str, size_t n )
		       tarray_tstring &assign( const tstring &str, size_t n )
		       tarray_tstring &assignf( size_t n, const char *format, ... )
		       tarray_tstring &vassignf( size_t n, const char *format, va_list ap )
                       オブジェクトの初期化
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr; 
    //"***%s"の書式で"Japanese quince"を追加
    my_arr.assignf(3,"***%s", "Japanese quince");
    sio.printf("my_arr length = %zu\n", my_arr.length());
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //old
    tarray_tstring hogeArr; 
    tarray_tstring piyoArr;

    //#1 &assign( const char *str, size_t n ),(const) char型のstrで初期化したn個の配列を作成する。
    hogeArr.assign("hoge",5);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    sio.printf("hogeArr[0] = %s\n",hogeArr.cstr(0));

    //#2 &assign( const tstring &str, size_t n ),(const) tstring型のstrで初期化したn個の配列を作成する。
    sio.printf("*** &assign( const tstring &str, size_t n ) sample ***\n");
    const tstring strPiyo = "Piyo";
    hogeArr.assign(strPiyo,10);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#3 &assignf( size_t n, const char *format, ...  ),文字列...をformatの書式で設定したn個の配列を作成する。
    sio.printf("*** &assign( size_t n, const char *format, ... ) sample ***\n");
    const char *strNyaa = "Nyaa";
    hogeArr.assignf(3,"***%s",strNyaa);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    return 0;
}

