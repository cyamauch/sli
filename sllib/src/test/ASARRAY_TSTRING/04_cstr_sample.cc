/**
  example code
  asarray_tstring�N���X:const char *cstr( size_t index ) const
                        const char *c_str( const char *key ) const
                        const char *cstr( const char *key ) const
                        const char *cstrf( const char *fmt, ... ) const
                        const char *vcstrf( const char *fmt, va_list ap ) const
                        �A�z�z��̃L�[�C���͎w��ʒu�ɊY�����镶����
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["Itamu Hito"]    = "Arata Tendou";
    my_asarr["Kucyu Buranko"] = "Hideo Okuda";
    my_asarr["Riyuu"]         = "Miyuki Miyabe";

    //cstr�����o�֐����g�p���ďo��
    sio.printf("my_asarr c_str ... [%s]\n", my_asarr.c_str("Riyuu"));

    sio.printf("my_asarr cstr ... [%s]\n", my_asarr.cstr("Itamu Hito"));
    sio.printf("my_asarr cstr ... [%s]\n", my_asarr.cstr(1));
    //���݂��Ȃ��L�[���w�肷���null���Ԃ�
    sio.printf("my_asarr cstr ... [%s]\n", my_asarr.cstr("aaa"));


   //���l�ɏ������s��
    asarray_tstring asarr;
    asarr["a"] = "linux";
    asarr["b"] = "solaris";
    asarr["c"] = "windows";
    asarr["d"] = "mac";
    asarr["e"] = "lindows";

    sio.printf("asarr total length ... [%zu]\n", asarr.length());
    sio.printf("asarr c_str ... [%s]\n", asarr.c_str("e"));
    sio.printf("asarr cstr ... [%s]\n", asarr.cstr("d"));
    sio.printf("asarr cstr ... [%s]\n", asarr.cstr(1));

    return 0;
}

