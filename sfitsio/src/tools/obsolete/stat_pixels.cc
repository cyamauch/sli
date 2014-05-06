#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A code to obtain basic image statistics
 *
 * ./stat_pixels in_file.fits[.gz or .bz2]
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    tarray_tstring args = argv;
    tarray_tstring infiles;
    tstring stat_options;
    size_t n_infiles;
    size_t i;

    if ( argc < 2 ) goto usage_quit;

    /* for now */
    infiles.resize(argc);
    n_infiles = 0;

    /* default */
    stat_options = "results=npix,mean,stddev,min,max,median";

    /* parse args */
    if ( 1 < argc ) {
	for ( i=1 ; i < args.length() ; i++ ) {
	    if ( args[i].strcmp("-p") == 0 ) {
		i++;
		stat_options = args[i];
	    }
	    else {
		infiles[n_infiles] = args[i];
		n_infiles ++;
	    }
	}
    }

    if ( n_infiles == 0 ) goto usage_quit;

    for ( i=0 ; i < n_infiles ; i++ ) {
	ssize_t sz;
	const char *in_file = infiles[i].cstr();
	fits_header stat_results;
	long i;

	/* reading file */
	sio.printf("# reading ...\n");
	sz = fits.read_stream(in_file);
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] fits.read_stream() failed\n");
	    goto quit;
	}

	sio.printf("# performing stat_pixels() ...\n");
	if ( fits.image("Primary").stat_pixels(&stat_results, 
					       stat_options.cstr()) < 0 ) {
	    sio.eprintf("[ERROR] stat_pixels() failed\n");
	    goto quit;
	}

	sio.printf("IMAGE = %s\n", in_file);
	for ( i=0 ; i < stat_results.length() ; i++ ) {
	    sio.printf("%s = %.15g\n", stat_results.at(i).keyword(),
		       stat_results.at(i).dvalue());
	}

    }

    return_status = 0;

 quit:
    return return_status;

 usage_quit:
    /* display usage */
    if ( argc == 1 ) {
	sio.printf("stat_pixels: obtain basic image statistics.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s -p options in0.fits in1.fits ...\n", argv[0]);
	sio.printf("[OPTIONS]\n");
	sio.printf("results=npix,mean,stddev,min,max,median,skew,kurtosis\n");
    }
    return return_status;
}

