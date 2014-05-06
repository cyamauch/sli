/**
  example code
  mdarray¥¯¥é¥¹: ssize_t mdarray::rotate_xy_copy(...)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {4,3,2};
    mdarray myarr(INT_ZT,true,naxisx,3,true);
    mdarray myarr1;
    size_t i,j,k;

    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		myarr.i(k,j,i) = 100*k + 10*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 70_rotate_sample ***\n");

    sio.printf("before:\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=myarr.row_length() ; 0 < j ; ) {
	    j --;
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.rotate_xy_copy(&myarr1, 90, 0,3, 0,2, 0,1);

    sio.printf("copied (left 90deg):\n");
    for ( i=0 ; i < myarr1.layer_length() ; i++ ) {
	for ( j=myarr1.row_length() ; 0 < j ; ) {
	    j --;
	    for ( k=0 ; k < myarr1.col_length() ; k++ ) {
		sio.printf("%03d ",myarr1.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.rotate_xy_copy(&myarr1, -90, 0,2, 0,2, 0,1);

    sio.printf("copied (right 90deg):\n");
    for ( i=0 ; i < myarr1.layer_length() ; i++ ) {
	for ( j=myarr1.row_length() ; 0 < j ; ) {
	    j --;
	    for ( k=0 ; k < myarr1.col_length() ; k++ ) {
		sio.printf("%03d ",myarr1.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.rotate_xy_copy(&myarr1, 180, 0,3, 0,2, 0,1);

    sio.printf("copied (180deg):\n");
    for ( i=0 ; i < myarr1.layer_length() ; i++ ) {
	for ( j=myarr1.row_length() ; 0 < j ; ) {
	    j --;
	    for ( k=0 ; k < myarr1.col_length() ; k++ ) {
		sio.printf("%03d ",myarr1.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    myarr.rotate_xy_copy(&myarr, 90);

    sio.printf("self (left 90deg):\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=myarr.row_length() ; 0 < j ; ) {
	    j --;
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr.i(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 70_rotate_sample ***\n");
    sio.printf("\n");

    return 0;
}
