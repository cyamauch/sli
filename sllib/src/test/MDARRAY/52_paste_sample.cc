/**
  example code
  mdarray¥¯¥é¥¹: 

    virtual mdarray &paste( const mdarray &src,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );
    virtual mdarray &paste_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,bool,void *), void *user_ptr,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    virtual mdarray &paste_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    virtual mdarray &paste_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

void my_func(double self[], double src[], size_t n, 
	     ssize_t x, ssize_t y, ssize_t z, mdarray *myptr, void *p)
{
    size_t i;
    for ( i=0 ; i < n ; i++ ) self[i] += src[i] + 50;
    return;
}

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx0[] = {4,4};
    mdarray my_fmdarr(FLOAT_ZT, true);
    my_fmdarr.resize(nx0,2,true);
    float my_float[] = {101, 102, 103, 104,
			201, 202, 203, 204,
			301, 302, 303, 304,
			401, 402, 403, 404};
    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    for (size_t j = 0; j < my_fmdarr.length(1); j++){
	for (size_t i = 0; i < my_fmdarr.length(0); i++){
	    sio.printf("[%g]", my_fmdarr.dvalue(i, j));
	}
	sio.printf("\n");
    }

    size_t nx1[] = {2,2};
    mdarray mypaste_mdarr(FLOAT_ZT, true);
    mypaste_mdarr.resize(nx1,2,true);
    float mypaste_float[] = {1000, 2000, 3000, 4000};
    mypaste_mdarr.putdata((const void *)mypaste_float, sizeof(mypaste_float));

    sio.printf(".paste(), .paste_via_udf()\n");
    my_fmdarr.paste(mypaste_mdarr);
    my_fmdarr.paste_via_udf(mypaste_mdarr, &my_func, NULL, 1,2);
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
	sio.printf("=> ");
	for (size_t i = 0; i < my_fmdarr.length(0); i++){
	    sio.printf("[%g]",  my_fmdarr.dvalue(i, j));
	}
	sio.printf("\n");
    }

    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    sio.printf(".pastef()\n");
    my_fmdarr.pastef(mypaste_mdarr, "1:*,2:3");
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
	sio.printf("=> ");
	for (size_t i = 0; i < my_fmdarr.length(0); i++){
	    sio.printf("[%g]",  my_fmdarr.dvalue(i, j));
	}
	sio.printf("\n");
    }

    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    sio.printf(".pastef(self)\n");
    my_fmdarr.pastef(my_fmdarr, "2:*,1:*");
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
	sio.printf("=> ");
	for (size_t i = 0; i < my_fmdarr.length(0); i++){
	    sio.printf("[%g]",  my_fmdarr.dvalue(i, j));
	}
	sio.printf("\n");
    }

    my_fmdarr.putdata((const void *)my_float, sizeof(my_float));

    sio.printf(".pastef(self)\n");
    my_fmdarr.pastef(my_fmdarr, "-1:*,-1:*");
    for (size_t j = 0; j < my_fmdarr.length(1); j++){
	sio.printf("=> ");
	for (size_t i = 0; i < my_fmdarr.length(0); i++){
	    sio.printf("[%g]",  my_fmdarr.dvalue(i, j));
	}
	sio.printf("\n");
    }

    return 0;
}
