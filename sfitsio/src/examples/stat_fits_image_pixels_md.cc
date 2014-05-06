#include <sli/fitscc.h>
#include <sli/mdarray_statistics.h>
using namespace sli;

/**
 * @file   stat_fits_image_pixels_md.cc
 * @brief  画像の統計値を取得するコード
 */

/*
 * A code to obtain basic image statistics
 * (using statistics functions for mdarray class)
 *
 * ./stat_fits_image_pixels_md in_file.fits[.gz or .bz2]
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
	double sdev, median;
	double gain = 1.0;

	//if ( 0 < prim.header_value_length("GAIN") ) {
	//    gain = prim.header("GAIN").dvalue();
	//}

	sio.printf("# performing convert and statistics ...\n");
	if ( prim.type() == FITS::BYTE_T || prim.type() == FITS::SHORT_T ||
	     prim.type() == FITS::FLOAT_T ) {
	    prim.convert_type(FITS::FLOAT_T);
	    mdarray_float &im32 = prim.float_array();	/* ref to array obj */
	    if ( 0 < gain ) im32 *= gain;
	    moment = md_moment(im32, true, NULL, &sdev);
	    median = md_median(im32);
	}
	else {
	    prim.convert_type(FITS::DOUBLE_T);
	    mdarray_double &im64 = prim.double_array();	/* ref to array obj */
	    if ( 0 < gain ) im64 *= gain;
	    moment = md_moment(im64, true, NULL, &sdev);
	    median = md_median(im64);
	}

	sio.printf("mean = %.10g\n", moment[0]);
	sio.printf("variance = %.10g\n", moment[1]);
	sio.printf("skewness = %.10g\n", moment[2]);
	sio.printf("kurtosis = %.10g\n", moment[3]);
	sio.printf("stddev = %.10g\n", sdev);
	sio.printf("median = %.10g\n", median);
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
