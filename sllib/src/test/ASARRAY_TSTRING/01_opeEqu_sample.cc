/**
  example code
  asarray_tstring�N���X:oasarray_tstring &operator=(const asarray_tstring &obj)
                        asarray_tstring�N���X�̃I�u�W�F�N�g�̃R�s�[
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["linux"]   = "Linus Torvalds";
    my_asarr["windows"] = "Bill Gates";
    my_asarr["mac"]     = "Steve Jobs";

    asarray_tstring my_asarrObj;
    /* my_asarr��my_asarrObj�ɃR�s�[ */
    my_asarrObj = my_asarr;
    /* ���ׂĂ̗v�f��\�� */
    for ( size_t i=0 ; i < my_asarrObj.length() ; i++ ) {
        const char *key = my_asarrObj.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarrObj.cstr(key));
    }

    return 0;
}

