#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A sample code to read a templete file using the SFITSIO
 *
 */

/* comment strings for blank comment in template */
static const char *Kwd_comment_any[] = {
    /*           |MIN                                       MAX| */
    "TIME-EPH", "epoch time for FITS header",
    "SMPLBASE", "base telemetry referred to shrink rows",
    /*           |MIN                                       MAX| */
    NULL
};
static const char *Kwd_comment_bintable[] = {
    /*           |MIN                                       MAX| */
    "TTNAM#",   "original telemetry name",
    "TCONV#",   "type of data conversion",
    "TSTAT#",   "definition of status values",
    "TINPL#",   "type of interpolation",
    "TSPAN#",   "[s] maximum span for interpolation",
    "", "",
    /*           |MIN                                       MAX| */
    NULL
};

int main( int argc, char *argv[] )
{ 
    int return_status = -1;
    stdstreamio sio;
    fitscc fits;

    /* update comment dictionaries */
    fits::update_comment_dictionary(FITS::ANY_HDU, 
    				    Kwd_comment_any);
    fits::update_comment_dictionary(FITS::BINARY_TABLE_HDU,
				    Kwd_comment_bintable);

    if ( 1 < argc ) {
	const char *templ_filename = argv[1];
	long i;
	/* test reading template */
	if ( fits.read_template(0, templ_filename) < 0 ) {
	    sio.eprintf("[ERROR] fits.read_template() failed\n");
	    goto quit;
	}
	/* test sorting field header */
	//for ( i=0 ; i < fits.length() ; i++ ) {
	//    if ( fits.hdutype(i) == FITS::BINARY_TABLE_HDU ) {
	//	fits.table(i).sort_col_header();
	//    }
	//}
    }

    if ( 2 < argc ) {
	const char *out_filename = argv[2];
	if ( fits.write_stream(out_filename) < 0 ) {
	    sio.eprintf("[ERROR] fits.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}

