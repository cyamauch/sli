/**
  example code
  mdarray¥¯¥é¥¹: 

    virtual mdarray &flipf( const char *exp_fmt, ... );
    virtual mdarray &vflipf( const char *exp_fmt, va_list ap );

*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx0[] = {5,4,3,2};
    mdarray arr0(DOUBLE_ZT, true);
    size_t i,j,k,l;

    sio.printf("*** BEGIN: 78_flipf_sample ***\n");

    arr0.resize(nx0, 4, true);

    /* set arr0 */
    for ( i=0 ; i < arr0.length(3) ; i++ ) {
	for ( j=0 ; j < arr0.length(2) ; j++ ) {
	    for ( k=0 ; k < arr0.length(1) ; k++ ) {
		for ( l=0 ; l < arr0.length(0) ; l++ ) {
		    arr0.assign(90000 + i*1000 + j*100 + k*10 + l,
				l,k,arr0.length(2) * i + j);
		}
	    }
	}
    }

    sio.printf("before\n");
    for ( i=0 ; i < arr0.length(3) ; i++ ) {
	for ( j=0 ; j < arr0.length(2) ; j++ ) {
	    sio.printf(" Z,z = %zu,%zu\n",i,j);
	    for ( k=0 ; k < arr0.length(1) ; k++ ) {
		sio.printf(" ");
		for ( l=0 ; l < arr0.length(0) ; l++ ) {
		    sio.printf("[%g]", arr0.dvalue(
			       l,k,arr0.length(2) * i + j));
		}
		sio.printf("\n");
	    }
	}
    }

    arr0.flipf("2:0,*,-*,*");

    sio.printf("after\n");
    for ( i=0 ; i < arr0.length(3) ; i++ ) {
	for ( j=0 ; j < arr0.length(2) ; j++ ) {
	    sio.printf(" Z,z = %zu,%zu\n",i,j);
	    for ( k=0 ; k < arr0.length(1) ; k++ ) {
		sio.printf(" ");
		for ( l=0 ; l < arr0.length(0) ; l++ ) {
		    sio.printf("[%g]", arr0.dvalue(
			       l,k,arr0.length(2) * i + j));
		}
		sio.printf("\n");
	    }
	}
    }

    sio.printf("*** END: 78_flipf_sample ***\n");

    return 0;
}

