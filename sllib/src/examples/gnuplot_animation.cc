#include <sli/pipestreamio.h>
using namespace sli;

/**
 * @file   gnuplot_animation.cc
 * @brief  gnuplot �ǥѥ�����³�������˥᡼������ɽ�����륳����
 */

int main( int argc, char *argv[] )
{
    pipestreamio pout;
    int i;

    pout.open("w", "gnuplot");
    pout.printf("set isosamples 48\n");
    for ( i=0 ; i < 1000 ; i++ ) {
        pout.printf("splot sin(%g + sqrt(x*x+y*y))\n", i/10.0);
        pout.flush();
    }
    pout.close();

    return 0;
}
