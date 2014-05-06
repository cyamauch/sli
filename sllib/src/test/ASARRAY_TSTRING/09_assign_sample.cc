/**
  example code
  asarray_tstring�N���X:asarray_tstring &assign( const char *key, const char *val )
		        asarray_tstring &assign( const char *key, const tstring &val )
     			asarray_tstring &assignf( const char *key, const char *fmt, ... )
		        asarray_tstring &vassignf( const char *key, const char *fmt, va_list ap )
                        �A�z�z��I�u�W�F�N�g�̏�����
*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    //new
    asarray_tstring my_asarr;

    const char *key0 = "Everest";
    const char *val0 = "Nepal";
    //my_asarr�̏�����
    my_asarr.assign(key0,val0);
    sio.printf("my_asarr total length ... [%zu]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //format��p����my_asarr�̏�����
    const char *key1 = "Kilima-Njaro";
    my_asarr.assignf(key1,"*** %s ***","Tanzania");
    sio.printf("my_asarr total length ... [%zu]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }


    //���l�ɏ������s��
    asarray_tstring asarr;
    asarr["F"] = "France";
    asarr["G"] = "Ghana";
    asarr["H"] = "Hungary";
    asarr["I"] = "India";
    asarr["J"] = "Jamaica";

    asarray_tstring asarr1;

    try{
            //&assign( const char *key, const char *val )�T���v��
            const char *keyO = "O";
            const char *valO = "Oman";
	    asarr1.assign(keyO,valO);
	    sio.printf("asarr1 total length ... [%zu]\n", asarr1.length());
	    for ( size_t i=0 ; i < asarr1.length() ; i++ ) {
	        const char *key1 = asarr1.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr1.cstr(key1));
	    }

            //&assign( const char *key, const tstring &val )�T���v��
            const char *keyP = "P";
            const tstring valP = "Puerto Rico";
	    asarr1.assign(keyP,valP);
	    sio.printf("asarr1 total length ... [%zu]\n", asarr1.length());
	    for ( size_t i=0 ; i < asarr1.length() ; i++ ) {
	        const char *key1 = asarr1.key(i);
	        sio.printf("%s ... [%s]\n", key1, asarr1.cstr(key1));
	    }

            //&assign( const char *key, const char *fmt, ...  )�T���v��
            const char *keyQ = "R";
	    asarr1.assignf(keyQ,"*** %s ***","Romania");
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

