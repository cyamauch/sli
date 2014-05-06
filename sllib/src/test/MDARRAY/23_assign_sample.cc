/**
  example code
  mdarray���饹: mdarray &assign( double value, ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			          ssize_t idx2 = MDARRAY_INDEF )
                 �ͤ�����
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    sio.printf("start\n");
    mdarray my_mdarr(DOUBLE_ZT,true);
    //1���ܤ����Ǥ��ͤ�����
    my_mdarr.assign(200, 1);
    for (size_t i = 0; i < my_mdarr.length(); i++){
        sio.printf("my_mdarr lvalue(%zu)... [%ld]\n", i, my_mdarr.lvalue(i));
    }


    //�ͼθ����Ĥ��������ͤ������ˤ��ư�����ǧ
    mdarray mdarr1(DOUBLE_ZT,true);
    mdarr1.set_rounding(true);

    mdarr1.assign(100.1, 0);
    mdarr1.assign(200.5, 1);
    mdarr1.assign(300.2, 2);
    mdarr1.assign(400.6, 3);
    mdarr1.assign(500.3, 4);
    //lvalue���Ѥ��ƽ���
    for (size_t i = 0; i < 5; i++){
        sio.printf("mdarr1 lvalue(%zu)... [%ld]\n", i, mdarr1.lvalue(i));
    }

    //d���Ѥ��ƽ���
    for (size_t i = 0; i < 5; i++){
        sio.printf("mdarr1 value(%zu)... [%g]\n", i, mdarr1.d(i));
    }


    //�ͼθ��������������֤Ǥ������ˤ��ư�����ǧ
    mdarray myfix_mdarr(LONG_ZT,true);

    myfix_mdarr.assign(100.1, 0,0);
    myfix_mdarr.assign(200.5, 1,0);
    myfix_mdarr.assign(300.2, 0,1);
    myfix_mdarr.assign(400.6, 1,1);
    myfix_mdarr.assign(500.3, 2,0);

    for (size_t i = 0; i < myfix_mdarr.length(1); i++){
      for (size_t j = 0; j < myfix_mdarr.length(0); j++){
        sio.printf("myfix_mdarr lvalue(%zu,%zu)... [%ld]\n", j, i, myfix_mdarr.lvalue(j,i));
      }
    }

    sio.printf("end\n");

    return 0;
}
