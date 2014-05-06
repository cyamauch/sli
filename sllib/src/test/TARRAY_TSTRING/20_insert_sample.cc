/**
  example code
  tarray_tstring�N���X:tarray_tstring &insert( size_t index, const char *el0, const char *el1, ... )
		       tarray_tstring &vinsert( size_t index, const char *el0, const char *el1, va_list ap )
		       tarray_tstring &insert( size_t index, const char *const *elements )
		       tarray_tstring &insert( size_t index, const char *const *elements, size_t n )
		       tarray_tstring &insert( size_t index, const tarray_tstring &src, size_t idx2 )
 		       tarray_tstring &insert( size_t index, const tarray_tstring &src, size_t idx2, size_t n2 )
                       �v�f�̑}��
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[]    = {"hawthorn", "oak", NULL};
    const char *addTree[] = {"cycad", "dogwood", NULL};
    tarray_tstring my_arr = tree;

    //addTree��0�ԖځA1�Ԗڂ̗v�f��my_arr��1�Ԗڂ̎��ɑ}��
    my_arr.insert(1, addTree, 2);
    for (size_t i = 0; i < my_arr.length(); i++){
        sio.printf("my_arr[%zu] = %s\n", i, my_arr.cstr(i));
    }

    return 0;

}

