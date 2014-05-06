/**
  example code
  mdarrayクラス: float *scan_along_x_f(...), 等
                 任意領域のスキャン
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {10,10,10,3,2};
    mdarray myarr(INT_ZT, true);
    size_t i,j,k;

    myarr.resize(nx,5,true);

    for ( i=0 ; i < 10 * 3 * 2 ; i++ ) {
	for ( j=0 ; j < 10 ; j++ ) {
	    for ( k=0 ; k < 10 ; k++ ) {
		myarr.i(k,j,i) = 1000*k + 100*j + i;
	    }
	}
    }

    sio.printf("*** BEGIN: 82_scan_f_sample ***\n");

    //myarr.scan_along_x(&func_1d,(void *)"scan_along_x", 1,5, 6,5, 3,2);
    {
	float *buf;
	size_t n;
	ssize_t x,y,z;
	//myarr.begin_scan_along_x(1,5, 6,5, 3,2);
	//myarr.beginf_scan_along_x("1:5, 6:9, 3:4");
	myarr.beginf_scan_along_x("1:5, 6:9, 3:4, 1:2");
	while ( (buf=myarr.scan_along_x_f(&n,&x,&y,&z)) != NULL ) {
	    sio.printf("%s: (%zd,%zd,%zd): ","scan_along_x",x,y,z);
	    for ( i=0 ; i < n ; i++ ) {
		sio.printf("[%g]",buf[i]);
	    }
	    sio.printf("\n");
	}
    }
    sio.printf("\n");

    //myarr.scan_along_y(&func_1d,(void *)"scan_along_y", 1,5, 6,5, 3,2);
    {
	float *buf;
	size_t n;
	ssize_t x,y,z;
	//myarr.begin_scan_along_y(1,5, 6,5, 3,2);
	//myarr.beginf_scan_along_y("1:5, 6:9, 3:4");
	myarr.beginf_scan_along_y("1:5, 6:9, 3:4, 1:2");
	while ( (buf=myarr.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
	    sio.printf("%s: (%zd,%zd,%zd): ","scan_along_y",x,y,z);
	    for ( i=0 ; i < n ; i++ ) {
		sio.printf("[%g]",buf[i]);
	    }
	    sio.printf("\n");
	}
    }
    sio.printf("\n");

    //myarr.scan_along_z(&func_1d,(void *)"scan_along_z", 1,5, 6,5, 3,2);
    {
	float *buf;
	size_t n;
	ssize_t x,y,z;
	//myarr.begin_scan_along_z(1,5, 6,5, 3,2);
	//myarr.beginf_scan_along_z("1:5, 6:9, 3:4");
	myarr.beginf_scan_along_z("1:5, 6:9, 3:4, 1:2");
	while ( (buf=myarr.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
	    sio.printf("%s: (%zd,%zd,%zd): ","scan_along_z",x,y,z);
	    for ( i=0 ; i < n ; i++ ) {
		sio.printf("[%g]",buf[i]);
	    }
	    sio.printf("\n");
	}
    }
    sio.printf("\n");

/* test result:
scan_xy_planes: (1,6,3): [163][263][363][463][563] [173][273][373][473][573] [183][283][383][483][583] [193][293][393][493][593] 
scan_xy_planes: (1,6,4): [164][264][364][464][564] [174][274][374][474][574] [184][284][384][484][584] [194][294][394][494][594] 
*/
    //myarr.scan_xy_planes(&func_2d,(void *)"scan_xy_planes", 1,5, 6,5, 3,2);
    //sio.printf("\n");

    //myarr.scan_zx_planes(&func_2d,(void *)"scan_zx_planes", 1,5, 6,5, 3,2);
    {
	float *buf;
	size_t n0, n1, ix;
	ssize_t x,y,z;
	//myarr.begin_scan_zx_planes(1,5, 6,5, 3,2);
	//myarr.beginf_scan_zx_planes("1:5, 6:9, 3:4");
	myarr.beginf_scan_zx_planes("1:5, 6:9, 3:4, 1:2");
	while ( (buf=myarr.scan_zx_planes_f(&n0,&n1,&x,&y,&z)) != NULL ) {
	    sio.printf("%s: (%zd,%zd,%zd): ","scan_zx_planes",x,y,z);
	    ix = 0;
	    for ( i=0 ; i < n1 ; i++ ) {
		for ( j=0 ; j < n0 ; j++ ) {
		    sio.printf("[%g]",buf[ix]);
		    ix ++;
		}
		sio.printf(" ");
	    }
	    sio.printf("\n");
	}
    }
    sio.printf("\n");

    //myarr.scan_a_cube(&func_3d,(void *)"scan_a_cube", 1,5, 6,5, 3,2);
    {
	float *buf;
	size_t n0,n1,n2;
	ssize_t x,y,z;
	size_t i,j,k,ix;
	//myarr.begin_scan_a_cube(1,5, 6,5, 3,2);
	//myarr.beginf_scan_a_cube("1:5, 6:9, 3:4");
	myarr.beginf_scan_a_cube("1:5, 6:9, 3:4, 1:2");
	while ( (buf=myarr.scan_a_cube_f(&n0,&n1,&n2,&x,&y,&z)) != NULL ) {
	    sio.printf("%s: (%zd,%zd,%zd): \n","scan_a_cube",x,y,z);
	    ix = 0;
	    for ( i=0 ; i < n2 ; i++ ) {
		for ( j=0 ; j < n1 ; j++ ) {
		    for ( k=0 ; k < n0 ; k++ ) {
			sio.printf("[%g]",buf[ix]);
			ix ++;
		    }
		    sio.printf(" ");
		}
		sio.printf("\n");
	    }
	}
    }
    sio.printf("\n");

    sio.printf("*** END: 82_scan_f_sample ***\n");
    sio.printf("\n");

    return 0;
}

