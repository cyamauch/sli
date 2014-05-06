#include <sli/tarray_tstring.h>
#include <sli/fitscc.h>
using namespace sli;

/*
 * A code to test median function for specitic pattern
 *
 */
int main( int argc, char *argv[] )
{
    int return_status = -1;
    stdstreamio sio;					/* standard I/O */
    fitscc fits;					/* FITS object  */
    double stat_results[8];
    int i;

    fits.append_image("Primary", FITS::INDEF,		/* EXTNAME, EXTVER */
		      FITS::SHORT_T, 3000, 1000 );
    fits::short_t *p= (fits::short_t *)fits.image(0L).data_ptr();
    fits.image("Primary").assign_blank(-32768);

    /*
     * �Τ� solaris �� qsort() �Ϥ��Υѥ�����ǥϥޤäƤ��餷����
     * ��ǥ������ͤ� 1�������֤äƤ��ʤ��褦���ȥޥ�����
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 3;
    }

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * �̤Υѥ����� [1] �ǥƥ���: ��ǥ������ͤ�ʿ���ͤˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 2;
    }

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * �̤Υѥ����� [2] �ǥƥ���: ��ǥ������ͤ� 1 �ˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 2;
    }
    p[0] = -32768;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * �̤Υѥ����� [3] �ǥƥ���: ��ǥ������ͤ� 0 �ˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = i % 2;
    }
    p[1] = -32768;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);


    /*
     * �̤Υѥ����� [4] �ǥƥ���: ��ǥ������ͤ� 1.5 �ˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 3;
    p[1] = 0;
    p[2] = 2;
    p[3] = 1;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * �̤Υѥ����� [5] �ǥƥ���: ��ǥ������ͤ� 1 �ˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 2;
    p[1] = 0;
    p[2] = 1;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);


    /*
     * �̤Υѥ����� [6] �ǥƥ���: ��ǥ������ͤ� 0.5 �ˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 1;
    p[1] = 0;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);

    /*
     * �̤Υѥ����� [7] �ǥƥ���: ��ǥ������ͤ� 0 �ˤʤ�٤���
     */
    for ( i=0 ; i < 3000*1000 ; i++ ) {
	p[i] = -32768;
    }
    p[0] = 0;

    sio.printf("performing stat_pixels() ...\n");
    if ( fits.image("Primary").stat_pixels(
	       stat_results, 8,
	       "results=npix,mean,min,max,median"
					       ) < 0 ) {
	sio.eprintf("[ERROR] stat_pixels() failed\n");
	goto quit;
    }
    i = 0;
    sio.printf("npix     = %.15g\n", stat_results[i++]);
    sio.printf("mean     = %.15g\n", stat_results[i++]);
    sio.printf("min      = %.15g\n", stat_results[i++]);
    sio.printf("max      = %.15g\n", stat_results[i++]);
    sio.printf("median   = %.15g\n", stat_results[i++]);


    return_status = 0;

 quit:
    return return_status;
}
