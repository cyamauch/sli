/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:54:07 cyamauch> */
 
#ifndef _SLI__MDARRAY_FLOAT_H
#define _SLI__MDARRAY_FLOAT_H 1

/**
 * @file   mdarray_float.h
 * @brief  float型の多次元配列を扱う mdarray_float クラスのヘッダファイル
 */

#include "sli_config.h"
#include "mdarray.h"
#if 0
#include "complex.h"
#endif

namespace sli
{

/*
 * sli::mdarray_float is an inherited class of sli::mdarray, and is a 
 * high-level class of SLLIB.  See also mdarray.h for all member functions.
 * 
 * This class can handle multidimensional arrays, and provide a lot of useful
 * member functions such as mathematical operations, image processings, etc.
 */

/**
 * @class  sli::mdarray_float
 * @brief  float型の多次元配列を扱うためのクラス
 *
 *   mdarray_floatクラスは，float型の多次元配列を IDL や Python のように
 *   手軽に扱えるようにします．下記のような機能を持ちます．<br>
 *    - 演算子等による配列とスカラー・配列との演算 <br>
 *    - 次元数・要素数の動的な変更 <br>
 *    - IDL/IRAF風の表現 (例: "0:99,*") による配列の編集・演算 <br>
 *    - 2D・3Dデータのポインタ配列の自動生成 <br>
 *    - y方向・z方向への高速スキャンのための高速transpose機能 <br>
 *    - 配列に対する数学関数 (mdarray_math.h) <br>
 *    - 統計用関数 (moment，median，etc.; mdarray_statistics.h) <br>
 *    - C99準拠の複素関数 (complex.h) 等 <br>
 *   配列の次元数に関係なく内部は常に1次元バッファで，間接参照に対しては緩く
 *   安全なAPIですので，従来のCで書かれた数学ライブラリとの併用も簡単です．<br>
 *   配列の型種別は動的に変更したい場合，基底クラスのmdarrayをお使いください．
 * 
 * @note  mdarray_floatクラスは mdarray(基底クラス) を継承しています．<br>
 *        SIMD命令(SSE2)により，基本的な部分が高速化されています．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class mdarray_float : public mdarray
  {

  public:
    /* constructor                                                           */
    /* The mdarray_float class has two kinds of operating modes:            */
    /*  1. automatic resize mode                                             */
    /*  2. manual resize mode                                                */
    /* The operating mode is decided by auto_resize arg of constructor or    */
    /* init() member functions.  set_auto_resize() can also change the mode. */
    /* Basically automatic resize mode has larger overhead when calling      */
    /* member functions that can change the size of internal buffer.         */
    mdarray_float();
    mdarray_float( bool auto_resize );
    mdarray_float( bool auto_resize, float **extptr_address );
    mdarray_float( bool auto_resize, float *const **extptr2d_address );
    mdarray_float( bool auto_resize, float *const *const **extptr3d_address );
    mdarray_float( bool auto_resize, const size_t naxisx[], size_t ndim, bool init_buf );
    mdarray_float( bool auto_resize, size_t naxis0 );
    mdarray_float( bool auto_resize, size_t naxis0, size_t naxis1 );
    mdarray_float( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2 );
    mdarray_float( bool auto_resize, size_t naxis0, 
		   const float vals[] );
    mdarray_float( bool auto_resize, size_t naxis0, size_t naxis1,
		   const float vals[] );
    mdarray_float( bool auto_resize, size_t naxis0, size_t naxis1,
		   const float *const vals[] );
    mdarray_float( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
		   const float vals[] );
    mdarray_float( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
		   const float *const *const vals[] );
#if 0
    mdarray_float( bool auto_resize, const char *dim_info, double v0, ... );
#endif
    mdarray_float( const mdarray_float &obj );


    /* --------------------------------------------------------------------- */

    /* `virtual' がついてるものはオーバーライドではなくオーバーロード     */
    /*   mdarray_float &operator=(const mdarray_float &obj);            */
    /* が隠れて存在する事に注意 (演算子 = だけは，特別な規則があるらしい) */

    /* substitute an array (copy the attribute, too)                      */
    virtual mdarray_float &operator=(const mdarray &obj);

    /* (overridden) */
    mdarray_float &operator+=(const mdarray &obj);
    mdarray_float &operator-=(const mdarray &obj);
    mdarray_float &operator*=(const mdarray &obj);
    mdarray_float &operator/=(const mdarray &obj);

    /* (overridden) */
    mdarray_float &operator=(dcomplex v);
    mdarray_float &operator=(double v);
    mdarray_float &operator=(long long v);
    mdarray_float &operator=(long v);
    mdarray_float &operator=(int v);
    mdarray_float &operator+=(dcomplex v);
    mdarray_float &operator+=(double v);
    mdarray_float &operator+=(long long v);
    mdarray_float &operator+=(long v);
    mdarray_float &operator+=(int v);
    mdarray_float &operator-=(dcomplex v);
    mdarray_float &operator-=(double v);
    mdarray_float &operator-=(long long v);
    mdarray_float &operator-=(long v);
    mdarray_float &operator-=(int v);
    mdarray_float &operator*=(dcomplex v);
    mdarray_float &operator*=(double v);
    mdarray_float &operator*=(long long v);
    mdarray_float &operator*=(long v);
    mdarray_float &operator*=(int v);
    mdarray_float &operator/=(dcomplex v);
    mdarray_float &operator/=(double v);
    mdarray_float &operator/=(long long v);
    mdarray_float &operator/=(long v);
    mdarray_float &operator/=(int v);

    /* taken over from mdarray class

    // compare //
    virtual bool operator==(const mdarray &obj) const;
    virtual bool operator!=(const mdarray &obj) const;

    // return the object which stores the result of calculation //
    virtual mdarray operator+(dcomplex v) const;
    virtual mdarray operator+(fcomplex v) const;
    virtual mdarray operator+(double v) const;
    virtual mdarray operator+(float v) const;
    virtual mdarray operator+(long long v) const;
    virtual mdarray operator+(long v) const;
    virtual mdarray operator+(int v) const;
    virtual mdarray operator+(const mdarray &obj) const;
    virtual mdarray operator-(dcomplex v) const;
    virtual mdarray operator-(fcomplex v) const;
    virtual mdarray operator-(double v) const;
    virtual mdarray operator-(float v) const;
    virtual mdarray operator-(long long v) const;
    virtual mdarray operator-(long v) const;
    virtual mdarray operator-(int v) const;
    virtual mdarray operator-(const mdarray &obj) const;
    virtual mdarray operator*(dcomplex v) const;
    virtual mdarray operator*(fcomplex v) const;
    virtual mdarray operator*(double v) const;
    virtual mdarray operator*(float v) const;
    virtual mdarray operator*(long long v) const;
    virtual mdarray operator*(long v) const;
    virtual mdarray operator*(int v) const;
    virtual mdarray operator*(const mdarray &obj) const;
    virtual mdarray operator/(dcomplex v) const;
    virtual mdarray operator/(fcomplex v) const;
    virtual mdarray operator/(double v) const;
    virtual mdarray operator/(float v) const;
    virtual mdarray operator/(long long v) const;
    virtual mdarray operator/(long v) const;
    virtual mdarray operator/(int v) const;
    virtual mdarray operator/(const mdarray &obj) const;

    */

    /* --------------------------------------------------------------------- */

    /*
     * member functions to handle the whole object and properties
     */

    /* initialization of the array */

    /* (overridden)                */
    mdarray_float &init();

    /* <overloaded>                */
    virtual mdarray_float &init( bool auto_resize );
    virtual mdarray_float &init( bool auto_resize, const size_t naxisx[], size_t ndim, bool init_buf );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0 );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, size_t naxis1 );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2 );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, 
				 const float vals[] );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, size_t naxis1,
				 const float vals[] );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, size_t naxis1,
				 const float *const vals[] );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
				 const float vals[] );
    virtual mdarray_float &init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
				 const float *const *const vals[] );
#if 0
    virtual mdarray_float &init( bool auto_resize, const char *dim_info,
				 double v0, ... );
    virtual mdarray_float &vinit( bool auto_resize, const char *dim_info,
				  double v0, va_list );
#endif
    /*  mdarray_float &init( const mdarray &obj ); が隠れている */
    /*  (↑mdarrayクラスのポインタ経由で呼び出し可能)            */

    /* (overridden)                                           */
    mdarray_float &init( const mdarray &obj );

    /* copy properties (auto_resize, auto_init, and rounding) */
    /* (overridden)                                           */
    mdarray_float &init_properties( const mdarray &src_obj );

    /* setting of the resize mode */
    /* (overridden)               */
    mdarray_float &set_auto_resize( bool tf );

    /* setting of the initialize mode */
    /* (overridden)                   */
    mdarray_float &set_auto_init( bool tf );

    /* setting of the rounding off possibility */
    /* (overridden)                            */
    mdarray_float &set_rounding( bool tf );

    /* setting of strategy of memory allocation */
    /* "auto", "min" and "pow" can be set.      */
    /* (overridden)                             */
    mdarray_float &set_alloc_strategy( const char *strategy );

    /* convert the value of the full array element */
    /* <overloaded>                                */
    virtual mdarray_float &convert_via_udf(
		void (*func)(const float *,float *,size_t,int,void *),
		void *user_ptr );

    /* swap contents between self and another objects */
    /* (overridden)                 */
    mdarray_float &swap( mdarray &sobj );

    /* ssize_t copy( mdarray &dest ) const; が隠れている．*/
    /* (↑mdarrayクラスのポインタ経由で呼び出し可能)      */

    /* copy an array into another object */
    /* (overridden)                      */
    ssize_t copy( mdarray *dest ) const;
    /* not recommended */
    ssize_t copy( mdarray &dest ) const;

    /* cut all values in an array and copy them */
    /* (overridden)                             */
    mdarray_float &cut( mdarray *dest );

    /* interchange rows and columns */
    /* (overridden)                 */
    mdarray_float &transpose_xy();

    /* interchange xyz to zxy */
    /* (overridden)           */
    mdarray_float &transpose_xyz2zxy();

    /* rotate image                                              */
    /*   angle: 90,-90, or 180                                   */
    /*          (anticlockwise when image is bottom-left origin) */
    /* (overridden)                                              */
    mdarray_float &rotate_xy( int angle );


    /* --------------------------------------------------------------------- */

    /*
     * APIs to obtain basic information of object
     */

    /* taken over from mdarray class

    // returns integer representing a data type. //
    // see also size_types.h.                    //
    virtual ssize_t size_type() const;

    // number of dimensions of an array //
    virtual size_t dim_length() const;

    // number of all elements //
    virtual size_t length() const;

    // number of elements in a dimension //
    virtual size_t length( size_t dim_index ) const;

    // the length of the array's column //
    virtual size_t col_length() const;		     // axis0 の長さ //
    virtual size_t x_length() const;		     // same as col_length //

    // the length of the array's row  //
    virtual size_t row_length() const;		     // axis1 の長さ //
    virtual size_t y_length() const;		     // same as row_length //

    // the layer number of the array //
    virtual size_t layer_length() const;	     // axis2(以降) の長さ //
    virtual size_t z_length() const;		     // same as layer_length //

    // number of bytes of an element //
    virtual size_t bytes() const;

    // total byte size of all elements //
    virtual size_t byte_length() const;

    // total byte size of elements in a dimension //
    virtual size_t byte_length( size_t dim_index ) const;

    // acquisition of the resize mode //
    virtual bool auto_resize() const;

    // acquisition of the initialize mode //
    virtual bool auto_init() const;

    // acquisition of the rounding off possibility //
    virtual bool rounding() const;

    // acquisition of strategy of memory allocation //
    virtual const char *alloc_strategy() const;

    */


    /* --------------------------------------------------------------------- */

    /*
     * member functions for basic data I/O
     */

    /* taken over from mdarray class

    // high-level access //
    virtual dcomplex dxvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
	      ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual double dvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual long lvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			 ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual long long llvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			       ssize_t idx2 = MDARRAY_INDEF ) const;
    */

    /* substitute a value for an element (high-level access) */
    /* (overridden)                                          */
    mdarray_float &assign( dcomplex value,
			    ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF );
    mdarray_float &assign( double value, 
			    ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF );

    /* a reference to the specified value of the element (1 dimension) */
    virtual float &operator[]( ssize_t idx0 );
    virtual const float &operator[]( ssize_t idx0 ) const;

    /* a reference to the specified value of the element (1-3 dimensions) */
    virtual float &operator()( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				ssize_t idx2 = MDARRAY_INDEF);
    virtual const float &operator()( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				ssize_t idx2 = MDARRAY_INDEF) const;

    /* a reference to the specified value of the element (1-3 dimensions) */
    virtual float &at( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			  ssize_t idx2 = MDARRAY_INDEF);
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const float &at( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				 ssize_t idx2 = MDARRAY_INDEF) const;
#endif
    virtual const float &at_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				 ssize_t idx2 = MDARRAY_INDEF) const;


    /* --------------------------------------------------------------------- */

    /*
     * C-like APIs to input and output data
     */

    /* set a value to an arbitrary element's point */
    /* <overloaded>                                */
    virtual mdarray_float &put( float value, ssize_t idx, size_t len );
    virtual mdarray_float &put( float value,
				size_t dim_index, ssize_t idx, size_t len );

    /* copy the array stored in object into the programmer's buffer. */
    /* <overloaded>                                                  */
    virtual ssize_t get_elements( float *dest_buf, size_t elem_size, 
				ssize_t idx0 = 0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;

    /* copy the array in the programmer's buffer into object's buffer. */
    /* <overloaded>                                                    */
    virtual ssize_t put_elements( const float *src_buf, size_t elem_size,
				ssize_t idx0 = 0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF );


    /* --------------------------------------------------------------------- */

    /*
     * member functions to update length, type, etc.
     */

    /* expansion of the dimension number */
    /* (overridden)                      */
    mdarray_float &increase_dim();

    /* reduction of the dimension number */
    /* (overridden)                      */
    mdarray_float &decrease_dim();

    /* change the length of the array */
    /* (overridden)                   */
    mdarray_float &resize( size_t len );
    mdarray_float &resize( size_t dim_index, size_t len );
    mdarray_float &resize( const mdarray &src );

    /* change the length of the 1-d array */
    /* (overridden)                       */
    mdarray_float &resize_1d( size_t x_len );

    /* change the length of the 2-d array */
    /* (overridden)                       */
    mdarray_float &resize_2d( size_t x_len, size_t y_len );

    /* change the length of the 3-d array */
    /* (overridden)                       */
    mdarray_float &resize_3d( size_t x_len, size_t y_len, size_t z_len );

    /* change the length of the array (supports n-dim) */
    /* (overridden)                                    */
    mdarray_float &resize( const size_t naxisx[], size_t ndim, 
			    bool init_buf );

    /* set a string like "3,2" using printf() style args.             */
    /* Omitting values is allowed.  For example, ",2" does not change */
    /* length of first dimension.                                     */
    /* (overridden)                                                   */
    mdarray_float &resizef( const char *exp_fmt, ... );
    mdarray_float &vresizef( const char *exp_fmt, va_list ap );

    /* change the length of the array relatively */
    /* (overridden)                              */
    mdarray_float &resizeby( ssize_t len );			/* for 1-d */
    mdarray_float &resizeby( size_t dim_index, ssize_t len );

    /* change the length of the 1-d array relatively */
    /* (overridden)                                  */
    mdarray_float &resizeby_1d( ssize_t x_len );

    /* change the length of the 2-d array relatively */
    /* (overridden)                                  */
    mdarray_float &resizeby_2d( ssize_t x_len, ssize_t y_len );

    /* change the length of the 3-d array relatively */
    /* (overridden)                                  */
    mdarray_float &resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len);

    /* change the length of the array relatively                            */
    /*   naxisx_rel:  number of elements to be increased for each dimension */
    /*   ndim:        number of dimension                                   */
    /*   init_buf:    set true to initialize new elements                   */
    /* (overridden)                                                         */
    mdarray_float &resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			       bool init_buf );

    /* set a string like "3,-2" using printf() style args.             */
    /* Omitting values is allowed.  For example, ",-2" does not change */
    /* length of first dimension.                                      */
    /* (overridden)                                                    */
    mdarray_float &resizebyf( const char *exp_fmt, ... );
    mdarray_float &vresizebyf( const char *exp_fmt, va_list ap );

    /* change length of array without adjusting buffer contents */
    /* (overridden)                                             */
    mdarray_float &reallocate( const size_t naxisx[], size_t ndim, 
				bool init_buf );

    /* free current buffer and alloc new memory */
    /* (overridden)                             */
    mdarray_float &allocate( const size_t naxisx[], size_t ndim, 
			      bool init_buf );

    /* setting of the initial value for .resize(), etc. */
    /* <overloaded>                                     */
    virtual mdarray_float &assign_default( float value );

    /* insert a blank section */
    /* (overridden)           */
    mdarray_float &insert( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_float &insert( size_t dim_index, ssize_t idx, size_t len );

    /* erase a section */
    /* (overridden)    */
    mdarray_float &erase( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_float &erase( size_t dim_index, ssize_t idx, size_t len );

    /* copy values between elements (without automatic resizing) */
    /* 移動 (バッファのサイズは変更しない)                       */
    /* (overridden)                                              */
    mdarray_float &move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );				/* for 1-d */
    mdarray_float &move( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );

    /* copy values between elements (with automatic resizing) */
    /* 移動 (バッファのサイズは必要に応じて変更する)          */
    /* (overridden)                                           */
    mdarray_float &cpy( ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );				/* for 1-d */
    mdarray_float &cpy( size_t dim_index, 
			  ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );

    /* replace values between elements */
    /* (overridden)                    */
    mdarray_float &swap( ssize_t idx_src, size_t len, 
			  ssize_t idx_dst );			/* for 1-d */
    mdarray_float &swap( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst );

    /* extract a section           */
    /* see also trimf() and trim() */
    /* (overridden)                */
    mdarray_float &crop( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_float &crop( size_t dim_index, ssize_t idx, size_t len );

    /* flip a section   */
    /* see also flipf() */
    /* (overridden)     */
    mdarray_float &flip( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_float &flip( size_t dim_index, ssize_t idx, size_t len );


    /* raise decimals to the next whole number in a float/double type value */
    /* (overridden)                                                         */
    mdarray_float &ceil();

    /* devalue decimals in a float/double type value */
    /* (overridden)                                  */
    mdarray_float &floor();

    /* round off decimals in a float/double type value */
    /* (overridden)                                    */
    mdarray_float &round();

    /* omit decimals in a float/double type value */
    /* (overridden)                               */
    mdarray_float &trunc();

    /* absolute value of all elements */
    /* (overridden)                   */
    mdarray_float &abs();


    /* --------------------------------------------------------------------- */

    /* 
     * member functions for image processing using IDL style argument 
     * such as "0:99,*".  The expression is set to exp_fmt argument in 
     * member functions.
     *
     * Number of dimension in the expression is unlimited.
     * Note that integer numbers in the string is 0-indexed. 
     *
     * [example]
     *   array1 = array0.sectionf("0:99,*");
     *   array1 = array0.sectionf("%ld:%ld,*", begin, end);
     *
     * Flipping elements in dimensions is supported in sectionf(), copyf(),
     * trimf(), and flipf().  Here is an example to perform flipping elements
     * in first 2 dimensions:
     *   array1 = array0.sectionf("99:0,-*");
     * 
     */

    /* taken over from mdarray class

    // returns trimmed array          //
    // Flipping elements is supported //
    virtual mdarray sectionf( const char *exp_fmt, ... ) const;
    virtual mdarray vsectionf( const char *exp_fmt, va_list ap ) const;

    */

    /* copy all or a section to another mdarray object */
    /* Flipping elements is supported                  */
    /* (overridden)                                    */
    ssize_t copyf( mdarray *dest, const char *exp_fmt, ... ) const;
    ssize_t vcopyf( mdarray *dest, const char *exp_fmt, va_list ap ) const;

    /* copy an convet all or a section to another mdarray object */
    /* Flipping elements is supported                            */
    /* (overridden)                                              */
    /* [not implemented] */
    ssize_t convertf_copy( mdarray *dest, const char *exp_fmt, ... ) const;
    ssize_t vconvertf_copy( mdarray *dest, const char *exp_fmt, va_list ap ) const;

    /* trim a section                 */
    /* Flipping elements is supported */
    /* (overridden)                   */
    mdarray_float &trimf( const char *exp_fmt, ... );
    mdarray_float &vtrimf( const char *exp_fmt, va_list ap );

    /* flip elements in a section */
    /* (overridden)               */
    mdarray_float &flipf( const char *exp_fmt, ... );
    mdarray_float &vflipf( const char *exp_fmt, va_list ap );

    /* interchange rows and columns and copy */
    /* Flipping elements is supported        */
    /* (overridden)                          */
    ssize_t transposef_xy_copy( mdarray *dest, 
				const char *exp_fmt, ... ) const;
    ssize_t vtransposef_xy_copy( mdarray *dest, 
				 const char *exp_fmt, va_list ap ) const;

    /* interchange xyz to zxy and copy */
    /* Flipping elements is supported  */
    /* (overridden)                    */
    ssize_t transposef_xyz2zxy_copy( mdarray *dest, 
				     const char *exp_fmt, ... ) const;
    ssize_t vtransposef_xyz2zxy_copy( mdarray *dest, 
				      const char *exp_fmt, va_list ap ) const;

    /* padding existing values in an array */
    /* (overridden)                        */
    mdarray_float &cleanf( const char *exp_fmt, ... );
    mdarray_float &vcleanf( const char *exp_fmt, va_list ap );

    /* rewrite element values with a value in a section */
    /* (overridden)                                     */
    mdarray_float &fillf( double value, const char *exp_fmt, ... );
    mdarray_float &vfillf( double value, const char *exp_fmt, va_list ap );

    /* <overloaded>                                     */
    virtual mdarray_float &fillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_float *,void *),
        void *user_ptr, const char *exp_fmt, ... );
    virtual mdarray_float &vfillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_float *,void *),
        void *user_ptr, const char *exp_fmt, va_list ap );

    /* add a scalar value to element values in a section */
    /* (overridden)                                      */
    mdarray_float &addf( double value, 
			  const char *exp_fmt, ... );
    mdarray_float &vaddf( double value, 
			   const char *exp_fmt, va_list ap );

    /* subtract a scalar value from element values in a section */
    /* (overridden)                                             */
    mdarray_float &subtractf( double value, 
			       const char *exp_fmt, ... );
    mdarray_float &vsubtractf( double value, 
				const char *exp_fmt, va_list ap );

    /* multiply element values in a section by a scalar value */
    /* (overridden)                                           */
    mdarray_float &multiplyf( double value, 
			       const char *exp_fmt, ... );
    mdarray_float &vmultiplyf( double value, 
				const char *exp_fmt, va_list ap );

    /* divide element values in a section by a scalar value */
    /* (overridden)                                         */
    mdarray_float &dividef( double value, 
			     const char *exp_fmt, ... );
    mdarray_float &vdividef( double value, 
			      const char *exp_fmt, va_list ap );

    /* paste up an array object                                              */

    /* paste without operation using fast method of that of .convert().      */
    /* (overridden)                                                          */
    mdarray_float &pastef( const mdarray &src,
			    const char *exp_fmt, ... );
    mdarray_float &vpastef( const mdarray &src,
			     const char *exp_fmt, va_list ap );

    /* paste with operation: all elements are converted into double type, so */
    /* the performance is inferior to above paste().                         */
    /*                                                                       */
    /* arguments of func are                                                 */
    /*   1. prepared temporary buffer that has converted elements from self; */
    /*      this array is the destination for programmer's calculation.      */
    /*   2. prepared temporary buffer that have converted elements from src  */
    /*   3. length of buffer of 1. or 2. (always column length of region)    */
    /*   4. position of x (always first position)                            */
    /*   5. position of y                                                    */
    /*   6. position of z                                                    */
    /*   7. address of self                                                  */
    /*   8. user pointer                                                     */
    /* <overloaded>                                                          */
    virtual mdarray_float &pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_float *,void *),
	void *user_ptr,
        const char *exp_fmt, ... );
    virtual mdarray_float &vpastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_float *,void *),
	void *user_ptr,
        const char *exp_fmt, va_list ap );

    /* add an array object */
    /* (overridden)        */
    mdarray_float &addf( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_float &vaddf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* subtract an array object */
    /* (overridden)             */
    mdarray_float &subtractf( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_float &vsubtractf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* multiply an array object */
    /* (overridden)             */
    mdarray_float &multiplyf( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_float &vmultiplyf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* divide an array object */
    /* (overridden)           */
    mdarray_float &dividef( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_float &vdividef( const mdarray &src_img, const char *exp_fmt, va_list ap );


    /* --------------------------------------------------------------------- */

    /* 
     * member functions for image processing
     */

    /* taken over from mdarray class

    // returns trimmed array //
    virtual mdarray section( ssize_t col_idx, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;
    */

    /* copy all or a section to another mdarray object */
    /* (overridden)                                    */
    ssize_t copy( mdarray *dest,
		  ssize_t col_idx, size_t col_len=MDARRAY_ALL,
		  ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		  ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* convert and copy the value of selected array element. */
    /* (overridden)                                          */
    /* [not implemented] */
    ssize_t convert_copy( mdarray *dest,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* set a section to be copied by move_to() */
    /* (overridden)                            */
    mdarray_float &move_from( 
			   ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* copy a section specified by move_from() */
    /* (overridden)                            */
    mdarray_float &move_to( 
	      ssize_t dest_col, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* trim a section */
    /* (overridden)   */
    mdarray_float &trim( 
			   ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* flip horizontal within a rectangular section */
    /* (overridden)                                 */
    mdarray_float &flip_cols( 
			   ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* flip vertical within a rectangular section */
    /* (overridden)                               */
    mdarray_float &flip_rows( 
			   ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* interchange rows and columns and copy */
    /* (overridden)                          */
    ssize_t transpose_xy_copy( mdarray *dest,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* interchange xyz to zxy and copy */
    /* (overridden)                    */
    ssize_t transpose_xyz2zxy_copy( mdarray *dest,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* rotate and copy a section to another mdarray object       */
    /*   angle: 90,-90, or 180                                   */
    /*          (anticlockwise when image is bottom-left origin) */
    /* (overridden)                                              */
    ssize_t rotate_xy_copy( mdarray *dest, int angle,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* padding existing values in an array */
    /* (overridden)                        */
    mdarray_float &clean( 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* rewrite element values with a value in a section */
    /* (overridden)                                     */
    mdarray_float &fill( double value,
			 ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
			 ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
			 ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* <overloaded>                                     */
    virtual mdarray_float &fill_via_udf( double value,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_float *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );


    /* add a scalar value to element values in a section */
    /* (overridden)                                      */
    mdarray_float &add( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* subtract a scalar value from element values in a section */
    /* (overridden)                                             */
    mdarray_float &subtract( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* multiply element values in a section by a scalar value */
    /* (overridden)                                           */
    mdarray_float &multiply( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* divide element values in a section by a scalar value */
    /* (overridden)                                         */
    mdarray_float &divide( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* paste up an array object */
    /* (overridden)             */
    mdarray_float &paste( const mdarray &src,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* <overloaded>             */
    virtual mdarray_float &paste_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray_float *,void *),
	void *user_ptr,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );


    /* add an array object */
    /* (overridden)        */
    mdarray_float &add( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* subtract an array object */
    /* (overridden)             */
    mdarray_float &subtract( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* multiply an array object */
    /* (overridden)             */
    mdarray_float &multiply( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* divide an array object */
    /* (overridden)           */
    mdarray_float &divide( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );


    /* --------------------------------------------------------------------- */

    /*
     * member functions to scan pixels (for image statistics, etc.)
     */

    /* taken over from mdarray class

    // test argument that expresses a rectangular section, and fix them //
    // if positions are out of range or sizes are too large.            //
    virtual int fix_section_args( ssize_t *r_col_index, size_t *r_col_size,
				  ssize_t *r_row_index, size_t *r_row_size,
				  ssize_t *r_layer_index, size_t *r_layer_size
				) const;

    // horizontally scans the specified section.  A temporary buffer of 1-D //
    // array is prepared and scan_along_x() returns it.                     //
    // A scan order is displayed in pseudocode:                             //
    //  for(...) {      <- layer                                            //
    //    for(...) {    <- row                                              //
    //      for(...) {  <- column                                           //
    virtual ssize_t beginf_scan_along_x( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_along_x( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_along_x( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_along_x_via_udf(
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_x_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_x_f_via_udf(
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_along_x() const;

    // vertically scans the specified section.  A temporary buffer of 1-D //
    // array is prepared and scan_along_y() returns it.                   //
    // scan order is displayed by pseudocode:                             //
    //  for(...) {      <- layer                                          //
    //    for(...) {    <- column                                         //
    //      for(...) {  <- row                                            //
    virtual ssize_t beginf_scan_along_y( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_along_y( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_along_y( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_along_y_via_udf( 
			 void (*func)(const void *, void *, size_t, void *),
			 void *user_ptr,
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_y_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_y_f_via_udf(
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_along_y() const;

    // scan the specified section along z-axis.  A temporary buffer of 1-D //
    // array is prepared and scan_along_z() returns it.                    //
    // scan order is displayed by pseudocode:                              //
    //  for(...) {      <- row                                             //
    //    for(...) {    <- column                                          //
    //      for(...) {  <- layer                                           //
    virtual ssize_t beginf_scan_along_z( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_along_z( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_along_z( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_along_z_via_udf( 
			 void (*func)(const void *, void *, size_t, void *),
			 void *user_ptr,
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_z_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_z_f_via_udf(
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_along_z() const;

    // scan the specified 3-D section with plane by plane (zx plane). //
    // A temporary buffer of 2-D array is prepared.                   //
    // scan order is displayed by pseudocode:                         //
    //  for(...) {      <- row                                        //
    //    for(...) {    <- column                                     //
    //      for(...) {  <- layer                                      //
    virtual ssize_t beginf_scan_zx_planes( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_zx_planes( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_zx_planes( 
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_zx_planes_via_udf( 
	  void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	  void *user_ptr,
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_zx_planes_f( 
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_zx_planes_f_via_udf(
	  void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	  void *user_ptr,
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_zx_planes() const;

    // scan the specified 3-D section.  A temporary buffer of 1-D array is //
    // prepared and scan_a_cube() returns it.                              //
    //  for(...) {      <- layer                                           //
    //    for(...) {    <- row                                             //
    //      for(...) {  <- column                                          //
    virtual ssize_t beginf_scan_a_cube( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_a_cube( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    // double version //
    virtual double *scan_a_cube( size_t *n_x, size_t *n_y, size_t *n_z, 
				 ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_a_cube_via_udf( 
			 void (*func)(const void *, void *, size_t, void *),
			 void *user_ptr,
			 size_t *n_x, size_t *n_y, size_t *n_z, 
			 ssize_t *x, ssize_t *y, ssize_t *z ) const;
    // float version //
    virtual float *scan_a_cube_f( size_t *n_x, size_t *n_y, size_t *n_z, 
				  ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_a_cube_f_via_udf( 
			 void (*func)(const void *, void *, size_t, void *),
			 void *user_ptr,
			 size_t *n_x, size_t *n_y, size_t *n_z, 
			 ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_a_cube() const;

    */


    /* --------------------------------------------------------------------- */

    /*
     * APIs for direct pointer access
     */

    /* register the address of an external pointer variable that is    */
    /* automatically updated when changed address of buffer in object. */
    /* <overloaded>                                                    */
    virtual mdarray_float &register_extptr( float **extptr_address );
    virtual mdarray_float &register_extptr_2d( float *const **extptr2d_address );
    virtual mdarray_float &register_extptr_3d( float *const *const **extptr3d_address );

    /* Acquire the specified element's address */
    virtual const float *carray() const;
    virtual const float *carray( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual float *array_ptr();
    virtual float *array_ptr( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const float *array_ptr() const;
    virtual const float *array_ptr( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;
#endif
    virtual const float *array_ptr_cs() const;
    virtual const float *array_ptr_cs( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;

    /* acquisition of address table for 2d array                            */
    /*   use: set true to enable automatic update of internal address table */
    /*        for 2d array and to obtain the address table.                 */
    /*        set false to free the memory for address table and to disable */
    /*        automatic update for it.                                      */
    /* *NOTE* To minimize internal processings for array resizing,          */
    /*        programmers have to call this member function with            */
    /*        "use=false" when there is no requirement of referring         */
    /*        obtained address table.                                       */
    virtual float *const *array_ptr_2d( bool use );
    /* followings have no overhead. Execute .array_ptr_2d(true) before use. */
    virtual float *const *array_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const float *const *array_ptr_2d() const;
#endif
    virtual const float *const *array_ptr_2d_cs() const;

    /* acquisition of address table for 3d array                            */
    /*   use: set true to enable automatic update of internal address table */
    /*        for 3d array and to obtain the address table.                 */
    /*        set false to free the memory for address table and to disable */
    /*        automatic update for it.                                      */
    /* *NOTE* To minimize internal processings for array resizing,          */
    /*        programmers have to call this member function with            */
    /*        "use=false" when there is no requirement of referring         */
    /*        obtained address table.                                       */
    virtual float *const *const *array_ptr_3d( bool use );
    /* followings have no overhead. Execute .array_ptr_3d(true) before use. */
    virtual float *const *const *array_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const float *const *const *array_ptr_3d() const;
#endif
    virtual const float *const *const *array_ptr_3d_cs() const;


    /* --------------------------------------------------------------------- */

    /*
     * etc. 
     */

    /* taken over from mdarray class

    // compare array objects //
    virtual bool compare(const mdarray &obj) const;

    */

    /* reverse the endian if necessary */
    /* (overridden)                    */
    mdarray_float &reverse_endian( bool is_little_endian );
    /* (overridden)                    */
    mdarray_float &reverse_endian( bool is_little_endian, 
				    size_t begin, size_t length );

    /* acquisition of the initial value */
    virtual float default_value() const;

    /* taken over from mdarray class

    // returns type of result of calculations between self and type of szt //
    virtual ssize_t ope_size_type( ssize_t szt ) const;

    // 一時オブジェクトの return の直前に使い，shallow copy を許可する場合に //
    // 使う．                                                                //
    virtual void set_scopy_flag();

    // output of the object information to the stderr output //
    // (for programmer's debug)                              //
    // オブジェクトの状態を表示する (プログラマの debug 用)  //
    virtual void dprint( const char *msg = NULL ) const;

    */

  protected:
    /* オーバーライド(動作設定) */
    ssize_t default_size_type();
    bool is_acceptable_size_type( ssize_t sz_type );

  private:
    size_t parse_dim_info( const char *dim_info );
    /* dim_info 用 */
    size_t dinfo_dlen[3];

  };


/*
 * inline member functions
 */

/**  
 * @brief  最初の次元についての指定された区間をパディング
 *
 *  自身が持つ配列の要素番号idxに，値valueをlen個書き込みます．
 *
 * @param      value セットする値
 * @param      idx 要素番号
 * @param      len 要素の個数
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 *
 */
inline mdarray_float &mdarray_float::put( float value, ssize_t idx, size_t len )
{
    this->mdarray::put((const void *)&value, idx, len);
    return *this;
}

/**  
 * @brief  任意の1つの次元についての指定された区間をパディング
 *
 *  自身が持つ配列の要素番号idxに，値valueをlen個書き込みます．
 *
 * @param      value セットする値
 * @param      idx 要素番号
 * @param      len 要素の個数
 * @param      dim_index 次元番号
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 *
 */
inline mdarray_float &mdarray_float::put( float value,
				    size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::put((const void *)&value, dim_index, idx, len);
    return *this;
}

/**
 * @brief  指定された1要素へ値を代入 (dcomplex型で指定)
 */
inline mdarray_float &mdarray_float::assign( dcomplex value,
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    this->mdarray::assign(value, idx0, idx1, idx2);
    return *this;
}

/**  
 * @brief  指定された1要素へ値を代入 (double型で指定)
 *
 *  自身の配列の，idxn で指定された1 要素に値を設定します．
 *
 * @param      value double 型の値
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     自身の参照
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 *
 */
inline mdarray_float &mdarray_float::assign( double value, 
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    this->mdarray::assign(value, idx0, idx1, idx2);
    return *this;
}


/**  
 * @brief  [] で指定された要素値の参照を返す (1次元)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @return     要素の値の参照
 * @throw      自動リサイズモードで内部バッファの確保に失敗した場合
 *
 */
inline float &mdarray_float::operator[]( ssize_t idx0 )
{
    return ((0 <= idx0 && idx0 < (ssize_t)this->length()) ? 
	    ((float *)(this->data_ptr()))[idx0] :
	    this->at(idx0));
}

/**  
 * @brief  [] で指定された要素値の参照を返す (1次元・読取専用)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @return     要素の値の参照
 *
 */
inline const float &mdarray_float::operator[]( ssize_t idx0 ) const
{
    return ((0 <= idx0 && idx0 < (ssize_t)this->length()) ? 
	    ((const float *)(this->data_ptr_cs()))[idx0] :
	    this->at_cs(idx0));
}

/**  
 * @brief  () で指定された要素値の参照を返す (1〜3次元)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     要素の値の参照
 * @throw      自動リサイズモードで内部バッファの確保に失敗した場合
 *
 */
inline float &mdarray_float::operator()( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    return this->at(idx0, idx1, idx2);
}

/**  
 * @brief  () で指定された要素値の参照を返す (1〜3次元・読取専用)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     要素の値の参照
 *
 */
inline const float &mdarray_float::operator()( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->at_cs(idx0, idx1, idx2);
}

/**  
 * @brief  idx0，idx1，idx2 で指定された配列要素の値を設定・取得
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     要素の値の参照
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 * @throw      オブジェクト内の要素の型が，メンバ関数の戻り値の型より小さい場合
 *
 */
inline float &mdarray_float::at( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	return ((float *)(this->data_ptr()))[idx];
    }
    else {
	float *p = (float *)(this->junk_rec);
	*p = NAN;
	return *p;
    }
}

/**  
 * @brief  idx0，idx1，idx2 で指定された配列要素の値を返す
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     要素の値の参照
 * @throw      オブジェクト内の要素の型が，メンバ関数の戻り値の型より小さい場合
 *
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const float &mdarray_float::at( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->at_cs(idx0, idx1, idx2);
}
#endif

/**  
 * @brief  idx0，idx1，idx2 で指定された配列要素の値を返す
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     要素の値の参照
 * @throw      オブジェクト内の要素の型が，メンバ関数の戻り値の型より小さい場合
 *
 */
inline const float &mdarray_float::at_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	return ((const float *)(this->data_ptr()))[idx];
    }
    else {
	float *p = (float *)(this->junk_rec);
	*p = NAN;
	return *p;
    }
}

/**  
 * @brief  全要素の小数部の値を切り上げ
 *
 *  自身の配列(浮動小数点型) の全要素の小数部の値を切り上げます．
 *
 * @return     自身の参照
 *
 */
inline mdarray_float &mdarray_float::ceil()
{
    this->mdarray::ceil();
    return *this;
}

/**  
 * @brief  全要素の小数部の値を切り下げ
 * 
 *  自身の配列(浮動小数点型) の全要素の小数部を，それぞれの要素の値を
 *  越えない最大の整数値に切り下げます．
 *
 * @return     自身の参照
 *
 */
inline mdarray_float &mdarray_float::floor()
{
    this->mdarray::floor();
    return *this;
}

/**  
 * @brief  全要素の小数部の値を四捨五入し整数値に変換
 *
 *  自身の配列(浮動小数点型) の全要素値の小数部を四捨五入し，整数値にします．
 *
 * @return     自身の参照
 *
 */
inline mdarray_float &mdarray_float::round()
{
    this->mdarray::round();
    return *this;
}

/**
 * @brief  全要素の値の小数部を切り捨て，0 に近い方の整数値に変換
 *
 *  自身の配列(浮動小数点型) の全要素の値の小数部を切り捨て，
 *  0 に近い方の整数値にします．
 *
 * @return     自身の参照
 *
 */
inline mdarray_float &mdarray_float::trunc()
{
    this->mdarray::trunc();
    return *this;
}

/**  
 * @brief  自身の配列の全要素について絶対値をとる
 *
 *  自身の配列の全要素について絶対値をとります．
 *
 * @return     自身の参照
 *
 */
inline mdarray_float &mdarray_float::abs()
{
    this->mdarray::abs();
    return *this;
}

/**  
 * @brief  配列の先頭要素のアドレスを取得 (読取専用)
 *
 * @return     自身が持つ配列の先頭要素のアドレス
 *
 */
inline const float *mdarray_float::carray() const
{
    return (const float *)(this->data_ptr_cs());
}

/**  
 * @brief  配列の指定要素のアドレスを取得 (読取専用)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     自身が持つ配列の指定要素のアドレス
 *
 */
inline const float *mdarray_float::carray( ssize_t idx0, 
				     ssize_t idx1, ssize_t idx2 ) const
{
    return (const float *)(this->data_ptr_cs(idx0,idx1,idx2));
}

/**  
 * @brief  配列の先頭要素のアドレスを取得
 *
 * @return     自身が持つ配列の先頭要素のアドレス
 *
 */
inline float *mdarray_float::array_ptr()
{
    return (float *)(this->data_ptr());
}

/**  
 * @brief  配列の指定要素のアドレスを取得
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     自身が持つ配列の指定要素のアドレス
 *
 */
inline float *mdarray_float::array_ptr( ssize_t idx0, 
				  ssize_t idx1, ssize_t idx2 )
{
    return (float *)(this->data_ptr(idx0,idx1,idx2));
}

/**  
 * @brief  配列の先頭要素のアドレスを取得 (読取専用)
 *
 * @return     自身が持つ配列の先頭要素のアドレス
 *
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const float *mdarray_float::array_ptr() const
{
    return (const float *)(this->data_ptr_cs());
}

/**  
 * @brief  配列の指定要素のアドレスを取得 (読取専用)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     自身が持つ配列の指定要素のアドレス
 *
 */
inline const float *mdarray_float::array_ptr( ssize_t idx0, 
					   ssize_t idx1, ssize_t idx2 ) const
{
    return (const float *)(this->data_ptr_cs(idx0,idx1,idx2));
}
#endif

/**  
 * @brief  配列の先頭要素のアドレスを取得 (読取専用)
 *
 * @return     自身が持つ配列の先頭要素のアドレス
 *
 */
inline const float *mdarray_float::array_ptr_cs() const
{
    return (const float *)(this->data_ptr_cs());
}

/**  
 * @brief  配列の指定要素のアドレスを取得 (読取専用)
 *
 * @param      idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param      idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param      idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return     自身が持つ配列の指定要素のアドレス
 *
 */
inline const float *mdarray_float::array_ptr_cs( ssize_t idx0, 
					   ssize_t idx1, ssize_t idx2 ) const
{
    return (const float *)(this->data_ptr_cs(idx0,idx1,idx2));
}

/* acquisition of address table for 2d array */
/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 */
inline float *const *mdarray_float::array_ptr_2d( bool use )
{
    return (float *const *)(this->mdarray::data_ptr_2d(use));
}

/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得
 */
inline float *const *mdarray_float::array_ptr_2d()
{
    return (float *const *)(this->mdarray::data_ptr_2d());
}

/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const float *const *mdarray_float::array_ptr_2d() const
{
    return (const float *const *)(this->mdarray::data_ptr_2d_cs());
}
#endif

/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const float *const *mdarray_float::array_ptr_2d_cs() const
{
    return (const float *const *)(this->mdarray::data_ptr_2d_cs());
}

/* acquisition of address table for 3d array */
/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 */
inline float *const *const *mdarray_float::array_ptr_3d( bool use )
{
    return (float *const *const *)(this->mdarray::data_ptr_3d(use));
}

/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得
 */
inline float *const *const *mdarray_float::array_ptr_3d()
{
    return (float *const *const *)(this->mdarray::data_ptr_3d());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const float *const *const *mdarray_float::array_ptr_3d() const
{
    return (const float *const *const *)(this->mdarray::data_ptr_3d_cs());
}
#endif

/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const float *const *const *mdarray_float::array_ptr_3d_cs() const
{
    return (const float *const *const *)(this->mdarray::data_ptr_3d_cs());
}

/**  
 * @brief  サイズ拡張時の初期値を取得
 *
 * @return     自身が持つサイズ拡張時の初期値
 *
 */
inline float mdarray_float::default_value() const
{
    const float *p = (const float *)(this->default_value_ptr());
    if ( p == NULL ) return 0;
    else return *p;
}


}

#endif	/* _SLI__MDARRAY_FLOAT_H */
