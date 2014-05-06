/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-24 14:45:32 cyamauch> */

#ifndef _SLI__FITS_IMAGE_STATISTICS_H
#define _SLI__FITS_IMAGE_STATISTICS_H 1

/*
 * statistics package for fits_image class
 */

/**
 * @file   fits_image_statistics.h
 * @brief  fits_image クラスで使用可能な統計用関数
 * @attention  パフォーマンスを求めるデータ解析の場合，SLLIB の 
 *             mdarray_statistics.h をご利用ください．
 * @attention  C++ 標準ライブラリの <cmath> を使う場合，マクロ SLI__USE_CMATH
 *             を定義してから mdarray_statistics.h を include してください．
 */

#include "fits_image.h"

namespace sli
{

#ifdef SLI__USE_CMATH
    using std::fabs;
    using std::pow;
    using std::sqrt;
    using std::isfinite;
#else
#undef fabs
#undef pow
#undef sqrt
    using ::fabs;
    using ::pow;
    using ::sqrt;
#endif

/*****************************************************************************/
/****  N P I X  **************************************************************/
/*****************************************************************************/

/* public function to obtain NPIX for all elements */
inline static double fitsim_npix( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v;
	long n_valid, n, i;
	long x, y, z;

	/* get npix */
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		}
	    }
	}
	ret_value = n_valid;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/*****************************************************************************/
/****  T O T A L  ************************************************************/
/*****************************************************************************/

/* public function to obtain TOTAL for all elements */
inline static double fitsim_total( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get total */
	sum = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	}
	if ( 0 < n_valid ) ret_value = sum;
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain TOTAL along x axis */
/* and return an array whose x_length = 1       */
inline static fits_image fitsim_total_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get total */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = sum;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along y axis */
/* and return an array whose y_length = 1       */
inline static fits_image fitsim_total_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get total */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = sum;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along z axis */
/* and return an array whose z_length = 1       */
inline static fits_image fitsim_total_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get total */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = sum;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along z axis */
/* optimized for small length of z.             */
/* and return an array whose z_length = 1       */
inline static fits_image fitsim_total_small_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1), 
				       0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *v_ptr;
	double v, sum;
	long n_valid, n_z, n_x, i, j;
	long x, y, z;

	/* get total */
	while ( (v_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
	    for ( j=0 ; j < n_x ; j++ ) {		/* loop for x */
		sum = 0;
		n_valid = 0;
		for ( i=0 ; i < n_z ; i++ ) {		/* loop for z */
		    v = *v_ptr;
		    v_ptr ++;
		    if ( isfinite(v) ) {
			n_valid ++;
			sum += v;
		    }
		}
		if ( 0 < n_valid ) *dest_ptr = sum;
		else *dest_ptr = NAN;
		dest_ptr ++;
	    }
	}
    }
    obj.end_scan_zx_planes();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  M E A N  **************************************************************/
/*****************************************************************************/

/* public function to obtain MEAN for all elements */
inline static double fitsim_mean( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get mean */
	sum = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	}
	if ( 0 < n_valid ) ret_value = sum / (double)n_valid;
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain MEAN along x axis */
/* and return an array whose x_length = 1      */
inline static fits_image fitsim_mean_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get mean */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = sum / (double)n_valid;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along y axis */
/* and return an array whose y_length = 1      */
inline static fits_image fitsim_mean_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get mean */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = sum / (double)n_valid;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along z axis */
/* and return an array whose z_length = 1      */
inline static fits_image fitsim_mean_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum;
	long n_valid, n, i;
	long x, y, z;

	/* get mean */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = sum / (double)n_valid;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along z axis */
/* optimized for small length of z.            */
/* and return an array whose z_length = 1      */
inline static fits_image fitsim_mean_small_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1), 
				       0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *v_ptr;
	double v, sum;
	long n_valid, n_z, n_x, i, j;
	long x, y, z;

	/* get mean */
	while ( (v_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
	    for ( j=0 ; j < n_x ; j++ ) {		/* loop for x */
		sum = 0;
		n_valid = 0;
		for ( i=0 ; i < n_z ; i++ ) {		/* loop for z */
		    v = *v_ptr;
		    v_ptr ++;
		    if ( isfinite(v) ) {
			n_valid ++;
			sum += v;
		    }
		}
		if ( 0 < n_valid ) *dest_ptr = sum / (double)n_valid;
		else *dest_ptr = NAN;
		dest_ptr ++;
	    }
	}
    }
    obj.end_scan_zx_planes();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  M E A N A B S D E V  **************************************************/
/*****************************************************************************/

/* public function to obtain MEANABSDEV for all elements */
inline static double fitsim_meanabsdev( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */
    double mean;

    mean = fitsim_mean(obj);

    if ( ! isfinite(mean) ) goto quit;

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, abs_sum;
	long n_valid, n, i;
	long x, y, z;

	/* get meanabsdev */
	abs_sum = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    abs_sum += fabs(v - mean);
		}
	    }
	}
	if ( 0 < n_valid ) ret_value = abs_sum / (double)n_valid;
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

 quit:
    return ret_value;
}

/* public function to obtain MEANABSDEV along x axis */
/* and return an array whose x_length = 1            */
inline static fits_image fitsim_meanabsdev_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, mean, abs_sum;
	long n_valid, n, i;
	long x, y, z;

	/* get meanabsdev */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {		/* 1st loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) {
		mean = sum / (double)n_valid;
		abs_sum = 0;
		for ( i=0 ; i < n ; i++ ) {	/* 2nd loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			abs_sum += fabs(v - mean);
		    }
		}
		*dest_ptr = abs_sum / (double)n_valid;
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEANABSDEV along y axis */
/* and return an array whose y_length = 1            */
inline static fits_image fitsim_meanabsdev_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum, mean, abs_sum;
	long n_valid, n, i;
	long x, y, z;

	/* get meanabsdev */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {		/* 1st loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) {
		mean = sum / (double)n_valid;
		abs_sum = 0;
		for ( i=0 ; i < n ; i++ ) {	/* 2nd loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			abs_sum += fabs(v - mean);
		    }
		}
		*dest_ptr = abs_sum / (double)n_valid;
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEANABSDEV along z axis */
/* and return an array whose z_length = 1            */
inline static fits_image fitsim_meanabsdev_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum, mean, abs_sum;
	long n_valid, n, i;
	long x, y, z;

	/* get meanabsdev */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {		/* 1st loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		}
	    }
	    if ( 0 < n_valid ) {
		mean = sum / (double)n_valid;
		abs_sum = 0;
		for ( i=0 ; i < n ; i++ ) {	/* 2nd loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			abs_sum += fabs(v - mean);
		    }
		}
		*dest_ptr = abs_sum / (double)n_valid;
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  V A R I A N C E  ******************************************************/
/*****************************************************************************/

/* public function to obtain VARIANCE for all elements */
inline static double fitsim_variance( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get variance */
	sum = 0;
	sum2 = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	}
	if ( 0 < n_valid ) {
	    double mean = sum / (double)n_valid;
	    /* get result */
	    ret_value = (sum2 - 2 * mean * sum + mean * mean * n_valid)
		        / (double)(n_valid - 1);
	}
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain VARIANCE along x axis */
/* and return an array whose x_length = 1          */
inline static fits_image fitsim_variance_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get variance */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)n_valid;
		/* get result */
		*dest_ptr = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			    / (double)(n_valid - 1);
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain VARIANCE along y axis */
/* and return an array whose y_length = 1          */
inline static fits_image fitsim_variance_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get variance */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)n_valid;
		/* get result */
		*dest_ptr = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			    / (double)(n_valid - 1);
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain VARIANCE along z axis */
/* and return an array whose z_length = 1          */
inline static fits_image fitsim_variance_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get variance */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)n_valid;
		/* get result */
		*dest_ptr = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			    / (double)(n_valid - 1);
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  S K E W N E S S  ******************************************************/
/*****************************************************************************/

/* public function to obtain SKEWNESS for all elements */
inline static double fitsim_skewness( const fits_image &obj, bool minus1 )
{
    double ret_value = NAN;			/* return value */
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2, sum3;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	sum = 0;
	sum2 = 0;
	sum3 = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		}
	    }
	}
	if ( 0 < n_valid ) {
	    double mean = sum / (double)(n_valid);
	    double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			      / (double)(n_valid - 1);
	    double stddev = sqrt(variance);
	    double s_sum = sum3 - 3 * mean * sum2 
			       + 3 * mean * mean * sum - pow(mean,3) * n_valid;
	    /* get result */
	    ret_value = (s_sum / pow(stddev,3)) / (double)(n_valid - m1);
	}
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain SKEWNESS along x axis */
/* and return an array whose x_length = 1          */
inline static fits_image fitsim_skewness_x( const fits_image &obj, bool minus1 )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2, sum3;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    sum3 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)(n_valid);
		double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			          / (double)(n_valid - 1);
		double stddev = sqrt(variance);
		double s_sum = sum3 - 3 * mean * sum2 
			       + 3 * mean * mean * sum - pow(mean,3) * n_valid;
		/* get result */
		*dest_ptr = (s_sum / pow(stddev,3)) / (double)(n_valid - m1);
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain SKEWNESS along y axis */
/* and return an array whose y_length = 1          */
inline static fits_image fitsim_skewness_y( const fits_image &obj, bool minus1 )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum, sum2, sum3;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    sum3 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)(n_valid);
		double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			          / (double)(n_valid - 1);
		double stddev = sqrt(variance);
		double s_sum = sum3 - 3 * mean * sum2 
			       + 3 * mean * mean * sum - pow(mean,3) * n_valid;
		/* get result */
		*dest_ptr = (s_sum / pow(stddev,3)) / (double)(n_valid - m1);
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain SKEWNESS along z axis */
/* and return an array whose z_length = 1          */
inline static fits_image fitsim_skewness_z( const fits_image &obj, bool minus1 )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum, sum2, sum3;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    sum3 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)(n_valid);
		double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			          / (double)(n_valid - 1);
		double stddev = sqrt(variance);
		double s_sum = sum3 - 3 * mean * sum2 
			       + 3 * mean * mean * sum - pow(mean,3) * n_valid;
		/* get result */
		*dest_ptr = (s_sum / pow(stddev,3)) / (double)(n_valid - m1);
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  K U R T O S I S  ******************************************************/
/*****************************************************************************/

/* public function to obtain KURTOSIS for all elements */
inline static double fitsim_kurtosis( const fits_image &obj, bool minus1 )
{
    double ret_value = NAN;			/* return value */
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2, sum3, sum4;
	long n_valid, n, i;
	long x, y, z;

	/* get kurtosis */
	sum = 0;
	sum2 = 0;
	sum3 = 0;
	sum4 = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		    sum4 += v * v * v * v;
		}
	    }
	}
	if ( 0 < n_valid ) {
	    double mean = sum / (double)(n_valid);
	    double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			      / (double)(n_valid - 1);
	    double stddev = sqrt(variance);
	    double k_sum = sum4 - 4 * mean * sum3 
			   + 6 * mean * mean * sum2 
			   - 4 * pow(mean,3) * sum + pow(mean,4) * n_valid;
	    /* get result */
	    ret_value = 
		      ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
	}
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain KURTOSIS along x axis */
/* and return an array whose x_length = 1          */
inline static fits_image fitsim_kurtosis_x( const fits_image &obj, bool minus1 )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2, sum3, sum4;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    sum3 = 0;
	    sum4 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		    sum4 += v * v * v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)(n_valid);
		double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			          / (double)(n_valid - 1);
		double stddev = sqrt(variance);
		double k_sum = sum4 - 4 * mean * sum3 
			       + 6 * mean * mean * sum2 
			       - 4 * pow(mean,3) * sum + pow(mean,4) * n_valid;
		/* get result */
		*dest_ptr = 
		      ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain KURTOSIS along y axis */
/* and return an array whose y_length = 1          */
inline static fits_image fitsim_kurtosis_y( const fits_image &obj, bool minus1 )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum, sum2, sum3, sum4;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    sum3 = 0;
	    sum4 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		    sum4 += v * v * v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)(n_valid);
		double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			          / (double)(n_valid - 1);
		double stddev = sqrt(variance);
		double k_sum = sum4 - 4 * mean * sum3 
			       + 6 * mean * mean * sum2 
			       - 4 * pow(mean,3) * sum + pow(mean,4) * n_valid;
		/* get result */
		*dest_ptr = 
		      ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain KURTOSIS along z axis */
/* and return an array whose z_length = 1          */
inline static fits_image fitsim_kurtosis_z( const fits_image &obj, bool minus1 )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;
    long m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum, sum2, sum3, sum4;
	long n_valid, n, i;
	long x, y, z;

	/* get skewness */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    sum3 = 0;
	    sum4 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		    sum4 += v * v * v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)(n_valid);
		double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			          / (double)(n_valid - 1);
		double stddev = sqrt(variance);
		double k_sum = sum4 - 4 * mean * sum3 
			       + 6 * mean * mean * sum2 
			       - 4 * pow(mean,3) * sum + pow(mean,4) * n_valid;
		/* get result */
		*dest_ptr = 
		      ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  S T D D E V  **********************************************************/
/*****************************************************************************/

/* public function to obtain STDDEV for all elements */
inline static double fitsim_stddev( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */
    double variance;

    variance = fitsim_variance(obj);

    if ( isfinite(variance) ) ret_value = sqrt(variance);

    return ret_value;
}

/* public function to obtain STDDEV along x axis */
/* and return an array whose x_length = 1        */
inline static fits_image fitsim_stddev_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get stddev */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)n_valid;
		/* get result */
		*dest_ptr = 
			sqrt( (sum2 - 2 * mean * sum + mean * mean * n_valid)
			      / (double)(n_valid - 1) );
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along y axis */
/* and return an array whose y_length = 1        */
inline static fits_image fitsim_stddev_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get stddev */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)n_valid;
		/* get result */
		*dest_ptr = 
			sqrt( (sum2 - 2 * mean * sum + mean * mean * n_valid)
			      / (double)(n_valid - 1) );
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along z axis */
/* and return an array whose z_length = 1        */
inline static fits_image fitsim_stddev_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, sum, sum2;
	long n_valid, n, i;
	long x, y, z;

	/* get stddev */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    sum = 0;
	    sum2 = 0;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		}
	    }
	    if ( 0 < n_valid ) {
		double mean = sum / (double)n_valid;
		/* get result */
		*dest_ptr = 
			sqrt( (sum2 - 2 * mean * sum + mean * mean * n_valid)
			      / (double)(n_valid - 1) );
	    }
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.begin_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  M I N  ****************************************************************/
/*****************************************************************************/

/* public function to obtain MIN for all elements */
inline static double fitsim_min( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, min;
	long n_valid, n, i;
	long x, y, z;

	/* get min */
	min = NAN;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    if ( isfinite(min) ) {
		for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			n_valid ++;
			if ( v < min ) min = v;
		    }
		}
	    }
	    else {
		for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			n_valid ++;
			if ( isfinite(min) ) {
			    if ( v < min ) min = v;
			}
			else min = v;
		    }
		}
	    }
	}
	if ( 0 < n_valid ) ret_value = min;
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain MIN along x axis */
/* and return an array whose x_length = 1     */
inline static fits_image fitsim_min_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, min;
	long n_valid, n, i;
	long x, y, z;

	/* get min */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    min = NAN;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    if ( isfinite(min) ) {
			if ( v < min ) min = v;
		    }
		    else min = v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = min;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along y axis */
/* and return an array whose y_length = 1     */
inline static fits_image fitsim_min_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, min;
	long n_valid, n, i;
	long x, y, z;

	/* get min */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    min = NAN;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    if ( isfinite(min) ) {
			if ( v < min ) min = v;
		    }
		    else min = v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = min;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along z axis */
/* and return an array whose z_length = 1     */
inline static fits_image fitsim_min_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, min;
	long n_valid, n, i;
	long x, y, z;

	/* get min */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    min = NAN;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    if ( isfinite(min) ) {
			if ( v < min ) min = v;
		    }
		    else min = v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = min;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along z axis */
/* optimized for small length of z.           */
/* and return an array whose z_length = 1     */
inline static fits_image fitsim_min_small_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1), 
				       0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *v_ptr;
	double v, min;
	long n_valid, n_z, n_x, i, j;
	long x, y, z;

	/* get min */
	while ( (v_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
	    for ( j=0 ; j < n_x ; j++ ) {		/* loop for x */
		min = NAN;
		n_valid = 0;
		for ( i=0 ; i < n_z ; i++ ) {		/* loop for z */
		    v = *v_ptr;
		    v_ptr ++;
		    if ( isfinite(v) ) {
			n_valid ++;
			if ( isfinite(min) ) {
			    if ( v < min ) min = v;
			}
			else min = v;
		    }
		}
		if ( 0 < n_valid ) *dest_ptr = min;
		else *dest_ptr = NAN;
		dest_ptr ++;
	    }
	}
    }
    obj.end_scan_zx_planes();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  M A X  ****************************************************************/
/*****************************************************************************/

/* public function to obtain MAX for all elements */
inline static double fitsim_max( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, max;
	long n_valid, n, i;
	long x, y, z;

	/* get max */
	max = NAN;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    if ( isfinite(max) ) {
		for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			n_valid ++;
			if ( max < v ) max = v;
		    }
		}
	    }
	    else {
		for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		    v = vals[i];
		    if ( isfinite(v) ) {
			n_valid ++;
			if ( isfinite(max) ) {
			    if ( max < v ) max = v;
			}
			else max = v;
		    }
		}
	    }
	}
	if ( 0 < n_valid ) ret_value = max;
	else ret_value = NAN;
    }
    obj.end_scan_along_x();

    return ret_value;
}

/* public function to obtain MAX along x axis */
/* and return an array whose x_length = 1     */
inline static fits_image fitsim_max_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, max;
	long n_valid, n, i;
	long x, y, z;

	/* get max */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    max = NAN;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    if ( isfinite(max) ) {
			if ( max < v ) max = v;
		    }
		    else max = v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = max;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along y axis */
/* and return an array whose y_length = 1     */
inline static fits_image fitsim_max_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, max;
	long n_valid, n, i;
	long x, y, z;

	/* get max */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    max = NAN;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    if ( isfinite(max) ) {
			if ( max < v ) max = v;
		    }
		    else max = v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = max;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along z axis */
/* and return an array whose z_length = 1     */
inline static fits_image fitsim_max_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, max;
	long n_valid, n, i;
	long x, y, z;

	/* get max */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    max = NAN;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    if ( isfinite(max) ) {
			if ( max < v ) max = v;
		    }
		    else max = v;
		}
	    }
	    if ( 0 < n_valid ) *dest_ptr = max;
	    else *dest_ptr = NAN;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along z axis */
/* optimized for small length of z.           */
/* and return an array whose z_length = 1     */
inline static fits_image fitsim_max_small_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1), 
				       0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *v_ptr;
	double v, max;
	long n_valid, n_z, n_x, i, j;
	long x, y, z;

	/* get max */
	while ( (v_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
	    for ( j=0 ; j < n_x ; j++ ) {		/* loop for x */
		max = NAN;
		n_valid = 0;
		for ( i=0 ; i < n_z ; i++ ) {		/* loop for z */
		    v = *v_ptr;
		    v_ptr ++;
		    if ( isfinite(v) ) {
			n_valid ++;
			if ( isfinite(max) ) {
			    if ( max < v ) max = v;
			}
			else max = v;
		    }
		}
		if ( 0 < n_valid ) *dest_ptr = max;
		else *dest_ptr = NAN;
		dest_ptr ++;
	    }
	}
    }
    obj.end_scan_zx_planes();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  M E D I A N  **********************************************************/
/*****************************************************************************/

/*
 * 正確な median (IRAF の「インチキmedian」こと midpt ではない) を求める．
 *
 * quick sort のアルゴリズムを参考に，必要な部分だけ並べかえるアルゴリズム
 * を使っている．std::sort() でやるより 2倍程度速く，IRAF の midpt 並の速度
 * で計算できる．やってる事は八木先生謹製のネコソフト median 関数とほぼ同じ．
 *
 * リファレンス:
 * http://www.sciencedirect.com/science/article/pii/S0022000073800339
 *
 * buf_ptr:  配列の先頭アドレス．バッファには NAN や INF を入れてはいけない．
 *           なお，バッファの内容は上書きされるので注意．
 * buf_len:  配列の個数
 *
 */
/* デバッグのスイッチ */
//#define DEBUG_GET_MEDIAN 1
template <class datatype>
static datatype get_median( datatype *buf_ptr, const size_t buf_len )
{
    datatype ret_value = NAN;
    size_t src_start, src_end1;			/* ループの開始，終了+1 */
    size_t buf_pos_right1;			/* ループ終了時の分岐点 */
    datatype pivot;

    if ( buf_ptr == NULL ) goto quit;
    if ( buf_len == 0 ) goto quit;

    src_start = 0;
    src_end1 = buf_len;

    pivot = buf_ptr[0];

    /*
     * メインループ
     */
    while ( 1 ) {
	datatype v, v1;
	size_t i, j;

#ifdef DEBUG_GET_MEDIAN
	err_report1(__FUNCTION__,"DEBUG", "buf_len/2: %zu", buf_len / 2);
	err_report1(__FUNCTION__,"DEBUG", "src_start: %zu", src_start);
	err_report1(__FUNCTION__,"DEBUG", "src_end1: %zu", src_end1);
	err_report1(__FUNCTION__,"DEBUG", "pivot: %.15g", pivot);
#endif

	j = src_end1;
	for ( i=src_start ; i < j ; i++ ) {
	    v = buf_ptr[i];
	    /* この不等号は `<=' である必要がある */
	    if ( v <= pivot ) {
		/* ここで次の pivot 値の候補をとっても良いが， */
		/* こっちの区間が捨てられる可能性があるからなぁ．．． */
	    }
	    else {
		while ( i < j ) {
		    /* ここは i==j の可能性があるが，OK */
		    j--;
		    v1 = buf_ptr[j];
		    if ( v1 <= pivot ) {
			buf_ptr[j] = v;
			buf_ptr[i] = v1;
			break;
		    }
		}
	    }
	}
	/* 注意: ここで i を取得してはいけない */
	buf_pos_right1 = j;

	/*
	 * 次回の試行区間を決定する
	 */

	/*
	 * pivot 値がほぼ median 値としてヒットしてしまった場合
	 */
	if ( buf_len % 2 == 0 && buf_pos_right1 == buf_len / 2 ) {
	    size_t m_pos;
	    datatype l_max, r_min;
	    /*
	     * 更新されたバッファにおいて，
	     * 左側区間では最大値を，右側区間では最小値をとる
	     */
	    m_pos = buf_len / 2;
	    /* */
	    j = src_start;
	    l_max = buf_ptr[j];
	    j++;
	    for ( ; j < m_pos ; j++ ) {
		if ( l_max < buf_ptr[j] ) l_max = buf_ptr[j];
	    }
	    /* */
	    j = m_pos;
	    r_min = buf_ptr[j];
	    j++;
	    for ( ; j < src_end1 ; j++ ) {
		if ( buf_ptr[j] < r_min ) r_min = buf_ptr[j];
	    }
	    /* 左側の最大，右側の最小，の平均をとって，終了 */
	    ret_value = (l_max + r_min) / 2.0;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [X]");
#endif
	    goto quit;
	}
	/*  pivot が左側にある場合
	 *                   piv    |
	 *  |_________________+____________________________|
	 *      do not use                 use
	 */
	/* ここの不等号は，要素数が奇数の場合に `<=' である必要がある．  */
	/* 要素数が偶数の時に == な場合は，↑の if 文の条件にかかるので，*/
	/* ここにはこない．                                              */
	else if ( buf_pos_right1 <= buf_len / 2 ) {
	    pivot = buf_ptr[buf_pos_right1];
	    src_start = buf_pos_right1;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [A]");
#endif
	}
	/*  pivot が右側にある場合
	 *                          |  piv
	 *  |___________________________+___________________|
	 *             use                   do not use
	 */
	else {

	    /* pivot 値よりすべてが左側にきた場合 */
	    if ( src_end1 == buf_pos_right1 ) {
		/* 次の pivot 値を見つける */
		for ( j=src_start ; j < src_end1 ; j++ ) {
		    if ( buf_ptr[j] != pivot ) {
			pivot = buf_ptr[j];
			break;
		    }
		}
		if ( j == src_end1 ) {
		    ret_value = buf_ptr[src_start];
		    goto quit;				/* finish! */
		}
	    }
	    else {
		pivot = buf_ptr[src_start];
	    }

	    /* */
	    src_end1 = buf_pos_right1;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [B]");
#endif
	}

#ifdef DEBUG_GET_MEDIAN
	err_report(__FUNCTION__,"DEBUG", "-----------------");
#endif

    }

#ifdef DEBUG_GET_MEDIAN
    err_report1(__FUNCTION__,"DEBUG", 
		"ret_value of median: %.15g", ret_value);
#endif


 quit:
    return ret_value;
}


/* public function to obtain MEDIAN for all elements */
inline static double fitsim_median( const fits_image &obj )
{
    double ret_value = NAN;			/* return value */

    /* start scan */
    if ( 0 < obj.begin_scan_a_cube() ) {
	long n_valid, n, n_x, n_y, n_z, i;
	long x, y, z;

	/* use FLOAT for 8-byte int, 16-byte int and float */
	if ( (obj.type() == FITS::BYTE_T || obj.type() == FITS::SHORT_T ||
	      obj.type() == FITS::FLOAT_T) && obj.bscale() == 1.0 ) {
	    float *vals;
	    float v, median;

	    /* get median */
	    vals = obj.scan_a_cube_f(&n_x,&n_y,&n_z,&x,&y,&z);
	    n = n_x * n_y * n_z;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {		/* loop for all */
		v = vals[i];
		if ( isfinite(v) ) {
		    vals[n_valid] = v;		/* accumulate finite values */
		    n_valid ++;
		}
	    }
	    if ( 2 < n_valid ) median = get_median(vals, n_valid);
	    else if ( n_valid == 2 ) median = (vals[0] + vals[1]) / 2.0;
	    else if ( n_valid == 1 ) median = vals[0];
	    else median = NAN;

	    ret_value = median;
	}
	else {
	    double *vals;
	    double v, median;

	    /* get median */
	    vals = obj.scan_a_cube(&n_x,&n_y,&n_z,&x,&y,&z);
	    n = n_x * n_y * n_z;
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {		/* loop for all */
		v = vals[i];
		if ( isfinite(v) ) {
		    vals[n_valid] = v;		/* accumulate finite values */
		    n_valid ++;
		}
	    }
	    if ( 2 < n_valid ) median = get_median(vals, n_valid);
	    else if ( n_valid == 2 ) median = (vals[0] + vals[1]) / 2.0;
	    else if ( n_valid == 1 ) median = vals[0];
	    else median = NAN;

	    ret_value = median;
	}

    }
    obj.end_scan_a_cube();

    return ret_value;
}

/* public function to obtain MEDIAN along x axis */
/* and return an array whose x_length = 1        */
inline static fits_image fitsim_median_x( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, median;
	long n_valid, n, i;
	long x, y, z;

	/* get median */
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    vals[n_valid] = v;		/* accumulate finite values */
		    n_valid ++;
		}
	    }
	    if ( 2 < n_valid ) median = get_median(vals, n_valid);
	    else if ( n_valid == 2 ) median = (vals[0] + vals[1]) / 2.0;
	    else if ( n_valid == 1 ) median = vals[0];
	    else median = NAN;
	    *dest_ptr = median;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_x();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along y axis */
/* and return an array whose y_length = 1        */
inline static fits_image fitsim_median_y( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {
	double *vals;
	double v, median;
	long n_valid, n, i;
	long x, y, z;

	/* get median */
	while ( (vals=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    vals[n_valid] = v;		/* accumulate finite values */
		    n_valid ++;
		}
	    }
	    if ( 2 < n_valid ) median = get_median(vals, n_valid);
	    else if ( n_valid == 2 ) median = (vals[0] + vals[1]) / 2.0;
	    else if ( n_valid == 1 ) median = vals[0];
	    else median = NAN;
	    *dest_ptr = median;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_y();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along z axis */
/* and return an array whose z_length = 1        */
inline static fits_image fitsim_median_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals;
	double v, median;
	long n_valid, n, i;
	long x, y, z;

	/* get median */
	while ( (vals=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
	    n_valid = 0;
	    for ( i=0 ; i < n ; i++ ) {			/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    vals[n_valid] = v;		/* accumulate finite values */
		    n_valid ++;
		}
	    }
	    if ( 2 < n_valid ) median = get_median(vals, n_valid);
	    else if ( n_valid == 2 ) median = (vals[0] + vals[1]) / 2.0;
	    else if ( n_valid == 1 ) median = vals[0];
	    else median = NAN;
	    *dest_ptr = median;
	    dest_ptr ++;
	}
    }
    obj.end_scan_along_z();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along z axis */
/* optimized for small length of z.              */
/* and return an array whose z_length = 1        */
inline static fits_image fitsim_median_small_z( const fits_image &obj )
{
    fits_image ret_array;			/* array to be returned */
    mdarray_long naxisx;			/* new dimension info */
    double *dest_ptr;				/* to store result */
    long ii;

    ret_array.init(FITS::DOUBLE_T);			/* init to DOUBLE */

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* resize and get pointer of dest */
    ret_array.resize( naxisx.carray(), obj.dim_length(), false );
    dest_ptr = ret_array.double_t_ptr();

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1), 
				       0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {
	double *vals2d;
	double v, median;
	long n_valid, n_z, n_x, i, j;
	long x, y, z;

	/* get median */
	while ( (vals2d=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
	    double *vals = vals2d;
	    for ( j=0 ; j < n_x ; j++ ) {	/* loop for x */
		n_valid = 0;
		for ( i=0 ; i < n_z ; i++ ) {	/* loop for z */
		    v = vals[i];
		    if ( isfinite(v) ) {
			vals[n_valid] = v;	/* accumulate finite values */
			n_valid ++;
		    }
		}
		if ( 2 < n_valid ) median = get_median(vals, n_valid);
		else if ( n_valid == 2 ) median = (vals[0] + vals[1]) / 2.0;
		else if ( n_valid == 1 ) median = vals[0];
		else median = NAN;
		*dest_ptr = median;
		dest_ptr ++;
		vals += n_z;
	    }
	}
    }
    obj.end_scan_zx_planes();

    ret_array.header_init( obj.header() );		/* copy FITS header */

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/****  M O M E N T  **********************************************************/
/*****************************************************************************/

/* public function to obtain MOMENT for all elements.              */
/* Set NULL to ret_mdev or ret_sdev when not required their return */
inline static mdarray_double fitsim_moment( const fits_image &obj, bool minus1,
					    double *ret_mdev, double *ret_sdev )
{
    mdarray_double ret_array;			/* array to be returned */
    double mean = NAN;
    long m1;

    ret_array.init_properties(obj.data_array());

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, sum, sum2, sum3, sum4;
	long n_valid, n, i;
	long x, y, z;

	/* get moment */
	sum = 0;
	sum2 = 0;
	sum3 = 0;
	sum4 = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    sum += v;
		    sum2 += v * v;
		    sum3 += v * v * v;
		    sum4 += v * v * v * v;
		}
	    }
	}

	ret_array.assign_default(NAN);
	ret_array.resize(4);

	if ( 0 < n_valid ) {
	    mean = sum / (double)(n_valid);
	    double variance = (sum2 - 2 * mean * sum + mean * mean * n_valid)
			      / (double)(n_valid - 1);
	    double stddev = sqrt(variance);
	    double s_sum = sum3 - 3 * mean * sum2 
		      + 3 * mean * mean * sum - pow(mean,3) * n_valid;
	    double skewness = (s_sum / pow(stddev,3)) / (double)(n_valid - m1);
	    double k_sum = sum4 - 4 * mean * sum3 
			   + 6 * mean * mean * sum2 
			   - 4 * pow(mean,3) * sum + pow(mean,4) * n_valid;
	    double kurtosis = 
		      ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
	    ret_array[0] = mean;
	    ret_array[1] = variance;
	    ret_array[2] = skewness;
	    ret_array[3] = kurtosis;
	    if ( ret_sdev != NULL ) *ret_sdev = sqrt(variance);
	}
	else {
	    if ( ret_sdev != NULL ) *ret_sdev = NAN;
	}
	if ( ret_mdev != NULL ) *ret_mdev = NAN;
    }

    if ( ret_mdev == NULL ) goto quit;
    if ( ! isfinite(mean) ) goto quit;

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {
	double *vals;
	double v, abs_sum;
	long n_valid, n, i;
	long x, y, z;

	/* get meanabsdev */
	abs_sum = 0;
	n_valid = 0;
	while ( (vals=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
	    for ( i=0 ; i < n ; i++ ) {		/* loop for a line */
		v = vals[i];
		if ( isfinite(v) ) {
		    n_valid ++;
		    abs_sum += fabs(v - mean);
		}
	    }
	}
	if ( 0 < n_valid ) *ret_mdev = abs_sum / (double)n_valid;
	else *ret_mdev = NAN;
    }

    obj.end_scan_along_x();

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


}	/* namespace */


#endif	/* _SLI__FITS_IMAGE_STATISTICS_H */
