#include <sli/stdstreamio.h>
#include <sli/tstring.h>
#include <sli/fitscc.h>
#include <libwcs/wcs.h>
#include <math.h>
using namespace sli;

/*
 *  A sample code to display position of real coordinate using libwcs.
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    struct WorldCoor *wcs = NULL;
    stdstreamio sio;

    if ( 3 < argc ) {

	fitscc fits;
	double lon,lat, x,y, v;
	int off;

	const char *in_file = argv[1];
	tstring pix_x = argv[2];
	tstring pix_y = argv[3];

	/* read all data from fits file */
	if ( fits.read_stream(in_file) < 0 ) {
	    sio.eprintf("[ERROR] read_stream() failed\n");
	    goto quit;
	}

	/* report number of HDUs */
	sio.printf("Numer of HDU = %ld\n",fits.length());

	/* create alias "pri" to Primary HDU */
	fits_image &pri = fits.image("Primary");

	/* report size of image */
	sio.printf("Size of image = %ldx%ld\n",
		   pri.col_length(), pri.row_length());

	/* initialize wcs structure */
	wcs = wcsinitn(pri.header_formatted_string(), NULL);

	/* convert pix -> wcs */
	x = pix_x.atof();  y = pix_y.atof();
	pix2wcs(wcs, x, y, &lon, &lat);
	sio.printf("ra=%.8f dec=%.8f\n",lon,lat);

	/* convert wcs -> pix */
	wcs2pix(wcs, lon, lat, &x, &y, &off);
	sio.printf("x=%.8f y=%.8f\n",x,y);

	/* read value of pixel (1-indexed) */
	v = pri.dvalue((long)floor(x-0.5),(long)floor(y-0.5));
	sio.printf("value=%.15g\n",v);

    }
    else {
	sio.eprintf("[USAGE]\n");
	sio.eprintf("$ %s in.fits[.gz][.bz2] x y\n",argv[0]);
	sio.eprintf("Note: pixel coordinate (x,y) is 1-indexed\n");
    }

    return_status = 0;
 quit:
    /* free wcs structure */
    if ( wcs != NULL ) wcsfree(wcs);
    return return_status;
}

