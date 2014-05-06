/**
  example code
  mdarray¥¯¥é¥¹: int *const *mdarray_int::array_ptr_2d(...)
                 int *const *const *mdarray_int::array_ptr_3d(...)
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {5,4,3};
    size_t new_naxisx[] = {5,4,3};
    mdarray_int myarr(false,naxisx,3,true);
    int *const *ptr_2d;
    int *const *const *ptr_3d;
    size_t i,j,k;

    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		myarr(k,j,i) = 100*k + 10*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 71_array_ptr_2d_sample ***\n");

    sio.printf("normal access:\n");
    for ( i=0 ; i < myarr.layer_length() ; i++ ) {
	for ( j=0 ; j < myarr.row_length() ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",myarr(k,j,i));
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("direct access via ptr_2d:\n");
    ptr_2d = myarr.array_ptr_2d(true);
    for ( j=0 ; ptr_2d[j] != NULL ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%03d ",ptr_2d[j][k]);
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    sio.printf("direct access via ptr_3d:\n");
    ptr_3d = myarr.array_ptr_3d(true);
    for ( i=0 ; ptr_3d[i] != NULL ; i++ ) {
	for ( j=0 ; ptr_3d[i][j] != NULL ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",ptr_3d[i][j][k]);
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    new_naxisx[0] = 7;
    new_naxisx[1] = 5;
    new_naxisx[2] = 2;
    myarr.resize(new_naxisx, 3, true);


    sio.printf("direct access via ptr_2d (after resize):\n");
    ptr_2d = myarr.array_ptr_2d(true);
    for ( j=0 ; ptr_2d[j] != NULL ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%03d ",ptr_2d[j][k]);
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    sio.printf("direct access via ptr_3d (after resize):\n");
    ptr_3d = myarr.array_ptr_3d(true);
    for ( i=0 ; ptr_3d[i] != NULL ; i++ ) {
	for ( j=0 ; ptr_3d[i][j] != NULL ; j++ ) {
	    for ( k=0 ; k < myarr.col_length() ; k++ ) {
		sio.printf("%03d ",ptr_3d[i][j][k]);
	    }
	    sio.printf("\n");
	}
	sio.printf("\n");
    }

    sio.printf("*** END: 71_array_ptr_2d_sample ***\n");
    sio.printf("\n");

    return 0;
}
