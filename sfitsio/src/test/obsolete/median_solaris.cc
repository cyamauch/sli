#include <sli/tarray_tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A code to test median function for specitic pattern
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    double stat_results[8];
    int i;

    fits.append_image("Primary", FITS::INDEF,		/* EXTNAME, EXTVER */
		      FITS::SHORT_T, 3000, 1000 );
    fits::short_t *p= (fits::short_t *)fits.image(0L).data_ptr();
    fits.image("Primary").assign_blank(-32768);

    /*
     * 昔の solaris の qsort() はこのパターンでハマってたらしい…
     * メディアン値は 1．すぐ返ってこないようだとマズい．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 3;
    }

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * 別のパターン [1] でテスト: メディアン値は平均値になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 2;
    }

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * 別のパターン [2] でテスト: メディアン値は 1 になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 2;
    }
    p[0] = -32768;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * 別のパターン [3] でテスト: メディアン値は 0 になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 2;
    }
    p[1] = -32768;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);


    /*
     * 別のパターン [4] でテスト: メディアン値は 1.5 になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 3;
    p[1] = 0;
    p[2] = 2;
    p[3] = 1;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * 別のパターン [5] でテスト: メディアン値は 1 になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 2;
    p[1] = 0;
    p[2] = 1;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);


    /*
     * 別のパターン [6] でテスト: メディアン値は 0.5 になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 1;
    p[1] = 0;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * 別のパターン [7] でテスト: メディアン値は 0 になるべき．
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 0;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);


    return_status = 0;

 quit:
    return return_status;
}
