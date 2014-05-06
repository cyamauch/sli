/**
  example code
  tarray_tstringクラス:tarray_tstring &insert( size_t index, const char *str, size_t n )
		       tarray_tstring &insert( size_t index, const tstring &str, size_t n )
		       tarray_tstring &insertf( size_t index, size_t n, const char *format, ... )
		       tarray_tstring &vinsertf( size_t index, size_t n, const char *format, va_list ap )
                       要素の挿入
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[]    = {"cycad", "dogwood", NULL};
    tarray_tstring my_arr = tree;

    //1番目の要素の次に文字列を追加
    my_arr.insert(1, "palm", "fir", NULL);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //他のメンバ関数のサンプル
    tarray_tstring tarr; 
    tarray_tstring tarr1;

    const char *hogeElement[] = {"HOGE", "hoge", "is", "Hoge",NULL};
    const char *ponyoElement[] = {"Ponyo", "ponyo", "poNyo", "sakana", "noko",NULL};

    const tarray_tstring hogeArr(hogeElement);
    const tarray_tstring ponyoArr(ponyoElement);
    tarr.assign(hogeArr);

    //#1 &insert( size_t index, const char *el0, const char *el1, ...  ),el...を配列のindex番目へ挿入する。要NULL終端。
    tarr.insert(1, "abc", "def", "hij", NULL);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#2 &vinsert( size_t index, const char *el0, const char *el1, va_list ap  ),el,apを配列のindex番目へ挿入する。
    //サンプル無

    //#3 &insert( size_t index, const char *const *elements ),elementsを配列のindex番目へ挿入する。
    tarr.insert(4, ponyoElement);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#4 &insert( size_t index, const char *const *elements, size_t n ),elementsを配列のindex番目からn個挿入する。
    tarr.insert(2, ponyoElement, 3);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#5 &insert( size_t index, const tarray_tstring &src, size_t idx2 ),srcのidx2番目以降を配列のindex番目へ挿入する。
    tarr.insert(13, ponyoArr, 2);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#6 &insert( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 ),srcのidx2番目からn2個を配列のindex番目へ挿入する。
    tarr.insert(6, ponyoArr, 1, 3);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    return 0;
}

