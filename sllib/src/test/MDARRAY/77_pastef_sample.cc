/**
  example code
  mdarray¥¯¥é¥¹: 

    virtual mdarray &pastef( const mdarray &src,
			     const char *exp_fmt, ... );

    virtual mdarray &pastef_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,bool,void *), void *user_ptr,
	  const char *exp_fmt, ... );

    virtual mdarray &pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
        const char *exp_fmt, ... );

    virtual mdarray &pastef_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
        const char *exp_fmt, ... );

*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

void my_func(double self[], double src[], size_t n, 
	     ssize_t x, ssize_t y, ssize_t z, mdarray *myptr, void *p)
{
    stdstreamio sio;
    size_t i;
    sio.printf("x,y,z = %zd,%zd,%zd\n",x,y,z);
    for ( i=0 ; i < n ; i++ ) self[i] = self[i] - src[i];
    return;
}

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx0[] = {5,4,3,2};
    mdarray arr0(DOUBLE_ZT, true);
    mdarray arr1(INT_ZT, true);
    size_t i,j,k,l;

    sio.printf("*** BEGIN: 77_pastef_sample ***\n");

    arr0.resize(nx0,4,true);
    arr1.resize_3d(4,3,2).resize(3,2);

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

    for ( i=0 ; i < arr1.length(2) ; i++ ) {
	for ( j=0 ; j < arr1.length(1) ; j++ ) {
	    for ( k=0 ; k < arr1.length(0) ; k++ ) {
		double v = 40000 + i*100 + j*10 + k;
		arr1.assign(v, k,j,i);
	    }
	}
    }

    arr0.pastef(arr1, "2:*,*,1:*");

    sio.printf("after .pastef(arr1, ...)\n");
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

    arr0.pastef_via_udf(arr1, &my_func, NULL, "-1:*,*,1:*");

    sio.printf("after .pastef(arr1, &my_func, ...)\n");
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

    sio.printf("*** END: 77_pastef_sample ***\n");

    return 0;
}
