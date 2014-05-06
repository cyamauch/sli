#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <sli/fits_image_statistics.h>

using namespace sli;

/*
 * A code to obtain basic image statistics
 *
 * ./stat_fits_image_pixels in_file.fits[.gz or .bz2]
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    const char *in_file;

    if ( argc < 2 ) goto usage_quit;

    in_file = argv[1];

    /* reading file */
    sio.printf("# reading ...\n");

    if ( fits.read_stream(in_file) < 0 ) {
	sio.eprintf("[ERROR] fits.read_stream() failed\n");
	goto quit;
    }

    {
	fits_image &prim = fits.image("Primary");
	mdarray_double moment;
	double sdev;
	sio.printf("# performing statistics ...\n");

	//sio.printf("npix = %.10g\n", fitsim_npix(prim));
	//sio.printf("total = %.10g\n", fitsim_total(prim));
	//sio.printf("meanabsdev = %.10g\n", fitsim_meanabsdev(prim));
	//sio.printf("variance = %.10g\n", fitsim_variance(prim));
	//sio.printf("skewness = %.10g\n", fitsim_skewness(prim, true));
	//sio.printf("kurtosis = %.10g\n", fitsim_kurtosis(prim, true));
	//sio.printf("stddev = %.10g\n", fitsim_stddev(prim));
	//sio.printf("min = %.10g\n", fitsim_min(prim));
	//sio.printf("max = %.10g\n", fitsim_max(prim));

	moment = fitsim_moment(prim, true, NULL, &sdev);
	sio.printf("mean = %.10g\n", moment[0]);
	sio.printf("variance = %.10g\n", moment[1]);
	sio.printf("skewness = %.10g\n", moment[2]);
	sio.printf("kurtosis = %.10g\n", moment[3]);
	sio.printf("stddev = %.10g\n", sdev);
	sio.printf("median = %.10g\n", fitsim_median(prim));

    }


    return_status = 0;

 quit:
    return return_status;

 usage_quit:
    /* display usage */
    if ( argc == 1 ) {
	sio.printf("stat_pixels: obtain basic image statistics.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s in_file.fits[.gz or .bz2]\n", argv[0]);
    }
    return return_status;
}

