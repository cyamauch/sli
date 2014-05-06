/**
  example code
  tarray_tstringクラス:tarray_tstring &erase()
    		       tarray_tstring &erase( size_t index, size_t num_elements = 1 )
                       要素の削除
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *menu[] = {"rice ball", "sushi", "tofu", NULL};
    tarray_tstring my_arr = menu;
    //1番目から2個を削除
    my_arr.erase(1,2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //配列長を超えた要素位置を指定してもエラーにはならない
    my_arr.erase(10,1);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }


    //削除のお試し
    tarray_tstring arr;
    //最後にNULLを入れること。
    const char *element[] = {"Hello", "Hoge", "World","Huge", "People",NULL};

    arr.init(element);
    for (size_t i = 0; i < arr.length(); i++){
        sio.printf("arr[%zu] = %s\n",i, arr.cstr(i));
    }

    sio.printf("*** after erase ***\n");
    arr.erase(1);
    for (size_t i = 0; i < arr.length(); i++){
        sio.printf("arr[%zu] = %s\n",i, arr.cstr(i));
    }

    sio.printf("*** after erase1 ***\n");
    arr.erase(1,2);
    for (size_t i = 0; i < arr.length(); i++){
        sio.printf("arr[%zu] = %s\n",i, arr.cstr(i));
    }

    sio.printf("*** after all erase ***\n");
    arr.erase();
    for (size_t i = 0; i < arr.length(); i++){
        sio.printf("arr[%zu] = %s\n",i, arr.cstr(i));
    }
    return 0;

}

