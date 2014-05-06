/**
  example code
  asarray_tstring�N���X:size_t length() const
                        size_t length( const char *key ) const
                        �A�z�z��̒���(��)�C�����񒷂̒���
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    asarray_tstring my_asarr;
    my_asarr["linux"]   = "Linus Torvalds";
    my_asarr["google"]  = "Larry Page";
    my_asarr["mac"]     = "Steve Jobs";

    //�z�񒷂��擾
    sio.printf("my_asarr total length   ... [%zu]\n", my_asarr.length());
    //'google'�ɑΉ�����l�̕����񒷂��擾
    sio.printf("my_asarr key='google' length ... [%zu]\n", my_asarr.length("google"));


    //asarr�ɂ��ē��l�ɏ�������
    asarray_tstring asarr;
    asarr["a"] = "linux";
    asarr["b"] = "solaris";
    asarr["c"] = "windows";
    asarr["d"] = "mac";
    asarr["e"] = "lindows";

    //�z�񒷂��擾
    sio.printf("asarr total length ... [%zu]\n", asarr.length());
    //'c'�ɑΉ�����l�̕����񒷂��擾
    sio.printf("asarr key='c' length ... [%zu]\n", asarr.length("c"));
    /* ���ׂĂ̗v�f��\�� */
    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
        const char *key = asarr.key(i);
        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
    }

    return 0;
}

