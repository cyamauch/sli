#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <math.h>
using namespace sli;

/*
 * A test code to edit a FITS binary table
 *
 * ./edit_bintable out_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */

    /* column definition */
    const fits::table_def tbl_def[] = {
        // TTYPE,comment,         TALAS,TELEM,TUNIT,comment,
	//                                             TDISP, TFORM, TDIM
        { "TIME","satellite time", "DATE", "", "s","","F16.3", "1D", "" },
        { "STATUS","status",       "","SAA,TEMP", "", "", "",  "8J", "" },
        { "NAME","",               "","","", "", "",  "128A16", "(4,2)" },
        { NULL }
    };
    const fits::table_def tbl_def_2[] = {
        // TTYPE,comment,         TALAS,TELEM,TUNIT,comment,
	//                                             TDISP, TFORM, TDIM
        { "TIME2","satellite time", "DATE","", "s","","F16.3", "1D", "" },
        { "STATUS2","status",      "","SAA,TEMP", "", "", "",  "8J", "" },
        { "NAME2","",              "","", "", "", "", "128A16", "(4,2)" },
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

    /* Allocate 256 rows in "EVENT" table */
    tbl.resize_rows(256);

    /* Put NULL to array */
    tbl.col("TIME").assign(NAN,4);
    tbl.col("STATUS").assign(NAN,2,3);
    tbl.col("NAME").assign(NAN,0,1);

    /* define new field keywords */
    //tbl.append_a_user_field_keyword("TDESC");
    //tbl.header_append("TDESC1", "this is satellite time", "description");
    //tbl.header_append("TDESC2", "this is status", "description");

    /* test append */
    //tbl.append_cols(tbl_def_2);

    /* test swap_cols() */
    //tbl.swap_cols(0,2,2);

    /* test insert */
    //tbl.insert_cols(1L, tbl_def_2);

    /* test erase */
    tbl.erase_cols(1, 1);

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
