/**
  example code
  asarray_tstring�N���X:asarray_tstring &clean(const char *str = "")
			asarray_tstring &clean(const tstring &str)
                        �����̘A�z�z��̒l�S�̂�C�ӂ̕����Ńp�f�B���O
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    asarray_tstring my_asarr;
    my_asarr["U2"] = "Beautiful Day";
    my_asarr["Eric Clapton"] = "Tears In Heaven";
    my_asarr["TOTO"] = "Rosanna";

    //�ݒ�l�m�F�p�o��
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //�S�l��"Grammy Award"�Ŗ��߂�
    my_asarr.clean("Grammy Award");
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    return 0;
}

