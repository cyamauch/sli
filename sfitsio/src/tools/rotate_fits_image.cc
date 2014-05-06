#include <sli/fitscc.h>
#include <sli/stdstreamio.h>
#include <stdlib.h>
using namespace sli;

/**
 * @file   rotate_fits_image.cc
 * @brief  FITS 画像の回転(90度単位)を行なうツール
 */

/*
 * A code to rotate FITS image.
 *
 * [example]
 * ./rotate_fits_image angle in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
 *
 *          angle: 90,-90, or 180
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    int angle = 0;
    stdstreamio sio;					/* standard I/O */
    fitscc obj;						/* FITS object  */

    /* display usage */
    if ( argc == 1 ) {
	sio.printf("rotate: rotate Primary image.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s angle in.fits[.gz or .bz2] out.fits[.gz or .bz2]\n",
		   argv[0]);
	sio.printf("[ANGLE]\n");
	sio.printf("90, -90, or 180\n");
	goto quit;
    }

    /* obtain angle */
    if ( 1 < argc ) {
	angle = atoi(argv[1]);
    }

    /* read a FITS file and rotate */
    if ( 2 < argc ) {
	const char *in_file = argv[2];
	ssize_t sz;
	sio.printf("# reading ...\n");
	sz = obj.read_stream(in_file);      /* reading file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.read_stream() failed\n");
	    goto quit;
	}
	sio.printf("# performing rotation ...\n");
	/* rotate */
	if ( 0 < obj.length() ) {
	    obj.image("Primary").rotate_xy(angle);
	}
    }

    /* write a FITS file */
    if ( 3 < argc ) {
	const char *out_file = argv[3];
	ssize_t sz;
	sio.printf("# writing ...\n");
	sz = obj.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;

 quit:
    return return_status;
}
