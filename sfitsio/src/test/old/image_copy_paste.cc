#include <sli/stdstreamio.h>
#include <sli/fitscc.h>
using namespace sli;

/* TO TEST:
   ./image_copy_paste ../../fits_sample/image/cyg_rgb_crop.fits.gz hoge.fits.gz
 */

int main( int argc, char *argv[] )
{
    fitscc fits;
    stdstreamio sio;

    if ( 1 < argc ) {
        ssize_t sz;
        const char *in_file = argv[1];
        sz = fits.read_stream(in_file);      /* reading file */
    }
    fits_image buf;
    fits.image("Primary").copy(&buf,490,250, 240,250, 0,3);
    //fits.image("Primary").copy(&buf);
    //fits.erase("Primary");
    //fits.append_image("COPY",100, buf);
    fits.image("Primary").paste(buf, 40L,80L,0L);
    fits.image("Primary").add(buf, 300L,80L,0L);
    fits.image("Primary").fill(192.0, 960L,111L, 470L,222L, 2L,1L);
    fits.image("Primary").assign(254, 1,1,0);
    fits.append_image(buf);

    if ( 2 < argc ) {
        ssize_t sz;
        const char *out_file = argv[2];
        sz = fits.write_stream(out_file);    /* writing file */
    }

    return 0;
}
