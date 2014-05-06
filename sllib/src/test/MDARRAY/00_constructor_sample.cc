/**
  example code
  mdarrayクラス: mdarray()
                 mdarray( ssize_t sz_type, void **extptr_ptr = NULL )
                 mdarray( ssize_t sz_type, const size_t naxisx[], size_t ndim )
                 mdarray( ssize_t sz_type, size_t naxis0 )
                 mdarray( ssize_t sz_type, size_t naxis0, size_t naxis1 )
                 mdarray( ssize_t sz_type, size_t naxis0, size_t naxis1, size_t naxis2 )
                 mdarray( const mdarray &obj )
                 mdarrayオブジェクトの作成(コンストラクター)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray my_mdarr(INT_ZT, true);
    size_t i, j;

    my_mdarr.i(0) = 10;
    my_mdarr.i(1) = 20;
    for ( i=0 ; i < my_mdarr.length() ; i++ ) {
        sio.printf("my_mdarr value(%zu)... [%d]\n", i, my_mdarr.i(i));
    }

    my_mdarr.i(0,1) = 30;
    for ( i=0 ; i < my_mdarr.length(1) ; i++ ) {
	for ( j=0 ; j < my_mdarr.length(0) ; j++ ) {
	    sio.printf("my_mdarr value(%zu,%zu)... [%d]\n", 
		       j, i, my_mdarr.i(j, i));
	}
    }

    return 0;
}

