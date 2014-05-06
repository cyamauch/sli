/**
  example code
  asarray_tstringクラス:tstring &at( const char *key )
			const tstring &at_cs( const char *key ) const
			tstring &at( size_t index )
			const tstring &at_cs( size_t index ) const
			tstring &atf( const char *fmt, ... )
			tstring &vatf( const char *fmt, va_list ap )
                        指定したキー，又は位置に該当する文字列オブジェクトの参照
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

    //my_asarr設定値確認用に出力
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //at関数を用いてmy_asarrにキーと値を設定
    my_asarr.at("Yasushi Inoue") = "Tougyu";
    //確認用出力
    sio.printf("my_asarr c_str ... [%s]\n", my_asarr.c_str("Yasushi Inoue"));

    //エラー確認用コード：指定したキーが存在しない場合エラーとなる
    //my_asarr.at(10) = "aaa";

    return 0;
}

