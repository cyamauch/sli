/**
  example code
  tarray_tstringクラス:tarray_tstring &put( size_t index, const char *el0, const char *el1, ... )
		       tarray_tstring &vput( size_t index, const char *el0, const char *el1, va_list ap )
		       tarray_tstring &put( size_t index, const char *const *elements )
		       tarray_tstring &put( size_t index, const char *const *elements, size_t n )
		       tarray_tstring &put( size_t index, const tarray_tstring &src, size_t idx2 )
		       tarray_tstring &put( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )
                       任意の要素位置へ文字列をセット
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;

    const char *mytree[] = {"maple", "larch", "camphor", NULL};
    const tarray_tstring tree_arr(mytree);

    //tree_arrの1番目、2番目をmy_arrの2番目と3番目にセット
    my_arr.put(2, tree_arr, 1, 2 );
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    // 他のメンバ関数のサンプル
    tarray_tstring hogeArr; 
    tarray_tstring piyoArr;
    tarray_tstring penArr;

    //#1 &put( size_t index, const char *el0, const char *el1, ...  )のサンプル
    const char *el0 = "hogo";
    const char *el1 = "nyan";
    const char *el2 = NULL;
    hogeArr.put(0, el0, el1, el2);
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#3 &put( size_t index, const char *const *elements )のサンプル
    const char *piyoElem[] = {"Piyo", "piyo", NULL};
    hogeArr.put(0, piyoElem );
    sio.printf("put( size_t index, const char *const *elements )サンプル\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n", i, hogeArr.cstr(i));
    }

    //#4 &put( size_t index, const tarray_tstring &src, size_t idx2 )のサンプル
    //const char *fooElement[] = {"foo", "bar", "hoo","var", "foobar", "hoovar"};
    const char *penElement[] = {"penkocyan", "penokun", "pentakun","penpen", "penbou", NULL};
    const tarray_tstring penArr2(penElement);
    hogeArr.put(10, penArr2, 2 );
    sio.printf("put( size_t index, const tarray_tstring &src, size_t idx2 )サンプル\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#5 &put( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )のサンプル
    hogeArr.put(3, penArr2, 1, 2 );
    sio.printf("put( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )サンプル\n hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    return 0;

}

