#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   create_fits_asciitable.cc
 * @brief  ASCIIテーブルを含む新規 FITS ファイルを作る例
 */

/*
 * A sample code to create a FITS ASCII table and converted binary table
 *
 * ./create_fits_asciitable out1.fits[.gz or .bz2] out2.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */

    const fits::table_def def[] = {
      /* ttype,comment,     talas,telem, tunit,comment, tdisp,  tform */
      { "PK","PK number",      "", "",   "","",         "A9",   "9A" },
      { "RAH","hours RA",      "", "",   "h","",        "I2",   "3A" },
      { "RAM","minutes RA",    "", "",   "min","",      "F5.2", "6A" },
      { "r0","reserved column","", "",   "","",         "A12",  "12A" },
      { NULL }
    };

    /* Set FMTTYPE name and its version (You can omit this, if unnecessary) */
    fits.assign_fmttype("ADC TEST TABLE", 101);

    /* Create ASCII Table HDU */
    fits.append_table("PLN",0, def, true);

    /* Set NULL value */
    fits.table("PLN").col("r0").assign_tnull(" ");

    /* Allocate 256 rows in "EVENT" table */
    fits.table("PLN").resize_rows(256);

    /* Save to file1 ... */
    if ( 1 < argc ) {
	ssize_t sz;
        const char *out_file = argv[1];
	sz = fits.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
    }

    /* convert ASCII table to binary */
    fits.table("PLN").ascii_to_binary();

    /* Save to file2 ... */
    if ( 2 < argc ) {
	ssize_t sz;
        const char *out_file = argv[2];
	sz = fits.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}
