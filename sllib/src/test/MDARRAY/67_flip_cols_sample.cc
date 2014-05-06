/**
  example code
  mdarrayクラス: mdarray &mdarray::flip_cols(...)
                 mdarray &mdarray::flip_rows(...)
                 画像の一部分の反転．
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {4,3,2};
    mdarray myarr(INT_ZT,true,naxisx,3,true);
    size_t i,j,k;

    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		myarr.i(k,j,i) = 100*k + 10*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 67_flip_cols_sample ***\n");

    sio.printf("before:\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.flip_cols(1,3, 0,2, 0,1);
    myarr.flip_rows(1,3, 0,2, 1,1);

    sio.printf("after:\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 67_flip_cols_sample ***\n");
    sio.printf("\n");

    return 0;
}
