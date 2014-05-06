/**
  example code
  tarray_tstring�N���X:tstring &operator[]( size_t index )
                       �Y�����̗v�f���Q��
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[] = {"hawthorn", "oak", NULL};
    tarray_tstring my_arr = tree;
    /* 2�Ԗڂ̗v�f��"camellia"���� */
    my_arr[2] = "camellia";

    /* �\�� */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    //�T�C�Y�͎����g������A�G���[�ɂȂ�Ȃ�
    my_arr[10] = "camellia";        //����
    /* �\�� */
    sio.printf("********************************\n");
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr[i].cstr());
    }

    tarray_tstring my_arr3(my_arr);
    //�T�C�Y�͎����g������A�G���[�ɂȂ�Ȃ�
    my_arr3[20] = "camellia";        //����
    for ( size_t i=0 ; i < my_arr3.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr3[i].cstr());
    }

    return 0;
}

