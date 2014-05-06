/**
  example code
  asarray_tstringクラス:tstring &operator[]( const char *key )
                        キーに対応する連想配列の値を参照
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    //my_asarrに値を設定
    my_asarr["google"]  = "Larry Page";
    my_asarr["YouTube"]   = "Steve Chen";

    sio.printf("YouTube ... [%s]\n", my_asarr["YouTube"].cstr());

    //値なしでキーのみ設定すると、値は""で返る
    my_asarr["Yahoo"];
    sio.printf("Yahoo ... [%s]\n", my_asarr["Yahoo"].cstr());

    return 0;
}

