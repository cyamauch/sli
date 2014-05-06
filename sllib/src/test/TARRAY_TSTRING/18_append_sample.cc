/**
  example code
  tarray_tstringクラス:tarray_tstring &append( const char *el0, const char *el1, ... )
		       tarray_tstring &vappend( const char *el0, const char *el1, va_list ap )
		       tarray_tstring &append( const char *const *elements )
		       tarray_tstring &append( const char *const *elements, size_t n )
		       tarray_tstring &append( const tarray_tstring &src, size_t idx2 )
		       tarray_tstring &append( const tarray_tstring &src, size_t idx2, size_t n2 )
                       要素の追加
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    my_arr.at(0) = "chestnut";
    const char *mytree[] = {"chestnut", "zelkova", "crape myrtle", "daphne", NULL};
    const tarray_tstring tree_arr(mytree);

    //tree_arrの2番目、3番目をmy_arrに追加
    my_arr.append(tree_arr,2,2);
    sio.printf("my_arr length = %zu\n", my_arr.length());
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }


    //他のメンバ関数のサンプル
    tarray_tstring tarr; 
    tarray_tstring tarr1;

    const char *hogeElement[] = {"HOGE", "hoge", "is", "Hoge",NULL};

    const tarray_tstring hogeArr(hogeElement);

    //#1 &append( const char *el0, const char *el1, ...  ),最後にel要素を追加する。NULL終端必要
    tarr.append("foo","var",".", NULL);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#3 &append( const char *const *elements ),n個のelements要素を配列に追加する。
    const char *element[] = {"abc", "def", NULL};
    tarr.append(element);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#4 &append( const char *const *elements, size_t n  ),n個のelements要素を配列に追加する。
    //element要素の個数を超えた値を使用する場合、element要素の個数が有効。
    const char *element1[] = {"ghi", "jkl", "mno", NULL};
    tarr.append(element1,3);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#5 &append( const tarray_tstring &src, size_t idx2  ),最後に&srcのidx2番目以降の要素を追加する。
    tarr.append(hogeArr,1);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    //#6 &append( const tarray_tstring &src, size_t idx2, size_t n2  ),最後に&srcのidx2番目の要素からn2を追加する。
    tarr.append(hogeArr,0,3);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }
    return 0;
}

