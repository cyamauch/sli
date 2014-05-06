/**
  example code
  tarray_tstring�N���X:tarray_tstring &clean(const char *str = "")
		       tarray_tstring &clean(const tstring &str)
                       �����̕�����z��S�̂�C�ӂ̕�����Ńp�f�B���O
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    //new sample
    const char *tree[] = {"katsura", "torreya", NULL};
    tarray_tstring my_arr = tree;

    //"paulownia"�Ńp�f�B���O
    my_arr.clean("paulownia");
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }

    //���|��
    my_arr.clean();
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n",i, my_arr.cstr(i));
    }

    return 0;
}

