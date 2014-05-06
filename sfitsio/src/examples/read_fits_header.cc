#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   read_fits_header.cc
 * @brief  FITS ファイルのヘッダを読み取るコード
 * @attention  速度を求める場合は，tools/hv.cc を参照．
 */

/*
 * A very simple code to read all header in a FITS file.
 *
 * ./read_fits_header in_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    tarray_tstring hdu_str;				/* string array */

    /* set values of string array */
    hdu_str[FITS::IMAGE_HDU]        = "Image";
    hdu_str[FITS::ASCII_TABLE_HDU]  = "Ascii Table";
    hdu_str[FITS::BINARY_TABLE_HDU] = "Binary Table";

    if ( 1 < argc ) {
	ssize_t sz;
	long i;
	const char *in_file = argv[1];

	/* reading file */
	sz = fits.read_stream(in_file);
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.read_stream() failed\n");
	    goto quit;
	}

	/* printing HDU info. */
	for ( i=0 ; i < fits.length() ; i++ ) {		/* loop for each hdu */
	    long j;
	    fits_hdu &c_hdu = fits.hdu(i);

	    /* display HDU name and HDU type */
	    sio.printf("==== HDU : %s (%s) ====\n",
		       fits.hduname(i), hdu_str[fits.hdutype(i)].cstr());

	    /* printing all header records */
	    for ( j=0 ; j < fits.hdu(i).header_length() ; j++ ) {
		if ( c_hdu.header(j).status() == FITS::NORMAL_RECORD ) {
		    sio.printf("[%s] = [%s] / [%s]\n",
			       c_hdu.header(j).keyword(),
			       c_hdu.header(j).value(),
			       c_hdu.header(j).comment());
		}
		else {
		    sio.printf("[%s] [%s]\n",
			       c_hdu.header(j).keyword(),
			       c_hdu.header(j).value());
		}
	    }
	    sio.printf("\n");

	    /* printing IRAF-style overscan section info */
	    j = c_hdu.header_index("BIASSEC");
	    if ( 0 <= j ) {
		long sec_info[4];
		c_hdu.header(j).get_section_info(sec_info,4);
		sio.printf("[OVERSCAN]\n");
		sio.printf("x=%ld len_x=%ld\n", sec_info[0],sec_info[1]);
		sio.printf("y=%ld len_y=%ld\n", sec_info[2],sec_info[3]);
	    }
	}
    }

    return_status = 0;

 quit:
    return return_status;
}

