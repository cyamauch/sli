#include <sli/stdstreamio.h>
#include <sli/fitscc.h>       /* SFITSIO 使用時はこれを includeすればOK */
using namespace sli;

int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;                                /* standard I/O */
    fitscc in_fits;                                 /* FITS object (in) */

    /* FITS ファイルを読む */
    if ( 1 < argc ) {
        const char *in_file = argv[1];
        if ( in_fits.read_stream(in_file) < 0 ) {
            sio.eprintf("[ERROR] in_fits.read_stream(\"%s\") failed\n",in_file);
            goto quit;
        }
    }

    /* ヘッダをコピーした新規 FITSファイルを作成 */
    if ( 2 < argc ) {
        const char *out_file = argv[2];
        fitscc out_fits;                            /* FITS object (out) */

        const fits_image &in_img = in_fits.image("Primary");

        out_fits.append_image(in_img.extname(), in_img.extver(), FITS::FLOAT_T,
                              in_img.col_length(), in_img.row_length());
        out_fits.image("Primary")
                .header_append_records( in_img.header(), false );

        if ( out_fits.write_stream(out_file) < 0 ) {
            sio.eprintf("[ERROR] out_fits.write_stream(\"%s\") failed\n",out_file);
            goto quit;
        }
    }

    return_status = 0;
 quit:
    return return_status;
}
