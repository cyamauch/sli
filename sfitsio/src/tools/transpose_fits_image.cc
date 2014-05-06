#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   transpose_fits_image.cc
 * @brief  FITS 画像の (x,y) を入れ替えるツール
 */

/*
 * A sample code to write xy-transposed FITS file.
 *
 * ./transpose_fits_image in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */

    /* reading a FITS file */
    if ( 1 < argc ) {
	ssize_t sz;
	const char *in_file = argv[1];
	/* reading ... */
	sz = fits.read_stream(in_file);
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.read_stream() failed\n");
	    goto quit;
	}
	/* xy-transpose */
	if ( 0 < fits.length() ) {
	    fits.image("Primary").transpose_xy();
	}
    }

    /* writing a FITS file */
    if ( 2 < argc ) {
	const char *out_file = argv[2];
	ssize_t sz;
	/* writing ... */
	sz = fits.write_stream(out_file);
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;

 quit:
    return return_status;
}

