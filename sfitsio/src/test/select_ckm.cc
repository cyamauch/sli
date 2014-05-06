#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A sample code using the SFITSIO
 *
 * ./select_ckm in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    fitscc obj;
    stdstreamio sio;

    /* Set the HDUs which you use */
    //fits.hdus_to_read().assign("HOGE",NULL);
    /* Set the columns which you use */
    //fits.cols_to_read().assign("WAVELENGTH",NULL);

    /* read */
    if ( 1 < argc ) {
	long i;
	ssize_t sz;
	const char *in_file = argv[1];
	sz = obj.read_stream(in_file);		/* reading file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.read_stream() failed\n");
	    goto quit;
	}
	if ( obj.size() < 2 ) {
	    sio.eprintf("[ERROR] Specified HDU not found\n");
	    goto quit;
	}
	/* printing HDU info. */
	//for ( i=0 ; i < obj.size() ; i++ ) {
	//    sio.printf("HDU %ld : hdutype = %d\n",i,obj.hdutype(i));
	//    sio.printf("HDU %ld : hduname = %s\n",i,obj.hduname(i));
	//    sio.printf("HDU %ld : hduver  = %lld\n",i,obj.hduver(i));
	//}
	/* dump */
	for ( i=0 ; i < obj.table(1).row_size() ; i++ ) {
	    sio.printf("WAVELENGTH %ld = %f\n",i,
		       obj.table(1).col("WAVELENGTH").dvalue(i));
	}
    }
    /* write */
    if ( 2 < argc ) {
	const char *out_file = argv[2];
	ssize_t sz;
	sz = obj.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;

 quit:
    return return_status;
}
