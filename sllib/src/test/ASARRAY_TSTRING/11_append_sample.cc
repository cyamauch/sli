/**
  example code
  asarray_tstringクラス:asarray_tstring &append( const char *key, const char *val )
			asarray_tstring &append( const char *key, const tstring &val )
			asarray_tstring &appendf( const char *key, const char *fmt, ... )
			asarray_tstring &vappendf( const char *key, const char *fmt, va_list ap )
                        連想配列に要素(キーと値)を追加する
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["rice"] = "China";
    my_asarr["coffee"] = "Brazile";
    const char *key_cacao = "cacao";

    //"### No.1 is %s ###"のフォーマットで"Cote d'Ivoire"を追加
    my_asarr.appendf(key_cacao,"### No.1 is %s ###","Cote d'Ivoire");
    sio.printf("my_asarr total length ... [%zu]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }


    //old
    asarray_tstring asarr;
    asarr["a"] = "agar";
    asarr["b"] = "bean jam";
    asarr["c"] = "candy";
    asarr["d"] = "dessert";
    asarr["e"] = "entree";

    const asarrdef_tstring elem[] = { {"K","Kenya"}, {"L","Laos"}, {"M","Morocco"}, {"N","Nicaragua"}, {NULL,NULL} };
    asarray_tstring asarr1;
    asarr1.assign(elem);

    try{
            //&append( const char *key, const char *val  )のサンプル
	    const char *keyN = "n";
	    const char *valN = "napkin";

	    asarr.append(keyN,valN);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&append( const char *key, const tstring &val  )のサンプル
	    const char *keyO = "o";
	    const tstring valO = "oyster";

	    asarr.append(keyO,valO);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&appendf( const char *key, const char *fmt, ... )のサンプル
	    const char *keyP = "p";

	    asarr.appendf(keyP,"### %s ###","parfait");
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

    }catch(err_rec msg){
        sio.eprintf("[EXCEPTION] function=[%s::%s] message=[%s]\n",
                    msg.class_name, msg.func_name, msg.message);
        return -1;
    }

    return 0;
}

