#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/pipestreamio.h>
#include <sli/tstring.h>
#include <sli/fitscc.h>
#include <stdlib.h>
using namespace sli;

/**
 * @file   fits_dataunit_md5.cc
 * @brief  FITS ファイルの dataunit の md5 を求めるツール
 * @note   ディスクベースの FITS I/O を行ないたい時の参考に．
 */

/*
 * A code to display md5 of each data unit
 *
 * ./fits_dataunit_md5 in_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;
    fits_header hdr1;

    /* display usage */
    if ( argc == 1 ) {
	sio.printf("dataunit_md5: display md5 of each data unit.\n");
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

	sio.printf("# filename: %s\n",in_file);

	hdu_idx = 0;
	while ( 1 ) {

	    ssize_t sz;
	    size_t i;
	    tstring formatted1, line;
	    bool finish_ok = false;
	    size_t bytepix, ndim, dlen;

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

	    /* check BITPIX, NAXIS, etc */
	    if ( hdr1.index("BITPIX") < 0 ) {
		sio.eprintf("[ERROR] not found BITPIX\n");
		goto quit;
	    }
	    bytepix = abs(hdr1.record("BITPIX").lvalue()) / 8;

	    if ( hdr1.index("NAXIS") < 0 ) {
		sio.eprintf("[ERROR] not found NAXIS\n");
		goto quit;
	    }
	    ndim = hdr1.record("NAXIS").lvalue();

	    if ( 0 < ndim ) {
		dlen = bytepix;
		for ( i=0 ; i < ndim ; i++ ) {
		    tstring kwd;
		    kwd.assignf("NAXIS%d",(int)(i+1));
		    if ( hdr1.index(kwd.cstr()) < 0 ) {
			sio.eprintf("[ERROR] not found %s\n",kwd.cstr());
			goto quit;
		    }
		    dlen *= hdr1.record(kwd.cstr()).lvalue();
		}
	    }
	    else {
		dlen = 0;
	    }

	    if ( 0 < hdr1.index("PCOUNT") ) {
		dlen += hdr1.record("PCOUNT").lvalue();
	    }

	    sio.printf("HDU index [%ld]:  ",hdu_idx);
	    sio.printf("bytepix = %zu, stream length = %zu\n", bytepix, dlen);

	    /* get md5 of data unit */
	    if ( 0 < dlen ) {
		char buf[2880];
		size_t nblock = ((dlen - 1) / 2880) + 1;
		pipestreamio pout;
		if ( pout.open("w","md5sum") < 0 ) {
		    sio.eprintf("[ERROR] pout.open() failed.\n");
		    goto quit;
		}
		for ( i=0 ; i < nblock ; i++ ) {
		    ssize_t s;
		    s = fh_in.read(buf,2880);
		    if ( s < 0 ) {
			sio.eprintf("[ERROR] read error!\n");
			goto quit;
		    }
		    if ( finish_ok == false ) {
			if ( s < 2880 ) {
			    sio.eprintf("[ERROR] read error!\n");
			    goto quit;
			}
		    }
		    pout.write(buf,s);
		    if ( s < 2880 ) break;
		}
		pout.close();
	    }

	    hdu_idx ++;
	}

	/* close input file */
	fh_in.close();

    }

    return_status = 0;
 quit:
    return return_status;
}

