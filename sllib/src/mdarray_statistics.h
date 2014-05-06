/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-24 14:18:50 cyamauch> */

#ifndef _SLI__MDARRAY_STATISTICS_H
#define _SLI__MDARRAY_STATISTICS_H 1

/*
 * statistics package for mdarray class
 */

/**
 * @file   mdarray_statistics.h
 * @brief  mdarrayクラスとその継承クラスのオブジェクトで使用可能な統計用関数
 * @attention  C++ 標準ライブラリの <cmath> を使う場合，マクロ SLI__USE_CMATH
 *             を定義してから mdarray_statistics.h を include してください．
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
 * @brief  合計値算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  合計値算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の合計値を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で合計値を計算し，x方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で合計値を計算し，y方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で合計値を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @attention  z方向の長さが小さい場合，パフォーマンスが低下します．
 *             その場合は，md_total_small_z() をお使いください．
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
 * @brief  z方向で合計値を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部での高速transposeに加え，zx面単位でメモリを確保するため，高速に
 *         動作します．
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
 * @brief  平均値算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  平均値算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の平均値を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で平均値を計算し，x方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で平均値を計算し，y方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で平均値を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @attention  z方向の長さが小さい場合，パフォーマンスが低下します．
 *             その場合は，md_mean_small_z() をお使いください．
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
 * @brief  z方向で平均値を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部での高速transposeに加え，zx面単位でメモリを確保するため，高速に
 *         動作します．
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
 * @brief  平均絶対偏差算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   mean 平均値．不明な場合は NAN をセットしても良い．
 * @return  統計値
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
 * @brief  平均絶対偏差算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   mean 平均値．不明な場合は NAN をセットしても良い．
 * @return  統計値
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
 * @brief  全要素の平均絶対偏差を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で平均絶対偏差を計算し，x方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で絶対平均偏差を計算し，y方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で絶対平均偏差を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  分散算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  分散算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の分散を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で分散を計算し，x方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で分散を計算し，y方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で分散を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  歪度算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   minus1 歪度の算出に (N-1) で割る定義を使うなら真
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  歪度算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   minus1 歪度の算出に (N-1) で割る定義を使うなら真
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の歪度を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 歪度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値．計算不能な場合はNAN．
 * @note    無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  x方向で歪度を計算し，x方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 歪度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note    無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  y方向で歪度を計算し，y方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 歪度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  z方向で歪度を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 歪度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  尖度算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   minus1 尖度の算出に (N-1) で割る定義を使うなら真
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  尖度算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   minus1 尖度の算出に (N-1) で割る定義を使うなら真
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の尖度を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 尖度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  x方向で尖度を計算し，x方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 尖度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  y方向で尖度を計算し，y方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 尖度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  z方向で尖度を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 尖度の算出に (N-1) で割る定義を使うなら真
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 * @brief  標準偏差算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
 */
template <class datatype>
inline static double get_stddev( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    const double variance = get_variance(vals, n, ret_n_valid);
    return (isfinite(variance) ? sqrt(variance) : NAN);
}

/* for float and double */
/**
 * @brief  標準偏差算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
 */
template <class datatype>
inline static double get_f_stddev( const datatype *vals, size_t n, size_t *ret_n_valid )
{
    const double variance = get_f_variance(vals, n, ret_n_valid);
    return (isfinite(variance) ? sqrt(variance) : NAN);
}

/* public function to obtain STDDEV for all elements */
/**
 * @brief  全要素の標準偏差を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で標準偏差を計算し，x方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で標準偏差を計算し，y方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で標準偏差を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @attention  z方向の長さが小さい場合，パフォーマンスが低下します．
 *             その場合は，md_stddev_small_z() をお使いください．
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
 * @brief  z方向で標準偏差を計算し，z方向の長さが1の配列を取得
 *
 *  統計値の定義については mdarray_double md_moment() 関数の解説を
 *  参照してください．
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部での高速transposeに加え，zx面単位でメモリを確保するため，高速に
 *         動作します．
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
 * @brief  最小値算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  最小値算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の最小値を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で最小値を計算し，x方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で最小値を計算し，y方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で最小値を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @attention  z方向の長さが小さい場合，パフォーマンスが低下します．
 *             その場合は，md_min_small_z() をお使いください．
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
 * @brief  z方向で最小値を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部での高速transposeに加え，zx面単位でメモリを確保するため，高速に
 *         動作します．
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
 * @brief  最大値算出のための共通のコード (整数値用)
 *
 * @param   vals 整数型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  最大値算出のための共通のコード (浮動小数点値用・finite判定有)
 *
 * @param   vals 浮動小数点型の配列
 * @param   n 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  統計値
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
 * @brief  全要素の最大値を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向で最大値を計算し，x方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向で最大値を計算し，y方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向で最大値を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @attention  z方向の長さが小さい場合，パフォーマンスが低下します．
 *             その場合は，md_max_small_z() をお使いください．
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
 * @brief  z方向で最大値を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部での高速transposeに加え，zx面単位でメモリを確保するため，高速に
 *         動作します．
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
 * @brief  高速に本物の median を算出
 *
 *  正確な median (IRAFの「インチキmedian」ことmidptではない) を求めます．<br>
 *  整数型・浮動小数点型両方に使用可能です．
 *
 * @param   buf_ptr 任意のプリミティブ型配列の先頭アドレス．無限大や無効値(NaN)
 *                  が含まれていない事．
 * @param   buf_len 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  median値
 *
 * @attention  配列の内容は上書きされます．<br>
 *             浮動小数点数の場合の finite チェックは行なわれません．
 *             したがって，配列に無限大や無効値(NaN)が含まれた場合の結果は
 *             未定義です．
 * @note  quick sort のアルゴリズムを参考に，必要な部分だけ並べかえるアルゴリズ
 *        ムを使っている．std::sort() でやるより 2倍程度速く，IRAF の midpt 並
 *        の速度で計算できる．やってる事は八木先生謹製のネコソフト median 関数
 *        とほぼ同じ．<br>
 *        リファレンス: <br>
 *        http://www.sciencedirect.com/science/article/pii/S0022000073800339
 *
 */
/* デバッグのスイッチ */
//#define DEBUG_GET_MEDIAN 1
template <class datatype>
static double get_median( datatype *buf_ptr, const size_t buf_len,
			  size_t *ret_n_valid )
{
    double ret_value = NAN;
    size_t src_start, src_end1;			/* ループの開始，終了+1 */
    size_t buf_pos_right1;			/* ループ終了時の分岐点 */
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
	    ret_value = ((double)l_max + (double)r_min) / 2.0;
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
 * @brief  高速に本物の median を算出 (浮動小数点値用・finite判定有)
 *
 *  正確な median (IRAFの「インチキmedian」ことmidptではない) を求めます．<br>
 *  浮動小数点型専用です．
 *
 * @param   buf_ptr 任意の浮動小数点型配列の先頭アドレス．
 * @param   buf_len 配列の個数
 * @param   ret_n_valid 統計値算出に使われた要素数 (返り値・不要時はNULLを設定)
 * @return  median値
 *
 * @attention  配列の内容は上書きされます．
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
 * @brief  全要素のmedian(本物)を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値．計算不能な場合はNAN．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  x方向でmedian(本物)を計算し，x方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  y方向でmedian(本物)を計算し，y方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部で高速transposeを実行し，y方向の高速スキャンを可能にしています．
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
 * @brief  z方向でmedian(本物)を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．
 * @attention  z方向の長さが小さい場合，パフォーマンスが低下します．
 *             その場合は，md_median_small_z() をお使いください．
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
 * @brief  z方向でmedian(本物)を計算し，z方向の長さが1の配列を取得
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @return  統計値が格納された配列．float型, unsigned char型, int16_t型の配列に
 *          対する計算の場合はfloat型の配列，それ以外はdouble型の配列を返す．
 * @note   無限大や無効値(NaN)は除外して計算されます．<br>
 *         内部での高速transposeに加え，zx面単位でメモリを確保するため，高速に
 *         動作します．
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
 * @brief  md_moment関数のための基本コード (浮動小数点,整数両用・finite判定有)
 *
 * @param   vals 任意のプリミティブ型の配列
 * @param   n 配列の個数
 * @param   minus1 歪度・尖度の算出に (N-1) で割る定義を使うなら真
 * @param   ret_array 平均値，分散，歪度，尖度を格納した長さ4の1次元配列
 * @return  統計値算出に使われた要素数
 * @note   無限大や無効値(NaN)は除外して計算されます．
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
 * @brief  全要素の平均値，分散，歪度，尖度，平均絶対偏差，標準偏差を取得
 *
 *  引数minus1がtrueの時，統計値の定義は下記のとおりです．<br>
 *  (IRAFのマニュアルの引用): <br>
 *        mean = sum (x1,...,xN) / N                                       <br>
 *           y = x - mean                                                  <br>
 *  meanabsdev = sum (abs(y1),...,abs(yN)) / N                             <br>
 *    variance = sum (y1 ** 2,...,yN ** 2) / (N-1)                         <br>
 *      stddev = sqrt (variance)                                           <br>
 *    skewness = sum ((y1 / stddev) ** 3,...,(yN / stddev) ** 3) / (N-1)   <br>
 *    kurtosis = sum ((y1 / stddev) ** 4,...,(yN / stddev) ** 4) / (N-1) - 3
 *
 * @param   obj 計算の対象となる配列オブジェクト
 * @param   minus1 歪度，尖度の算出に (N-1) で割る定義を使うなら真
 * @param   ret_mdev 平均絶対偏差を取得する場合に non-NULL をセット
 * @param   ret_sdev 標準偏差を取得する場合に non-NULL をセット
 * @return  平均値，分散，歪度，尖度を格納した長さ4の1次元配列
 * @note    無限大や無効値(NaN)は除外して計算されます．
 * @note   歪度・尖度については IDL では N で割る定義を，IRAF では (N-1) で割る
 *         定義を採用しています．
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
 *           多次元配列に対する統計用関数を使った例
 */

}	/* namespace sli */


#endif	/* _SLI__MDARRAY_STATISTICS_H */
