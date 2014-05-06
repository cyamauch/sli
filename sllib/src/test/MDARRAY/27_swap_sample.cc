/**
  example code
  mdarray���饹: mdarray &swap( ssize_t idx_src, size_t len, ssize_t idx_dst )
    		 mdarray &swap( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst )
                 ���󥪥֥����������Ǵ֤��ͤ������ؤ�
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    size_t nx[] = {2, 2};
    mdarray my_cmdarr(UCHAR_ZT, true);
    my_cmdarr.resize(nx,2,true);

    unsigned char my_char[] = {51, 52, 101, 102};
    my_cmdarr.putdata((const void *)my_char, sizeof(my_char));
    //���������ǧ�ΰٽ���
    for (size_t j = 0; j < my_cmdarr.length(1); j++){
        for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    my_cmdarr.swap( 1, 0, 1, 1 );
    //swap����ͤ����
    for (size_t j = 0; j < my_cmdarr.length(1); j++){
        for (size_t i = 0; i < my_cmdarr.length(0); i++){
            sio.printf("=> my_cmdarr value(%zu,%zu)... [%hhu]\n", i, j, my_cmdarr.c(i, j));
        }
    }

    return 0;
}
