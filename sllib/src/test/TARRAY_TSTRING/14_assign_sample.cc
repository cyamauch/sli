/**
  example code
  tarray_tstringクラス:tarray_tstring &assign( const char *el0, const char *el1, ... )
		       tarray_tstring &vassign( const char *el0, const char *el1, va_list ap )
		       tarray_tstring &assign( const char *const *elements )
		       tarray_tstring &assign( const char *const *elements, size_t n )
		       tarray_tstring &assign( const tarray_tstring &src, size_t idx2 = 0 )
		       tarray_tstring &assign( const tarray_tstring &src, size_t idx2, size_t n2 )
                       オブジェクトの初期化
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr; 

    const char *tree[] = {"fir", "magnolia", "dogwood", NULL};
    const tarray_tstring myTree(tree);

    /* 配列myTreeの1番目から2個でmy_arrを初期化 */
    my_arr.assign(myTree,1,2);
    sio.printf("*** my_arr length = %zu\n", my_arr.length());
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("*** my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //他のメンバ関数のサンプル
    tarray_tstring hogeArr; 
    tarray_tstring piyoArr;

    //#1 &assign( const char *el0, const char *el1, ... ),引数で指定した個数分の配列を作成し、先頭より順に引数文字列で埋める。
    sio.printf("*** &assign( const char *el0, const char *el1, ... ) sample ***\n");
    hogeArr.assign( "hoga", "huga", "hega", "hoge", NULL);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#3 &assign( const char *const *elements ),elementsで定義した要素分の配列を作成する。
    sio.printf("*** &assign( const char *const *elements ) sample ***\n");
    const char *piyoElement[] = {"Piyo", "piyo", "Nyan","Goo", "Pau","Pou",NULL};
    hogeArr.assign(piyoElement);
    sio.printf("hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#4 &assign( const char *const *elements, size_t n ),n個の配列を作成しelementsで定義した要素で初期化する。
    //const char *piyoElement[] = {"Piyo", "piyo", "Nyan","Goo", "Pau","Pou",NULL};
    //定義数より少ない場合
    hogeArr.assign(piyoElement,3);
    sio.printf("定義数より少ない**** hogeArr length = %zu\n", hogeArr.length());
    for (size_t i = 0; i < hogeArr.length(); i++){
        sio.printf("hogeArr[%zu] = %s\n",i, hogeArr.cstr(i));
    }

    //#5 &assign( const tarray_tstring &src, size_t idx2 = 0 ),　n個の配列を作成しelementsで定義した要素で初期化する。
    sio.printf("*** &assign( const tarray_tstring &src, size_t idx2 = 0 ) sample ***\n");
    const char *penElement[] = {"penkocyan", "penokun", "pentakun","penpen", "penbou",NULL};
    tarray_tstring penArr(penElement);
    for (size_t i = 0; i < penArr.length(); i++){
        sio.printf("penArr[%zu] = %s\n",i, penArr.cstr(i));
    }
    const char *pen2Element[] = {"penkichi","penmicyan",NULL};
    //NULLがないと、"2 [main] assign_sample 1684 _cygtls::handle_exceptions: Error while dumping state (probably co
    //rrupted stack)"
    //const char *pen2Element[] = {"penkichi","penmicyan"};
    sio.printf("*** &assign( const tarray_tstring &src, size_t idx2, size_t n2  ) sample ***\n");
    const tarray_tstring penArr2(pen2Element);
    for (size_t i = 0; i < penArr2.length(); i++){
        sio.printf("penArr2[%zu] = %s\n",i, penArr2.cstr(i));
    }

    //#6 &assign( const tarray_tstring &src, size_t idx2, size_t n2)
    penArr.assign(penArr2,0,2);
    sio.printf("*** penArr length = %zu\n", penArr.length());
    for (size_t i = 0; i < penArr.length(); i++){
        sio.printf("*** penArr[%zu] = %s\n",i, penArr.cstr(i));
    }

    return 0;

}

