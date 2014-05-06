/**
  example code
  asarray_tstringクラス:asarray_tstring &append( const asarray_tstring &src )
			asarray_tstring &append( const asarrdef_tstring elements[] )
			asarray_tstring &append( const asarrdef_tstring elements[], size_t n )
			asarray_tstring &append( const char *key0, const char *val0, const char *key1, ... )
			asarray_tstring &vappend( const char *key0, const char *val0, const char *key1, va_list ap )
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
    const asarrdef_tstring foods[] = { {"banana","India"}, {"wheat","China"}, {NULL,NULL}};

    //my_asarrにfoodsの文字列を追加
    my_asarr.append(foods);
    sio.printf("my_asarr total length ... [%d]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //他のメンバ関数のサンプル
    asarray_tstring asarr;
    asarr["a"] = "agar";
    asarr["b"] = "bean jam";
    asarr["c"] = "candy";
    asarr["d"] = "dessert";
    asarr["e"] = "entree";

    const asarrdef_tstring elem[] = { {"K","Kenya"}, {"L","Laos"}, {"M","Morocco"}, {"N","Nicaragua"}, {NULL,NULL} };
    asarray_tstring asarr1;
    asarr1.assign(elem);

    const asarrdef_tstring elem1[] = { {"f","frozen yogurt"}, {"g","gelato"}, {"h","ham"}, {"i","ice cream"}, {NULL,NULL} };
    const asarrdef_tstring elem2[] = { {"j","juice"}, {"k","kiwi"}, {"l","lemonade"}, {"m","muffin"}, {NULL,NULL} };

    try{
            //&append( const asarray_tstring &src )のサンプル
	    asarr.append(asarr1);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&append( const asarrdef_tstring elements[] )のサンプル
	    asarr.append(elem1);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&append( const asarrdef_tstring elements[], size_t n )のサンプル
	    asarr.append(elem2);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&append( const char *key0, const char *val0, const char *key1, ... )のサンプル
	    const char *keyR = "r";
	    const char *valR = "rice cake";
	    const char *keyS = "s";
	    const char *valS = "scorn";

	    asarr.appendf(keyR, valR, keyS, valS);
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

