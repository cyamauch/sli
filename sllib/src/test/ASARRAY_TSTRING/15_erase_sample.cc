/**
  example code
  asarray_tstringクラス:asarray_tstring &erase()
			asarray_tstring &erase( const char *key, size_t num_elements = 1 )
                        連想配列の要素(キーと値の組)の削除
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["Kingdom of Lesotho"] = "Letsie III";
    my_asarr["Democratic People's Republic of Korea"] = "Kim Jong-il";
    my_asarr["Republic of Cote d'Ivoire"] = "Laurent Gbagbo";

    //"Democratic People's Republic of Korea"と値を削除
    my_asarr.erase("Democratic People's Republic of Korea");
    sio.printf("my_asarr total length ... [%zu]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //他のメンバ関数のサンプル
    asarray_tstring asarr;
    asarray_tstring asarr1;
    asarr["A"] = "linux";
    asarr["B"] = "solaris";
    asarr["C"] = "windows";
    asarr["D"] = "mac";
    asarr["E"] = "lindows";

    try{
           //設定値を確認
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

           //"D"のキーと値を削除
	    sio.printf("*** asarr.erase() ***\n");
	    asarr.erase("D");
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

	    sio.printf("*** asarr.erase() ***\n");
           //"C"から2個ののキーと値を削除
	    asarr.erase("C",2);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

	    sio.printf("*** all asarr.erase() ***\n");
           //全項目削除
	    asarr.erase();
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());

    }catch(err_rec msg){
        sio.eprintf("[EXCEPTION] function=[%s::%s] message=[%s]\n",
                    msg.class_name, msg.func_name, msg.message);
        return -1;
    }

    return 0;
}

