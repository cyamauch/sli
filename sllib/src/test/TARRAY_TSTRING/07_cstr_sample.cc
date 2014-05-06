/**
  example code
  tarray_tstring�N���X:const char *c_str( size_t index ) const
		       const char *cstr( size_t index ) const
                       ������z��v�f�̐擪�A�h���X
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    /* �I�u�W�F�N�g�̏����� */
    const char *tmp[] = {"linux", "windows", "mac", NULL};
    tarray_tstring my_arr;
    my_arr = tmp;

    /* �\�� */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    /* �z�񒷂𒴂��Ă̎Q�Ƃ�null���Ԃ� */
    sio.printf("[%s]\n", my_arr.cstr(3));

    return 0;
}

