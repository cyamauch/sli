#include <sli/stdstreamio.h>
#include <sli/mdarray_dcomplex.h>
#include <sli/mdarray_complex.h>
using namespace sli;

/**
 * main function
 *
 * @param      argc  Number of arguments
 * @param      argv  Pointer array of arguments
 * @return     Status
 */
int main( int argc, char *argv[] )
{
    stdstreamio sio;
    mdarray_dcomplex cvr0;
    mdarray_dcomplex cvr1;
    size_t i;

    cvr0[0] = 1.2 + 3.4*I;
    cvr0[1] = 5.6 + 7.8*I;

    cvr1[0] = 0.2 + 0.4*I;
    cvr1[1] = 0.6 + 0.8*I;

    sio.printf("bytes of an element = %zd\n", cvr0.bytes());

    sio.printf("[stage1]\n");
    for ( i=0 ; i < cvr0.length() ; i++ ) {
      sio.printf("%zd: %.15g %.15gi\n", i, creal(cvr0[i]), cimag(cvr0[i]));
    }

    cvr0 -= cvr1;

    sio.printf("[stage2]\n");
    for ( i=0 ; i < cvr0.length() ; i++ ) {
      sio.printf("%zd: %.15g %.15gi\n", i, creal(cvr0[i]), cimag(cvr0[i]));
    }

    cvr0 = cvr0 + cvr1;

    sio.printf("[stage3]\n");
    for ( i=0 ; i < cvr0.length() ; i++ ) {
      sio.printf("%zd: %.15g %.15gi\n", i, creal(cvr0[i]), cimag(cvr0[i]));
    }

    cvr0 *= 2.0;

    sio.printf("[stage4]\n");
    for ( i=0 ; i < cvr0.length() ; i++ ) {
      sio.printf("%zd: %.15g %.15gi\n", i, creal(cvr0[i]), cimag(cvr0[i]));
    }

    cvr0 = cvr0 / 2.0;

    sio.printf("[stage5]\n");
    for ( i=0 ; i < cvr0.length() ; i++ ) {
      sio.printf("%zd: %.15g %.15gi\n", i, creal(cvr0[i]), cimag(cvr0[i]));
    }

    cvr0 = ccos(cvr0);

    sio.printf("[stage6]\n");
    for ( i=0 ; i < cvr0.length() ; i++ ) {
      sio.printf("%zd: %.15g %.15gi\n", i, creal(cvr0[i]), cimag(cvr0[i]));
    }

    return 0;
}

