#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A test code to read headers only with fast access.
 *
 * ./read_header_only in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;
    fits_header hdr1, hdr2;

    if ( 1 < argc ) {
        const char *in_file = argv[1];
	digeststreamio fh_in;
	digeststreamio fh_out;

	/* open file to write */
	if ( 2 < argc ) {
	    const char *out_file = argv[2];
	    /* open */
	    if ( fh_out.open("w", out_file) < 0 ) {
		sio.eprintf("[ERROR] fh_out.open() failed\n");
		goto quit;
	    }
	}
	/* open file to read */
	if ( fh_in.open("r", in_file) < 0 ) {
	    sio.eprintf("[ERROR] fh_in.open() failed\n");
	    goto quit;
	}

	while ( 1 ) {
	    ssize_t sz;
	    tstring formatted1, formatted2;

	    /*
	     * 1st object: hdr1
	     */
	    /* read header from stream */
	    sz = hdr1.read_stream(fh_in);
	    if ( sz == 0 ) break;
	    if ( sz < 0 ) {
		sio.eprintf("[ERROR] hdr1.read_stream() failed\n");
		goto quit;
	    }
	    /* skip data block */
	    sz = hdr1.skip_data_stream(fh_in);
	    if ( sz < 0 ) {
		sio.eprintf("[ERROR] hdr1.skip_data_stream() failed\n");
		goto quit;
	    }

	    /*
	     * 2nd object: hdr2
	     */
	    formatted1 = hdr1.formatted_string();
	    /* read 80-chars formatted header string */
	    sz = hdr2.read_buffer(formatted1.cstr());
	    formatted2 = hdr2.formatted_string();
	    /* compare to test */
	    if ( formatted1.strcmp(formatted2) == 0 ) {
		sio.printf("Compare ... OK!\n");
	    }
	    else {
		sio.eprintf("[ERROR] Compare ... FAILED\n");
		goto quit;
	    }

	    /* display formatted string */
	    sio.printf("==== OUTPUT HEADER BYTE DATA ====\n");
	    //sio.printf("== checksum (as is): %lu\n", hdr1.checksum());
	    //sio.printf("== encoded checksum (as is): %s\n", 
	    //	    	       hdr1.encoded_checksum());
	    sio.printf("%s\n", formatted1.cstr());

	    /* write header only! */
	    if ( 2 < argc ) {
		/* output header to stream */
		sz = hdr1.write_stream(fh_out,false);
		sio.printf("%zd bytes written\n",sz);
	    }

	}

	/* close input file */
	fh_in.close();

	if ( 2 < argc ) {
	    /* close output file */
	    fh_out.close();
	}

    }

    return_status = 0;
 quit:
    return return_status;
}

