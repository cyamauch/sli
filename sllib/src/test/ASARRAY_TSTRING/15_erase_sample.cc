/**
  example code
  asarray_tstring�N���X:asarray_tstring &erase()
			asarray_tstring &erase( const char *key, size_t num_elements = 1 )
                        �A�z�z��̗v�f(�L�[�ƒl�̑g)�̍폜
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

    //"Democratic People's Republic of Korea"�ƒl���폜
    my_asarr.erase("Democratic People's Republic of Korea");
    sio.printf("my_asarr total length ... [%zu]\n", my_asarr.length());
    for ( size_t i=0 ; i < my_asarr.length() ; i++ ) {
        const char *key = my_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, my_asarr.cstr(key));
    }

    //���̃����o�֐��̃T���v��
    asarray_tstring asarr;
    asarray_tstring asarr1;
    asarr["A"] = "linux";
    asarr["B"] = "solaris";
    asarr["C"] = "windows";
    asarr["D"] = "mac";
    asarr["E"] = "lindows";

    try{
           //�ݒ�l���m�F
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

           //"D"�̃L�[�ƒl���폜
	    sio.printf("*** asarr.erase() ***\n");
	    asarr.erase("D");
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

	    sio.printf("*** asarr.erase() ***\n");
           //"C"����2�̂̃L�[�ƒl���폜
	    asarr.erase("C",2);
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());
	    for ( size_t i=0 ; i < asarr.length() ; i++ ) {
	        const char *key = asarr.key(i);
	        sio.printf("%s ... [%s]\n", key, asarr.cstr(key));
	    }

	    sio.printf("*** all asarr.erase() ***\n");
           //�S���ڍ폜
	    asarr.erase();
	    sio.printf("asarr total length ... [%zu]\n", asarr.length());

    }catch(err_rec msg){
        sio.eprintf("[EXCEPTION] function=[%s::%s] message=[%s]\n",
                    msg.class_name, msg.func_name, msg.message);
        return -1;
    }

    return 0;
}

