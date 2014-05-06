/**
  example code
  tarray_tstringクラス:tarray_tstring &append( const char *str, size_t n )
	               tarray_tstring &append( const tstring &str, size_t n )
		       tarray_tstring &appendf( size_t n, const char *format, ... )
		       tarray_tstring &vappendf( size_t n, const char *format, va_list ap )
                       要素の追加
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *mytree[] = {"maple", "larch", NULL};
    tarray_tstring my_arr(mytree);

    const tstring mytrr = "gardenia";
    //my_arrにmytrrを2個追加
    my_arr.append(mytrr,2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }


    //他のメンバ関数のサンプル
    tarray_tstring tarr; 
    tarray_tstring tarr1;

    const char *hogeElement[] = {"HOGE", "hoge", "is", "Hoge",NULL};

    const tarray_tstring hogeArr(hogeElement);

    const char *element[] = {"abc", "def", NULL};
    tarr.append(element);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#1 &append( const char *str, size_t n  ),strをn個追加する。
    tarr.append("anyway", 2);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#2 &append( const tstring &str, size_t n   ),tstring型strをn個追加する。
    const tstring hogeStr = "hoge";

    tarr.append(hogeStr, 2);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#3 &appendf( size_t n, const char *format, ...  ),...で指定する文字列をformatの書式にあてはめて最後にn個追加する。
    tarr.appendf(2, "*** %s %s ***", "is", "grate");
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }


    return 0;

}

