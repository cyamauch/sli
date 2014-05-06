/**
  example code
  mdarrayクラス: void *data_ptr()
    		 void *data_ptr( ssize_t idx0,
		 		 ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF )
                 指定要素のアドレスの取得
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    size_t nx[] = {2, 2};
    mdarray my_fmdarr(FLOAT_ZT, true);
    my_fmdarr.resize(nx,2,true);

    my_fmdarr.f(0,0) = 1000.8;
    my_fmdarr.f(1,0) = 2000.6;
    my_fmdarr.f(0,1) = 3000.3;
    my_fmdarr.f(1,1) = 4000.5;

    const float *mycarray_ptr = (float *)my_fmdarr.data_ptr(0, 1);
    sio.printf("*** my_fmdarr data_ptr[0] ---> [%g] *** \n",  mycarray_ptr[0]);
    sio.printf("*** my_fmdarr data_ptr[1] ---> [%g] *** \n",  mycarray_ptr[1]);

    /* sio.printf("*** data_ptr *** \n"); */

    float *dest_ptr;
    dest_ptr = (float *)my_fmdarr.data_ptr(1,0);
    *dest_ptr = 5000;

    for (size_t j = 0; j < my_fmdarr.length(1); j++){
      for (size_t i = 0; i < my_fmdarr.length(0); i++){
        sio.printf("my_fmdarr value(%zu,%zu)... [%g]\n",
	    	    i, j, my_fmdarr.f_cs(i, j));
      }
    }

    sio.printf("end\n");

    return 0;
}
