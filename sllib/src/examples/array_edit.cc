#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
using namespace sli;

/**
 * @file   array_edit.cc
 * @brief  多次元配列の編集の例
 */

/*
 * Example for editing array objects
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    /* false is set to disable auto resizing when assign(), etc. are called */
    mdarray_float arr0(false);			/* array of float type */

    /* set length of 2-d array */
    arr0.resize_2d(8,4);

    arr0 = 4.0;
    arr0 *= 2;

    /* display */
    arr0.dprint();

    sio.eprintf("\n");


    /* insert blank elements */
    arr0.insert(0, 1, 2);

    /* resize y */
    arr0.resize(1, 5);

    sio.eprintf("after insert() and resize()\n");
    arr0.dprint();

    sio.eprintf("\n");


    /* resize (x,y,z) */
    arr0.resize_3d(5,3,2);

    sio.eprintf("after resize_3d()\n");
    arr0.dprint();

    return 0;
}
