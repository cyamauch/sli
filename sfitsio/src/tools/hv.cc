#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   hv.cc
 * @brief  高速に動作する FITS ヘッダの閲覧ツール
 * @note   ディスクベースの FITS I/O を行ないたい時の参考に．
 */

/*
 * A code to display headers with fast file access.
 *
 * ./hv in_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;
    fits_header hdr1;

    /* display usage */
    if ( argc == 1 ) {
	sio.printf("hv: display headers with fast file access.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s in.fits[.gz or .bz2]\n", argv[0]);
	goto quit;
    }

    if ( 1 < argc ) {
        const char *in_file = argv[1];
	digeststreamio fh_in;
	long hdu_idx;

	/* open file to read */
	if ( fh_in.open("r", in_file) < 0 ) {
	    sio.eprintf("[ERROR] fh_in.open() failed\n");
	    goto quit;
	}

	hdu_idx = 0;
	while ( 1 ) {

	    ssize_t sz;
	    size_t i;
	    tstring formatted1, line;
	    bool finish_ok = false;

	    /* read header from stream */
	    sz = hdr1.read_stream(fh_in);
	    if ( sz == 0 ) break;
	    if ( sz < 0 ) {
		sio.eprintf("[ERROR] hdr1.read_stream() failed\n");
		goto quit;
	    }

	    /* check EXTEND keyword */
	    if ( hdu_idx == 0 ) {
		if ( 0 <= hdr1.index("EXTEND") ) {
		    if ( hdr1.record("EXTEND").bvalue() == false ) {
			finish_ok = true;
		    }
		}
		else {
		    finish_ok = true;
		}
	    }

	    if ( finish_ok == false ) {
		/* skip data block */
		sz = hdr1.skip_data_stream(fh_in);
		if ( sz < 0 ) {
		    sio.eprintf("[ERROR] hdr1.skip_data_stream() failed\n");
		    goto quit;
		}
	    }

	    formatted1 = hdr1.formatted_string();

	    /* display formatted string */
	    line.assign('#',80);
	    line.putf(4, " HDU index=%ld ", hdu_idx);
	    sio.printf("%s\n",line.cstr());
	    for ( i=0 ; i < formatted1.length() / 80 ; i++ ) {
		line.put(0, formatted1, i*80, 80).rtrim();
		sio.printf("%s\n", line.cstr());
	    }
	    sio.printf("\n");

	    if ( finish_ok == true ) break;

	    hdu_idx ++;
	}

	/* close input file */
	fh_in.close();

    }

    return_status = 0;
 quit:
    return return_status;
}

