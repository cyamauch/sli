/**
  example code
  asarray_tstringクラス:asarray_tstring &init()
		    	asarray_tstring &init(const asarray_tstring &obj)
                        オブジェクトの完全初期化
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring IgNobel_asarr;
    asarray_tstring my_asarr;
    IgNobel_asarr["2008"] = "Toshiyuki Nakagaki";
    IgNobel_asarr["2007"] = "Mayu Yamamoto";
    IgNobel_asarr["2006"] = "Dr.Nakamatsu";

    //IgNobel_asarrでmy_asarrを初期化
    my_asarr.init(IgNobel_asarr);
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //my_asarrを完全初期化
    my_asarr.init();
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    return 0;
}

