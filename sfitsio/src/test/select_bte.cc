#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A sample code using the SFITSIO
 *
 * ./select_bte in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    fitscc fits;
    stdstreamio sio;

    /* Set the HDUs which you use */
    fits.hdus_to_read().assign("FIS_OBS","GADS",NULL);
    /* Set the columns which you use */
    fits.cols_to_read().assign("AFTIME","DET","RA","DEC",NULL);
    
    /* read */
    if ( 1 < argc ) {
	long i;
	ssize_t sz;
	const char *in_file = argv[1];
	sz = fits.read_stream(in_file);		/* reading file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.read_stream() failed\n");
	    goto quit;
	}
	if ( fits.size() < 2 ) {
	    sio.eprintf("[ERROR] Specified HDU not found\n");
	    goto quit;
	}
	/* printing HDU info. */
	//for ( i=0 ; i < fits.size() ; i++ ) {
	//    sio.printf("HDU %ld : hdutype = %d\n",i,fits.hdutype(i));
	//    sio.printf("HDU %ld : hduname = %s\n",i,fits.hduname(i));
	//    sio.printf("HDU %ld : hduver  = %lld\n",i,fits.hduver(i));
	//}
	//fits.table("FIS_OBS").col("AFTIME")
	//    .convert_type(Fits::Longlong_t,200000000.0,0.0001);
	/* dump */
	for ( i=0 ; i < fits.table("FIS_OBS").row_size() ; i++ ) {
	    sio.printf("AFTIME %ld = %f(%f)\n",i,
		       fits.table("FIS_OBS").col("AFTIME").dvalue(i),
		       fits.table("FIS_OBS").col("AFTIME").double_value(i));
	}
    }
    /* write */
    if ( 2 < argc ) {
	const char *out_file = argv[2];
	ssize_t sz;
	sz = fits.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;

 quit:
    return return_status;
}
