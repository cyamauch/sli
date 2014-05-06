/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2012-12-10 12:10:57 cyamauch> */

/*
 * statistics package for mdarray class
 */

#ifndef _SLI__MDARRAY_STATISTICS_H
#define _SLI__MDARRAY_STATISTICS_H 1

#include "mdarray.h"
/* for debug */
//#include "private/err_report.h"
//#define CLASS_NAME "NONE"

namespace sli
{

/*****************************************************************************/
/****  T O T A L  ************************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_total_prms {
    double sum;
    double *dest_array;
    bool returns_array;
};

/* core function to calculate TOTAL: called by mdarray class */
static ssize_t md_total_func( double vals[], size_t n_vals, 
			      ssize_t x, ssize_t y, ssize_t z,
			      const mdarray *thisp, void *_prms )
{
    struct md_total_prms &prms = *((struct md_total_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.sum += v;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	if ( 0 < n_valid ) *(prms.dest_array) = prms.sum;
	else *(prms.dest_array) = NAN;
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain TOTAL for all elements */
inline double md_total( const mdarray &obj )
{
    struct md_total_prms prms;			/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;
    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_total_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) ret_value = prms.sum;

 quit:
    return ret_value;
}

/* public function to obtain TOTAL along x axis */
/* and return an array whose x-length = 1       */
inline mdarray_double md_total_x( const mdarray &obj )
{
    struct md_total_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_total_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along x axis */
/* and return an array whose y-length = 1       */
inline mdarray_double md_total_y( const mdarray &obj )
{
    struct md_total_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_total_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along x axis */
/* and return an array whose z-length = 1       */
inline mdarray_double md_total_z( const mdarray &obj )
{
    struct md_total_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_total_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  M E A N  **************************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_mean_prms {
    double sum;
    double *dest_array;
    bool returns_array;
};

/* core function to calculate MEAN: called by mdarray class */
static ssize_t md_mean_func( double vals[], size_t n_vals, 
			     ssize_t x, ssize_t y, ssize_t z,
			     const mdarray *thisp, void *_prms )
{
    struct md_mean_prms &prms = *((struct md_mean_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.sum += v;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	if ( 0 < n_valid ) *(prms.dest_array) = prms.sum / (double)n_valid;
	else *(prms.dest_array) = NAN;
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain MEAN for all elements */
inline double md_mean( const mdarray &obj )
{
    struct md_mean_prms prms;			/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_mean_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) ret_value = prms.sum / (double)n_valid_all;

 quit:
    return ret_value;
}

/* public function to obtain MEAN along x axis */
/* and return an array whose x-length = 1      */
inline mdarray_double md_mean_x( const mdarray &obj )
{
    struct md_mean_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_mean_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along x axis */
/* and return an array whose y-length = 1      */
inline mdarray_double md_mean_y( const mdarray &obj )
{
    struct md_mean_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_mean_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along x axis */
/* and return an array whose z-length = 1      */
inline mdarray_double md_mean_z( const mdarray &obj )
{
    struct md_mean_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_mean_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  M E A N A B S D E V  **************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_meanabsdev_prms {
    double sum;
    double mean;
    double abs_sum;
    double *dest_array;
    bool returns_array;
};

/* core function to calculate MEANABSDEV: called by mdarray class */
static ssize_t md_meanabsdev_func_1( double vals[], size_t n_vals, 
				     ssize_t x, ssize_t y, ssize_t z,
				     const mdarray *thisp, void *_prms )
{
    struct md_meanabsdev_prms &prms = *((struct md_meanabsdev_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.sum += v;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	/* store mean */
	if ( 0 < n_valid ) *(prms.dest_array) = prms.sum / (double)n_valid;
	else *(prms.dest_array) = NAN;
	prms.dest_array ++;
    }

    return n_valid;
}

static ssize_t md_meanabsdev_func_2( double vals[], size_t n_vals, 
				     ssize_t x, ssize_t y, ssize_t z,
				     const mdarray *thisp, void *_prms )
{
    struct md_meanabsdev_prms &prms = *((struct md_meanabsdev_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.abs_sum = 0;
	prms.mean = *(prms.dest_array);
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.abs_sum += fabs(v - prms.mean);
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	/* get meanabsdev */
	if ( 0 < n_valid ) *(prms.dest_array) = prms.abs_sum / (double)n_valid;
	else *(prms.dest_array) = NAN;
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain MEANABSDEV for all elements */
inline double md_meanabsdev( const mdarray &obj )
{
    struct md_meanabsdev_prms prms;			/* parameters */
    double ret_value = NAN;				/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.abs_sum = 0;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_meanabsdev_func_1, (void *)(&prms) );
    if ( 0 < n_valid_all ) {
	prms.mean = prms.sum / (double)n_valid_all;
	obj.scan_along_x( &md_meanabsdev_func_2, (void *)(&prms) );
	/* get meanabsdev */
	ret_value = prms.abs_sum / (double)n_valid_all;
    }

 quit:
    return ret_value;
}

/* public function to obtain MEANABSDEV along x axis */
/* and return an array whose x-length = 1            */
inline mdarray_double md_meanabsdev_x( const mdarray &obj )
{
    struct md_meanabsdev_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_meanabsdev_func_1, (void *)(&prms) );

    prms.dest_array = ret_array.array_ptr();

    obj.scan_along_x( &md_meanabsdev_func_2, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEANABSDEV along x axis */
/* and return an array whose y-length = 1            */
inline mdarray_double md_meanabsdev_y( const mdarray &obj )
{
    struct md_meanabsdev_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_meanabsdev_func_1, (void *)(&prms) );

    prms.dest_array = ret_array.array_ptr();

    obj.scan_along_y( &md_meanabsdev_func_2, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEANABSDEV along x axis */
/* and return an array whose z-length = 1            */
inline mdarray_double md_meanabsdev_z( const mdarray &obj )
{
    struct md_meanabsdev_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z (1st) */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_meanabsdev_func_1, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

    prms.dest_array = ret_array.array_ptr();

    /* scan along z (2nd) */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_meanabsdev_func_2, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  V A R I A N C E  ******************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_variance_prms {
    double sum;
    double sum2;	/* sum(v^2) */
    double *dest_array;
    bool returns_array;
};

/* core function to calculate VARIANCE: called by mdarray class */
static ssize_t md_variance_func( double vals[], size_t n_vals, 
				 ssize_t x, ssize_t y, ssize_t z,
				 const mdarray *thisp, void *_prms )
{
    struct md_variance_prms &prms = *((struct md_variance_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
	prms.sum2 = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.sum += v;
	    prms.sum2 += v * v;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	if ( 0 < n_valid ) {
	    double mean = prms.sum / (double)(n_valid);
	    /* get variance */
	    *(prms.dest_array) = 
		      (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid)
		      / (n_valid - 1);
	}
	else {
	    *(prms.dest_array) = NAN;
	}
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain VARIANCE for all elements */
inline double md_variance( const mdarray &obj )
{
    struct md_variance_prms prms;		/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.sum2 = 0;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_variance_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) {
	double mean = prms.sum / (double)(n_valid_all);
	/* get variance */
	ret_value = 
		  (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid_all)
		  / (n_valid_all - 1);
    }

 quit:
    return ret_value;
}

/* public function to obtain VARIANCE along x axis */
/* and return an array whose x-length = 1          */
inline mdarray_double md_variance_x( const mdarray &obj )
{
    struct md_variance_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_variance_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain VARIANCE along x axis */
/* and return an array whose y-length = 1          */
inline mdarray_double md_variance_y( const mdarray &obj )
{
    struct md_variance_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_variance_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain VARIANCE along x axis */
/* and return an array whose z-length = 1          */
inline mdarray_double md_variance_z( const mdarray &obj )
{
    struct md_variance_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_variance_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  S K E W N E S S  ******************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_skewness_prms {
    double sum;
    double sum2;	/* sum(v^2) */
    double sum3;	/* sum(v^3) */
    double *dest_array;
    ssize_t m1;
    bool returns_array;
};

/* core function to calculate SKEWNESS: called by mdarray class */
static ssize_t md_skewness_func( double vals[], size_t n_vals, 
				 ssize_t x, ssize_t y, ssize_t z,
				 const mdarray *thisp, void *_prms )
{
    struct md_skewness_prms &prms = *((struct md_skewness_prms *)_prms);
    ssize_t n_valid = 0;
    double v, v2;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
	prms.sum2 = 0;
	prms.sum3 = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    v2 = v * v;
	    prms.sum += v;
	    prms.sum2 += v2;
	    prms.sum3 += v2 * v;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	if ( 0 < n_valid ) {
	    double mean = prms.sum / (double)(n_valid);
	    double variance = 
		      (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid)
		      / (n_valid - 1);
	    double stddev = sqrt(variance);
	    double s_sum = prms.sum3 - 3 * mean * prms.sum2 
			  + 3 * mean * mean * prms.sum - pow(mean,3) * n_valid;
	    /* get skewness */
	    *(prms.dest_array) = (s_sum / pow(stddev,3)) / (n_valid - prms.m1);
	}
	else {
	    *(prms.dest_array) = NAN;
	}
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain SKEWNESS for all elements */
inline double md_skewness( const mdarray &obj, bool minus1 )
{
    struct md_skewness_prms prms;		/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.sum2 = 0;
    prms.sum3 = 0;
    prms.returns_array = false;

    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    n_valid_all = obj.scan_along_x( &md_skewness_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) {
	double mean = prms.sum / (double)(n_valid_all);
	double variance = 
		  (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid_all)
		  / (n_valid_all - 1);
	double stddev = sqrt(variance);
	double s_sum = prms.sum3 - 3 * mean * prms.sum2 
		      + 3 * mean * mean * prms.sum - pow(mean,3) * n_valid_all;
	/* get skewness */
	ret_value = (s_sum / pow(stddev,3)) / (n_valid_all - prms.m1);
    }

 quit:
    return ret_value;
}

/* public function to obtain SKEWNESS along x axis */
/* and return an array whose x-length = 1          */
inline mdarray_double md_skewness_x( const mdarray &obj, bool minus1 )
{
    struct md_skewness_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;
    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    obj.scan_along_x( &md_skewness_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain SKEWNESS along x axis */
/* and return an array whose y-length = 1          */
inline mdarray_double md_skewness_y( const mdarray &obj, bool minus1 )
{
    struct md_skewness_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;
    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    obj.scan_along_y( &md_skewness_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain SKEWNESS along x axis */
/* and return an array whose z-length = 1          */
inline mdarray_double md_skewness_z( const mdarray &obj, bool minus1 )
{
    struct md_skewness_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;
    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_skewness_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  K U R T O S I S  ******************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_kurtosis_prms {
    double sum;
    double sum2;	/* sum(v^2) */
    double sum3;	/* sum(v^3) */
    double sum4;	/* sum(v^4) */
    double *dest_array;
    ssize_t m1;
    bool returns_array;
};

/* core function to calculate KURTOSIS: called by mdarray class */
static ssize_t md_kurtosis_func( double vals[], size_t n_vals, 
				 ssize_t x, ssize_t y, ssize_t z,
				 const mdarray *thisp, void *_prms )
{
    struct md_kurtosis_prms &prms = *((struct md_kurtosis_prms *)_prms);
    ssize_t n_valid = 0;
    double v, v2;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
	prms.sum2 = 0;
	prms.sum3 = 0;
	prms.sum4 = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    v2 = v * v;
	    prms.sum += v;
	    prms.sum2 += v2;
	    prms.sum3 += v2 * v;
	    prms.sum4 += v2 * v2;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	if ( 0 < n_valid ) {
	    double mean = prms.sum / (double)(n_valid);
	    double variance = 
		      (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid)
		      / (n_valid - 1);
	    double stddev = sqrt(variance);
	    double k_sum = prms.sum4 - 4 * mean * prms.sum3 
			  + 6 * mean * mean * prms.sum2 
			  - 4 * pow(mean,3) * prms.sum + pow(mean,4) * n_valid;
	    /* get kurtosis */
	    *(prms.dest_array) = 
		((k_sum / pow(stddev,4)) / (n_valid - prms.m1)) - 3.0;
	}
	else {
	    *(prms.dest_array) = NAN;
	}
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain KURTOSIS for all elements */
inline double md_kurtosis( const mdarray &obj, bool minus1 )
{
    struct md_kurtosis_prms prms;		/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.sum2 = 0;
    prms.sum3 = 0;
    prms.sum4 = 0;
    prms.returns_array = false;

    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    n_valid_all = obj.scan_along_x( &md_kurtosis_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) {
	double mean = prms.sum / (double)(n_valid_all);
	double variance = 
		  (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid_all)
		  / (n_valid_all - 1);
	double stddev = sqrt(variance);
	double k_sum = prms.sum4 - 4 * mean * prms.sum3 
		      + 6 * mean * mean * prms.sum2 
		      - 4 * pow(mean,3) * prms.sum + pow(mean,4) * n_valid_all;
	/* get kurtosis */
	ret_value = ((k_sum / pow(stddev,4)) / (n_valid_all - prms.m1)) - 3.0;
    }

 quit:
    return ret_value;
}

/* public function to obtain KURTOSIS along x axis */
/* and return an array whose x-length = 1          */
inline mdarray_double md_kurtosis_x( const mdarray &obj, bool minus1 )
{
    struct md_kurtosis_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;
    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    obj.scan_along_x( &md_kurtosis_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain KURTOSIS along x axis */
/* and return an array whose y-length = 1          */
inline mdarray_double md_kurtosis_y( const mdarray &obj, bool minus1 )
{
    struct md_kurtosis_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;
    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    obj.scan_along_y( &md_kurtosis_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain KURTOSIS along x axis */
/* and return an array whose z-length = 1          */
inline mdarray_double md_kurtosis_z( const mdarray &obj, bool minus1 )
{
    struct md_kurtosis_prms prms;		/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;
    if ( minus1 == true ) prms.m1 = 1;
    else prms.m1 = 0;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_kurtosis_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  S T D D E V  **********************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_stddev_prms {
    double sum;
    double sum2;	/* sum(v^2) */
    double *dest_array;
    bool returns_array;
};

/* core function to calculate STDDEV: called by mdarray class */
static ssize_t md_stddev_func( double vals[], size_t n_vals, 
			       ssize_t x, ssize_t y, ssize_t z,
			       const mdarray *thisp, void *_prms )
{
    struct md_stddev_prms &prms = *((struct md_stddev_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.sum = 0;
	prms.sum2 = 0;
    }

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.sum += v;
	    prms.sum2 += v * v;
	    n_valid ++;
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	if ( 0 < n_valid ) {
	    double mean = prms.sum / (double)(n_valid);
	    double variance = 
		      (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid)
		      / (n_valid - 1);
	    /* get stddev */
	    *(prms.dest_array) = sqrt(variance);
	}
	else {
	    *(prms.dest_array) = NAN;
	}
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain STDDEV for all elements */
inline double md_stddev( const mdarray &obj )
{
    struct md_stddev_prms prms;			/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.sum = 0;
    prms.sum2 = 0;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_stddev_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) {
	double mean = prms.sum / (double)(n_valid_all);
	double variance = 
		 (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid_all) 
	         / (n_valid_all - 1);
	/* get stddev */
	ret_value = sqrt(variance);
    }

 quit:
    return ret_value;
}

/* public function to obtain STDDEV along x axis */
/* and return an array whose x-length = 1        */
inline mdarray_double md_stddev_x( const mdarray &obj )
{
    struct md_stddev_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_stddev_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along x axis */
/* and return an array whose y-length = 1        */
inline mdarray_double md_stddev_y( const mdarray &obj )
{
    struct md_stddev_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_stddev_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along x axis */
/* and return an array whose z-length = 1        */
inline mdarray_double md_stddev_z( const mdarray &obj )
{
    struct md_stddev_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_stddev_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  M I N  ****************************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_min_prms {
    double min;
    double *dest_array;
    bool returns_array;
};

/* core function to calculate MIN: called by mdarray class */
static ssize_t md_min_func( double vals[], size_t n_vals, 
			    ssize_t x, ssize_t y, ssize_t z,
			    const mdarray *thisp, void *_prms )
{
    struct md_min_prms &prms = *((struct md_min_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.min = NAN;
    }

    if ( isfinite(prms.min) ) {			/* loop for 1st dimension */
	for ( i=0 ; i < n_vals ; i++ ) {
	    v = vals[i];
	    if ( isfinite(v) ) {
		if ( v < prms.min ) prms.min = v;
		n_valid ++;
	    }
	}
    }
    else {
	for ( i=0 ; i < n_vals ; i++ ) {
	    v = vals[i];
	    if ( isfinite(v) ) {
		if ( isfinite(prms.min) ) {
		    if ( v < prms.min ) prms.min = v;
		}
		else prms.min = v;
		n_valid ++;
	    }
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	*(prms.dest_array) = prms.min;
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain MIN for all elements */
inline double md_min( const mdarray &obj )
{
    struct md_min_prms prms;			/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;
    if ( obj.length() == 0 ) goto quit;

    prms.min = NAN;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_min_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) ret_value = prms.min;

 quit:
    return ret_value;
}

/* public function to obtain MIN along x axis */
/* and return an array whose x-length = 1     */
inline mdarray_double md_min_x( const mdarray &obj )
{
    struct md_min_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_min_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along x axis */
/* and return an array whose y-length = 1     */
inline mdarray_double md_min_y( const mdarray &obj )
{
    struct md_min_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_min_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along x axis */
/* and return an array whose z-length = 1     */
inline mdarray_double md_min_z( const mdarray &obj )
{
    struct md_min_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_min_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  M A X  ****************************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_max_prms {
    double max;
    double *dest_array;
    bool returns_array;
};

/* core function to calculate MAX: called by mdarray class */
static ssize_t md_max_func( double vals[], size_t n_vals, 
			    ssize_t x, ssize_t y, ssize_t z,
			    const mdarray *thisp, void *_prms )
{
    struct md_max_prms &prms = *((struct md_max_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    if ( prms.returns_array == true ) {		/* for returning array */
	prms.max = NAN;
    }

    if ( isfinite(prms.max) ) {			/* loop for 1st dimension */
	for ( i=0 ; i < n_vals ; i++ ) {
	    v = vals[i];
	    if ( isfinite(v) ) {
		if ( prms.max < v ) prms.max = v;
		n_valid ++;
	    }
	}
    }
    else {
	for ( i=0 ; i < n_vals ; i++ ) {
	    v = vals[i];
	    if ( isfinite(v) ) {
		if ( isfinite(prms.max) ) {
		    if ( prms.max < v ) prms.max = v;
		}
		else prms.max = v;
		n_valid ++;
	    }
	}
    }

    if ( prms.returns_array == true ) {		/* for returning array */
	*(prms.dest_array) = prms.max;
	prms.dest_array ++;
    }

    return n_valid;
}

/* public function to obtain MAX for all elements */
inline double md_max( const mdarray &obj )
{
    struct md_max_prms prms;			/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;
    if ( obj.length() == 0 ) goto quit;

    prms.max = NAN;
    prms.returns_array = false;

    n_valid_all = obj.scan_along_x( &md_max_func, (void *)(&prms) );
    if ( 0 < n_valid_all ) ret_value = prms.max;

 quit:
    return ret_value;
}

/* public function to obtain MAX along x axis */
/* and return an array whose x-length = 1     */
inline mdarray_double md_max_x( const mdarray &obj )
{
    struct md_max_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_max_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along x axis */
/* and return an array whose y-length = 1     */
inline mdarray_double md_max_y( const mdarray &obj )
{
    struct md_max_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_max_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along x axis */
/* and return an array whose z-length = 1     */
inline mdarray_double md_max_z( const mdarray &obj )
{
    struct md_max_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_max_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

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
static double get_median( double *buf_ptr, const size_t buf_len )
{
    double ret_value = NAN;
    size_t src_start, src_end1;			/* ループの開始，終了+1 */
    size_t buf_pos_right1;			/* ループ終了時の分岐点 */
    double pivot;

    if ( buf_ptr == NULL ) goto quit;
    if ( buf_len == 0 ) goto quit;

    src_start = 0;
    src_end1 = buf_len;

    pivot = buf_ptr[0];

    /*
     * メインループ
     */
    while ( 1 ) {
	double v, v1;
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
	    double l_max, r_min;
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

/* please define members first that have larger size of type */
struct md_median_prms {
    double median;
    double *dest_array;
    bool returns_array;
};

/* core function to calculate MEDIAN: called by mdarray class */
static ssize_t md_median_func( double vals[], size_t n_vals, 
			       ssize_t x, ssize_t y, ssize_t z,
			       const mdarray *thisp, void *_prms )
{
    struct md_median_prms &prms = *((struct md_median_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    prms.median = NAN;

    /* check NAN */
    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    vals[n_valid] = v;
	    n_valid ++;
	}
    }

    if ( 2 < n_valid ) prms.median = get_median(vals, n_valid);
    else if ( n_valid == 2 ) prms.median = (vals[0] + vals[1]) / 2.0;
    else if ( n_valid == 1 ) prms.median = vals[0];

    if ( prms.returns_array == true ) {		/* for returning array */
	*(prms.dest_array) = prms.median;
	prms.dest_array ++;
    }

    return n_valid;
}

static ssize_t md_median_func_3d( double vals[], 
				  size_t nx, size_t ny, size_t nz,
				  ssize_t x, ssize_t y, ssize_t z,
				  const mdarray *thisp, void *_prms )
{
    return md_median_func(vals, nx*ny*nz, x,y,z, thisp, _prms);
}

/* public function to obtain MEDIAN for all elements */
inline double md_median( const mdarray &obj )
{
    struct md_median_prms prms;			/* parameters */
    double ret_value = NAN;			/* return value */
    ssize_t n_valid_all;

    if ( obj.length() == 0 ) goto quit;

    prms.median = NAN;
    prms.returns_array = false;

    n_valid_all = obj.scan_a_cube( &md_median_func_3d, (void *)(&prms) );
    if ( 0 < n_valid_all ) ret_value = prms.median;

 quit:
    return ret_value;
}

/* public function to obtain MEDIAN along x axis */
/* and return an array whose x-length = 1        */
inline mdarray_double md_median_x( const mdarray &obj )
{
    struct md_median_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    naxisx[0] = 1;					/* combine at x */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_x( &md_median_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along x axis */
/* and return an array whose y-length = 1        */
inline mdarray_double md_median_y( const mdarray &obj )
{
    struct md_median_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    obj.scan_along_y( &md_median_func, (void *)(&prms) );

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along x axis */
/* and return an array whose z-length = 1        */
inline mdarray_double md_median_z( const mdarray &obj )
{
    struct md_median_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    mdarray_size naxisx;			/* dimension info */
    size_t i, nn;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    naxisx.resize( obj.dim_length() );
    naxisx.put_elements( obj.cdimarray(), obj.dim_length() );
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    ret_array.resize( naxisx.carray(), obj.dim_length(), false );

    prms.dest_array = ret_array.array_ptr();
    prms.returns_array = true;

    /* count 4th, 5th, ... dimension */
    nn = 1;
    for ( i=3 ; i < obj.dim_length() ; i++ ) nn *= obj.length();

    /* scan along z */
    for ( i=0 ; i < nn ; i++ ) {
	obj.scan_along_z( &md_median_func, (void *)(&prms),
			  0, obj.length(0), 0, obj.length(1), 
			  0 + obj.length(2) * i, obj.length(2) );
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}

/*****************************************************************************/
/****  M O M E N T  **********************************************************/
/*****************************************************************************/

/* please define members first that have larger size of type */
struct md_moment_prms {
    double sum;
    double sum2;	/* sum(v^2) */
    double sum3;	/* sum(v^3) */
    double sum4;	/* sum(v^4) */
    double mean;
    double abs_sum;
};

/* core function to calculate MOMENT: called by mdarray class */
static ssize_t md_moment_func_1( double vals[], size_t n_vals, 
				 ssize_t x, ssize_t y, ssize_t z,
				 const mdarray *thisp, void *_prms )
{
    struct md_moment_prms &prms = *((struct md_moment_prms *)_prms);
    ssize_t n_valid = 0;
    double v, v2;
    size_t i;

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    v2 = v * v;
	    prms.sum += v;
	    prms.sum2 += v2;
	    prms.sum3 += v2 * v;
	    prms.sum4 += v2 * v2;
	    n_valid ++;
	}
    }

    return n_valid;
}

static ssize_t md_moment_func_2( double vals[], size_t n_vals, 
				 ssize_t x, ssize_t y, ssize_t z,
				 const mdarray *thisp, void *_prms )
{
    struct md_moment_prms &prms = *((struct md_moment_prms *)_prms);
    ssize_t n_valid = 0;
    double v;
    size_t i;

    for ( i=0 ; i < n_vals ; i++ ) {		/* loop for 1st dimension */
	v = vals[i];
	if ( isfinite(v) ) {
	    prms.abs_sum += fabs(v - prms.mean);
	    n_valid ++;
	}
    }

    return n_valid;
}

/* public function to obtain MOMENT for all elements.              */
/* Set NULL to ret_mdev or ret_sdev when not required their return */
inline mdarray_double md_moment( const mdarray &obj, bool minus1,
				 double *ret_mdev, double *ret_sdev )
{
    struct md_moment_prms prms;			/* parameters */
    mdarray_double ret_array;			/* array to be returned */
    ssize_t n_valid_all;

    ret_array.init_properties(obj);

    if ( obj.length() == 0 ) goto quit;

    ret_array.assign_default(NAN);
    ret_array.resize(4);

    prms.sum = 0;
    prms.sum2 = 0;
    prms.sum3 = 0;
    prms.sum4 = 0;
    prms.abs_sum = 0;

    /* get mean ... kurtosis */
    n_valid_all = obj.scan_along_x( &md_moment_func_1, (void *)(&prms) );
    if ( 0 < n_valid_all ) {
	ssize_t m1;
	if ( minus1 == true ) m1 = 1;
	else m1 = 0;
	double mean = prms.sum / (double)(n_valid_all);
	double variance = 
		  (prms.sum2 - 2 * mean * prms.sum + mean * mean * n_valid_all)
	          / (n_valid_all - 1);
	double stddev = sqrt(variance);
	double s_sum = prms.sum3 - 3 * mean * prms.sum2 
		      + 3 * mean * mean * prms.sum - pow(mean,3) * n_valid_all;
	double skewness = (s_sum / pow(stddev,3)) / (n_valid_all - m1);
	double k_sum = prms.sum4 - 4 * mean * prms.sum3 
		      + 6 * mean * mean * prms.sum2 
		      - 4 * pow(mean,3) * prms.sum + pow(mean,4) * n_valid_all;
	double kurtosis = ((k_sum / pow(stddev,4)) / (n_valid_all - m1)) - 3.0;
	prms.mean = mean;
	/* to return */
	ret_array[0] = mean;
	ret_array[1] = variance;
	ret_array[2] = skewness;
	ret_array[3] = kurtosis;
	if ( ret_sdev != NULL ) *ret_sdev = sqrt(variance);
    }
    else {
	if ( ret_sdev != NULL ) *ret_sdev = NAN;
    }

    if ( ret_mdev != NULL ) {
	/* get mean absolute deviation */
	n_valid_all = obj.scan_along_x( &md_moment_func_2, (void *)(&prms) );
	if ( 0 < n_valid_all ) *ret_mdev = prms.abs_sum / (double)n_valid_all;
	else *ret_mdev = NAN;
    }

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*****************************************************************************/
/*****************************************************************************/

}	/* namespace sli */


#endif	/* _SLI__MDARRAY_STATISTICS_H */
