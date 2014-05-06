/**
  example code
  tarray_tstring�N���X:tarray_tstring &erase()
    		       tarray_tstring &erase( size_t index, size_t num_elements = 1 )
                       �v�f�̍폜
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *menu[] = {"rice ball", "sushi", "tofu", NULL};
    tarray_tstring my_arr = menu;
    //1�Ԗڂ���2���폜
    my_arr.erase(1,2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    //�z�񒷂𒴂����v�f�ʒu���w�肵�Ă��G���[�ɂ͂Ȃ�Ȃ�
    my_arr.erase(10,1);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }


    //�폜�̂�����
    tarray_tstring arr;
    //�Ō��NULL�����邱�ƁB
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

