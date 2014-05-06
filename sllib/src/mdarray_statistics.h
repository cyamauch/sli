/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-24 14:18:50 cyamauch> */

#ifndef _SLI__MDARRAY_STATISTICS_H
#define _SLI__MDARRAY_STATISTICS_H 1

/*
 * statistics package for mdarray class
 */

/**
 * @file   mdarray_statistics.h
 * @brief  mdarray���饹�Ȥ��ηѾ����饹�Υ��֥������Ȥǻ��Ѳ�ǽ�������Ѵؿ�
 * @attention  C++ ɸ��饤�֥��� <cmath> ��Ȥ���硤�ޥ��� SLI__USE_CMATH
 *             ��������Ƥ��� mdarray_statistics.h �� include ���Ƥ���������
 */

#include "mdarray.h"

/* for debug */
//#include "private/err_report.h"
//#define CLASS_NAME "NONE"

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

/*===========================================================================*/
/*===  LIST OF PUBLIC FUNCTIONS  ============================================*/
/*===========================================================================*/
/*
inline static double md_total( const mdarray &obj )
inline static mdarray md_total_x( const mdarray &obj )
inline static mdarray md_total_y( const mdarray &obj )
inline static mdarray md_total_z( const mdarray &obj )
inline static mdarray md_total_small_z( const mdarray &obj )

inline static double md_mean( const mdarray &obj )
inline static mdarray md_mean_x( const mdarray &obj )
inline static mdarray md_mean_y( const mdarray &obj )
inline static mdarray md_mean_z( const mdarray &obj )
inline static mdarray md_mean_small_z( const mdarray &obj )

inline static double md_meanabsdev( const mdarray &obj )
inline static mdarray md_meanabsdev_x( const mdarray &obj )
inline static mdarray md_meanabsdev_y( const mdarray &obj )
inline static mdarray md_meanabsdev_z( const mdarray &obj )

inline static double md_variance( const mdarray &obj )
inline static mdarray md_variance_x( const mdarray &obj )
inline static mdarray md_variance_y( const mdarray &obj )
inline static mdarray md_variance_z( const mdarray &obj )

inline static double md_skewness( const mdarray &obj, bool minus1 )
inline static mdarray md_skewness_x( const mdarray &obj, bool minus1 )
inline static mdarray md_skewness_y( const mdarray &obj, bool minus1 )
inline static mdarray md_skewness_z( const mdarray &obj, bool minus1 )

inline static double md_kurtosis( const mdarray &obj, bool minus1 )
inline static mdarray md_kurtosis_x( const mdarray &obj, bool minus1 )
inline static mdarray md_kurtosis_y( const mdarray &obj, bool minus1 )
inline static mdarray md_kurtosis_z( const mdarray &obj, bool minus1 )

inline static double md_stddev( const mdarray &obj )
inline static mdarray md_stddev_x( const mdarray &obj )
inline static mdarray md_stddev_y( const mdarray &obj )
inline static mdarray md_stddev_z( const mdarray &obj )
inline static mdarray md_stddev_small_z( const mdarray &obj )

inline static double md_min( const mdarray &obj )
inline static mdarray md_min_x( const mdarray &obj )
inline static mdarray md_min_y( const mdarray &obj )
inline static mdarray md_min_z( const mdarray &obj )
inline static mdarray md_min_small_z( const mdarray &obj )

inline static double md_max( const mdarray &obj )
inline static mdarray_double md_max_x( const mdarray &obj )
inline static mdarray_double md_max_y( const mdarray &obj )
inline static mdarray_double md_max_z( const mdarray &obj )
inline static mdarray_double md_max_small_z( const mdarray &obj )

inline static double md_median( const mdarray &obj )
inline static mdarray md_median_x( const mdarray &obj )
inline static mdarray md_median_y( const mdarray &obj )
inline static mdarray md_median_z( const mdarray &obj )
inline static mdarray md_median_small_z( const mdarray &obj )

inline static mdarray_double md_moment( const mdarray &obj, bool minus1,
					double *ret_mdev, double *ret_sdev )
*/

/*===========================================================================*/
/*===  T O T A L  ===========================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ����ͻ��ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_total( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum;
    size_t n_valid, i;

    /* get total */
    sum = 0;
    n_valid = n;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	sum += v;
    }
    if ( 0 < n_valid ) ret_value = sum;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  ����ͻ��ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_total( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum;
    size_t n_valid, i;

    /* get total */
    sum = 0;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	if ( isfinite(v) ) {
	    n_valid ++;
	    sum += v;
	}
    }
    if ( 0 < n_valid ) ret_value = sum;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain TOTAL for all elements */
/**
 * @brief  �����Ǥι���ͤ����
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_total( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */

    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_total((const float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_total((const double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_total((const unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_total((const int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_total((const int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_total((const int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain TOTAL along x axis */
/* and return an array whose x_length = 1       */
/**
 * @brief  x�����ǹ���ͤ�׻�����x������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_total_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());	/* data source */
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get total */
	if ( obj.size_type() == FLOAT_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_f_total((const float *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_total((const unsigned char *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_total((const int16_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get total */
	if ( obj.size_type() == DOUBLE_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_f_total((const double *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_total((const int32_t *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_total((const int64_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along y axis */
/* and return an array whose y_length = 1       */
/**
 * @brief  y�����ǹ���ͤ�׻�����y������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_total_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get total */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_total(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_total(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along z axis */
/* and return an array whose z_length = 1       */
/**
 * @brief  z�����ǹ���ͤ�׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @attention  z������Ĺ������������硤�ѥե����ޥ󥹤��㲼���ޤ���
 *             ���ξ��ϡ�md_total_small_z() �򤪻Ȥ�����������
 */
inline static mdarray md_total_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get total */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_total(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_total(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain TOTAL along z axis */
/* optimized for small length of z.             */
/* and return an array whose z_length = 1       */
/**
 * @brief  z�����ǹ���ͤ�׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����Ǥι�®transpose�˲ä���zx��ñ�̤ǥ������ݤ��뤿�ᡤ��®��
 *         ư��ޤ���
 */
inline static mdarray md_total_small_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {

	mdarray_float ret_arr0;				/* to store result */
	float *dest_ptr;
	const float *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get total */
	    while ( (s_ptr=obj.scan_zx_planes_f(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_total(s_ptr, n_z, NULL);     /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {

	mdarray_double ret_arr0;			/* to store result */
	double *dest_ptr;
	const double *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get total */
	    while ( (s_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_total(s_ptr, n_z, NULL);     /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  M E A N  =============================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ʿ���ͻ��ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_mean( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum;
    size_t n_valid, i;

    /* get mean */
    sum = 0;
    n_valid = n;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	sum += v;
    }
    if ( 0 < n_valid ) ret_value = sum / (double)n_valid;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  ʿ���ͻ��ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_mean( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum;
    size_t n_valid, i;

    /* get mean */
    sum = 0;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	if ( isfinite(v) ) {
	    n_valid ++;
	    sum += v;
	}
    }
    if ( 0 < n_valid ) ret_value = sum / (double)n_valid;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain MEAN for all elements */
/**
 * @brief  �����Ǥ�ʿ���ͤ����
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_mean( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */
    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_mean((const float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_mean((const double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_mean((const unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_mean((const int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_mean((const int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_mean((const int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain MEAN along x axis */
/* and return an array whose x_length = 1      */
/**
 * @brief  x������ʿ���ͤ�׻�����x������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_mean_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get mean */
	if ( obj.size_type() == FLOAT_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_mean((const float *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_mean((const unsigned char *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_mean((const int16_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get mean */
	if ( obj.size_type() == DOUBLE_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_f_mean((const double *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_mean((const int32_t *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_mean((const int64_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along y axis */
/* and return an array whose y_length = 1      */
/**
 * @brief  y������ʿ���ͤ�׻�����y������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_mean_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get mean */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_mean(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_mean(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along z axis */
/* and return an array whose z_length = 1      */
/**
 * @brief  z������ʿ���ͤ�׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @attention  z������Ĺ������������硤�ѥե����ޥ󥹤��㲼���ޤ���
 *             ���ξ��ϡ�md_mean_small_z() �򤪻Ȥ�����������
 */
inline static mdarray md_mean_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get mean */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_mean(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_mean(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEAN along z axis */
/* optimized for small length of z.            */
/* and return an array whose z_length = 1      */
/**
 * @brief  z������ʿ���ͤ�׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����Ǥι�®transpose�˲ä���zx��ñ�̤ǥ������ݤ��뤿�ᡤ��®��
 *         ư��ޤ���
 */
inline static mdarray md_mean_small_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {

	mdarray_float ret_arr0;				/* to store result */
	float *dest_ptr;
	const float *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get mean */
	    while ( (s_ptr=obj.scan_zx_planes_f(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_mean(s_ptr, n_z, NULL);  /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {

	mdarray_double ret_arr0;			/* to store result */
	double *dest_ptr;
	const double *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get mean */
	    while ( (s_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_mean(s_ptr, n_z, NULL);  /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  M E A N A B S D E V  =================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ʿ�������к����ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   mean ʿ���͡������ʾ��� NAN �򥻥åȤ��Ƥ��ɤ���
 * @return  ������
 */
template <class datatype>
static double get_meanabsdev( const datatype *vals, size_t n, double mean )
{
    double ret_value = NAN;			/* return value */
    double v, sum;
    size_t i;

    /* get sum */
    sum = 0;
    if ( ! isfinite(mean) ) {
	for ( i=0 ; i < n ; i++ ) {
	    v = (double)(vals[i]);
	    sum += v;
	}
	mean = sum / (double)n;
    }
    if ( 0 < n ) {
	double abs_sum = 0;
	/* get abs_sum */
	for ( i=0 ; i < n ; i++ ) {
	    v = (double)(vals[i]);
	    abs_sum += fabs(v - mean);
	}
	ret_value = abs_sum / (double)n;
    }
    else ret_value = NAN;

    return ret_value;
}

/* for float and double */
/**
 * @brief  ʿ�������к����ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   mean ʿ���͡������ʾ��� NAN �򥻥åȤ��Ƥ��ɤ���
 * @return  ������
 */
template <class datatype>
static double get_f_meanabsdev( const datatype *vals, size_t n, double mean )
{
    double ret_value = NAN;			/* return value */
    double v, sum;
    size_t i;

    /* get sum */
    sum = 0;
    if ( ! isfinite(mean) ) {
	size_t n_valid = 0;
	for ( i=0 ; i < n ; i++ ) {
	    v = (double)(vals[i]);
	    if ( isfinite(v) ) {
		n_valid ++;
		sum += v;
	    }
	}
	if ( 0 < n_valid ) mean = sum / (double)n_valid;
    }
    if ( isfinite(mean) ) {
	size_t n_valid = 0;
	double abs_sum = 0;
	/* get abs_sum */
	for ( i=0 ; i < n ; i++ ) {
	    v = (double)(vals[i]);
	    if ( isfinite(v) ) {
		n_valid ++;
		abs_sum += fabs(v - mean);
	    }
	}
	ret_value = abs_sum / (double)n_valid;
    }
    else ret_value = NAN;

    return ret_value;
}

/* public function to obtain MEANABSDEV for all elements */
/**
 * @brief  �����Ǥ�ʿ�������к������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_meanabsdev( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */
    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_meanabsdev((const float *)ptr, len, NAN);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_meanabsdev((const double *)ptr, len, NAN);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_meanabsdev((const unsigned char *)ptr, len, NAN);
    else if ( t == INT16_ZT ) 
	ret_value = get_meanabsdev((const int16_t *)ptr, len, NAN);
    else if ( t == INT32_ZT ) 
	ret_value = get_meanabsdev((const int32_t *)ptr, len, NAN);
    else if ( t == INT64_ZT ) 
	ret_value = get_meanabsdev((const int64_t *)ptr, len, NAN);

    return ret_value;
}

/* public function to obtain MEANABSDEV along x axis */
/* and return an array whose x_length = 1            */
/**
 * @brief  x������ʿ�������к���׻�����x������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_meanabsdev_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get mdev */
	if ( obj.size_type() == FLOAT_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_meanabsdev((const float *)s_ptr, len_x, NAN);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_meanabsdev((const unsigned char *)s_ptr, len_x, NAN);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_meanabsdev((const int16_t *)s_ptr, len_x, NAN);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get mdev */
	if ( obj.size_type() == DOUBLE_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_meanabsdev((const double *)s_ptr, len_x, NAN);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_meanabsdev((const int32_t *)s_ptr, len_x, NAN);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_meanabsdev((const int64_t *)s_ptr, len_x, NAN);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEANABSDEV along y axis */
/* and return an array whose y_length = 1            */
/**
 * @brief  y����������ʿ���к���׻�����y������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_meanabsdev_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get mdev */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_meanabsdev(s_ptr, n, NAN);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_meanabsdev(s_ptr, n, NAN);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEANABSDEV along z axis */
/* and return an array whose z_length = 1            */
/**
 * @brief  z����������ʿ���к���׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_meanabsdev_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get mdev */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_meanabsdev(s_ptr, n, NAN);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_meanabsdev(s_ptr, n, NAN);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  V A R I A N C E  =====================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ʬ�����ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_variance( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum, sum2;
    size_t n_valid, i;

    /* get required values */
    sum = 0;
    sum2 = 0;
    n_valid = n;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	sum += v;
	sum2 += v * v;
    }
    if ( 0 < n_valid ) {
	double mean = sum / (double)n_valid;
	/* get result */
	ret_value = (sum2 - 2 * mean * sum + mean * mean * n_valid)
		    / (double)(n_valid - 1);
    }
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  ʬ�����ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_variance( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum, sum2;
    size_t n_valid, i;

    /* get required values */
    sum = 0;
    sum2 = 0;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	if ( isfinite(v) ) {
	    n_valid ++;
	    sum += v;
	    sum2 += v * v;
	}
    }
    if ( 0 < n_valid ) {
	double mean = sum / (double)n_valid;
	/* get result */
	ret_value = (sum2 - 2 * mean * sum + mean * mean * n_valid)
		    / (double)(n_valid - 1);
    }
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain VARIANCE for all elements */
/**
 * @brief  �����Ǥ�ʬ�������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_variance( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */
    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_variance((const float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_variance((const double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_variance((const unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_variance((const int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_variance((const int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_variance((const int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain VARIANCE along x axis */
/* and return an array whose x_length = 1          */
/**
 * @brief  x������ʬ����׻�����x������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_variance_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get variance */
	if ( obj.size_type() == FLOAT_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_variance((const float *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_variance((const unsigned char *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_variance((const int16_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get variance */
	if ( obj.size_type() == DOUBLE_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_f_variance((const double *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_variance((const int32_t *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_variance((const int64_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain VARIANCE along y axis */
/* and return an array whose y_length = 1          */
/**
 * @brief  y������ʬ����׻�����y������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_variance_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get variance */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_variance(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_variance(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain VARIANCE along z axis */
/* and return an array whose z_length = 1          */
/**
 * @brief  z������ʬ����׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_variance_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get variance */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_variance(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_variance(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  S K E W N E S S  =====================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ���ٻ��ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_skewness( const datatype *vals, size_t n, bool minus1, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum, sum2, sum3;
    size_t n_valid, i;
    size_t m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* get required values */
    sum = 0;
    sum2 = 0;
    sum3 = 0;
    n_valid = n;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	sum += v;
	sum2 += v * v;
	sum3 += v * v * v;
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

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  ���ٻ��ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_skewness( const datatype *vals, size_t n, bool minus1, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum, sum2, sum3;
    size_t n_valid, i;
    size_t m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* get required values */
    sum = 0;
    sum2 = 0;
    sum3 = 0;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
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
	ret_value = (s_sum / pow(stddev,3)) / (double)(n_valid - m1);
    }
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain SKEWNESS for all elements */
/**
 * @brief  �����Ǥ����٤����
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note    ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static double md_skewness( const mdarray &obj, bool minus1 )
{
    double ret_value = NAN;			/* return value */
    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_skewness((const float *)ptr, len, minus1, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_skewness((const double *)ptr, len, minus1, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_skewness((const unsigned char *)ptr, len, minus1, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_skewness((const int16_t *)ptr, len, minus1, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_skewness((const int32_t *)ptr, len, minus1, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_skewness((const int64_t *)ptr, len, minus1, NULL);

    return ret_value;
}

/* public function to obtain SKEWNESS along x axis */
/* and return an array whose x_length = 1          */
/**
 * @brief  x���������٤�׻�����x������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note    ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray md_skewness_x( const mdarray &obj, bool minus1 )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get skewness */
	if ( obj.size_type() == FLOAT_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_skewness((const float *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_skewness((const unsigned char *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_skewness((const int16_t *)s_ptr, len_x, minus1, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get skewness */
	if ( obj.size_type() == DOUBLE_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_skewness((const double *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_skewness((const int32_t *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_skewness((const int64_t *)s_ptr, len_x, minus1, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain SKEWNESS along y axis */
/* and return an array whose y_length = 1          */
/**
 * @brief  y���������٤�׻�����y������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray md_skewness_y( const mdarray &obj, bool minus1 )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get skewness */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_skewness(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_skewness(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain SKEWNESS along z axis */
/* and return an array whose z_length = 1          */
/**
 * @brief  z���������٤�׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray md_skewness_z( const mdarray &obj, bool minus1 )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get skewness */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_skewness(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_skewness(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  K U R T O S I S  =====================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ���ٻ��ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_kurtosis( const datatype *vals, size_t n, bool minus1, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum, sum2, sum3, sum4;
    size_t n_valid, i;
    size_t m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* get required values */
    sum = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    n_valid = n;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
	sum += v;
	sum2 += v * v;
	sum3 += v * v * v;
	sum4 += v * v * v * v;
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
	ret_value = ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
    }
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  ���ٻ��ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_kurtosis( const datatype *vals, size_t n, bool minus1, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    double v, sum, sum2, sum3, sum4;
    size_t n_valid, i;
    size_t m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* get required values */
    sum = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
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
	ret_value = ((k_sum / pow(stddev,4)) / (double)(n_valid - m1)) - 3.0;
    }
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain KURTOSIS for all elements */
/**
 * @brief  �����Ǥ����٤����
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static double md_kurtosis( const mdarray &obj, bool minus1 )
{
    double ret_value = NAN;			/* return value */
    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_kurtosis((const float *)ptr, len, minus1, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_kurtosis((const double *)ptr, len, minus1, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_kurtosis((const unsigned char *)ptr, len, minus1, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_kurtosis((const int16_t *)ptr, len, minus1, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_kurtosis((const int32_t *)ptr, len, minus1, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_kurtosis((const int64_t *)ptr, len, minus1, NULL);

    return ret_value;
}

/* public function to obtain KURTOSIS along x axis */
/* and return an array whose x_length = 1          */
/**
 * @brief  x���������٤�׻�����x������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray md_kurtosis_x( const mdarray &obj, bool minus1 )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get kurtosis */
	if ( obj.size_type() == FLOAT_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_kurtosis((const float *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_kurtosis((const unsigned char *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_kurtosis((const int16_t *)s_ptr, len_x, minus1, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get kurtosis */
	if ( obj.size_type() == DOUBLE_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_kurtosis((const double *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_kurtosis((const int32_t *)s_ptr, len_x, minus1, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_kurtosis((const int64_t *)s_ptr, len_x, minus1, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain KURTOSIS along y axis */
/* and return an array whose y_length = 1          */
/**
 * @brief  y���������٤�׻�����y������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray md_kurtosis_y( const mdarray &obj, bool minus1 )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get kurtosis */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_kurtosis(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_kurtosis(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain KURTOSIS along z axis */
/* and return an array whose z_length = 1          */
/**
 * @brief  z���������٤�׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray md_kurtosis_z( const mdarray &obj, bool minus1 )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get kurtosis */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_kurtosis(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_kurtosis(s_ptr, n, minus1, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  S T D D E V  =========================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  ɸ���к����ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
inline static double get_stddev( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    const double variance = get_variance(vals, n, ret_n_valid);
    return (isfinite(variance) ? sqrt(variance) : NAN);
}

/* for float and double */
/**
 * @brief  ɸ���к����ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
inline static double get_f_stddev( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    const double variance = get_f_variance(vals, n, ret_n_valid);
    return (isfinite(variance) ? sqrt(variance) : NAN);
}

/* public function to obtain STDDEV for all elements */
/**
 * @brief  �����Ǥ�ɸ���к������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_stddev( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */
    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_stddev((const float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_stddev((const double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_stddev((const unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_stddev((const int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_stddev((const int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_stddev((const int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain STDDEV along x axis */
/* and return an array whose x_length = 1        */
/**
 * @brief  x������ɸ���к���׻�����x������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_stddev_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get stddev */
	if ( obj.size_type() == FLOAT_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_f_stddev((const float *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_stddev((const unsigned char *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	  for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	    *dest_ptr = get_stddev((const int16_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get stddev */
	if ( obj.size_type() == DOUBLE_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_f_stddev((const double *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_stddev((const int32_t *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_stddev((const int64_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along y axis */
/* and return an array whose y_length = 1        */
/**
 * @brief  y������ɸ���к���׻�����y������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_stddev_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get stddev */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_stddev(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_stddev(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along z axis */
/* and return an array whose z_length = 1        */
/**
 * @brief  z������ɸ���к���׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @attention  z������Ĺ������������硤�ѥե����ޥ󥹤��㲼���ޤ���
 *             ���ξ��ϡ�md_stddev_small_z() �򤪻Ȥ�����������
 */
inline static mdarray md_stddev_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get stddev */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_stddev(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_stddev(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain STDDEV along z axis */
/* optimized for small length of z.              */
/* and return an array whose z_length = 1        */
/**
 * @brief  z������ɸ���к���׻�����z������Ĺ����1����������
 *
 *  �����ͤ�����ˤĤ��Ƥ� mdarray_double md_moment() �ؿ��β����
 *  ���Ȥ��Ƥ���������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����Ǥι�®transpose�˲ä���zx��ñ�̤ǥ������ݤ��뤿�ᡤ��®��
 *         ư��ޤ���
 */
inline static mdarray md_stddev_small_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {

	mdarray_float ret_arr0;				/* to store result */
	float *dest_ptr;
	const float *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get stddev */
	    while ( (s_ptr=obj.scan_zx_planes_f(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_stddev(s_ptr, n_z, NULL);    /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {

	mdarray_double ret_arr0;			/* to store result */
	double *dest_ptr;
	const double *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get stddev */
	    while ( (s_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_stddev(s_ptr, n_z, NULL);    /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  M I N  ===============================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  �Ǿ��ͻ��ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_min( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    datatype min = 0;
    size_t n_valid, i;

    /* get min */
    n_valid = n;
    i = 0;
    if ( i < n ) {
	min = vals[i];
	i++;
    }
    for ( ; i < n ; i++ ) {
	if ( vals[i] < min ) min = vals[i];
    }
    if ( 0 < n_valid ) ret_value = (double)min;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  �Ǿ��ͻ��ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_min( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    datatype min;
    size_t n_valid, i;

    /* get min */
    min = NAN;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(vals[i]) ) {
	    n_valid ++;
	    min = vals[i];
	    i ++;
	    break;
	}
    }
    for ( ; i < n ; i++ ) {
	if ( isfinite(vals[i]) ) {
	    n_valid ++;
	    if ( vals[i] < min ) min = vals[i];
	}
    }
    if ( 0 < n_valid ) ret_value = (double)min;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain MIN for all elements */
/**
 * @brief  �����ǤκǾ��ͤ����
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_min( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */

    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_min((const float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_min((const double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_min((const unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_min((const int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_min((const int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_min((const int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain MIN along x axis */
/* and return an array whose x_length = 1     */
/**
 * @brief  x�����ǺǾ��ͤ�׻�����x������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_min_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());	/* data source */
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get min */
	if ( obj.size_type() == FLOAT_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_f_min((const float *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_min((const unsigned char *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_min((const int16_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get min */
	if ( obj.size_type() == DOUBLE_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_f_min((const double *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_min((const int32_t *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_min((const int64_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along y axis */
/* and return an array whose y_length = 1     */
/**
 * @brief  y�����ǺǾ��ͤ�׻�����y������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_min_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get min */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_min(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_min(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along z axis */
/* and return an array whose z_length = 1     */
/**
 * @brief  z�����ǺǾ��ͤ�׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @attention  z������Ĺ������������硤�ѥե����ޥ󥹤��㲼���ޤ���
 *             ���ξ��ϡ�md_min_small_z() �򤪻Ȥ�����������
 */
inline static mdarray md_min_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get min */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_min(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_min(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MIN along z axis */
/* optimized for small length of z.           */
/* and return an array whose z_length = 1     */
/**
 * @brief  z�����ǺǾ��ͤ�׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����Ǥι�®transpose�˲ä���zx��ñ�̤ǥ������ݤ��뤿�ᡤ��®��
 *         ư��ޤ���
 */
inline static mdarray md_min_small_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {

	mdarray_float ret_arr0;				/* to store result */
	float *dest_ptr;
	const float *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get min */
	    while ( (s_ptr=obj.scan_zx_planes_f(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_min(s_ptr, n_z, NULL);   /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {

	mdarray_double ret_arr0;			/* to store result */
	double *dest_ptr;
	const double *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get min */
	    while ( (s_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_min(s_ptr, n_z, NULL);   /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  M A X  ===============================================================*/
/*===========================================================================*/

/* for integer */
/**
 * @brief  �����ͻ��ФΤ���ζ��̤Υ����� (��������)
 *
 * @param   vals ������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_max( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    datatype max = 0;
    size_t n_valid, i;

    /* get max */
    n_valid = n;
    i = 0;
    if ( i < n ) {
	max = vals[i];
	i++;
    }
    for ( ; i < n ; i++ ) {
	if ( max < vals[i] ) max = vals[i];
    }
    if ( 0 < n_valid ) ret_value = (double)max;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* for float and double */
/**
 * @brief  �����ͻ��ФΤ���ζ��̤Υ����� (��ư���������ѡ�finiteȽ��ͭ)
 *
 * @param   vals ��ư��������������
 * @param   n ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  ������
 */
template <class datatype>
static double get_f_max( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    double ret_value = NAN;			/* return value */
    datatype max;
    size_t n_valid, i;

    /* get max */
    max = NAN;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	if ( isfinite(vals[i]) ) {
	    n_valid ++;
	    max = vals[i];
	    i ++;
	    break;
	}
    }
    for ( ; i < n ; i++ ) {
	if ( isfinite(vals[i]) ) {
	    n_valid ++;
	    if ( max < vals[i] ) max = vals[i];
	}
    }
    if ( 0 < n_valid ) ret_value = (double)max;
    else ret_value = NAN;

    if ( ret_n_valid != NULL ) *ret_n_valid = n_valid;

    return ret_value;
}

/* public function to obtain MAX for all elements */
/**
 * @brief  �����Ǥκ����ͤ����
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_max( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */

    const void *ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_max((const float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_max((const double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_max((const unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_max((const int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_max((const int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_max((const int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain MAX along x axis */
/* and return an array whose x_length = 1     */
/**
 * @brief  x�����Ǻ����ͤ�׻�����x������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_max_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    const char *s_ptr = (const char *)(obj.data_ptr());	/* data source */
    const size_t len_x = obj.col_length();
    const size_t blen_x = obj.bytes() * len_x;
    size_t ii, i;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_float ret_arr0;				/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	float *dest_ptr = ret_arr0.array_ptr();

	/* get max */
	if ( obj.size_type() == FLOAT_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_f_max((const float *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == UCHAR_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_max((const unsigned char *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT16_ZT ) {
	   for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
	      *dest_ptr = get_max((const int16_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {
	const size_t n_lines = obj.length() / obj.col_length();
	mdarray_double ret_arr0;			/* to store result */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	double *dest_ptr = ret_arr0.array_ptr();

	/* get max */
	if ( obj.size_type() == DOUBLE_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_f_max((const double *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT32_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_max((const int32_t *)s_ptr, len_x, NULL);
	}
	else if ( obj.size_type() == INT64_ZT ) {
	    for ( i=0 ; i < n_lines ; i++, s_ptr+=blen_x, dest_ptr++ ) 
		*dest_ptr = get_max((const int64_t *)s_ptr, len_x, NULL);
	}

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along y axis */
/* and return an array whose y_length = 1     */
/**
 * @brief  y�����Ǻ����ͤ�׻�����y������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_max_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get max */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_max(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_max(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along z axis */
/* and return an array whose z_length = 1     */
/**
 * @brief  z�����Ǻ����ͤ�׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @attention  z������Ĺ������������硤�ѥե����ޥ󥹤��㲼���ޤ���
 *             ���ξ��ϡ�md_max_small_z() �򤪻Ȥ�����������
 */
inline static mdarray md_max_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get max */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    const float *s_ptr;				/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_max(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    const double *s_ptr;			/* to be read */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_max(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MAX along z axis */
/* optimized for small length of z.           */
/* and return an array whose z_length = 1     */
/**
 * @brief  z�����Ǻ����ͤ�׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����Ǥι�®transpose�˲ä���zx��ñ�̤ǥ������ݤ��뤿�ᡤ��®��
 *         ư��ޤ���
 */
inline static mdarray md_max_small_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {

	mdarray_float ret_arr0;				/* to store result */
	float *dest_ptr;
	const float *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get max */
	    while ( (s_ptr=obj.scan_zx_planes_f(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_max(s_ptr, n_z, NULL);   /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {

	mdarray_double ret_arr0;			/* to store result */
	double *dest_ptr;
	const double *s_ptr;				/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get max */
	    while ( (s_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_max(s_ptr, n_z, NULL);   /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  M E D I A N  =========================================================*/
/*===========================================================================*/

/**
 * @brief  ��®����ʪ�� median �򻻽�
 *
 *  ���Τ� median (IRAF�Ρ֥������median�פ���midpt�ǤϤʤ�) ����ޤ���<br>
 *  ����������ư��������ξ���˻��Ѳ�ǽ�Ǥ���
 *
 * @param   buf_ptr Ǥ�դΥץ�ߥƥ��ַ��������Ƭ���ɥ쥹��̵�����̵����(NaN)
 *                  ���ޤޤ�Ƥ��ʤ�����
 * @param   buf_len ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  median��
 *
 * @attention  ��������ƤϾ�񤭤���ޤ���<br>
 *             ��ư���������ξ��� finite �����å��ϹԤʤ��ޤ���
 *             �������äơ������̵�����̵����(NaN)���ޤޤ줿���η�̤�
 *             ̤����Ǥ���
 * @note  quick sort �Υ��르�ꥺ��򻲹ͤˡ�ɬ�פ���ʬ�����¤٤����륢�르�ꥺ
 *        ���ȤäƤ��롥std::sort() �Ǥ���� 2������®����IRAF �� midpt ��
 *        ��®�٤Ƿ׻��Ǥ��롥��äƤ����Ȭ�����������Υͥ����ե� median �ؿ�
 *        �Ȥۤ�Ʊ����<br>
 *        ��ե����: <br>
 *        http://www.sciencedirect.com/science/article/pii/S0022000073800339
 *
 */
/* �ǥХå��Υ����å� */
//#define DEBUG_GET_MEDIAN 1
template <class datatype>
static double get_median( datatype *buf_ptr, const size_t buf_len,
			  size_t *ret_n_valid )
{
    double ret_value = NAN;
    size_t src_start, src_end1;			/* �롼�פγ��ϡ���λ+1 */
    size_t buf_pos_right1;			/* �롼�׽�λ����ʬ���� */
    datatype pivot;

    if ( buf_ptr == NULL ) goto quit;

    /* small optimize */
    if ( buf_len <= 2 ) {
	if (buf_len == 2) 
	    ret_value = ((double)(buf_ptr[0]) + (double)(buf_ptr[1])) / 2.0;
	else if (buf_len == 1) 
	    ret_value = (double)(buf_ptr[0]);
	goto quit;
    }

    src_start = 0;
    src_end1 = buf_len;

    pivot = buf_ptr[0];

    /*
     * �ᥤ��롼��
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
	    /* ����������� `<=' �Ǥ���ɬ�פ����� */
	    if ( v <= pivot ) {
		/* �����Ǽ��� pivot �ͤθ����ȤäƤ��ɤ����� */
		/* ���ä��ζ�֤��ΤƤ����ǽ�������뤫��ʤ������� */
	    }
	    else {
		while ( i < j ) {
		    /* ������ i==j �β�ǽ�������뤬��OK */
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
	/* ���: ������ i ��������ƤϤ����ʤ� */
	buf_pos_right1 = j;

	/*
	 * ����λ�Զ�֤���ꤹ��
	 */

	/*
	 * pivot �ͤ��ۤ� median �ͤȤ��ƥҥåȤ��Ƥ��ޤä����
	 */
	if ( buf_len % 2 == 0 && buf_pos_right1 == buf_len / 2 ) {
	    size_t m_pos;
	    datatype l_max, r_min;
	    /*
	     * �������줿�Хåե��ˤ����ơ�
	     * ��¦��֤ǤϺ����ͤ򡤱�¦��֤ǤϺǾ��ͤ�Ȥ�
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
	    /* ��¦�κ��硤��¦�κǾ�����ʿ�Ѥ�Ȥäơ���λ */
	    ret_value = ((double)l_max + (double)r_min) / 2.0;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [X]");
#endif
	    goto quit;
	}
	/*  pivot ����¦�ˤ�����
	 *                   piv    |
	 *  |_________________+____________________________|
	 *      do not use                 use
	 */
	/* ������������ϡ����ǿ�������ξ��� `<=' �Ǥ���ɬ�פ����롥  */
	/* ���ǿ��������λ��� == �ʾ��ϡ����� if ʸ�ξ��ˤ�����Τǡ�*/
	/* �����ˤϤ��ʤ���                                              */
	else if ( buf_pos_right1 <= buf_len / 2 ) {
	    pivot = buf_ptr[buf_pos_right1];
	    src_start = buf_pos_right1;
#ifdef DEBUG_GET_MEDIAN
	    err_report(__FUNCTION__,"DEBUG", "case [A]");
#endif
	}
	/*  pivot ����¦�ˤ�����
	 *                          |  piv
	 *  |___________________________+___________________|
	 *             use                   do not use
	 */
	else {

	    /* pivot �ͤ�ꤹ�٤Ƥ���¦�ˤ������ */
	    if ( src_end1 == buf_pos_right1 ) {
		/* ���� pivot �ͤ򸫤Ĥ��� */
		for ( j=src_start ; j < src_end1 ; j++ ) {
		    if ( buf_ptr[j] != pivot ) {
			pivot = buf_ptr[j];
			break;
		    }
		}
		if ( j == src_end1 ) {
		    ret_value = (double)(buf_ptr[src_start]);
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
    if ( ret_n_valid != NULL ) *ret_n_valid = buf_len;
    return ret_value;
}

/**
 * @brief  ��®����ʪ�� median �򻻽� (��ư���������ѡ�finiteȽ��ͭ)
 *
 *  ���Τ� median (IRAF�Ρ֥������median�פ���midpt�ǤϤʤ�) ����ޤ���<br>
 *  ��ư�����������ѤǤ���
 *
 * @param   buf_ptr Ǥ�դ���ư���������������Ƭ���ɥ쥹��
 * @param   buf_len ����θĿ�
 * @param   ret_n_valid �����ͻ��Ф˻Ȥ�줿���ǿ� (�֤��͡����׻���NULL������)
 * @return  median��
 *
 * @attention  ��������ƤϾ�񤭤���ޤ���
 */
template <class datatype>
inline static double get_f_median( datatype *buf_ptr, const size_t buf_len,
				   size_t *ret_n_valid )
{
    size_t n_valid, i;
    datatype v;

    n_valid = 0;
    for ( i=0 ; i < buf_len ; i++ ) {		/* loop for all */
	v = buf_ptr[i];
	if ( isfinite(v) ) {
	    buf_ptr[n_valid] = v;		/* accumulate finite values */
	    n_valid ++;
	}
    }

    return get_median(buf_ptr, n_valid, ret_n_valid);
}

/* public function to obtain MEDIAN for all elements */
/**
 * @brief  �����Ǥ�median(��ʪ)�����
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����͡��׻���ǽ�ʾ���NAN��
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static double md_median( const mdarray &obj )
{
    double ret_value = NAN;			/* return value */
    mdarray tmp_obj = obj;			/* temporary object */

    void *ptr = tmp_obj.data_ptr();
    const size_t len = tmp_obj.length();
    const int t = tmp_obj.size_type();

    if ( t == FLOAT_ZT ) 
	ret_value = get_f_median((float *)ptr, len, NULL);
    else if ( t == DOUBLE_ZT ) 
	ret_value = get_f_median((double *)ptr, len, NULL);
    else if ( t == UCHAR_ZT ) 
	ret_value = get_median((unsigned char *)ptr, len, NULL);
    else if ( t == INT16_ZT ) 
	ret_value = get_median((int16_t *)ptr, len, NULL);
    else if ( t == INT32_ZT ) 
	ret_value = get_median((int32_t *)ptr, len, NULL);
    else if ( t == INT64_ZT ) 
	ret_value = get_median((int64_t *)ptr, len, NULL);

    return ret_value;
}

/* public function to obtain MEDIAN along x axis */
/* and return an array whose x_length = 1        */
/**
 * @brief  x������median(��ʪ)��׻�����x������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
inline static mdarray md_median_x( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    naxisx[0] = 1;					/* combine at x */

    /* start scan */
    if ( 0 < obj.begin_scan_along_x() ) {

	size_t n;
	ssize_t x, y, z;

	/* get median */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    float *s_ptr;				/* to be sorted */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_x_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_median(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    double *s_ptr;				/* to be sorted */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_x(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_median(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_x();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along y axis */
/* and return an array whose y_length = 1        */
/**
 * @brief  y������median(��ʪ)��׻�����y������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����ǹ�®transpose��¹Ԥ���y�����ι�®���������ǽ�ˤ��Ƥ��ޤ���
 */
inline static mdarray md_median_y( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 1 < obj.dim_length() ) naxisx[1] = 1;		/* combine at y */

    /* start scan */
    if ( 0 < obj.begin_scan_along_y() ) {

	size_t n;
	ssize_t x, y, z;

	/* get median */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    float *s_ptr;				/* to be sorted */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_median(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    double *s_ptr;				/* to be sorted */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_y(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_median(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_y();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along z axis */
/* and return an array whose z_length = 1        */
/**
 * @brief  z������median(��ʪ)��׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @attention  z������Ĺ������������硤�ѥե����ޥ󥹤��㲼���ޤ���
 *             ���ξ��ϡ�md_median_small_z() �򤪻Ȥ�����������
 */
inline static mdarray md_median_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    for ( ii=0 ; 
	  0 < obj.begin_scan_along_z(0, obj.length(0),  0, obj.length(1), 
				     0 + obj.length(2) * ii, obj.length(2)) ;
	  ii++ ) {

	size_t n;
	ssize_t x, y, z;

	/* get median */
	if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	     obj.size_type() == INT16_ZT ) {

	    mdarray_float ret_arr0;			/* to store result */
	    float *dest_ptr;
	    float *s_ptr;				/* to be sorted */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z_f(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_median(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
	else {

	    mdarray_double ret_arr0;			/* to store result */
	    double *dest_ptr;
	    double *s_ptr;				/* to be sorted */

	    /* resize and get pointer of dest */
	    ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	    dest_ptr = ret_arr0.array_ptr();

	    while ( (s_ptr=obj.scan_along_z(&n,&x,&y,&z)) != NULL ) {
		*dest_ptr = get_f_median(s_ptr, n, NULL);
		dest_ptr ++;
	    }

	    ret_arr0.cut(&ret_array);			/* move data */
	}
    }
    obj.end_scan_along_z();

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/* public function to obtain MEDIAN along z axis */
/* optimized for small length of z.              */
/* and return an array whose z_length = 1        */
/**
 * @brief  z������median(��ʪ)��׻�����z������Ĺ����1����������
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @return  �����ͤ���Ǽ���줿����float��, unsigned char��, int16_t���������
 *          �Ф���׻��ξ���float�������󡤤���ʳ���double����������֤���
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���<br>
 *         �����Ǥι�®transpose�˲ä���zx��ñ�̤ǥ������ݤ��뤿�ᡤ��®��
 *         ư��ޤ���
 */
inline static mdarray md_median_small_z( const mdarray &obj )
{
    mdarray ret_array;				/* array to be returned */
    mdarray_size naxisx;			/* new dimension info */
    size_t ii;

    if ( obj.length() == 0 ) goto quit;

    /* setup dim info for returned array */
    naxisx.resize( obj.dim_length() );
    for ( ii=0 ; ii < obj.dim_length() ; ii++ ) naxisx[ii] = obj.length(ii);
    if ( 2 < obj.dim_length() ) naxisx[2] = 1;		/* combine at z */

    /* start scan */
    if ( obj.size_type() == FLOAT_ZT || obj.size_type() == UCHAR_ZT ||
	 obj.size_type() == INT16_ZT ) {

	mdarray_float ret_arr0;				/* to store result */
	float *dest_ptr;
	float *s_ptr;					/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get median */
	    while ( (s_ptr=obj.scan_zx_planes_f(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_median(s_ptr, n_z, NULL);    /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }
    else {

	mdarray_double ret_arr0;			/* to store result */
	double *dest_ptr;
	double *s_ptr;					/* to be read */

	/* resize and get pointer of dest */
	ret_arr0.resize( naxisx.carray(), obj.dim_length(), false );
	dest_ptr = ret_arr0.array_ptr();

	for ( ii=0 ; 
	      0 < obj.begin_scan_zx_planes(0, obj.length(0),  0, obj.length(1),
	      0 + obj.length(2) * ii, obj.length(2)) ;
	      ii++ ) {
	    size_t n_z, n_x, j;
	    ssize_t x, y, z;
	    /* get median */
	    while ( (s_ptr=obj.scan_zx_planes(&n_z,&n_x,&x,&y,&z)) != NULL ) {
		for ( j=0 ; j < n_x ; j++ ) {		       /* loop for x */
		    *dest_ptr = get_f_median(s_ptr, n_z, NULL);    /* scan z */
		    s_ptr += n_z;
		    dest_ptr ++;
		}
	    }
	}
	obj.end_scan_zx_planes();

	ret_arr0.cut(&ret_array);			/* move data */
    }

 quit:
    ret_array.init_properties(obj);		/* copy the properties */
    ret_array.set_scopy_flag();
    return ret_array;
}

/*===========================================================================*/
/*===  M O M E N T  =========================================================*/
/*===========================================================================*/

/* for integer, float and double */
/**
 * @brief  md_moment�ؿ��Τ���δ��ܥ����� (��ư������,����ξ�ѡ�finiteȽ��ͭ)
 *
 * @param   vals Ǥ�դΥץ�ߥƥ��ַ�������
 * @param   n ����θĿ�
 * @param   minus1 ���١����٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @param   ret_array ʿ���͡�ʬ�������١����٤��Ǽ����Ĺ��4��1��������
 * @return  �����ͻ��Ф˻Ȥ�줿���ǿ�
 * @note   ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 */
template <class datatype>
static size_t get_f_moment( const datatype *vals, size_t n, bool minus1,
			    double ret_array[] )
{
    double v, sum, sum2, sum3, sum4;
    size_t n_valid, i;
    size_t m1;

    if ( minus1 == true ) m1 = 1;
    else m1 = 0;

    /* get required values */
    sum = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    n_valid = 0;
    for ( i=0 ; i < n ; i++ ) {
	v = (double)(vals[i]);
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
    }
    else {
	ret_array[0] = NAN;
	ret_array[1] = NAN;
	ret_array[2] = NAN;
	ret_array[3] = NAN;
    }

    return n_valid;
}

/* public function to obtain MOMENT for all elements.              */
/* Set NULL to ret_mdev or ret_sdev when not required their return */
/**
 * @brief  �����Ǥ�ʿ���͡�ʬ�������١����١�ʿ�������к���ɸ���к������
 *
 *  ����minus1��true�λ��������ͤ�����ϲ����ΤȤ���Ǥ���<br>
 *  (IRAF�Υޥ˥奢��ΰ���): <br>
 *        mean = sum (x1,...,xN) / N                                       <br>
 *           y = x - mean                                                  <br>
 *  meanabsdev = sum (abs(y1),...,abs(yN)) / N                             <br>
 *    variance = sum (y1 ** 2,...,yN ** 2) / (N-1)                         <br>
 *      stddev = sqrt (variance)                                           <br>
 *    skewness = sum ((y1 / stddev) ** 3,...,(yN / stddev) ** 3) / (N-1)   <br>
 *    kurtosis = sum ((y1 / stddev) ** 4,...,(yN / stddev) ** 4) / (N-1) - 3
 *
 * @param   obj �׻����оݤȤʤ����󥪥֥�������
 * @param   minus1 ���١����٤λ��Ф� (N-1) �ǳ�������Ȥ��ʤ鿿
 * @param   ret_mdev ʿ�������к������������� non-NULL �򥻥å�
 * @param   ret_sdev ɸ���к������������� non-NULL �򥻥å�
 * @return  ʿ���͡�ʬ�������١����٤��Ǽ����Ĺ��4��1��������
 * @note    ̵�����̵����(NaN)�Ͻ������Ʒ׻�����ޤ���
 * @note   ���١����٤ˤĤ��Ƥ� IDL �Ǥ� N �ǳ�������IRAF �Ǥ� (N-1) �ǳ��
 *         �������Ѥ��Ƥ��ޤ���
 */
inline static mdarray_double md_moment( const mdarray &obj, bool minus1,
					double *ret_mdev, double *ret_sdev )
{
    mdarray_double ret_array;			/* array to be returned */
    size_t n_valid = 0;

    const void *s_ptr = obj.data_ptr();
    const size_t len = obj.length();
    const int t = obj.size_type();

    ret_array.init_properties(obj);
    ret_array.resize(4).fill(NAN);
    double *d_ptr = ret_array.array_ptr();

    if ( t == FLOAT_ZT ) 
	n_valid = get_f_moment((const float *)s_ptr, len, minus1, d_ptr);
    else if ( t == DOUBLE_ZT ) 
	n_valid = get_f_moment((const double *)s_ptr, len, minus1, d_ptr);
    else if ( t == UCHAR_ZT ) 
	n_valid = get_f_moment((const unsigned char *)s_ptr, len, minus1, d_ptr);
    else if ( t == INT16_ZT ) 
	n_valid = get_f_moment((const int16_t *)s_ptr, len, minus1, d_ptr);
    else if ( t == INT32_ZT ) 
	n_valid = get_f_moment((const int32_t *)s_ptr, len, minus1, d_ptr);
    else if ( t == INT64_ZT ) 
	n_valid = get_f_moment((const int64_t *)s_ptr, len, minus1, d_ptr);

    const double mean = ret_array[0];
    const double variance = ret_array[1];

    if ( ret_sdev != NULL ) {
	if ( finite(variance) ) *ret_sdev = sqrt(variance);
	else *ret_sdev = NAN;
    }

    if ( ret_mdev == NULL ) goto quit;
    if ( ! isfinite(mean) ) goto quit;

    if ( t == FLOAT_ZT ) 
	*ret_mdev = get_f_meanabsdev((const float *)s_ptr, len, mean);
    else if ( t == DOUBLE_ZT ) 
	*ret_mdev = get_f_meanabsdev((const double *)s_ptr, len, mean);
    else if ( t == UCHAR_ZT ) 
	*ret_mdev = get_meanabsdev((const unsigned char *)s_ptr, len, mean);
    else if ( t == INT16_ZT ) 
	*ret_mdev = get_meanabsdev((const int16_t *)s_ptr, len, mean);
    else if ( t == INT32_ZT ) 
	*ret_mdev = get_meanabsdev((const int32_t *)s_ptr, len, mean);
    else if ( t == INT64_ZT ) 
	*ret_mdev = get_meanabsdev((const int64_t *)s_ptr, len, mean);
    else *ret_mdev = NAN;

 quit:
    ret_array.set_scopy_flag();
    return ret_array;
}


/*===========================================================================*/
/*===========================================================================*/

/**
 * @example  examples_sllib/array_statistics.cc
 *           ¿����������Ф��������Ѵؿ���Ȥä���
 */

}	/* namespace sli */


#endif	/* _SLI__MDARRAY_STATISTICS_H */
