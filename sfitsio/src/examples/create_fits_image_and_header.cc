#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
#include <math.h>
using namespace sli;

/**
 * @file   create_fits_image_and_header.cc
 * @brief  新規の FITS イメージを作る例．少し長いヘッダを作る場合の参考に
 */

/*
 * A sample code to create a FITS image and header.
 *
 * ./create_fits_image_and_header out_file.fits[.gz or .bz2]
 *
 */

/* size of image */
static const long COL_SIZE = 256;
static const long ROW_SIZE = 256;
static const double SOFT_BIAS = 100.0;

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    long i,j;
    long x_cen, y_cen;


    /*
     *  Determine basic FITS format
     */

    /* Header definition (do not set SIMPLE, BITPIX, NAXIS and EXTENDED) */
    const fits::header_def hdr_defs[] = 
       { {"COMMENT",  "---- USER DEFINED HEADER BEGINS ---------------------"},
	 {"TELESCOP", "'ASTRO-X'",   ""                                      },
	 {"OBSERVAT", "'ISAS/JAXA'", ""                                      },
	 {"CNTTYPE",  "",            "type of data content"                  },
	 {"INSTRUME", "",            ""                                      },
	 {"DETECTOR", "",            ""                                      },
	 {"RA",       "",            "[deg] target position"                 },
	 {"DEC",      "",            "[deg] target position"                 },
	 {"",  "       -----------------------------------------------------"},
	 {"MESSAGE",  "FITS (Flexible Image Transport System) format is "
	              "defined in \"Astronomy and Astrophysics\", volume 376,"
	              " page 359; bibcode: 2001A&A...376..359H",
	              "In SFITSIO, this message is not written automatically. "
	              "Therefore, SFITSIO is not CFITSIO :-)"},
	 {"MESSAGE1", "JAXA's", "should be the same string 1-2"},
	 {"MESSAGE2", "JAXA''s", "should be the same string 1-2"},
	 {"MESSAGE3", "'JAXA's'", "should be the same string 3-4"},
	 {"MESSAGE4", "'JAXA''s'", "should be the same string 3-4"},
	 {"COMMENT",  "-----------------------------------------------------"},
	 {NULL} };

    /* Set FMTTYPE name and its version (You can omit this, if unnecessary) */
    fits.assign_fmttype("ASTRO-X ALL-SKY SURVEY RAW DATA", 101);

    /* Create Primary HDU */
    fits.append_image("Primary", FITS::INDEF,		/* EXTNAME, EXTVER */
		      FITS::FLOAT_T, 0);

    /* Create an image extension */
    fits.append_image("2nd", FITS::INDEF,		/* EXTNAME, EXTVER */
		      FITS::BYTE_T, 0);

    /* Prepare reference of Primary */
    fits_image &primary = fits.image("Primary");


    /*
     *  Handle header of Primary HDU
     */

    /* Initialize Header */
    primary.header_init(hdr_defs);

    /* Modify records */
    primary.header("CNTTYPE").assign("SEP Deep Survey");
    primary.header("INSTRUME").assign("SuperBigCam");
    primary.header("DETECTOR").assign("JAXA's UltraManiacCCD");

    /* Append records */
    primary.header("DATE").assign_system_time();
    primary.header("EXPTIME").assign(300.0);
    primary.header("SOFTBIAS").assign(SOFT_BIAS)
	                      .assign_comment("software bias");
    primary.header_append("MJD");

    /* Insert records before EXPTIME */
    primary.header_insert("EXPTIME", "EQUINOX","2000.0",NULL);

    /* Erase MJD keyword */
    primary.header_erase("MJD");

    /* Append CHECKSUM and DATASUM keywords                          */
    /* (values and comments are automatically set by .write_stream() */
    primary.header_append("CHECKSUM");
    primary.header_append("DATASUM");

    /* fill all uncommented header records using built-in comment dictionary */
    primary.header_fill_blank_comments();

    /* Change header comments */
    primary.header("TELESCOP").assign_comment("satellite name");
    primary.header("OBSERVAT").assign_comment("institute name");


    /*
     *  Handle image of Primary HDU
     */

    /* Set default value and alloc buffer */
    primary.assign_default(SOFT_BIAS);
    primary.resize_2d(COL_SIZE, ROW_SIZE);

    /* METHOD-1: obtain top address of buffer */
#if 0
    /* Obtain address of internal buffer */
    fits::float_t *img_ptr = primary.float_t_ptr();

    /* Set values to buffer directly */
    x_cen = COL_SIZE / 2;
    y_cen = ROW_SIZE / 2;
    for ( i=0 ; i < ROW_SIZE ; i++ ) {
	for ( j=0 ; j < COL_SIZE ; j++ ) {
	    double r2, val;
	    /* simple PSF function */
	    r2 = ((j-x_cen)*(j-x_cen) + (i-y_cen)*(i-y_cen)) / 100.0;
	    val = pow(sin(sqrt(r2)),2)/r2;
	    if ( ! isfinite(val) ) val = 1.0;
	    img_ptr[COL_SIZE * i + j] = SOFT_BIAS + val;
	}
    }
#endif 

    /* METHOD-2: obtain address table for 2d */
#if 1
    /* Obtain address table of internal buffer */
    fits::float_t *const *img_ptr = primary.float_t_ptr_2d(true);

    /* Set values to buffer directly */
    x_cen = COL_SIZE / 2;
    y_cen = ROW_SIZE / 2;
    for ( i=0 ; i < ROW_SIZE ; i++ ) {
	for ( j=0 ; j < COL_SIZE ; j++ ) {
	    double r2, val;
	    /* simple PSF function */
	    r2 = ((j-x_cen)*(j-x_cen) + (i-y_cen)*(i-y_cen)) / 100.0;
	    val = pow(sin(sqrt(r2)),2)/r2;
	    if ( ! isfinite(val) ) val = 1.0;
	    img_ptr[i][j] = SOFT_BIAS + val;
	}
    }
#endif 

    /* Resize again                         */
    /* NOTE: img_ptr is invalid after this! */
    primary.resize_2d(COL_SIZE+32, ROW_SIZE+32);


    /*
     * Save to file...
     */

    if ( 1 < argc ) {
	ssize_t sz;
        const char *out_file = argv[1];
	sz = fits.write_stream(out_file);    /* writing file */
	if ( sz < 0 ) {
	    sio.eprintf("[ERROR] obj.write_stream() failed\n");
	    goto quit;
	}
    }

    return_status = 0;
 quit:
    return return_status;
}
