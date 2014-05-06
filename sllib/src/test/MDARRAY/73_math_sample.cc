/**
  example code
  mdarray¥¯¥é¥¹: math functions
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_math.h>
#include <math.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {6,2};
    mdarray myarr(DOUBLE_ZT,true,naxisx,2,true);
    size_t naxisx1[] = {8,3};
    mdarray myarr1(INT_ZT,true,naxisx1,2,true);
    size_t j,k;

    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    myarr.d(k,j) = 10*j + k;
	}
    }
    for ( j=0 ; j < myarr1.row_length() ; j++ ) {
	for ( k=0 ; k < myarr1.col_length() ; k++ ) {
	    myarr1.i(k,j) = 10*j + k;
	}
    }

    sio.printf("*** BEGIN: 73_math_sample ***\n");

    sio.printf("before:\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%.10g ",myarr.dvalue(k,j));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    myarr = pow(myarr,2.0);

    sio.printf("myarr = pow(myarr,2.0)\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%.10g ",myarr.dvalue(k,j));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    myarr = sqrt(myarr);

    sio.printf("myarr = sqrt(myarr)\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%.10g ",myarr.dvalue(k,j));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    sio.printf("reference: hypot(myarr, myarr1)\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%.10g ",hypot(myarr.dvalue(k,j),myarr1.dvalue(k,j)));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    myarr = hypot(myarr, myarr1);

    sio.printf("myarr = hypot(myarr, myarr1)\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    sio.printf("%.10g ",myarr.dvalue(k,j));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    sio.printf("*** END: 73_math_sample ***\n");
    sio.printf("\n");

    return 0;
}
