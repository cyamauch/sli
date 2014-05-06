#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A code to combine Primary images
 *
 * ./combine_images -p "combine=... outtype=..." in0.fits in1 ... -o out.fits"
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc tmpfits, outfits;				/* FITS object  */
    tarray_tstring args = argv;
    tstring combine_options;
    tstring outfile;
    tarray_tstring infiles;
    size_t n_infiles;
    size_t i;

    if ( argc < 2 ) goto usage_quit;

    /* for now */
    infiles.resize(argc);
    n_infiles = 0;

    /* parse args */
    if ( 1 < argc ) {
	for ( i=1 ; i < args.length() ; i++ ) {
	    if ( args[i].strcmp("-p") == 0 ) {
		i++;
		combine_options = args[i];
	    }
	    else if ( args[i].strcmp("-o") == 0 ) {
		i++;
		outfile = args[i];
	    }
	    else {
		infiles[n_infiles] = args[i];
		n_infiles ++;
	    }
	}
    }

    if ( n_infiles == 0 ) goto usage_quit;

    sio.printf("# reading files: 0 ");
    sio.flush();

    /* read first fits file */
    if ( tmpfits.read_stream(infiles[0].cstr()) < 0 ) {
	sio.eprintf("[ERROR] tmpfits.read_stream() failed\n");
	goto quit;
    }
    else {

	/* prepare alias */
	fits_image &tmppri = tmpfits.image("Primary");

	/* alloc layers */
	tmppri.resize(2, n_infiles);

	/* read and paste to each layer */
	for ( i=1 ; i < n_infiles; i++ ) {
	    fitscc fits;
	    sio.printf("%zd ",i);
	    sio.flush();
	    if ( fits.read_stream(infiles[i].cstr()) < 0 ) {
		sio.eprintf("[ERROR] fits.read_stream() failed\n");
		goto quit;
	    }
	    /* paste */
	    tmppri.paste(fits.image("Primary"), 0L, 0L, i /* layer */);
	}

	sio.printf("\n");

	/* prepare fits for output */
	outfits.append_image(NULL,FITS::INDEF, FITS::DOUBLE_T,1,1);

	sio.printf("# performing combine ...\n");

	/* combine */
	tmppri.combine_layers(&outfits.image("Primary"), combine_options.cstr());

	if ( 0 < outfile.length() ) {
	    sio.printf("# writing ...\n");
	    /* to confirm */
	    //if ( tmpfits.write_stream(outfile.cstr()) < 0 ) {
	    //	sio.eprintf("[ERROR] tmpfits.write_stream() failed\n");
	    //	goto quit;
	    //}
	    /*  */
	    if ( outfits.write_stream(outfile.cstr()) < 0 ) {
	    	sio.eprintf("[ERROR] tmpfits.write_stream() failed\n");
	    	goto quit;
	    }
	}
    }

    return_status = 0;

 quit:
    return return_status;

 usage_quit:
    /* display usage */
    if ( argc == 1 ) {
       sio.printf("combine_images: combine Primary images.\n");
       sio.printf("[USAGE]\n");
       sio.printf("%s -p 'combine=... outtype=...' "
		  "in0.fits in1.fits ... -o out.fits\n", argv[0]);
       sio.printf("[OPTIONS]\n");
       sio.printf(" combine=average (average|median|sum|min|max)\n");
       sio.printf(" outtype=double (short|ushort|long|longlong|float|double)\n");
    }
    return return_status;
}

