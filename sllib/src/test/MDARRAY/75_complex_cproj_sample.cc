/**
  example code
  mdarray¥¯¥é¥¹: complex functions
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_complex.h>
#include <math.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    const double m_infval = -1.0 * INFINITY;		/* -INF */
    const double m_zero = 1.0 / m_infval;		/* -0.0 */
    stdstreamio sio;
    dcomplex ac = 2.0 + 3.0 * I;

    sio.printf("*** BEGIN: 75_complex_cproj_sample ***\n");

    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    creal(&ac) = 2.0;
    cimag(&ac) = INFINITY;
    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    creal(&ac) = INFINITY;
    cimag(&ac) = 3.0;
    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    creal(&ac) = 2.0;
    cimag(&ac) = m_infval;
    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    creal(&ac) = INFINITY;
    cimag(&ac) = -3.0;
    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    creal(&ac) = INFINITY;
    cimag(&ac) = 0.0;
    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    creal(&ac) = INFINITY;
    cimag(&ac) = m_zero;
    sio.printf("val = (%.10g,%.10g)  =>  ",creal(ac),cimag(ac));
    sio.printf("cproj: (%.10g,%.10g)\n",creal(cproj(ac)),cimag(cproj(ac)));

    sio.printf("*** END: 75_complex_cproj_sample ***\n");
    sio.printf("\n");

    return 0;
}
