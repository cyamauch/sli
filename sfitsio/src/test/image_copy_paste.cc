#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A test code of image manipulation of SFITSIO
 *
 * ./image_copy_paste out_file.fits[.gz or .bz2]
 *
 */

/* size of an image */
static const long COL_SIZE = 64;
static const long ROW_SIZE = 64;

static void fill_func( double buf[],double v,long n,
		       long x,long y,long z, fits_image *img,void *ptr )
{
    long i;
    for ( i=0 ; i < n ; i++ ) {
	buf[i] = (x+i) + 2*y;
    }
    return;
}

#if 0
static long scan_func( double buf[],long n, long x,long y,long z,
		       const fits_image *img, void *ptr )
{
    long i;
    stdstreamio sio;
    sio.printf("x,y,z=%ld,%ld,%ld\n",x,y,z);
    for ( i=0 ; i < n ; i++ ) {
	sio.printf("[%g]",buf[i]);
    }
    sio.printf("\n");
    return n;
}
#endif

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    long i,j;

    /* Create Primary HDU */
    fits.append_image(NULL, FITS::INDEF,		/* EXTNAME, EXTVER */
		      FITS::SHORT_T, 512, 512, 2);

    /* Prepare reference */
    fits_image &primary = fits.image("Primary");

    primary.fill_via_udf(0.0, &fill_func, NULL, 0,COL_SIZE, 0,ROW_SIZE);

    /* Set values to pixels */
    for ( i=0 ; i < ROW_SIZE ; i++ ) {
	for ( j=0 ; j < COL_SIZE ; j++ ) {
	    double pixel_value = i + 2*j;
	    primary.assign( pixel_value, j+ROW_SIZE, i );
	}
    }

    fits_image buf0;

    primary.copy(&buf0, 0,COL_SIZE, 0,ROW_SIZE, 0,1);

    primary.paste(buf0, 64,64);
    primary.paste(buf0, 128,64);
    primary.paste(buf0, -32,64);

    //primary.cut(&buf0, 0,COL_SIZE, 0,ROW_SIZE, 0,1);

    primary.add(buf0, 128,128,0);
    primary.subtract(buf0, 128,128,1);

    //primary.scan_along_x(&scan_func,NULL, 128,5, 128,3);
    //sio.printf("\n");

    //primary.scan_along_y(&scan_func,NULL, 128,5, 128,3);
    //sio.printf("\n");

    /* test fix_section_args */
    long col_index;
    long col_size;
    long row_index;
    long row_size;
    long layer_index;
    long layer_size;
    //bool col_ok;
    //bool row_ok;
    //bool layer_ok;
    int status;

    sio.printf("TEST of .fix_section_args()\n");
    col_index = 511;
    col_size = 1;
    row_index = 0;
    row_size = 1;
    layer_index = -1;
    layer_size = 2;
    sio.printf("length of image: %ld,%ld,%ld\n",
	       primary.col_length(),primary.row_length(),
	       primary.layer_length());
    sio.printf("col: index=%ld size=%ld\n",col_index,col_size);
    sio.printf("row: index=%ld size=%ld\n",row_index,row_size);
    sio.printf("lay: index=%ld size=%ld\n",layer_index,layer_size);
    status = primary.fix_section_args(&col_index, &col_size,
				      &row_index, &row_size,
				      &layer_index, &layer_size);
    sio.printf("=>col: index=%ld size=%ld\n",col_index,col_size);
    sio.printf("=>row: index=%ld size=%ld\n",row_index,row_size);
    sio.printf("=>lay: index=%ld size=%ld\n",layer_index,layer_size);
    //sio.printf("OK: col,row,lay: = %d,%d,%d\n",
    //	       (int)col_ok,(int)row_ok,(int)layer_ok);
    sio.printf("status = %d\n",status);

    /* Save to file... */
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
