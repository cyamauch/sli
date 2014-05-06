/**
  example code
  mdarray¥¯¥é¥¹: complex functions
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_complex.h>
#include <math.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t naxisx[] = {5,4};
    mdarray myarr(DCOMPLEX_ZT, true);
    mdarray myarr1;
    size_t j,k;

    myarr.reallocate(naxisx,2,true);

    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    myarr.dx(k,j) = (k+1) + (j+1) * I;
	}
    }

    sio.printf("*** BEGIN: 74_complex_sample ***\n");

    sio.printf("before:\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    dcomplex v = myarr.dx(k,j);
	    sio.printf("(%.10g,%.10gi) ",creal(v), cimag(v));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    myarr1 = cabs(myarr);

    sio.printf("myarr1 = cabs(myarr)\n");
    for ( j=0 ; j < myarr1.row_length() ; j++ ) {
	for ( k=0 ; k < myarr1.col_length() ; k++ ) {
	    sio.printf("%.10g ",myarr1.dvalue(k,j));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    sio.printf("reference: csin(myarr)\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    dcomplex v = csin(myarr.dx(k,j));
	    sio.printf("(%.10g,%.10gi) ",creal(v), cimag(v));
	}
	sio.printf("\n");
    }
    sio.printf("\n");

    myarr = csin(myarr);

    sio.printf("myarr = csin(myarr)\n");
    for ( j=0 ; j < myarr.row_length() ; j++ ) {
	for ( k=0 ; k < myarr.col_length() ; k++ ) {
	    dcomplex v = myarr.dx(k,j);
	    sio.printf("(%.10g,%.10gi) ",creal(v), cimag(v));
	}
	sio.printf("\n");
    }
    sio.printf("\n");


    sio.printf("*** END: 74_complex_sample ***\n");
    sio.printf("\n");

    return 0;
}
