/**
  example code for mdarray_stat.h
*/
#include <sli/stdstreamio.h>
#include <sli/mdarray.h>
#include <sli/mdarray_statistics.h>
#include <math.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;
    const float testval_idl[15] = {65, 63, 67, 64, 
				   68, 62, 70, 66, 
				   68, 67, 69, 71, 
				   66, 65, 70};

    mdarray_float arr0, arr1;
    mdarray_double arr2;
    size_t i,j,k;
    double ref_val, mdev=0, sdev=0;

    sio.printf("*** BEGIN: 76_statistics_sample ***\n");

    arr0.resize_1d(15);
    arr0.put_elements(testval_idl, 15);

    sio.printf("length = %zu\narray = ", arr0.length());
    for ( k=0 ; k < arr0.length() ; k++ ) {
	sio.printf("[%g]", arr0[k]);
    }
    sio.printf("\n");

    arr2 = md_moment(arr0, false, &mdev, &sdev);
    sio.printf("result of md_moment(arr0, false, ...):\n");
    for ( i=0 ; i < arr2.length() ; i++ ) {
	sio.printf("arr2[%zu] = %.10g\n",i,arr2[i]);
    }
    sio.printf("mdev = %.10g\n",mdev);
    sio.printf("sdev = %.10g\n",sdev);
    sio.printf("\n");

    arr2 = md_moment(arr0, true, &mdev, &sdev);
    sio.printf("result of md_moment(arr0, true, ...):\n");
    for ( i=0 ; i < arr2.length() ; i++ ) {
	sio.printf("arr2[%zu] = %.10g\n",i,arr2[i]);
    }
    sio.printf("mdev = %.10g\n",mdev);
    sio.printf("sdev = %.10g\n",sdev);
    sio.printf("\n");

    arr0.resize_3d( 5, 4, 2 );

    for ( i=0 ; i < arr0.layer_length() ; i++ ) {
	for ( j=0 ; j < arr0.row_length() ; j++ ) {
	    for ( k=0 ; k < arr0.col_length() ; k++ ) {
		arr0.assign( ((k+1)*100.0 + (j+1)*10.0 + (i+1)*1.0), k,j,i);
	    }
	}
    }

    sio.printf("length of x,y,z = %zu,%zu,%zu\n",
	       arr0.col_length(),arr0.row_length(),arr0.layer_length());

    ref_val = 0;
    for ( i=0 ; i < arr0.layer_length() ; i++ ) {
	for ( j=0 ; j < arr0.row_length() ; j++ ) {
	    sio.printf("(?,%zu,%zu) ... ",j,i);
	    for ( k=0 ; k < arr0.col_length() ; k++ ) {
		double v = arr0.dvalue(k,j,i);
		sio.printf("[%g]", v);
		ref_val += v;
	    }
	    sio.printf("\n");
	}
    }

    sio.printf("total(ref) = %.10g\n", ref_val);
    sio.printf("total = %.10g\n", md_total(arr0));
    sio.printf("mean(ref) = %.10g\n", ref_val/(double)arr0.length());
    sio.printf("mean = %.10g\n", md_mean(arr0));
    sio.printf("variance = %.10g\n", md_variance(arr0));
    sio.printf("skewness = %.10g\n", md_skewness(arr0,false));
    sio.printf("kurtosis = %.10g\n", md_kurtosis(arr0,false));
    sio.printf("meanabsdev = %.10g\n", md_meanabsdev(arr0));
    sio.printf("stddev = %.10g\n", md_stddev(arr0));
    sio.printf("min = %.10g\n", md_min(arr0));
    sio.printf("max = %.10g\n", md_max(arr0));
    sio.printf("median = %.10g\n", md_median(arr0));
    sio.printf("\n");

    arr2 = md_moment(arr0, false, &mdev, &sdev);
    sio.printf("result of md_moment():\n");
    for ( i=0 ; i < arr2.length() ; i++ ) {
	sio.printf("arr2[%zu] = %.10g\n",i,arr2[i]);
    }
    sio.printf("mdev = %.10g\n",mdev);
    sio.printf("sdev = %.10g\n",sdev);
    sio.printf("\n");

    arr1 = md_min_x(arr0);

    sio.printf("result of md_min_x(arr0): \n");
    for ( i=0 ; i < arr1.layer_length() ; i++ ) {
	for ( j=0 ; j < arr1.row_length() ; j++ ) {
	    sio.printf("(?,%zu,%zu) ... ",j,i);
	    for ( k=0 ; k < arr1.col_length() ; k++ ) {
		sio.printf("[%g]", arr1.dvalue(k,j,i));
	    }
	    sio.printf("\n");
	}
    }

    arr1 = md_max_y(arr0);

    sio.printf("result of md_max_y(arr0): \n");
    for ( i=0 ; i < arr1.layer_length() ; i++ ) {
	for ( j=0 ; j < arr1.row_length() ; j++ ) {
	    sio.printf("(?,%zu,%zu) ... ",j,i);
	    for ( k=0 ; k < arr1.col_length() ; k++ ) {
		sio.printf("[%g]", arr1.dvalue(k,j,i));
	    }
	    sio.printf("\n");
	}
    }

    arr1 = md_mean_z(arr0);

    sio.printf("result of md_mean_z(arr0): \n");
    for ( i=0 ; i < arr1.layer_length() ; i++ ) {
	for ( j=0 ; j < arr1.row_length() ; j++ ) {
	    sio.printf("(?,%zu,%zu) ... ",j,i);
	    for ( k=0 ; k < arr1.col_length() ; k++ ) {
		sio.printf("[%g]", arr1.dvalue(k,j,i));
	    }
	    sio.printf("\n");
	}
    }

    arr1 = md_mean_small_z(arr0);

    sio.printf("result of md_mean_small_z(arr0): \n");
    for ( i=0 ; i < arr1.layer_length() ; i++ ) {
	for ( j=0 ; j < arr1.row_length() ; j++ ) {
	    sio.printf("(?,%zu,%zu) ... ",j,i);
	    for ( k=0 ; k < arr1.col_length() ; k++ ) {
		sio.printf("[%g]", arr1.dvalue(k,j,i));
	    }
	    sio.printf("\n");
	}
    }

    sio.printf("*** END: 76_statistics_sample ***\n");
    sio.printf("\n");

    return 0;
}
