/**
  example code
  asarray_tstringクラス:asarray_tstring &insert( const char *key, const char *newkey, const char *newval )
			asarray_tstring &insert( const char *key, const char *newkey, const tstring &newval )
			asarray_tstring &insertf( const char *key, const char *newkey, const char *fmt, ... )
			asarray_tstring &vinsertf( const char *key, const char *newkey, const char *fmt, va_list ap )
                        連想配列オブジェクトに要素(キーと値)を挿入する
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    my_asarr["Nile River"] = "Africa";
    my_asarr["the Amazon"] = "South America";

    //文字列を挿入
    my_asarr.insert("the Amazon", "Chang River", "China");
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
            //&insert( const char *key, const char *newkey, const char *newval )のサンプル
	    const char *newkeyN = "n";
	    const char *newvalN = "night train";
	    asarr.insert("e", newkeyN, newvalN);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&insert( const char *key, const char *newkey, const tstring &newval )のサンプル
	    const char *newkeyO = "O";
	    const tstring newvalO = "one-way traffic";
	    asarr.insert("n", newkeyO, newvalO);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&insertf( const char *key, const char *newkey, const char *fmt, ... )のサンプル
	    const char *newkeyP = "P";
	    asarr.insertf("K", newkeyP, "@@@ %s @@@", "police car");
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

