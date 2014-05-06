#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   read_and_write_fits.cc
 * @brief  FITS ファイルの読み書きを行なう最も基本的なコード
 */

/*
 * A sample code to simply read a FITS file and write it in another file name.
 *
 * ./read_and_write_fits in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
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
	long i;
	const char *in_file = argv[1];
	/* reading ... */
	sz = fits.read_stream(in_file);
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.read_stream() failed\n");
	    goto quit;
	}
	/* printing HDU info. */
	for ( i=0 ; i < fits.length() ; i++ ) {
	    sio.printf("HDU %ld : hdutype = %d\n",i,fits.hdutype(i));
	    sio.printf("HDU %ld : hduname = %s\n",i,fits.hduname(i));
	    sio.printf("HDU %ld : hduver  = %lld\n",i,fits.hduver(i));
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

