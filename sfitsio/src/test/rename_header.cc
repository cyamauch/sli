#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
#include <sli/tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A test code rename keywords of FITS header
 *
 * ./rename_header -i in.fits -o out.fits 'OLD=NEW' 'OLD#=NEW#'
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;                                    /* standard I/O */
    fitscc fits;                                        /* FITS object  */
    tarray_tstring args = argv;				/* string array */
    tstring in_file;
    tstring out_file;
    size_t i;

    /* parse args */
    args.erase(0,1);
    for ( i=0 ; i < args.length() ; ) {
	if ( args[i] == "-i" ) {
	    in_file = args[i+1];
	    args.erase(i,2);
	}
	else if ( args[i] == "-o" ) {
	    out_file = args[i+1];
	    args.erase(i,2);
	}
	else {
	    i ++;
	}
    }

    /* reading a FITS file and update header */
    if ( 0 < in_file.length() ) {
        ssize_t sz;
        long i;
        /* reading ... */
        sz = fits.read_stream(in_file.cstr());
        if ( sz < 0 ) {
            sio.eprintf("[ERROR] fits.read_stream() failed\n");
            goto quit;
        }
        /* print HDU info and change keywords */
        for ( i=0 ; i < fits.length() ; i++ ) {
	    size_t j;
	    tarray_tstring old_and_new;
            sio.printf("HDU %ld : hdutype = %d\n",i,fits.hdutype(i));
            sio.printf("HDU %ld : hduname = %s\n",i,fits.hduname(i));
	    for ( j=0 ; j < args.length() ; j++ ) {
		/* split "OLD", "=", and "NEW", using regular expression */
		old_and_new.regassign(args[j], "^([^=]+)([=])([^=]+)$");
		//old_and_new.dprint();				/* for debug */
		if ( old_and_new.length() == 4 ) {
		    tstring old_kwd = old_and_new[1];
		    tstring new_kwd = old_and_new[3];
		    /* check "#" suffix */
		    if ( 0 < old_kwd.strrspn("#") ) {
			/* erase # suffix */
			old_kwd.rtrim("#");
			new_kwd.rtrim("#");
			/* only for BTE or ATE */
			if ( fits.hdutype(i) == FITS::BINARY_TABLE_HDU || 
			     fits.hdutype(i) == FITS::ASCII_TABLE_HDU ) {
			    fits_table &tbl = fits.table(i);
			    /* check if old_kwd is registered in TXFLDKWD */
			    if ( tbl.is_user_col_keyword(old_kwd.cstr()) 
				 == true ) {
				/* rename keywords of column header and 
				   update TXFLDKWD */
				tbl.rename_col_header(old_kwd.cstr(),
						      new_kwd.cstr());
			    }
			}
		    }
		    /* NOT having "#" suffix */
		    else {
			/* rename keyword simply */
			fits.hdu(i).header_rename(old_kwd.cstr(), 
						  new_kwd.cstr());
		    }
		}
	    }

        }
    }

    /* writing a FITS file */
    if ( 0 < out_file.length() ) {
        ssize_t sz;
        /* writing ... */
        sz = fits.write_stream(out_file.cstr());
        if ( sz < 0 ) {
            sio.eprintf("[ERROR] fits.write_stream() failed\n");
            goto quit;
        }
    }

    return_status = 0;

 quit:
    return return_status;
}

