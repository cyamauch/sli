#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <math.h>
using namespace sli;

/*
 * A sample code to create a FITS binary table using TZERO and TSCALE
 *
 * ./tzerotscale_bintable out_file.fits[.gz or .bz2]
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
	//                             TDISP, TFORM, TDIM
        { "COL_D","",   "",  "", "","","", "1D", "", "", "-10", "0.5" },
        { "COL_E","",   "",  "", "","","", "1E", "", ""},
        { "COL_B","",   "",  "", "","","", "1B", "", "", "-10", "0.5" },
        { "COL_I","",   "",  "", "","","", "1I", "", ""},
        { "COL_J","",   "",  "", "","","", "1J", "", "", "-10", "0.5" },
        { "COL_K","",   "",  "", "","","", "1K", "", "", "-10", "0.5" },
        { NULL }
    };

    /* Set FMTTYPE name and its version (You can omit this, if unnecessary) */
    fits.assign_fmttype("TEST", 101) ;

    /* Create Binary Table HDU */
    fits.append_table("TSTZ_TEST",0, tbl_def);

    fits_table &tbl = fits.table("TSTZ_TEST");

    tbl.col(1L).assign_tzero(-10.0);
    tbl.col(1L).assign_tscal(0.5);
    tbl.col(3L).assign_tzero(-10.0);
    tbl.col(3L).assign_tscal(0.5);

    /* Allocate 256 rows in "EVENT" table */
    tbl.resize_rows(256);

    tbl.col(0L).assign(64, 0);
    tbl.col(1L).assign(64, 0);
    tbl.col(2L).assign(64, 0);
    tbl.col(3L).assign(64, 0);
    tbl.col(4L).assign(64, 0);
    tbl.col(5L).assign(64, 0);

    tbl.col(0L).assign(32.0, 1);
    tbl.col(1L).assign(32.0, 1);
    tbl.col(2L).assign(32.0, 1);
    tbl.col(3L).assign(32.0, 1);
    tbl.col(4L).assign(32.0, 1);
    tbl.col(5L).assign(32.0, 1);

    /* test user's keyword */
    tbl.update_col_header(0L, "KW_A", "abc1", "comment A");
    tbl.update_col_header(1L, "KW_A", "abc2", "comment A");
    tbl.update_col_header(0L, "KW_B", "def", "comment B");

    /* test reserved keyword */
    tbl.update_col_header(0L, "TUNIT", "mag", "test TUNIT");
    tbl.update_col_header(1L, "TUNIT", "mag", "test TUNIT");
    tbl.erase_col_header(0L, "TUNIT");

    /* test col_header() */
    tbl.rename_col_header("KW_A", "KEYWD_A");

    /* test header_rename() */
    tbl.header_append("MESSAGE1", "hoge", "message comment");
    tbl.header_rename("MESSAGE1", "MSG_1");

    sio.printf("[%d]\n",tbl.is_user_col_keyword("KW_B123"));
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
