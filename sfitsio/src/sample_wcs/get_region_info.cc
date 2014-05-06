#include <sli/stdstreamio.h>
#include <sli/digeststreamio.h>
#include <sli/fitscc.h>
#include <sli/tstring.h>

#include <libwcs/wcs.h>

#include <math.h>

using namespace sli;

/*
 *  A sample code to get table data from a FITS image with WCS.
 *
 *  Output format is:
 *   filename rid ra_cen dec_cen cx_cen cy_cen cz_cen max_r x0 y0 width height
 *                [deg]  [deg]                        [']   [pixels]
 *
 */

static const double CC_DEG_TO_RAD = 0.017453292519943295;
static const double CC_RAD_TO_ARCMIN = 3437.7467707849396;
static const double FITS_X_ORIGIN = 0.5;
static const double FITS_Y_ORIGIN = 0.5;

/* function to obtain angle between two positions */
static double get_distance_arcmin_of_eq( double lon1, double lat1,
					 double lon2, double lat2 )
{
    double mx,my,mz,mxyz;
    double cos_lat1, cos_lat2;
    lon1 *= CC_DEG_TO_RAD;
    lat1 *= CC_DEG_TO_RAD;
    lon2 *= CC_DEG_TO_RAD;
    lat2 *= CC_DEG_TO_RAD;
    cos_lat1 = cos(lat1);
    cos_lat2 = cos(lat2);
    mx = cos(lon1) * cos_lat1;
    my = sin(lon1) * cos_lat1;
    mz =             sin(lat1);
    mx *= cos(lon2) * cos_lat2;
    my *= sin(lon2) * cos_lat2;
    mz *=             sin(lat2);
    mxyz = mx + my + mz;
    if ( 1.0 < mxyz ) mxyz = 1.0;
    else if ( mxyz < -1.0 ) mxyz = -1.0;
    return acos( mxyz ) * CC_RAD_TO_ARCMIN;
}

/* main function */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    struct WorldCoor *wcs = NULL;
    stdstreamio sio;
    tstring region_width;

    if ( argc <= 2 ) {
	sio.eprintf("[USAGE]\n");
	sio.eprintf("$ %s region_width(arcmin) in.fits[.gz][.bz2] [out.fits[.gz][.bz2]]\n",argv[0]);
	goto quit;
    }

    if ( 1 < argc ) {
	region_width = argv[1];
    }

    if ( 2 < argc ) {
	const char *pathname = argv[2];
	tstring filename;
	digeststreamio fh_in;
	fitscc fits;
	fits_header hdr;
	ssize_t sz, spos;
	long base_width, base_height, n_divx, n_divy, n_jutoutx, n_jutouty;
	long idx, naxis1, naxis2, i, rid;

	filename = pathname;
	if ( 0 <= (spos=filename.rfind('/')) ) {
	    filename.erase(0,spos+1);
	}

	/* read all FITS file if argv[3] is set */
	if ( 3 < argc ) {
	    if ( fits.read_stream(pathname) < 0 ) {
		sio.eprintf("[ERROR] fits.read_stream() failed\n");
		goto quit;
	    }
	}

	/* open stream */
	if ( fh_in.open("r", pathname) < 0 ) {
	    sio.eprintf("[ERROR] cannot open %s\n",pathname);
	    goto quit;
	}

	/* read a header of primary HDU */
	sz = hdr.read_stream(fh_in);
	if ( sz <= 0 ) {
	    sio.eprintf("[ERROR] hdr.read_stream() failed\n");
	    goto quit;
	}

	/* obtain size of image */
	if (0 <= (idx=hdr.index("NAXIS1"))) naxis1 = hdr.record(idx).lvalue();
	else {
	    sio.eprintf("[ERROR] NAXIS1 is not found\n");
	    goto quit;
	}
	if (0 <= (idx=hdr.index("NAXIS2"))) naxis2 = hdr.record(idx).lvalue();
	else {
	    sio.eprintf("[ERROR] NAXIS2 is not found\n");
	    goto quit;
	}

	/* create instance of wcs structure */
	wcs = wcsinitn(hdr.formatted_string(), NULL);
	if ( iswcs(wcs) == 0 ) {
	    sio.eprintf("[ERROR] failed setting wcs\n");
	    goto quit;
	}

	/* set base width and base height of a box (pixels) */
	/* (wcs->xinc and wcs->yinc keep CDELTx values) */
	base_width = lround(fabs((region_width.atof()/60.0) / wcs->xinc));
	base_height = lround(fabs((region_width.atof()/60.0) / wcs->yinc));

	if ( naxis1 < base_width ) {
	    sio.eprintf("[WARNING] too small NAXIS1\n");
	}
	if ( naxis2 < base_height ) {
	    sio.eprintf("[WARNING] too small NAXIS2\n");
	}

	//sio.printf("image w,h = [%ld,%ld]\n",naxis1,naxis2);
	//sio.printf("base_w,base_h = [%ld,%ld]\n",base_width,base_height);

	/* obtain number of boxes (n_divx * n_divy) */
	n_divx = (naxis1 - 1) / base_width + 1;
	n_divy = (naxis2 - 1) / base_height + 1;

	/* obtain length of jut out */
	n_jutoutx = base_width * n_divx - naxis1;
	n_jutouty = base_height * n_divy - naxis2;

	/* output all box regions */
	rid = 0;
	for ( i=0 ; i < n_divy ; i++ ) {
	    long j;
	    long y0;
	    if ( i == 0 ) {
		if ( base_height <= naxis2 ) y0 = 0;
		else y0 = 0 - n_jutouty / 2;
	    }
	    else y0 = i * base_height - (long)(n_jutouty * ((double)i/(n_divy-1)));
	    for ( j=0 ; j < n_divx ; j++ ) {
		long x0;
		double x_cen, y_cen, lon_cen, lat_cen, max_r, r;
		double lon0, lat0;
		if ( j == 0 ) {
		    if ( base_width <= naxis1 ) x0 = 0;
		    else x0 = 0 - n_jutoutx / 2;
		}
		else x0 = j * base_width - (long)(n_jutoutx * ((double)j/(n_divx-1)));

		/* obtain center position of a box */
		x_cen = x0 + base_width / 2.0;
		y_cen = y0 + base_height / 2.0;
		pix2wcs( wcs, FITS_X_ORIGIN + x_cen, FITS_Y_ORIGIN + y_cen, 
			 &lon_cen, &lat_cen );

		/* search max_r (distance between center and corner) */
		/* from 4-corners */
		pix2wcs( wcs, FITS_X_ORIGIN + x0, FITS_Y_ORIGIN + y0, 
			 &lon0, &lat0 );
		max_r = get_distance_arcmin_of_eq(lon_cen,lat_cen, lon0,lat0);

		pix2wcs( wcs, 
			 FITS_X_ORIGIN + x0 + base_width, FITS_Y_ORIGIN + y0, 
			 &lon0, &lat0 );
		r = get_distance_arcmin_of_eq(lon_cen,lat_cen,lon0,lat0);
		if ( max_r < r ) max_r = r;

		pix2wcs( wcs, 
			 FITS_X_ORIGIN + x0, FITS_Y_ORIGIN + y0 + base_height, 
			 &lon0, &lat0 );
		r = get_distance_arcmin_of_eq(lon_cen,lat_cen,lon0,lat0);
		if ( max_r < r ) max_r = r;

		pix2wcs( wcs, 
			 FITS_X_ORIGIN + x0 + base_width, 
			 FITS_Y_ORIGIN + y0 + base_height,
			 &lon0, &lat0 );
		r = get_distance_arcmin_of_eq(lon_cen,lat_cen,lon0,lat0);
		if ( max_r < r ) max_r = r;

		/* output data for DB */
		sio.printf("%s\t%ld\t%.15g\t%.15g\t%.15g\t%.15g\t%.15g\t",
		       filename.cstr(),rid,lon_cen,lat_cen,
		       cos(lon_cen*CC_DEG_TO_RAD) * cos(lat_cen*CC_DEG_TO_RAD),
		       sin(lon_cen*CC_DEG_TO_RAD) * cos(lat_cen*CC_DEG_TO_RAD),
		       sin(lat_cen*CC_DEG_TO_RAD));
		sio.printf("%.15g\t%ld\t%ld\t%ld\t%ld\n",
			   max_r,x0,y0,base_width,base_height);

		/* draw box in FITS if argv[3] is set */
		if ( 3 < argc ) {
		    long k;
		    const double value = 64;
		    fits_image &pri = fits.image("Primary");
		    /* draw a box */
		    for ( k=0 ; k < base_height ; k+=2 ) {
			pri.assign(value, x0, y0+k);
			pri.assign(value, x0 + base_width, y0+k);
		    }
		    for ( k=0 ; k < base_width ; k+=2 ) {
			pri.assign(value, x0+k, y0);
			pri.assign(value, x0+k, y0+base_height);
		    }
		    /* draw a cross on center */
		    pri.assign(value, (long)x_cen, (long)y_cen);
		    pri.assign(value, (long)x_cen+1, (long)y_cen);
		    pri.assign(value, (long)x_cen+2, (long)y_cen);
		    pri.assign(value, (long)x_cen-1, (long)y_cen);
		    pri.assign(value, (long)x_cen-2, (long)y_cen);
		    pri.assign(value, (long)x_cen, (long)y_cen+1);
		    pri.assign(value, (long)x_cen, (long)y_cen+2);
		    pri.assign(value, (long)x_cen, (long)y_cen-1);
		    pri.assign(value, (long)x_cen, (long)y_cen-2);
		}

		rid ++;
	    }
	}

	/* write FITS file if argv[3] is set */
	if ( 3 < argc ) {
	    const char *to_out = argv[3];
	    if ( fits.write_stream(to_out) < 0 ) {
		sio.eprintf("[ERROR] fits.write_stream() failed\n");
		goto quit;
	    }
	}

    }

    return_status = 0;
 quit:
    if ( wcs != NULL ) wcsfree(wcs);
    return return_status;
}
