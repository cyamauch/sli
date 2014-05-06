/**
  example code
  asarray_tstring�N���X:tstring &at( const char *key )
			const tstring &at_cs( const char *key ) const
			tstring &at( size_t index )
			const tstring &at_cs( size_t index ) const
			tstring &atf( const char *fmt, ... )
			tstring &vatf( const char *fmt, va_list ap )
                        �w�肵���L�[�C���͈ʒu�ɊY�����镶����I�u�W�F�N�g�̎Q��
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{

    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["Takeshi kaikou"]  = "Hadaka no Oosama";
    my_asarr["Koubou Abe"]      = "Kabe";
    my_asarr["Kenzaburou Ooe"]  = "Shiiku";

    //my_asarr�ݒ�l�m�F�p�ɏo��
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //at�֐���p����my_asarr�ɃL�[�ƒl��ݒ�
    my_asarr.at("Yasushi Inoue") = "Tougyu";
    //�m�F�p�o��
    sio.printf("my_asarr c_str ... [%s]\n", my_asarr.c_str("Yasushi Inoue"));

    //�G���[�m�F�p�R�[�h�F�w�肵���L�[�����݂��Ȃ��ꍇ�G���[�ƂȂ�
    //my_asarr.at(10) = "aaa";

    return 0;
}

