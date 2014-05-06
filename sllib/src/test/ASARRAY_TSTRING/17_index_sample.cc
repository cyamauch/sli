/**
  example code
  asarray_tstringクラス:ssize_t index( const char *key ) const
			ssize_t indexf( const char *fmt, ... ) const
			size_t vindexf( const char *fmt, va_list ap ) const
                        キーに該当する要素番号
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

    //"Muritaniya"の要素番号を取得&出力
    sio.printf("my_country.index(\"Muritaniya\") ... [%zd]\n",
		my_country.index("Muritaniya"));

    //確認の為、設定値を出力
    for ( size_t i=0 ; i < my_country.length() ; i++ ) {
        const char *key = my_country.key(i);
        sio.printf("%s ... [%s]\n", key, my_country.cstr(key));
    }

    return 0;
}

