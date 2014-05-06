/**
  example code
  asarray_tstringクラス:asarray_tstring &clean(const char *str = "")
			asarray_tstring &clean(const tstring &str)
                        既存の連想配列の値全体を任意の文字でパディング
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

    //設定値確認用出力
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //全値を"Grammy Award"で埋める
    my_asarr.clean("Grammy Award");
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    return 0;
}

