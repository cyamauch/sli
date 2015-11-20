#include <sli/stdstreamio.h>
#include <sli/tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   dump_fits_table.cc
 * @brief  バイナリテーブルまたはASCIIテーブルをテキストで出力
 * @attention  可変長配列には対応していない．
 */

/*
 * A sample code to dump a FITS binary or ascii table.
 * (variable length is not supported)
 *
 * ./dump_fits_table in_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_value = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */

    /* reading a FITS file */
    if ( 1 < argc ) {
	const char *path = argv[1];
        if ( fits.read_stream(path) < 0 ) {
	    sio.eprintf("[ERROR] cannot open: %s\n",path);
	    goto quit;
	}
    }

    /* dump or display HDU info */
    if ( 2 < argc ) {

	tstring s;
	long i, idx;

	/* obtain index of HDU */
	idx = s.assign(argv[2]).atol();

	/* dump */
	if ( fits.hdutype(idx) == FITS::BINARY_TABLE_HDU || 
	     fits.hdutype(idx) == FITS::ASCII_TABLE_HDU ) {

	    /* create alias */
	    fits_table &bte = fits.table(idx);

	    /* loop of rows */
	    for ( i=0 ; i < bte.row_length() ; i++ ) {
		long j;
		/* loop of columns (fields) */
		for ( j=0 ; j < bte.col_length() ; j++ ) {
		    long k, n;
		    if ( 0 < j ) sio.putchr(',');
		    n = bte.col(j).elem_length();
		    /* loop of elements */
		    if ( 0 < n ) {
			for ( k=0 ; k < n ; k++ ) {
			    if ( 0 < k ) sio.putchr(',');
			    sio.printf("%s",bte.col(j).svalue(i,k));
			}
		    }
		    else {
			sio.printf("%s",bte.col(j).svalue(i));
		    }
		}
		sio.printf("\n");
	    }
	}

    }
    else {

	/* display HDU info of FITS file */
	long i;

	/* eprintf() is to output a string to STDERR */
	sio.eprintf("[INFO] Specify index.\n");

	for ( i=0 ; i < fits.length() ; i++ ) {
	    if ( fits.hdutype(i) == FITS::BINARY_TABLE_HDU ||
		 fits.hdutype(i) == FITS::ASCII_TABLE_HDU ) {
		sio.eprintf("Index:%ld  Name:%s\n",
			    i,fits.hduname(i));
	    }
	}

    }

    return_value = 0;
 quit:
    return return_value;
}
