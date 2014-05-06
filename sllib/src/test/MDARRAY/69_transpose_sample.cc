/**
  example code
  mdarray¥¯¥é¥¹: mdarray &mdarray::transpose_xy(...)
                 ssize_t mdarray::transpose_xy_copy(...)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {5,4,3,2};
    mdarray myarr(INT_ZT,true,naxisx,4,true);
    mdarray myarr1;
    size_t i,j,k;

    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		myarr.i(k,j,i) = 100*k + 10*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 69_transpose_sample ***\n");

    sio.printf("before:\n");
    sio.printf("dimmension : ");
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

    myarr.transpose_xy();

    sio.printf("after:\n");
    sio.printf("dimmension : ");
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

    myarr.transpose_xy_copy(&myarr1, 1,3, 2,2, 0,1);

    sio.printf("copied:\n");
    sio.printf("dimmension : ");
    for ( i=0 ; i < myarr1.dim_length() ; i++ ) {
	sio.printf("%zd ",myarr1.length(i));
    }
    sio.printf("\n");
    for ( i=0 ; i < myarr1.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr1.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr1.col_length() ; k++ ) {
		sio.printf("%03d ",myarr1.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 69_transpose_sample ***\n");
    sio.printf("\n");

    return 0;
}
