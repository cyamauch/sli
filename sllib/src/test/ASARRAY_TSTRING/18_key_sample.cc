/**
  example code
  asarray_tstring�N���X:const char *key( size_t index ) const
                        �L�[�̒l
*/#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>
#include <sli/ctindex.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_city;
    my_city["Yokohama"] = "Fumiko Hayashi";
    my_city["Osaka"]    = "Kunio Hiramatsu";
    my_city["Fukuoka"]  = "Hiroshi Yoshida";

    sio.printf("my_city total length ... [%zu]\n", my_city.length());
    //�S���ڂ̃L�[�l���o��
    for ( size_t i=0 ; i < my_city.length() ; i++ ) {
        const char *key = my_city.key(i);
        sio.printf("#%zx -> %s ... [%s]\n", i, key, my_city.cstr(key));
    }

    //�͈͊O�̎w��̏ꍇ�Anull���Ԃ�
    sio.printf("[%s]\n", my_city.key(5));

    return 0;
}

