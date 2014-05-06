/**
  example code
  asarray_tstringクラス:asarray_tstring &insert( const char *key, const asarray_tstring &src )
			asarray_tstring &insert( const char *key, const asarrdef_tstring elements[] )
			asarray_tstring &insert( const char *key, const asarrdef_tstring elements[], size_t n )
			asarray_tstring &insert( const char *key, const char *key0, const char *val0, const char *key1, ... )
			asarray_tstring &vinsert( const char *key, const char *key0, const char *val0, const char *key1, va_list ap )
                        連想配列オブジェクトに要素(キーと値)を挿入する
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const asarrdef_tstring lakes[] = { {"Lake Superior","North America"}, {"Lake Victoria","Tanzania"}, {NULL,NULL} };
    asarray_tstring my_lake;
    my_lake.assign(lakes);

    asarray_tstring my_asarr;
    my_asarr["Caspian Sea"] = "Eurasia";
    my_asarr["Aral Sea"] = "Kazakhstan";

    //文字列をmy_asarrに挿入
    my_asarr.insert("Aral Sea", my_lake);
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
            //&insert( const char *key, const asarray_tstring &src )のサンプル
	    asarr.insert("c",asarr1);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&insert( const char *key, const asarrdef_tstring elements[] )のサンプル
	    asarr.insert("K",elem1);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&insert( const char *key, const asarrdef_tstring elements[], size_t n )のサンプル
	    asarr.insert("M",elem2,2);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key1 = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr.cstr(key1));
	    }

            //&insert( const char *key, const char *key0, const char *val0, const char *key1, ... )のサンプル
	    const char *keyR = "R";
	    const char *valR = "radiator";
	    const char *keyS = "S";
	    const char *valS = "seat belt";
	    asarr.insert("b", keyR, valR, keyS, valS, NULL, NULL);
	    sio.printf("asarr total length ... [%d]\n", asarr.length());
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

