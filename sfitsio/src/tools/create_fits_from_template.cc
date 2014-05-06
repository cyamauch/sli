#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/**
 * @file   create_fits_from_template.cc
 * @brief  FITS テンプレートから新規 FITS ファイルを作成するコード
 */

/*
 * A code to read a templete file and write a FITS file
 *
 */

int main( int argc, char *argv[] )
{ 
    int return_status = -1;
    stdstreamio sio;
    fitscc fits;

    /* display usage */
    if ( argc == 1 ) {
	sio.printf("create_from_template: create a FITS file from a template.\n");
	sio.printf("[USAGE]\n");
	sio.printf("%s template.tpl out.fits[.gz or .bz2]\n", argv[0]);
	goto quit;
    }

    if ( 1 < argc ) {
	const char *templ_filename = argv[1];
	sio.printf("# reading template ...\n");
	/* reading template */
	if ( fits.read_template(0, templ_filename) < 0 ) {
	    sio.eprintf("[ERROR] fits.read_template() failed\n");
	    goto quit;
	}
    }

    if ( 2 < argc ) {
	const char *out_filename = argv[2];
	sio.printf("# writing ...\n");
	if ( fits.write_stream(out_filename) < 0 ) {
	    sio.eprintf("[ERROR] fits.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}

