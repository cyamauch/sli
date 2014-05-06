/**
  example code
  asarray_tstring�N���X:ssize_t index( const char *key ) const
			ssize_t indexf( const char *fmt, ... ) const
			size_t vindexf( const char *fmt, va_list ap ) const
                        �L�[�ɊY������v�f�ԍ�
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_country;
    my_country["Saudi Arabia"] = "Abdullah bin Abdulaziz al-Saud";
    my_country["Muritaniya"] = "Mohamed Ould Abdel Aziz";
    my_country["Cyprus"] = "Demetris Christofias";

    //"Muritaniya"�̗v�f�ԍ����擾&�o��
    sio.printf("my_country.index(\"Muritaniya\") ... [%zd]\n",
		my_country.index("Muritaniya"));

    //�m�F�ׁ̈A�ݒ�l���o��
    for ( size_t i=0 ; i < my_country.length() ; i++ ) {
        const char *key = my_country.key(i);
        sio.printf("%s ... [%s]\n", key, my_country.cstr(key));
    }

    return 0;
}

