#include <sli/fitscc.h>
#include <sli/stdstreamio.h>
#include <stdlib.h>
using namespace sli;

/**
 * @file   conv_fits_image_bitpix.cc
 * @brief  FITS 画像の BITPIX を変換するツール
 */

/*
 * A code to convert BITPIX of FITS image.
 *
 * [example]
 * ./conv_fits_image_bitpix -32 in_file.fits[.gz or .bz2] out_file.fits[.gz or .bz2]
 *                        [bitpix]
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    int bitpix = -64;
    bool flag_uint = false;
    stdstreamio sio;					/* standard I/O */
    fitscc obj;						/* FITS object  */

    /* display usage */
    if ( argc == 1 ) {
	sio.printf("conv_bitpix: convert BITPIX of Primary image.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s bitpix in.fits[.gz or .bz2] out.fits[.gz or .bz2]\n",
		   argv[0]);
	sio.printf("[BITPIX]\n");
	sio.printf("8, 16, u16, 32, 64, -32 or -64\n");
	goto quit;
    }

    /* obtain new bitpix value */
    if ( 1 < argc ) {
	if ( argv[1][0] == 'u' || argv[1][0] == 'U' ) {
	    flag_uint = true;
	    bitpix = atoi(argv[1] + 1);
	}
	else bitpix = atoi(argv[1]);
    }

    /* read a FITS file and convert BITPIX */
    if ( 2 < argc ) {
	const char *in_file = argv[2];
	ssize_t sz;
	sio.printf("# reading ...\n");
	sz = obj.read_stream(in_file);      /* reading file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.read_stream() failed\n");
	    goto quit;
	}
	sio.printf("# converting ...\n");
	/* convert bitpix */
	switch ( bitpix ) {
	case 8:
	    obj.image("Primary").convert_type(FITS::BYTE_T); break;
	case 16:
	    if ( flag_uint == true ) {
		obj.image("Primary").convert_type(FITS::SHORT_T, 32768.0);
	    }
	    else {
		obj.image("Primary").convert_type(FITS::SHORT_T);
	    }
	    break;
	case 32:
	    obj.image("Primary").convert_type(FITS::LONG_T); break;
	case 64:
	    obj.image("Primary").convert_type(FITS::LONGLONG_T); break;
	case -32:
	    obj.image("Primary").convert_type(FITS::FLOAT_T); break;
	default:
	    obj.image("Primary").convert_type(FITS::DOUBLE_T); break;
	}
    }

    /* write a FITS file */
    if ( 3 < argc ) {
	const char *out_file = argv[3];
	ssize_t sz;
	sio.printf("# writing ...\n");
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
