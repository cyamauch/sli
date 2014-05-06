/**
  example code
  tarray_tstring�N���X:const char *const *cstrarray() const
                       ������̃|�C���^�z��̃A�h���X
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

    /* ������̃|�C���^�z��̃A�h���X���擾���� */
    const char *const *ptr = my_arr.cstrarray();
    if ( ptr != NULL ) {
        int i;
        for ( i=0 ; ptr[i] != NULL ; i++ ) {
            sio.printf("%d ... [%s]\n", i, ptr[i]);
        }
    }
    return 0;
}

