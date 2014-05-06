/**
  example code
  tarray_tstring�N���X:size_t length() const
		       size_t length( size_t index ) const
                       ������z��̒���(��)�C���͕�����̒���
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    my_arr.at(0).printf("Hello");
    my_arr.at(1).printf("Hoge");

    //my_arr�̒�����\��
    sio.printf("my_arr length = %zu\n",my_arr.length());
    //my_arr�̊e�v�f�̕����񒷂�\��
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr index%zu length = %zu\n",i, my_arr.length(i));
        //sio.printf("index %d ... [%d]\n",i, my_arr.length(i));
    }

    return 0;
}

