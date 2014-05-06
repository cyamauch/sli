#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/tarray_tstring.h>
#include <sli/fitscc.h>
#include <libwcs/wcs.h>
#include <math.h>
using namespace sli;

/*
 *  Reading star list (ra,dec,mag) and plot them on a FITS file.
 *
 *  Plotting condition is set in plot.tpl (SFITSIO template).
 *
 */

int main( int argc, char *argv[] )
{
    int return_status = -1;
    struct WorldCoor *wcs = NULL;
    stdstreamio sio;

    if ( 2 < argc ) {

	digeststreamio list_in;
	fitscc fits;
	tarray_tstring cols;
	tstring line;
	double lon,lat, x,y, v, max_mag;
	double v_s1, v_s2, v_s3;
	long i;
	int off;

	const char *in_file = argv[1];
	const char *out_fits = argv[2];

	/* create new FITS from template */
	if ( fits.read_template(0, "plot.tpl") < 0 ) {
	    sio.eprintf("[ERROR] fits.read_template() failed\n");
	    goto quit;
	}

	/* report number of HDUs */
	sio.printf("Numer of HDU = %ld\n",fits.length());

	/* create alias "pri" to Primary HDU */
	fits_image &pri = fits.image("Primary");

	/* report size of image */
	sio.printf("Size of image = %ldx%ld\n",
		   pri.col_length(), pri.row_length());

	/* initialize wcs structure */
	wcs = wcsinitn(pri.header_formatted_string(), NULL);

	/* open file of star list */
	if ( list_in.open("r", in_file) < 0 ) {
	    sio.eprintf("[ERROR] list_in.open() failed\n");
	    goto quit;
	}

	/* cleanup */
	if ( 0 <= pri.header_index("MAGWHITE") ) {
	    pri.fill(pri.header("MAGWHITE").dvalue());
	}
	else pri.fill(23.0);

	/* set maximum values */
	if ( 0 <= pri.header_index("MAXMAG") ) {
	    max_mag = pri.header("MAXMAG").dvalue();
	    sio.printf("maximum mag for plotting ... %g\n",max_mag);
	}
	else max_mag = 99.9;

	if ( 0 <= pri.header_index("MAXMAGS1") ) {
	    v_s1 = pri.header("MAXMAGS1").dvalue();
	}
	else v_s1 = -99.9;

	if ( 0 <= pri.header_index("MAXMAGS2") ) {
	    v_s2 = pri.header("MAXMAGS2").dvalue();
	}
	else v_s2 = -99.9;

	if ( 0 <= pri.header_index("MAXMAGS3") ) {
	    v_s3 = pri.header("MAXMAGS3").dvalue();
	}
	else v_s3 = -99.9;

	pri.header("LISTFILE").assign(in_file)
	   .assign_comment("used flie of star list");

	/* plot one by one */
	i = 0;
	while ( (line=list_in.getline()) != NULL ) {
	    line.chomp();
	    if ( i % 10000 == 0 ) {
		sio.printf("plotting ... %ld\r",i);
		sio.flush();
	    }
	    cols.explode(line,",",true);
	    if ( 0 < cols[2].length() ) {
		lon = cols[0].atof();
		lat = cols[1].atof();
		v   = cols[2].atof();
		//sio.printf("%g %g %g\n",lon,lat,v);
		if ( v <= max_mag ) {
		    long ix, iy;
		    /* convert wcs -> pix */
		    wcs2pix(wcs, lon, lat, &x, &y, &off);
		    /* plot one */
		    ix = (long)floor(x-0.5);
		    iy = (long)floor(y-0.5);
		    if ( v <= v_s3 ) {
			pri.assign(v, ix, iy);
			pri.assign(v, ix+1, iy);
			pri.assign(v, ix+1, iy+1);
			pri.assign(v, ix, iy+1);
			pri.assign(v, ix-1, iy+1);
			pri.assign(v, ix-1, iy);
			pri.assign(v, ix-1, iy-1);
			pri.assign(v, ix, iy-1);
			pri.assign(v, ix+1, iy-1);
			/* */
			pri.assign(v, ix+2, iy);
			pri.assign(v, ix+2, iy+1);
			//pri.assign(v, ix+2, iy+2);
			pri.assign(v, ix+1, iy+2);
			pri.assign(v, ix,   iy+2);
			pri.assign(v, ix-1, iy+2);
			//pri.assign(v, ix-2, iy+2);
			pri.assign(v, ix-2, iy+1);
			pri.assign(v, ix-2, iy);
			pri.assign(v, ix-2, iy-1);
			//pri.assign(v, ix-2, iy-2);
			pri.assign(v, ix-1, iy-2);
			pri.assign(v, ix, iy-2);
			pri.assign(v, ix+1, iy-2);
			//pri.assign(v, ix+2, iy-2);
			pri.assign(v, ix+2, iy-1);
		    }
		    else if ( v <= v_s2 ) {
			pri.assign(v, ix, iy);
			pri.assign(v, ix+1, iy);
			pri.assign(v, ix+1, iy+1);
			pri.assign(v, ix, iy+1);
			pri.assign(v, ix-1, iy+1);
			pri.assign(v, ix-1, iy);
			pri.assign(v, ix-1, iy-1);
			pri.assign(v, ix, iy-1);
			pri.assign(v, ix+1, iy-1);
		    }
		    else if ( v <= v_s1 ) {
			pri.assign(v, ix, iy);
			pri.assign(v, ix+1, iy);
			pri.assign(v, ix-1, iy);
			pri.assign(v, ix, iy+1);
			pri.assign(v, ix, iy-1);
		    }
		    else {
			pri.assign(v, ix, iy);
		    }
		}
	    }
	    i ++;
	}

	/* output fits */
	fits.write_stream(out_fits);

    }
    else {
	sio.eprintf("[USAGE]\n");
	sio.eprintf("$ %s in_list_csv.txt out.fits[.gz][.bz2]\n",argv[0]);
    }

    return_status = 0;
 quit:
    /* free wcs structure */
    if ( wcs != NULL ) wcsfree(wcs);
    return return_status;
}
