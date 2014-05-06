/**
  example code
  mdarrayクラス: mdarray &mdarray::flip_cols(...)
                 任意次元での要素の反転
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

#define LEN_ELM 8

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {LEN_ELM, LEN_ELM, LEN_ELM};
    mdarray myarr(INT_ZT, true);
    size_t i,j,k;

    myarr.resize(nx,3,true);

    for ( i=0 ; i < LEN_ELM ; i++ ) {
	for ( j=0 ; j < LEN_ELM ; j++ ) {
	    for ( k=0 ; k < LEN_ELM ; k++ ) {
		myarr.i(k,j,i) = 100*k + 10*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 64_flip_sample ***\n");

    myarr.flip(0, 2,3);
    myarr.flip(1, -2,4);
    myarr.flip(2, 5,10);

    for ( i=0 ; i < LEN_ELM ; i++ ) {
	for ( j=0 ; j < LEN_ELM ; j++ ) {
	    for ( k=0 ; k < LEN_ELM ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 64_flip_sample ***\n");
    sio.printf("\n");

    return 0;
}
