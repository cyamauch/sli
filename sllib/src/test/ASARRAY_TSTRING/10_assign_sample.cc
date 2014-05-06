/**
  example code
  asarray_tstringクラス:asarray_tstring &assign( const asarray_tstring &src )
 			asarray_tstring &assign( const asarrdef_tstring elements[] )
			asarray_tstring &assign( const asarrdef_tstring elements[], size_t n )
			asarray_tstring &assign( const char *key0, const char *val0, const char *key1, ... )
			asarray_tstring &vassign( const char *key0, const char *val0, const char *key1, va_list ap )
                        連想配列オブジェクトの初期化
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    asarray_tstring my_asarr;
    const asarrdef_tstring mount_elem[] = { {"K2","China"},
	 {"Kangchenjunga","Nepal"}, {"Mount Kenya","Kenya"},{NULL,NULL} };

    //my_asarrをmount_elemの文字列で初期化
    my_asarr.assign(mount_elem);
    sio.printf("my_asarr total length ... [%zu]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //同様に他のメンバ関数の処理を行う
    asarray_tstring asarr;
    asarr["F"] = "France";
    asarr["G"] = "Ghana";
    asarr["H"] = "Hungary";
    asarr["I"] = "India";
    asarr["J"] = "Jamaica";

    const asarrdef_tstring elem[] = { {"K","Kenya"}, {"L","Laos"},
		 {"M","Morocco"}, {"N","Nicaragua"}, {NULL,NULL} };
    asarray_tstring asarr1;

    try{
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

            //&assign( const asarray_tstring &src )のサンプル
	    asarr1.assign(asarr);
	    sio.printf("asarr1 total length ... [%zu]\n", asarr1.length());
	    for ( size_t i=0 ; i < asarr1.length() ; i++ ) {
	        const char *key1 = asarr1.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr1.cstr(key1));
	    }


            //&assign( const asarrdef_tstring elements[] )のサンプル
	    asarr1.assign(elem);
	    sio.printf("asarr1 total length ... [%zu]\n", asarr1.length());
	    for ( size_t i=0 ; i < asarr1.length() ; i++ ) {
	        const char *key1 = asarr1.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr1.cstr(key1));
	    }

            //&assign( const asarrdef_tstring elements[], size_t n )のサンプル
	    asarr1.assign(elem,3);
	    sio.printf("asarr1 total length ... [%zu]\n", asarr1.length());
	    for ( size_t i=0 ; i < asarr1.length() ; i++ ) {
	        const char *key1 = asarr1.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr1.cstr(key1));
	    }

            //&assign( const char *key0, const char *val0,const char *key1, ... )のサンプル 要NULL終端
            const char *keyS = "S";
            const char *valS = "Somalia";
            const char *keyT = "T";
            const char *valT = "Tanzania";
            const char *keyU = "U";
            const char *valU = "Uruguay";
	    asarr1.assign( keyS, valS, keyT, valT, keyU, valU, NULL, NULL);
	    sio.printf("asarr1 total length ... [%zu]\n", asarr1.length());
	    for ( size_t i=0 ; i < asarr1.length() ; i++ ) {
	        const char *key1 = asarr1.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr1.cstr(key1));
	    }

    }catch(err_rec msg){
        sio.eprintf("[EXCEPTION] function=[%s::%s] message=[%s]\n",
                    msg.class_name, msg.func_name, msg.message);
        return -1;
    }

    return 0;
}

