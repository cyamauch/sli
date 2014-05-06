/**
  example code
  mdarrayクラス: mdarray &mdarray::trim(...)
                 画像の一部分のトリミング．
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {4,3,2,1};
    mdarray myarr(INT_ZT,true,naxisx,4,true);
    size_t i,j,k;

    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		myarr.i(k,j,i) = 100*k + 10*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 66_trim_sample ***\n");

    sio.printf("before:\n");
    sio.printf("dim_length = ");
    for ( i=0 ; i < myarr.dim_length() ; i++ ) {
	sio.printf("%zd ",myarr.length(i));
    }
    sio.printf("\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.trim(1,3, 0,3);

    sio.printf("after (1):\n");
    sio.printf("dim_length = ");
    for ( i=0 ; i < myarr.dim_length() ; i++ ) {
	sio.printf("%zd ",myarr.length(i));
    }
    sio.printf("\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.trim(0,3, 0,2, 1,1);

    sio.printf("after (2):\n");
    sio.printf("dim_length = ");
    for ( i=0 ; i < myarr.dim_length() ; i++ ) {
	sio.printf("%zd ",myarr.length(i));
    }
    sio.printf("\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 66_trim_sample ***\n");
    sio.printf("\n");

    return 0;
}
