/**
  example code
  asarray_tstringクラス:const char *key( size_t index ) const
                        キーの値
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
    //全項目のキー値を出力
    for ( size_t i=0 ; i < my_city.length() ; i++ ) {
        const char *key = my_city.key(i);
        sio.printf("#%zx -> %s ... [%s]\n", i, key, my_city.cstr(key));
    }

    //範囲外の指定の場合、nullが返る
    sio.printf("[%s]\n", my_city.key(5));

    return 0;
}

