#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_math.h>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
using namespace sli;

const size_t Width = 4096*2;
const size_t Height = 4096*3;

//static ssize_t scan_1d( double v[], size_t n, ssize_t x, ssize_t y, ssize_t z,
//			const mdarray_double *thisp, void *u_p )
//{
//    return n;
//}

//#define TYPE_TARGET double
//#define TYPE_MD_TARGET mdarray_double

#define TYPE_TARGET float
#define TYPE_MD_TARGET mdarray_float

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    TYPE_TARGET *arr0_ptr;
    TYPE_TARGET *arr1_ptr;
    TYPE_TARGET *const *arr0_ptr2d;
    TYPE_TARGET *const *arr1_ptr2d;
    TYPE_MD_TARGET arr0, arr1;
    double tm0, tm1;
    size_t i,j;
    long ii;

    srand48(12345);

    sio.printf("******** array length = %zu x %zu, size_type = %zd ********\n",
	       Width, Height, arr0.size_type());

    arr0.register_extptr(&arr0_ptr);
    arr0.register_extptr_2d(&arr0_ptr2d);
    arr0.set_auto_resize(false);

    arr1.register_extptr(&arr1_ptr);
    arr1.register_extptr_2d(&arr1_ptr2d);

    sio.printf("******** machine performance test (counter) ********\n");

    {
	std::vector<double> varr;
	varr.resize(Width*Height);
	varr.assign(Width*Height,0.0);

	tm0 = clock() / (double)CLOCKS_PER_SEC;
	varr.assign(Width*Height,1.0);
	tm1 = clock() / (double)CLOCKS_PER_SEC;
	sio.printf("std::vector varr.assign(length,0.0): %g\n",tm1 - tm0);
    }

    arr0.resize_2d(Width,Height);

    memset((void *)arr0.array_ptr(), 0, arr0.byte_length());

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    memset((void *)arr0.array_ptr(), 1, arr0.byte_length());
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("memset(): %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    sio.printf("******** test of basic functions ********\n");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.clean();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.clean(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.fill(123.4);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.fill(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = 12.345;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = scalar: %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr1 = arr0;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr1 = arr0: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 += 34.7;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 += scalar: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 -= 3.59;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 -= scalar: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 *= 1.23;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 *= scalar: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 /= 23.45;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 /= scalar: %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 + 34.7;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 + scalar: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 - 3.59;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 - scalar: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 * 1.23;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 * scalar: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 / 23.45;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 / scalar: %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 += arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 += arr1: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 -= arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 -= arr1: %g\n",tm1 - tm0);
    
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 *= arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 *= arr1: %g\n",tm1 - tm0);
    
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 /= arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 /= arr1: %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();
    
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 + arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 + arr1: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 - arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 - arr1: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 * arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 * arr1: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 = arr0 / arr1;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 = arr0 / arr1: %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.add(34.7);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.add(scalar): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.subtract(1.234);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.subtract(scalar): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.multiply(1.342);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.multiply(scalar): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.divide(1.74);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.divide(scalar): %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0 += arr0;
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0 += arr0: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.addf(34.1234,"*");
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.addf(scalar,\"*\"): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.addf(arr1,"*");
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.addf(arr1,\"*\"): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.pastef(arr1,"*");
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.pastef(arr1,\"*\"): %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.flip_rows();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.flip_rows(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.flip_cols();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.flip_cols(): %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.begin_scan_along_x();
    {
	size_t n;
	ssize_t x,y,z;
	j = 0;
	while ( arr0.scan_along_x(&n,&x,&y,&z) != NULL ) {
	    //sio.printf("n,x,y,z = %zu,%zd,%zd,%zd\n",n,x,y,z);
	    j++;
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.scan_along_x() cnt:%zu ... : %g\n",j, tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.begin_scan_along_y();
    {
	size_t n;
	ssize_t x,y,z;
	j = 0;
	while ( arr0.scan_along_y(&n,&x,&y,&z) != NULL ) {
	    //sio.printf("n,x,y,z = %zu,%zd,%zd,%zd\n",n,x,y,z);
	    j++;
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.scan_along_y() cnt:%zu ... : %g\n",j, tm1 - tm0);

    //tm0 = clock() / (double)CLOCKS_PER_SEC;
    //arr0.scan_along_x(&scan_1d, NULL);
    //tm1 = clock() / (double)CLOCKS_PER_SEC;
    //sio.printf("arr0.scan_along_x() [old] cnt:%zu ... : %g\n",j, tm1 - tm0);

    //tm0 = clock() / (double)CLOCKS_PER_SEC;
    //arr0.scan_along_y(&scan_1d, NULL);
    //tm1 = clock() / (double)CLOCKS_PER_SEC;
    //sio.printf("arr0.scan_along_y() [old] cnt:%zu ... : %g\n",j, tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.reverse_endian(false);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.reverse_endian(false): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.transpose_xy();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.transpose_xy(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.rotate_xy(90);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.rotate_xy(90): %g\n",tm1 - tm0);

    arr1.init();
    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.copy(&arr1);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.copy(&arr1) [1st]: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.copy(&arr1);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.copy(&arr1) [2nd]: %g\n",tm1 - tm0);

    sio.printf("******** test of transpose using direct 2d ptr ********\n");

    if ( arr0.col_length() == arr1.row_length() ) {
	const size_t len_sq = arr0.col_length();
	arr1 = arr0;
	tm0 = clock() / (double)CLOCKS_PER_SEC;
	for ( i=0 ; i < len_sq ; i++ ) {
	    for ( j=0 ; j < len_sq ; j++ ) {
		arr1_ptr2d[i][j] = arr0_ptr2d[j][i];
	    }
	}
	tm1 = clock() / (double)CLOCKS_PER_SEC;
	sio.printf("direct 2d ptr transpose [hscan <= vscan]: %g\n",tm1 - tm0);

	tm0 = clock() / (double)CLOCKS_PER_SEC;
	for ( i=0 ; i < len_sq ; i++ ) {
	    for ( j=0 ; j < len_sq ; j++ ) {
		arr1_ptr2d[j][i] = arr0_ptr2d[i][j];
	    }
	}
	tm1 = clock() / (double)CLOCKS_PER_SEC;
	sio.printf("direct 2d ptr transpose [vscan <= hscan]: %g\n",tm1 - tm0);
    }

    sio.printf("******** test of math functions ********\n");

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.abs();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.abs() %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.floor();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.floor() %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.ceil();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.ceil() %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr0.round();
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr0.round() %g\n",tm1 - tm0);
    

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr1 = sin(arr0);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr1 = sin(arr0): %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr1 = sqrt(arr0);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr1 = sqrt(arr0): %g\n",tm1 - tm0);

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    arr1 = log(arr0);
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("arr1 = log(arr0): %g\n",tm1 - tm0);

#if 0
    sio.printf("******** test of 2d ptr direct write: hscan v.s. vscan ********\n");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
	for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0_ptr2d[i][j] = ii;
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 2d ptr access: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	for ( i=0 ; i < arr0.row_length() ; i++ ) {
	    arr0_ptr2d[i][j] = ii;
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 2d ptr access: %g\n",tm1 - tm0);
#endif

#if 0
    sio.printf("******** test of 2d ptr direct read: hscan v.s. vscan ********\n");

    for ( j=0 ; j < arr0.length() ; j++ ) arr0_ptr[j] = drand48();

    {
	double sum;
	tm0 = clock() / (double)CLOCKS_PER_SEC;
	sum = 0;
	for ( i=0 ; i < arr0.row_length() ; i++ ) {
	    for ( j=0 ; j < arr0.col_length() ; j++ ) {
		sum += arr0_ptr2d[i][j];
	    }
	}
	tm1 = clock() / (double)CLOCKS_PER_SEC;
	sio.printf("direct 2d ptr read: hscan: %g\n",tm1 - tm0);

	tm0 = clock() / (double)CLOCKS_PER_SEC;
	sum = 0;
	for ( j=0 ; j < arr0.col_length() ; j++ ) {
	    for ( i=0 ; i < arr0.row_length() ; i++ ) {
		sum += arr0_ptr2d[i][j];
	    }
	}
	tm1 = clock() / (double)CLOCKS_PER_SEC;
	sio.printf("direct 2d ptr read: vscan: %g\n",tm1 - tm0);
    }
#endif

    sio.printf("******** test of ptr direct access v.s. operators (1) ********\n");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.length() ; j++, ii++ ) {
	arr0_ptr[j] = ii;
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 1d ptr access: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
	for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0_ptr2d[i][j] = ii;
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 2d ptr access: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.length() ; j++, ii++ ) {
	arr0[j] = ii;
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("access by arr0[x]: %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
      for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0(j,i) = ii;
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("access by arr0(x,y): %g\n",tm1 - tm0);

    sio.printf("******** test of ptr direct access v.s. operators (2) ********\n");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.length() ; j++, ii++ ) {
	arr0_ptr[j] = sin((double)ii);
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 1d ptr access using sin(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
	for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0_ptr2d[i][j] = sin((double)ii);
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 2d ptr access using sin(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.length() ; j++, ii++ ) {
	arr0[j] = sin((double)ii);
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("access by arr0[x] using sin(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
	for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0(j,i) = sin((double)ii);
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("access by arr0(x,y) using sin(): %g\n",tm1 - tm0);

    sio.printf("******** test of ptr direct access v.s. operators (3) ********\n");

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.length() ; j++, ii++ ) {
	arr0_ptr[j] = sin((double)ii) + sqrt((double)ii);
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 1d ptr access using sin()+sqrt(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
      for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0_ptr2d[i][j] = sin((double)ii) + sqrt((double)ii);
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("direct 2d ptr access using sin()+sqrt(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( j=0 ; j < arr0.length() ; j++, ii++ ) {
	arr0[j] = sin((double)ii) + sqrt((double)ii);
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("access by arr0[x] using sin()+sqrt(): %g\n",tm1 - tm0);

    tm0 = clock() / (double)CLOCKS_PER_SEC;
    ii = 0;
    for ( i=0 ; i < arr0.row_length() ; i++ ) {
	for ( j=0 ; j < arr0.col_length() ; j++, ii++ ) {
	    arr0(j,i) = sin((double)ii) + sqrt((double)ii);
	}
    }
    tm1 = clock() / (double)CLOCKS_PER_SEC;
    sio.printf("access by arr0(x,y) using sin()+sqrt(): %g\n",tm1 - tm0);


    return 0;
}
