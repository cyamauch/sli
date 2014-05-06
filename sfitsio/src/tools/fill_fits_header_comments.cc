#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   fill_fits_header_comments.cc
 * @brief  ブランクコメントを SFITSIO の辞書で埋めるツール
 */

/*
 * A code to fill blank comments automatically.
 *
 * ./fill_fits_header_comments [-f] in.fits[.gz or .bz2] out.fits[.gz or .bz2]
 *
 * Use switch `-f' to overwrite all comments with SFITSIO built-in strings.
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    const char *in_file = NULL;
    const char *out_file = NULL;
    bool force_ovwt = false;				/* flag to overwrite */
    long i;

    /* display usage */
    if ( argc == 1 ) {
	sio.printf("fill_header_comments: "
		   "fill blank comments automatically.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s [-f] in.fits[.gz or .bz2] out.fits[.gz or .bz2]\n",
		   argv[0]);
	goto quit;
    }

    /* parse arguments */
    for ( i=1 ; i < argc ; i++ ) {
	if ( argv[i][0] == '-' && argv[i][1] == 'f' ) force_ovwt = true;
	else if ( in_file == NULL ) in_file = argv[i];
	else if ( out_file == NULL ) out_file = argv[i];
    }

    /* reading FITS ... */
    if ( in_file != NULL ) {
	sio.printf("# reading ...\n");
	/* read a FITS file */
	if ( fits.read_stream(in_file) < 0 ) {
	    sio.eprintf("[ERROR] fits.read_stream() failed\n");
	    goto quit;
	}
	sio.printf("# filling comments ...\n");
	/* fill (blank) comments automatically */
	for ( i=0 ; i < fits.length() ; i++ ) {
	    if ( force_ovwt == true ) {
		/* force overwirte all */
		fits.hdu(i).header().assign_default_comments();
	    }
	    else {
		/* fill blank only */
		fits.hdu(i).header().fill_blank_comments();
	    }
	}
    }

    /* write */
    if ( out_file != NULL ) {
	sio.printf("# writing ...\n");
	if ( fits.write_stream(out_file) < 0 ) {
	    sio.eprintf("[ERROR] fits.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}

