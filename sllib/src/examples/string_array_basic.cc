#include <sli/tarray_tstring.h>
using namespace sli;

/**
 * @file   string_array_basic.cc
 * @brief  ʸ��������ΰ������δ��ܤ򼨤���������
 */

/*
 * Basic examples of string array
 */

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    const char *group1[] = {"sakura", "mizuho", NULL};
    tarray_tstring my_arr = group1;		/* string array object */
    size_t i;

    my_arr[2] = "fuji";
    my_arr[3] = "hayabusa";

    for ( i=0 ; i < my_arr.length() ; i++ ) {                   /* ɽ�� */
        sio.printf("%zu: [%s]\n", i, my_arr[i].cstr());
    }

    my_arr.dprint();

    return 0;
}
