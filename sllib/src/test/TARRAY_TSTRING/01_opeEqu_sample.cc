/**
  example code
  tarray_tstring�N���X:tarray_tstring &operator=(const tarray_tstring &obj)
		       const char *const *operator=(const char *const *elements)
                       ���������
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *menu[] = {"rice ball", "sushi", "tofu", NULL};
    tarray_tstring my_arr;
    my_arr = menu;          /* '='�� tarray_tstring�N���X�̉��Z�q */

    /* �\�� */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    return 0;
}

