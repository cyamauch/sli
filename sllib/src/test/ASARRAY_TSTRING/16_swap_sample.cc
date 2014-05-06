/**
  example code
  asarray_tstring�N���X:asarray_tstring &swap( asarray_tstring &sobj )
                        �I�u�W�F�N�g�̓��ւ�
*/#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_africa;
    my_africa["Rwanda"] = "Kigali";
    my_africa["Cameroon"] = "Yaounde";

    asarray_tstring my_america;
    my_america["Honduras"] = "Tegucigalpa";
    my_america["Jamaica"] = "Kingston";

    //my_africa��my_america�����ւ�
    my_africa.swap(my_america);
    sio.printf("my_africa total length ... [%zu]\n", my_africa.length());
    for ( size_t i=0 ; i < my_africa.length() ; i++ ) {
        const char *africa_key = my_africa.key(i);
        const char *africa__key = my_america.key(i);
        sio.printf("[%s]:%s <===> [%s]:%s\n", africa_key, my_africa.cstr(africa_key),
		    africa__key, my_africa.cstr(africa_key));
    }

    return 0;
}

