/**
  example code
  mdarrayクラス: mdarray &convert( ssize_t sz_type )
		 mdarray &convert_via_udf( ssize_t sz_type,
			      	   void (*func)(const void *,void *,void *),
			      	   void *user_ptr )
                 型変換
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_fmdarr(DOUBLE_ZT, true);

    my_fmdarr.d(0) = 1000.1;
    my_fmdarr.d(1) = -2000.6;
    my_fmdarr.d(2) = 3000.7;
    my_fmdarr.d(3) = -4000.2;

    sio.printf("my_fmdarr type... [%zd]\n", my_fmdarr.size_type());
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("my_fmdarr value(%zu)... [%g]\n", i, my_fmdarr.d(i));
    }

    my_fmdarr.convert(INT_ZT);
    //convert後の値を出力
    sio.printf("=> my_fmdarr type... [%zd]\n", my_fmdarr.size_type());
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("=> my_fmdarr value(%zu)... [%d]\n", i, my_fmdarr.i(i));
    }

    my_fmdarr.convert(LLONG_ZT);
    //convert後の値を出力
    sio.printf("==> my_fmdarr type... [%zd]\n", my_fmdarr.size_type());
    for (size_t i = 0; i < my_fmdarr.length(); i++){
        sio.printf("==> my_fmdarr value(%zu)... [%lld]\n", i, my_fmdarr.ll(i));
    }

    return 0;
}
