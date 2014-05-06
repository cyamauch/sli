#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <math.h>
using namespace sli;

/**
 * @file   create_fits_bintable.cc
 * @brief  �Х��ʥ�ơ��֥��ޤ࿷�� FITS �ե����������
 */

/*
 * A sample code to create a FITS binary table
 *
 * ./create_fits_bintable out_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    long i;

    /* column definition */
    const fits::table_def tbl_def[] = {
        // TTYPE,comment,         TALAS,TELEM,TUNIT,comment,
	//                                             TDISP, TFORM, TDIM
        { "TIME","satellite time", "DATE", "", "s","","F16.3", "1D", "" },
        { "STATUS","status",       "","SAA,TEMP", "", "", "",  "8J", "" },
        { "NAME","",                 "",  "", "", "", "",  "128A16", "(4,2)" },
        { "SWITCH","test for bool",  "","", "", "", "L5",  "1L", "" },
        { NULL }
    };

    /* Set FMTTYPE name and its version (You can omit this, if unnecessary) */
    fits.assign_fmttype("ASTRO-X XXX Event Table", 101);

    /* Create Binary Table HDU */
    fits.append_table("EVENT",0, tbl_def);

    fits_table &tbl = fits.table("EVENT");

    /* Set NULL value (TNULL of string type is local convension of SFITSIO) */
    tbl.col("STATUS").assign_tnull(-1L);
    tbl.col("NAME").assign_tnull("NULL");

    /* Allocate 16 rows in "EVENT" table */
    tbl.resize_rows(16);

    /* Assign NULL as default value in all columns */
    for ( i=0 ; i < tbl.col_length() ; i++ ) {
	tbl.col(i).assign_default(NAN);
    }

    /* Allocate 64 rows in "EVENT" table */
    tbl.resize_rows(64);

    /* Put NULL to array */
    tbl.col("TIME").assign(NAN,4);
    tbl.col("STATUS").assign(NAN,2,3);
    tbl.col("NAME").assign(NAN,0,1);
    tbl.col("SWITCH").assign(false,0);
    tbl.col("SWITCH").assign(true,1);

    /* Save to file... */
    if ( 1 < argc ) {
	ssize_t sz;
        const char *out_file = argv[1];
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
