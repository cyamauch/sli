#include <stdio.h>
#include <sli/fitscc.h>        /* required by every source that uses SFITSIO */
using namespace sli;

/**
 * @file   create_fits_image.cc
 * @brief  新規の FITS イメージを作る簡単な例
 */

int main( int argc, char *argv[] )
{
    fitscc fits;                 /* fitscc object that expresses a FITS file */
    const long width = 300, height = 300;                   /* size of image */
    long len_written, i, j;

    /* Create the Primary HDU */
    fits.append_image("Primary",0, FITS::SHORT_T, width,height);
    fits_image &pri = fits.image("Primary");

    /* Add an "EXPOSURE" header record */
    pri.header("EXPOSURE").assign(1500.).assign_comment("Total Exposure Time");

    /* Set BZERO, BSCALE and initialize */
    pri.assign_bzero(32768.0).assign_bscale(1.0).fill(0);
    pri.assign_blank(65535);
    pri.assign_bunit("ADU");

    /* Set the values in the image with a linear ramp function */
    for ( j=0 ; j < height ; j++ ) {
        for ( i=0 ; i < width ; i++ ) pri.assign(i + j, i,j);
    }

    /* Output a FITS file */
    len_written = fits.write_stream("testfile.fits");
    printf("saved %ld bytes.\n", len_written);

    return (len_written < 0 ? -1 : 0);
}
