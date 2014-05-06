#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>
#include <sli/asarray_tstring.h>
#include <sli/fitscc.h>
#include <sli/fits_image_statistics.h>
using namespace sli;

/*
 * A code to combine Primary images
 *
 * ./combine_fits_images -p "combine=... outtype=..." in0.fits in1 ... -o out.fits"
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc tmpfits, outfits;				/* FITS object  */
    tarray_tstring args = argv;
    tstring combine_options_str;
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
		combine_options_str = args[i];
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

	tarray_tstring options_list;
	asarray_tstring options;

	/* split with space */
	options_list.split(combine_options_str, " ", false);

	for ( i=0 ; i < options_list.length() ; i++ ) {
	    tarray_tstring kv;
	    kv.split(options_list[i], "= ", false);
	    options[kv[0].cstr()] = kv[1].cstr();
	}

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
	outfits.append_image("Primary", FITS::INDEF, FITS::DOUBLE_T);

	sio.printf("# performing combine ...\n");

	/* combine */
	if ( options["combine"] == "sum" ) {
	    outfits.image("Primary") = fitsim_total_small_z(tmppri);
	}
	else if ( options["combine"] == "average" ) {
	    outfits.image("Primary") = fitsim_mean_small_z(tmppri);
	}
	else if ( options["combine"] == "min" ) {
	    outfits.image("Primary") = fitsim_min_small_z(tmppri);
	}
	else if ( options["combine"] == "max" ) {
	    outfits.image("Primary") = fitsim_max_small_z(tmppri);
	}
	else if ( options["combine"] == "median" ) {
	    outfits.image("Primary") = fitsim_median_small_z(tmppri);
	}

	sio.printf("# performing convert ...\n");

	/* convert */
	if ( options["outtype"] == "float" ) {
	    outfits.image("Primary").convert_type(FITS::FLOAT_T);
	}

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
       sio.printf(" outtype=double (float|double)\n");
    }
    return return_status;
}

