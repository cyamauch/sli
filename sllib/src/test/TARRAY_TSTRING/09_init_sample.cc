/**
  example code
  tarray_tstring�N���X:tarray_tstring &init()
		       tarray_tstring &init(const tarray_tstring &obj)
                       ������z��̊��S������
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    tarray_tstring my_arr;
    const char *tree[] = {"pine", "willow", NULL};
    tarray_tstring my_treeArr = tree;

    /* my_treeArr�ŏ����� */
    my_arr.init(my_treeArr); 
    /* �\�� */
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("[%s]\n", my_arr.cstr(i));
    }

    /* ���S������ */
    my_arr.init(); 
    /* �\�� */
    sio.printf("my_arr.length = [%zu]\n",  my_arr.length());

    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        sio.printf("my_arr.length[%zu] = [%zu]\n", i, my_arr.length(i));
    }

    return 0;
}

