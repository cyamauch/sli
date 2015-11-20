/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-20 13:21:44 cyamauch> */

#ifndef _SLI__MDARRAY_H
#define _SLI__MDARRAY_H 1

/**
 * @file   mdarray.h
 * @brief  多次元配列を扱うための基底クラス mdarray の定義とinlineメンバ関数
 */

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#ifdef SLI__USE_CMATH
#include <cmath>
#else
#include <math.h>
#endif
#include <sys/types.h>

#include "heap_mem.h"
#include "sli_config.h"
#include "complex_defs.h"
#include "size_types.h"
#include "numeric_indefs.h"
#include "numeric_minmax.h"
#include "slierr.h"

namespace sli
{

/*
 * sli::mdarry is a low-level class of SLLIB.
 * Please use sli::mdarray_int, sli::mdarray_double, etc. for typical purposes.
 * 
 * This class can handle multidimensional arrays of primitive types in C or
 * structures.  This class provide two type of access methods:
 *  - safe (never causes buffer overrun) and automatic buffer allocation, 
 *    but not very good for performance.
 *  - fast, but not safe.
 * In this class, multidimensional arrays are always managed as single memory
 * buffer, so programmers can access internal buffer as normal heap buffer in
 * C language.
 *
 * NOTE: do not use `long double' type.  Implementation for it is not complete.
 */

/**
 * @class  sli::mdarray
 * @brief  多次元配列を扱うための低レベルなクラス(基底クラス)
 *
 *   mdarray クラスは，SLLIB の低レベルなクラスで，プリミティブ型や構造体の
 *   多次元配列を IDL や Python のように手軽に扱えるようにします．下記のような
 *   機能を持ちます．<br>
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
 *   基底クラスのmdarrayでは，配列の型種別は動的に変更する事ができますが，
 *   []演算子などいくつか使えないAPIがあります．一般的な用途には，型種別が
 *   定まっていて安全な mdarray_float 等の継承クラスをお使いください．
 * 
 * @note  SIMD命令(SSE2)により，基本的な部分が高速化されています．
 * @attention  C++ 標準ライブラリの <cmath> を使う場合，マクロ SLI__USE_CMATH
 *             を定義してから mdarray.h を include してください．
 * @attention  long double型に関しては実装が完全ではありません．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */
 
/*
 * 整数，浮動小数点，構造体 (プリミティブ型) の multi-dimensional array を
 * 扱うためのクラス(クラスをつっこむ事はできない)
 *
 * 【注意】long double については，実装が中途半端なので使わない事．
 *         (Cygwin がちゃんと long double 対応したらサポートする予定)
 */

#ifdef SLI__USE_CMATH
  using std::isfinite;
#endif

  const size_t MDARRAY_ALL = ~((size_t)0);
  const ssize_t MDARRAY_INDEF = (ssize_t)1 << (8*sizeof(ssize_t)-1);
  /* */
  class mdarray
  {

  public:
    /* constructor                                                           */
    /* The mdarray class has two kinds of operating modes:                   */
    /*  1. automatic resize mode                                             */
    /*  2. manual resize mode                                                */
    /* The operating mode is decided when the object is initialized by the   */
    /* constructors or the init() member function.  It works in the          */
    /* automatic resize mode when the number of the arrays is not given and  */
    /* in the manual resize mode when the number of the arrays is given upon */
    /* initialization.  The mode can be changed by set_auto_resize() after   */
    /* initialization.                                                       */
    /* Basically automatic resize mode has larger overhead when calling      */
    /* member functions that can change the size of internal buffer.         */
    mdarray();
    /* see size_type.h for sz_type arg                                       */
    mdarray( ssize_t sz_type );
    mdarray( ssize_t sz_type, bool auto_resize );
    mdarray( ssize_t sz_type, bool auto_resize, void *extptr_address );
    mdarray( ssize_t sz_type, bool auto_resize, 
	     const size_t naxisx[], size_t ndim, bool init_buf );

    /* copy constructor                                                      */
    mdarray( const mdarray &obj );

    /* destructor */
    virtual ~mdarray();


    /* --------------------------------------------------------------------- */

    /*
     * operator
     */

    /* same as .init(obj)                            */
    /* 注意: この = は init() を呼ぶ                 */
    virtual mdarray &operator=(const mdarray &obj);
    /* 以下の演算子と同じ振舞いで = を使いたい場合， */
    /* obj = 0.0; obj += src; とする                 */

    /* add an array to self */
    virtual mdarray &operator+=(const mdarray &obj);

    /* subtract an array from self */
    virtual mdarray &operator-=(const mdarray &obj);

    /* multiply self by an array */
    virtual mdarray &operator*=(const mdarray &obj);

    /* divide self by an array */
    virtual mdarray &operator/=(const mdarray &obj);

    /* substitute a scalar value */
    virtual mdarray &operator=(dcomplex v);
    virtual mdarray &operator=(double v);
    virtual mdarray &operator=(long long v);
    virtual mdarray &operator=(long v);
    virtual mdarray &operator=(int v);

    /* add a scalar value to self */
    virtual mdarray &operator+=(dcomplex v);
    virtual mdarray &operator+=(double v);
    virtual mdarray &operator+=(long long v);
    virtual mdarray &operator+=(long v);
    virtual mdarray &operator+=(int v);

    /* subtract a scalar value from self */
    virtual mdarray &operator-=(dcomplex v);
    virtual mdarray &operator-=(double v);
    virtual mdarray &operator-=(long long v);
    virtual mdarray &operator-=(long v);
    virtual mdarray &operator-=(int v);

    /* multiply self by a scalar value */
    virtual mdarray &operator*=(dcomplex v);
    virtual mdarray &operator*=(double v);
    virtual mdarray &operator*=(long long v);
    virtual mdarray &operator*=(long v);
    virtual mdarray &operator*=(int v);

    /* divide self by a scalar value */
    virtual mdarray &operator/=(dcomplex v);
    virtual mdarray &operator/=(double v);
    virtual mdarray &operator/=(long long v);
    virtual mdarray &operator/=(long v);
    virtual mdarray &operator/=(int v);

    /* compare */
    virtual bool operator==(const mdarray &obj) const;
    virtual bool operator!=(const mdarray &obj) const;

    /* return the object which stores the result of calculation */
    /* 数学的オペレータとその仲間たち                           */
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


    /* --------------------------------------------------------------------- */

    /*
     * member functions to handle the whole object and properties
     */

    /* initialization of the array */
    virtual mdarray &init();
    virtual mdarray &init( ssize_t sz_type );
    virtual mdarray &init( ssize_t sz_type, bool auto_resize );
    virtual mdarray &init( ssize_t sz_type, bool auto_resize, 
			  const size_t naxisx[],  size_t ndim, bool init_buf );
    virtual mdarray &init( const mdarray &obj );

    /* copy properties (auto_resize, auto_init, and rounding) */
    virtual mdarray &init_properties( const mdarray &src_obj );

    /* setting of the resize mode */
    virtual mdarray &set_auto_resize( bool tf );

    /* setting of the initialize mode */
    virtual mdarray &set_auto_init( bool tf );

    /* setting of the rounding off possibility */
    virtual mdarray &set_rounding( bool tf );

    /* setting of strategy of memory allocation */
    /* "auto", "min" and "pow" can be set.      */
    virtual mdarray &set_alloc_strategy( const char *strategy );

    /* convert the value of the full array element.                     */
    /* All elements are converted, therefore, a byte length of internal */
    /* buffer is changed if necessary.                                  */
    virtual mdarray &convert( ssize_t sz_type );

    /* for SFITSIO */
    virtual mdarray &convert_via_udf( ssize_t sz_type,
			  void (*func)(const void *,void *,size_t,int,void *),
			  void *user_ptr );

    /* swap contents between self and another objects */
    virtual mdarray &swap( mdarray &sobj );

    /* copy an array and object's attributes into another object */
    virtual ssize_t copy( mdarray *dest ) const;
    /* not recommended */
    virtual ssize_t copy( mdarray &dest ) const;

    /* move all contents of an array into another object. */
    /* an array of this object becomes empty.             */
    /* object's attributes are also copied.               */
    virtual mdarray &cut( mdarray *dest );

    /* interchange rows and columns */
    virtual mdarray &transpose_xy();

    /* interchange xyz to zxy */
    virtual mdarray &transpose_xyz2zxy();

    /* rotate image                                              */
    /*   angle: 90,-90, or 180                                   */
    /*          (anticlockwise when image is bottom-left origin) */
    virtual mdarray &rotate_xy( int angle );


    /* --------------------------------------------------------------------- */

    /*
     * APIs to obtain basic information of object
     */

    /* returns integer representing a data type. */
    /* see also size_types.h.                    */
    virtual ssize_t size_type() const;		       /* sz_type_rec を返す */

    /* number of dimensions of an array */
    virtual size_t dim_length() const;		       /* 次元の数 */

    /* number of all elements */
    virtual size_t length() const;		       /* 全要素の個数 */

    /* number of elements in a dimension */
    virtual size_t length( size_t dim_index ) const;   /* 各軸における要素数 */

    /* the length of the array's column */
    virtual size_t col_length() const;		     /* axis0 の長さ       */
    virtual size_t x_length() const;		     /* same as col_length */

    /* the length of the array's row  */
    virtual size_t row_length() const;		     /* axis1 の長さ       */
    virtual size_t y_length() const;		     /* same as row_length */

    /* the layer number of the array */
    virtual size_t layer_length() const;	     /* axis2(以降) の長さ   */
    virtual size_t z_length() const;		     /* same as layer_length */

    /* the length of x * y */
    virtual size_t col_row_length() const;

    /* the length of y * z (degenerated z) */
    virtual size_t row_layer_length() const;

    /* number of bytes of an element */
    virtual size_t bytes() const;

    /* total byte size of all elements */
    virtual size_t byte_length() const;

    /* total byte size of elements in a dimension */
    virtual size_t byte_length( size_t dim_index ) const;

    /* acquisition of the resize mode */
    virtual bool auto_resize() const;

    /* acquisition of the initialize mode */
    virtual bool auto_init() const;

    /* acquisition of the rounding off possibility */
    virtual bool rounding() const;

    /* acquisition of strategy of memory allocation */
    virtual const char *alloc_strategy() const;


    /* --------------------------------------------------------------------- */

    /*
     * member functions for basic data I/O
     */

    /* high-level access */
    virtual dcomplex dxvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
	      ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual double dvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual long lvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			 ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual long long llvalue( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			       ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual mdarray &assign( dcomplex value,
			     ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			     ssize_t idx2 = MDARRAY_INDEF );
    virtual mdarray &assign( double value, 
			     ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			     ssize_t idx2 = MDARRAY_INDEF );

    /* low-level access */
    /* r&w version */
    virtual fcomplex &fx( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			  ssize_t idx2 = MDARRAY_INDEF );
    virtual dcomplex &dx( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			  ssize_t idx2 = MDARRAY_INDEF );
    virtual ldcomplex &ldx( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF );
    virtual float &f( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		      ssize_t idx2 = MDARRAY_INDEF );
    virtual double &d( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		       ssize_t idx2 = MDARRAY_INDEF );
    virtual long double &ld(ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			    ssize_t idx2 = MDARRAY_INDEF);
    virtual unsigned char &c(ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			     ssize_t idx2 = MDARRAY_INDEF);
    virtual short &s( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		      ssize_t idx2 = MDARRAY_INDEF );
    virtual int &i( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		    ssize_t idx2 = MDARRAY_INDEF );
    virtual long &l( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		     ssize_t idx2 = MDARRAY_INDEF );
    virtual long long &ll( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			   ssize_t idx2 = MDARRAY_INDEF );
    virtual int16_t &i16( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			  ssize_t idx2 = MDARRAY_INDEF );
    virtual int32_t &i32( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			  ssize_t idx2 = MDARRAY_INDEF );
    virtual int64_t &i64( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			  ssize_t idx2 = MDARRAY_INDEF );
    virtual size_t &z( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		       ssize_t idx2 = MDARRAY_INDEF );
    virtual ssize_t &sz( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			 ssize_t idx2 = MDARRAY_INDEF );
    virtual bool &b( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
		     ssize_t idx2 = MDARRAY_INDEF );
    virtual uintptr_t &p( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			  ssize_t idx2 = MDARRAY_INDEF );
    /* read-only version */
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fcomplex &fx( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
		  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const dcomplex &dx( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
		  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const ldcomplex &ldx( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
		  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const float &f( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const double &d( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			     ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const long double &ld( ssize_t idx0, 
	     ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF) const;
    virtual const unsigned char &c( ssize_t idx0,
	     ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF) const;
    virtual const short &s( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int &i( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const long &l( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const long long &ll( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				 ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int16_t &i16( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int32_t &i32( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int64_t &i64( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const size_t &z( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			     ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const ssize_t &sz( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			       ssize_t idx2 = MDARRAY_INDEF) const;
    virtual const bool &b( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const uintptr_t &p( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;
#endif
    /* read-only version */
    virtual const fcomplex &fx_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
		  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const dcomplex &dx_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
		  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const ldcomplex &ldx_cs(ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
		  ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const float &f_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			       ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const double &d_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const long double &ld_cs( ssize_t idx0, 
	     ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF) const;
    virtual const unsigned char &c_cs( ssize_t idx0,
	     ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF) const;
    virtual const short &s_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			       ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int &i_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			     ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const long &l_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			      ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const long long &ll_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				    ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int16_t &i16_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int32_t &i32_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const int64_t &i64_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				   ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const size_t &z_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const ssize_t &sz_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				  ssize_t idx2 = MDARRAY_INDEF) const;
    virtual const bool &b_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			      ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual const uintptr_t &p_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
				   ssize_t idx2 = MDARRAY_INDEF ) const;


    /* --------------------------------------------------------------------- */

    /*
     * C-like APIs to input and output data
     */

    /* set a value to an arbitrary element's point */
    virtual mdarray &put( const void *value_ptr, ssize_t idx, size_t len );
    virtual mdarray &put( const void *value_ptr,
			  size_t dim_index, ssize_t idx, size_t len );

    /* copy the array stored in object into the programmer's buffer. */
    /* buffer length is set in count of element.                     */
    virtual ssize_t get_elements( void *dest_buf, size_t elem_size, 
				ssize_t idx0 = 0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;

    /* copy the array in the programmer's buffer into object's buffer. */
    /* buffer length is set in count of element.                       */
    virtual ssize_t put_elements( const void *src_buf, size_t elem_size,
				ssize_t idx0 = 0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF );

    /* copy the array stored in object into the programmer's buffer */
    /* buffer length is set in bytes.                               */
    virtual ssize_t getdata( void *dest_buf, size_t buf_size, ssize_t idx0 = 0,
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;

    /* copy the array stored in object into the programmer's buffer */
    /* buffer length is set in bytes.                               */
    virtual ssize_t putdata( const void *src_buf, size_t buf_size,
		  ssize_t idx0 = 0,
		  ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF );

    /* copy the array stored in object into the programmer's buffer.    */
    /* SIMD stream instruction is used when total_copied_size is large. */
    /* buffer length is set in bytes.                                   */
    virtual ssize_t getdata_fast( void *dest_buf, size_t buf_size, 
	    size_t total_copied_size,  ssize_t idx0 = 0,
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;

    /* copy the array stored in object into the programmer's buffer.    */
    /* SIMD stream instruction is used when total_copied_size is large. */
    /* buffer length is set in bytes.                                   */
    virtual ssize_t putdata_fast( const void *src_buf, size_t buf_size,
		  size_t total_copied_size,  ssize_t idx0 = 0,
		  ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF );


    /* --------------------------------------------------------------------- */

    /*
     * member functions to update length, type, etc.
     */

    /* expansion of the dimension number */
    virtual mdarray &increase_dim();

    /* reduction of the dimension number */
    virtual mdarray &decrease_dim();

    /* change the length of the array                    */
    /*   dim_index: an axis for modification (0-indexed) */
    /*   len:       number of pixels to be set           */
    virtual mdarray &resize( size_t len );			/* for 1-d */
    virtual mdarray &resize( size_t dim_index, size_t len );
    virtual mdarray &resize( const mdarray &src );

    /* change the length of the 1-d array */
    virtual mdarray &resize_1d( size_t x_len );
    /* change the length of the 2-d array */
    virtual mdarray &resize_2d( size_t x_len, size_t y_len );
    /* change the length of the 3-d array */
    virtual mdarray &resize_3d( size_t x_len, size_t y_len, size_t z_len );

    /* change the length of the array (supports n-dim)   */
    /*   naxisx:   number of elements for each dimension */
    /*   ndim:     number of dimension                   */
    /*   init_buf: set true to initialize new elements   */
    virtual mdarray &resize( const size_t naxisx[], size_t ndim, 
			     bool init_buf );

    /* set a string like "3,2" using printf() style args.             */
    /* Omitting values is allowed.  For example, ",2" does not change */
    /* length of first dimension.                                     */
    virtual mdarray &resizef( const char *exp_fmt, ... );
    virtual mdarray &vresizef( const char *exp_fmt, va_list ap );

    /* change the length of the array relatively         */
    /*   dim_index: an axis for modification (0-indexed) */
    /*   len:       number of pixels to be increased     */
    virtual mdarray &resizeby( ssize_t len );			/* for 1-d */
    virtual mdarray &resizeby( size_t dim_index, ssize_t len );

    /* change the length of the 1-d array relatively */
    virtual mdarray &resizeby_1d( ssize_t x_len );
    /* change the length of the 2-d array relatively */
    virtual mdarray &resizeby_2d( ssize_t x_len, ssize_t y_len );
    /* change the length of the 3-d array relatively */
    virtual mdarray &resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len);

    /* change the length of the array relatively                            */
    /*   naxisx_rel:  number of elements to be increased for each dimension */
    /*   ndim:        number of dimension                                   */
    /*   init_buf:    set true to initialize new elements                   */
    virtual mdarray &resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			       bool init_buf );

    /* set a string like "3,-2" using printf() style args.             */
    /* Omitting values is allowed.  For example, ",-2" does not change */
    /* length of first dimension.                                      */
    virtual mdarray &resizebyf( const char *exp_fmt, ... );
    virtual mdarray &vresizebyf( const char *exp_fmt, va_list ap );

    /* change length of array without adjusting buffer contents */
    /*   naxisx:   number of elements for each dimension        */
    /*   ndim:     number of dimension                          */
    /*   init_buf: set true to initialize new elements          */
    virtual mdarray &reallocate( const size_t naxisx[], size_t ndim, 
				 bool init_buf );

    /* free current buffer and alloc new memory          */
    /*   naxisx:   number of elements for each dimension */
    /*   ndim:     number of dimension                   */
    /*   init_buf: set true to initialize new elements   */
    virtual mdarray &allocate( const size_t naxisx[], size_t ndim, 
			       bool init_buf );

    /* setting of the initial value for .resize(), etc. */
    /* high-level */
    virtual mdarray &assign_default( double value );
    /* low-level */
    virtual mdarray &assign_default( const void *value_ptr );

    /* insert a blank section */
    virtual mdarray &insert( ssize_t idx, size_t len );		/* for 1-d */
    virtual mdarray &insert( size_t dim_index, ssize_t idx, size_t len );

    /* erase a section */
    virtual mdarray &erase( ssize_t idx, size_t len );		/* for 1-d */
    virtual mdarray &erase( size_t dim_index, ssize_t idx, size_t len );

    /* copy values between elements (without automatic resizing) */
    /* 移動(バッファのサイズは変更しない)                        */
    virtual mdarray &move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );				/* for 1-d */
    virtual mdarray &move( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );

    /* copy values between elements (with automatic resizing) */
    /* 移動(バッファのサイズは必要に応じて変更する)           */
    virtual mdarray &cpy( ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );				/* for 1-d */
    virtual mdarray &cpy( size_t dim_index, 
			  ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );

    /* replace values between elements */
    /* バッファ内での入れ換え */
    virtual mdarray &swap( ssize_t idx_src, size_t len, 
			   ssize_t idx_dst );			/* for 1-d */
    virtual mdarray &swap( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst );

    /* extract a section           */
    /* see also trimf() and trim() */
    virtual mdarray &crop( ssize_t idx, size_t len );		/* for 1-d */
    virtual mdarray &crop( size_t dim_index, ssize_t idx, size_t len );

    /* flip a section   */
    /* see also flipf() */
    virtual mdarray &flip( ssize_t idx, size_t len );		/* for 1-d */
    virtual mdarray &flip( size_t dim_index, ssize_t idx, size_t len );

    /* raise decimals to the next whole number in a float/double type value */
    virtual mdarray &ceil();

    /* devalue decimals in a float/double type value */
    virtual mdarray &floor();

    /* round off decimals in a float/double type value */
    virtual mdarray &round();

    /* omit decimals in a float/double type value */
    virtual mdarray &trunc();

    /* absolute value of all elements */
    virtual mdarray &abs();


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

    /* returns trimmed array          */
    /* Flipping elements is supported */
    virtual mdarray sectionf( const char *exp_fmt, ... ) const;
    virtual mdarray vsectionf( const char *exp_fmt, va_list ap ) const;

    /* copy all or a section to another mdarray object */
    /* Flipping elements is supported                  */
    virtual ssize_t copyf( mdarray *dest, const char *exp_fmt, ... ) const;
    virtual ssize_t vcopyf( mdarray *dest, const char *exp_fmt, va_list ap ) const;

    /* copy an convet all or a section to another mdarray object */
    /* Flipping elements is supported                            */
    /* [not implemented] */
    virtual ssize_t convertf_copy( mdarray *dest, const char *exp_fmt, ... ) const;
    virtual ssize_t vconvertf_copy( mdarray *dest, const char *exp_fmt, va_list ap ) const;

    /* for SFITSIO */
    /* [not implemented] */
    virtual ssize_t convertf_copy_via_udf( mdarray *dest, 
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, ... ) const;
    virtual ssize_t vconvertf_copy_via_udf( mdarray *dest, 
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap ) const;

    /* trim a section                 */
    /* Flipping elements is supported */
    virtual mdarray &trimf( const char *exp_fmt, ... );
    virtual mdarray &vtrimf( const char *exp_fmt, va_list ap );

    /* flip elements in a section */
    virtual mdarray &flipf( const char *exp_fmt, ... );
    virtual mdarray &vflipf( const char *exp_fmt, va_list ap );

    /* interchange rows and columns and copy */
    /* Flipping elements is supported        */
    virtual ssize_t transposef_xy_copy( mdarray *dest, 
					const char *exp_fmt, ... ) const;
    virtual ssize_t vtransposef_xy_copy( mdarray *dest, 
				       const char *exp_fmt, va_list ap ) const;

    /* interchange xyz to zxy and copy */
    /* Flipping elements is supported  */
    virtual ssize_t transposef_xyz2zxy_copy( mdarray *dest, 
					const char *exp_fmt, ... ) const;
    virtual ssize_t vtransposef_xyz2zxy_copy( mdarray *dest, 
				       const char *exp_fmt, va_list ap ) const;

    /* padding existing values in an array */
    virtual mdarray &cleanf( const char *exp_fmt, ... );
    virtual mdarray &vcleanf( const char *exp_fmt, va_list ap );

    /* rewrite element values with a value in a section */
    virtual mdarray &fillf( double value, const char *exp_fmt, ... );
    virtual mdarray &vfillf( double value, const char *exp_fmt, va_list ap );

    /* for SFITSIO */
    virtual mdarray &fillf_via_udf( double value,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
						    const char *exp_fmt, ... );
    virtual mdarray &vfillf_via_udf( double value,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
					     const char *exp_fmt, va_list ap );

    /* double type udf; for programmers */
    virtual mdarray &fillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
        void *user_ptr, const char *exp_fmt, ... );
    virtual mdarray &vfillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
        void *user_ptr, const char *exp_fmt, va_list ap );

    /* for SFITSIO */
    virtual mdarray &fillf_via_udf( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
        void *user_ptr_func,
        const char *exp_fmt, ... );
    virtual mdarray &vfillf_via_udf( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
        void *user_ptr_func,
	const char *exp_fmt, va_list ap );

    /* add a scalar value to element values in a section */
    virtual mdarray &addf( double value, 
			   const char *exp_fmt, ... );
    virtual mdarray &vaddf( double value, 
			    const char *exp_fmt, va_list ap );

    /* subtract a scalar value from element values in a section */
    virtual mdarray &subtractf( double value, 
				const char *exp_fmt, ... );
    virtual mdarray &vsubtractf( double value, 
				 const char *exp_fmt, va_list ap );

    /* multiply element values in a section by a scalar value */
    virtual mdarray &multiplyf( double value, 
				const char *exp_fmt, ... );
    virtual mdarray &vmultiplyf( double value, 
				 const char *exp_fmt, va_list ap );

    /* divide element values in a section by a scalar value */
    virtual mdarray &dividef( double value, 
			      const char *exp_fmt, ... );
    virtual mdarray &vdividef( double value, 
			       const char *exp_fmt, va_list ap );

    /* paste up an array object                                           */

    /* paste without operation using fast method of that of .convert().   */
    /* 演算なしペースト: convert() と同じ手法で変換した結果を高速に格納． */
    virtual mdarray &pastef( const mdarray &src,
			     const char *exp_fmt, ... );
    virtual mdarray &vpastef( const mdarray &src,
			      const char *exp_fmt, va_list ap );

    /* for SFITSIO */
    virtual mdarray &pastef_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, ... );
    virtual mdarray &vpastef_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap );

    /* paste with operation: all elements are converted into double type, so */
    /* the performance is inferior to above paste().                         */
    /* 演算ありペースト: 一旦 double 型に変換されてから演算した結果を格納．  */
    /* 当然，↑の演算なし版に比べて遅い．                                    */
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
    virtual mdarray &pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
        const char *exp_fmt, ... );
    virtual mdarray &vpastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
        const char *exp_fmt, va_list ap );

    /* for SFITSIO */
    virtual mdarray &pastef_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
        const char *exp_fmt, ... );
    virtual mdarray &vpastef_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
        const char *exp_fmt, va_list ap );

    /* add an array object */
    virtual mdarray &addf( const mdarray &src_img, const char *exp_fmt, ... );
    virtual mdarray &vaddf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* subtract an array object */
    virtual mdarray &subtractf( const mdarray &src_img, const char *exp_fmt, ... );
    virtual mdarray &vsubtractf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* multiply an array object */
    virtual mdarray &multiplyf( const mdarray &src_img, const char *exp_fmt, ... );
    virtual mdarray &vmultiplyf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* divide an array object */
    virtual mdarray &dividef( const mdarray &src_img, const char *exp_fmt, ... );
    virtual mdarray &vdividef( const mdarray &src_img, const char *exp_fmt, va_list ap );


    /* --------------------------------------------------------------------- */

    /* 
     * member functions for image processing
     */

    /* returns trimmed array */
    virtual mdarray section( ssize_t col_idx, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* copy all or a section to another mdarray object */
    virtual ssize_t copy( mdarray *dest,
		     ssize_t col_idx, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* convert and copy the value of selected array element. */
    /* [not implemented] */
    virtual ssize_t convert_copy( mdarray *dest,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* for SFITSIO */
    /* [not implemented] */
    virtual ssize_t convert_copy_via_udf( mdarray *dest,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* set a section to be copied by move_to() */
    virtual mdarray &move_from( 
			   ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* copy a section specified by move_from() */
    virtual mdarray &move_to( 
	      ssize_t dest_col, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* trim a section */
    virtual mdarray &trim( ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* flip horizontal within a rectangular section */
    virtual mdarray &flip_cols( 
			   ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* flip vertical within a rectangular section */
    virtual mdarray &flip_rows( 
			   ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* interchange rows and columns and copy */
    virtual ssize_t transpose_xy_copy( mdarray *dest,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* interchange xyz to zxy and copy */
    virtual ssize_t transpose_xyz2zxy_copy( mdarray *dest,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* rotate and copy a region to another mdarray object        */
    /*   angle: 90,-90, or 180                                   */
    /*          (anticlockwise when image is bottom-left origin) */
    virtual ssize_t rotate_xy_copy( mdarray *dest, int angle,
		     ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
		     ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
		     ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL ) const;

    /* padding existing values in an array */
    virtual mdarray &clean( 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* rewrite element values with a value in a section */
    virtual mdarray &fill( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* for SFITSIO */
    virtual mdarray &fill_via_udf( double value,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* double type udf; for programmers */
    virtual mdarray &fill_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* for SFITSIO */
    virtual mdarray &fill_via_udf( double value, 
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
        void *user_ptr_func,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* add a scalar value to element values in a section */
    virtual mdarray &add( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* subtract a scalar value from element values in a section */
    virtual mdarray &subtract( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* multiply element values in a section by a scalar value */
    virtual mdarray &multiply( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* divide element values in a section by a scalar value */
    virtual mdarray &divide( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* paste up an array object                                           */

    /* paste without operation using fast method of that of .convert().   */
    /* 演算なしペースト: convert() と同じ手法で変換した結果を高速に格納． */
    virtual mdarray &paste( const mdarray &src,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* for SFITSIO */
    virtual mdarray &paste_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* paste with operation: all elements are converted into double type, so */
    /* the performance is inferior to above paste().                         */
    /* 演算ありペースト: 一旦 double 型に変換されてから演算した結果を格納．  */
    /* 当然，↑の演算なし版に比べて遅い．                                    */
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
    virtual mdarray &paste_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* for SFITSIO */
    virtual mdarray &paste_via_udf( const mdarray &src,
	void (*func_src2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), 
	void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), 
	void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* add an array object */
    virtual mdarray &add( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* subtract an array object */
    virtual mdarray &subtract( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* multiply an array object */
    virtual mdarray &multiply( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* divide an array object */
    virtual mdarray &divide( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );


    /* --------------------------------------------------------------------- */

    /*
     * member functions to scan pixels (for image statistics, etc.)
     */

    /* test argument that expresses a rectangular section, and fix them */
    /* if positions are out of range or sizes are too large.            */
    virtual int fix_section_args( ssize_t *r_col_index, size_t *r_col_size,
				  ssize_t *r_row_index, size_t *r_row_size,
				  ssize_t *r_layer_index, size_t *r_layer_size
				) const;

    /* horizontally scans the specified section.  A temporary buffer of 1-D */
    /* array is prepared and scan_along_x() returns it.                     */
    /* A scan order is displayed in pseudocode:                             */
    /*  for(...) {      <- layer                                            */
    /*    for(...) {    <- row                                              */
    /*      for(...) {  <- column                                           */
    virtual ssize_t beginf_scan_along_x( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_along_x( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_along_x( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_along_x_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_x_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_x_f_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_along_x() const;

    /* vertically scans the specified section.  A temporary buffer of 1-D */
    /* array is prepared and scan_along_y() returns it.                   */
    /* scan order is displayed by pseudocode:                             */
    /*  for(...) {      <- layer                                          */
    /*    for(...) {    <- column                                         */
    /*      for(...) {  <- row                                            */
    virtual ssize_t beginf_scan_along_y( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_along_y( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_along_y( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_along_y_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_y_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_y_f_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_along_y() const;

    /* scan the specified section along z-axis.  A temporary buffer of 1-D */
    /* array is prepared and scan_along_z() returns it.                    */
    /* scan order is displayed by pseudocode:                              */
    /*  for(...) {      <- row                                             */
    /*    for(...) {    <- column                                          */
    /*      for(...) {  <- layer                                           */
    virtual ssize_t beginf_scan_along_z( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_along_z( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_along_z( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_along_z_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_z_f( 
			 size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_along_z_f_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_along_z() const;

    /* scan the specified 3-D section with plane by plane (zx plane). */
    /* A temporary buffer of 2-D array is prepared.                   */
    /* scan order is displayed by pseudocode:                         */
    /*  for(...) {      <- row                                        */
    /*    for(...) {    <- column                                     */
    /*      for(...) {  <- layer                                      */
    virtual ssize_t beginf_scan_zx_planes( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_zx_planes( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    virtual double *scan_zx_planes( 
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_zx_planes_via_udf(		/* for SFITSIO */
	  void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	  void *user_ptr,
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_zx_planes_f( 
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_zx_planes_f_via_udf(		/* for SFITSIO */
	  void (*func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *),
	  void *user_ptr,
	  size_t *n_z, size_t *n_x, ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_zx_planes() const;

    /* scan the specified 3-D section.  A temporary buffer of 1-D array is */
    /* prepared and scan_a_cube() returns it.                              */
    /*  for(...) {      <- layer                                           */
    /*    for(...) {    <- row                                             */
    /*      for(...) {  <- column                                          */
    virtual ssize_t beginf_scan_a_cube( const char *exp_fmt, ... ) const;
    virtual ssize_t vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const;
    virtual ssize_t begin_scan_a_cube( 
              ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	      ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	      ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL ) const;
    /* double version */
    virtual double *scan_a_cube( size_t *n_x, size_t *n_y, size_t *n_z, 
				 ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual double *scan_a_cube_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n_x, size_t *n_y, size_t *n_z, 
		       ssize_t *x, ssize_t *y, ssize_t *z ) const;
    /* float version */
    virtual float *scan_a_cube_f( size_t *n_x, size_t *n_y, size_t *n_z, 
				  ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual float *scan_a_cube_f_via_udf(		/* for SFITSIO */
		       void (*func)(const void *, void *, size_t, int, void *),
		       void *user_ptr,
		       size_t *n_x, size_t *n_y, size_t *n_z, 
		       ssize_t *x, ssize_t *y, ssize_t *z ) const;
    virtual void end_scan_a_cube() const;


    /* --------------------------------------------------------------------- */

    //virtual mdarray &register_length_change_notify_callback( 
    //					 void (*func)(const mdarray *, void *),
    //					 void *user_ptr );

    //virtual const void *carray( ssize_t idx0 = 0, 
    //	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;


    /*
     * APIs for direct pointer access
     */

    /* register the address of an external pointer variable that is    */
    /* automatically updated when changed address of buffer in object. */
    virtual mdarray &register_extptr(void *extptr_address);
    virtual mdarray &register_extptr_2d(void *extptr2d_address);
    virtual mdarray &register_extptr_3d(void *extptr3d_address);

    /* acquisition of the specified element's address */
    /* data_ptr() without arguments have no overhead. */
    virtual void *data_ptr();
    virtual void *data_ptr( ssize_t idx0,
		  ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *data_ptr() const;
    virtual const void *data_ptr( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;
#endif
    virtual const void *data_ptr_cs() const;
    virtual const void *data_ptr_cs( ssize_t idx0, 
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
    virtual void *const *data_ptr_2d( bool use );
    /* followings have no overhead. Execute .data_ptr_2d(true) before use.  */
    virtual void *const *data_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *const *data_ptr_2d() const;
#endif
    virtual const void *const *data_ptr_2d_cs() const;

    /* acquisition of address table for 3d array                            */
    /*   use: set true to enable automatic update of internal address table */
    /*        for 3d array and to obtain the address table.                 */
    /*        set false to free the memory for address table and to disable */
    /*        automatic update for it.                                      */
    /* *NOTE* To minimize internal processings for array resizing,          */
    /*        programmers have to call this member function with            */
    /*        "use=false" when there is no requirement of referring         */
    /*        obtained address table.                                       */
    virtual void *const *const *data_ptr_3d( bool use );
    /* followings have no overhead. Execute .data_ptr_3d(true) before use.  */
    virtual void *const *const *data_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *const *const *data_ptr_3d() const;
#endif
    virtual const void *const *const *data_ptr_3d_cs() const;

    /* acquisition of the pointer of the array that have element length of */
    /* each dimension.                                                     */
    virtual const size_t *cdimarray() const;

    /* acquisition of the initial value */
    virtual const void *default_value_ptr() const;


    /* --------------------------------------------------------------------- */

    /*
     * etc. 
     */

    /* compare array objects */
    virtual bool compare(const mdarray &obj) const;

    /* reverse the endian if necessary */
    virtual mdarray &reverse_endian( bool is_little_endian );
    virtual mdarray &reverse_endian( bool is_little_endian,
				     size_t begin, size_t length );

    /* reverse the endian if necessary (low-level) */
    virtual mdarray &reverse_byte_order( bool is_little_endian, 
					 ssize_t sz_type );
    virtual mdarray &reverse_byte_order( bool is_little_endian, 
				ssize_t sz_type, size_t begin, size_t length );


    /* returns type of result of calculations between self and type of szt */
    /* 自身の型と型 szt の演算が行なわれた時の型を返す                     */
    virtual ssize_t ope_size_type( ssize_t szt ) const;

    /* 一時オブジェクトの return の直前に使い，shallow copy を許可する場合に */
    /* 使う．                                                                */
    virtual void set_scopy_flag();

    /* output of the object information to the stderr output */
    /* (for programmer's debug)                              */
    /* オブジェクトの状態を表示する (プログラマの debug 用)  */
    virtual void dprint( const char *msg = NULL ) const;

    /* --------------------------------------------------------------------- */

    /* member functions for mdarray_math.cc */
    virtual mdarray &calc1( double (*func)(double), float (*funcf)(float),
			    long double (*funcl)(long double) );
    virtual mdarray &calc2(const mdarray &obj, bool obj_1st,
			   double (*func)(double,double),
			   float (*funcf)(float,float),
			   long double (*funcl)(long double,long double));
    virtual mdarray &calcx1( dcomplex (*func)(dcomplex),
			     fcomplex (*funcf)(fcomplex),
			     ldcomplex (*funcl)(ldcomplex) );
    virtual mdarray &calcx2(const mdarray &obj, bool obj_1st,
			    dcomplex (*func)(dcomplex,dcomplex),
			    fcomplex (*funcf)(fcomplex,fcomplex),
			    ldcomplex (*funcl)(ldcomplex,ldcomplex));

    /* --------------------------------------------------------------------- */

  protected:
    virtual mdarray &ope_plus_equal(const mdarray &obj);
    virtual mdarray &ope_minus_equal(const mdarray &obj);
    virtual mdarray &ope_star_equal(const mdarray &obj);
    virtual mdarray &ope_slash_equal(const mdarray &obj);
    virtual mdarray &ope_plus_equal(const mdarray &obj, bool do_resize);
    virtual mdarray &ope_minus_equal(const mdarray &obj, bool do_resize);
    virtual mdarray &ope_star_equal(const mdarray &obj, bool do_resize);
    virtual mdarray &ope_slash_equal(const mdarray &obj, bool do_resize);
    virtual mdarray &ope_equal(ssize_t szt, const void *v_ptr);
    virtual mdarray &ope_plus_equal(ssize_t szt, const void *v_ptr);
    virtual mdarray &ope_minus_equal(ssize_t szt, const void *v_ptr);
    virtual mdarray &ope_star_equal(ssize_t szt, const void *v_ptr);
    virtual mdarray &ope_slash_equal(ssize_t szt, const void *v_ptr);
    virtual bool ope_equal_equal(const mdarray &obj) const;
    virtual mdarray ope_plus(ssize_t szt, const void *v_ptr) const;
    virtual mdarray ope_plus(const mdarray &obj) const;
    virtual mdarray ope_minus(ssize_t szt, const void *v_ptr) const;
    virtual mdarray ope_minus(const mdarray &obj) const;
    virtual mdarray ope_star(ssize_t szt, const void *v_ptr) const;
    virtual mdarray ope_star(const mdarray &obj) const;
    virtual mdarray ope_slash(ssize_t szt, const void *v_ptr) const;
    virtual mdarray ope_slash(const mdarray &obj) const;
    /* */
    virtual ssize_t get_idx_3d( ssize_t ix0, ssize_t ix1, ssize_t ix2 );
    virtual ssize_t get_idx_3d_cs( ssize_t ix0, ssize_t ix1, ssize_t ix2 ) const;
    /* */
    virtual int class_level() const;
    /* */
    virtual ssize_t default_size_type();
    virtual bool is_acceptable_size_type( ssize_t sz_type );
    /* 各種長さが更新されると必ず呼びだされる．継承したクラスでオーバライド */
    /* すると，各種長さ情報のミラーが可能                                   */
    virtual void update_length_info();

  private:
    /* constructor 用の特別なイニシャライザ */
    void __force_init( ssize_t sz_type, bool is_constructor );
    /* destructor 用の特別なメモリ開放用メンバ関数 */
    void __force_free();
    /* shallow copy 関係 */
    bool request_shallow_copy( mdarray *from_obj ) const;
    void cancel_shallow_copy( mdarray *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;
    /* 特別なイニシャライザ */
    mdarray &__shallow_init( const mdarray &obj, bool is_move );
    mdarray &__deep_init( const mdarray &obj );
    //mdarray &_init();
    /* */
    mdarray &do_resize( size_t dim_index, size_t len, bool clr );
    mdarray &do_insert( size_t dim_index, ssize_t idx, size_t len, bool clr );
    /* */
    mdarray &r_calc2(const mdarray &obj, bool obj_1st, size_t dim_idx,
		     size_t len_src, size_t len_dest,
		     size_t pos_src, size_t pos_dest,
		     double (*func)(double,double),
		     float (*funcf)(float,float),
		     long double (*funcl)(long double,long double),
		     void (*func_calc)(const void *, void *, bool, size_t,
				   const void *, double (*)(double,double),
				   float (*)(float,float),
				   long double (*)(long double,long double)) );
    mdarray &r_calcx2(const mdarray &obj, bool obj_1st, size_t dim_idx,
		      size_t len_src, size_t len_dest,
		      size_t pos_src, size_t pos_dest,
		      dcomplex (*func)(dcomplex,dcomplex),
		      fcomplex (*funcf)(fcomplex,fcomplex),
		      ldcomplex (*funcl)(ldcomplex,ldcomplex),
		      void (*func_calc)(const void *, void *, bool, size_t,
				   const void *, dcomplex (*)(dcomplex,dcomplex),
				   fcomplex (*)(fcomplex,fcomplex),
				   ldcomplex (*)(ldcomplex,ldcomplex)) );
    /* */
    mdarray &image_vflipf( const char *exp_fmt, va_list ap );
    ssize_t image_vtransposef_xy_copy( mdarray *dest_obj,
				       const char *exp_fmt, va_list ap ) const;
    ssize_t image_vtransposef_xyz2zxy_copy( mdarray *dest_ret,
				       const char *exp_fmt, va_list ap ) const;
    mdarray &image_vcleanf( const void *value_ptr,
			   const char *exp_fmt, va_list ap );
    mdarray &image_clean( const void *value_ptr,
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );
    ssize_t image_vcopyf( mdarray *dest_obj, 
			  const char *exp_fmt, va_list ap ) const;
    ssize_t image_copy( mdarray *dest_obj,
			ssize_t col_index, size_t col_size, 
			ssize_t row_index, size_t row_size,
			ssize_t layer_index, size_t layer_size ) const;
    mdarray &image_vfillf( double value,
	   void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	   const char *exp_fmt, va_list ap );
    mdarray &image_vfillf( double value,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, va_list ap );
    mdarray &image_fill( double value,
	   void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
			 ssize_t col_index, size_t col_size,
			 ssize_t row_index, size_t row_size,
			 ssize_t layer_index, size_t layer_size );
    mdarray &image_fill( double value,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size );
    int test_section_args( ssize_t *r_col_index, size_t *r_col_size,
			   ssize_t *r_row_index, size_t *r_row_size,
			   ssize_t *r_layer_index, size_t *r_layer_size,
			   bool *r_col_ok, bool *r_row_ok, 
			   bool *r_layer_ok ) const;
    mdarray &image_vpastef( const mdarray &src_img,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  const char *exp_fmt, va_list ap );
    mdarray &image_vpastef( const mdarray &src_img,
        void (*func_src2d)(const void *,void *,size_t,int,void *), void *user_ptr_src2d,
        void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
        void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	const char *exp_fmt, va_list ap );
    mdarray &image_paste( const mdarray &src_img,
		      void (*func)(const void *,void *,size_t,int,void *),
		      void *user_ptr,
		      ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer );
    mdarray &image_paste( const mdarray &src_img,
	void (*func_src2d)(const void *,void *,size_t,int,void *), void *user_ptr_src2d,
	void (*func_dest2d)(const void *,void *,size_t,int,void *), void *user_ptr_dest2d,
	void (*func_d2dest)(const void *,void *,size_t,int,void *), void *user_ptr_d2dest,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *),
	void *user_ptr_func,
	ssize_t dst_col, ssize_t dst_row, ssize_t dst_layer );
    int test_paste_arg( const mdarray &src_img,
     ssize_t *r_col_idx, ssize_t *r_row_idx, ssize_t *r_layer_idx,
     bool *r_col_ok, bool *r_row_ok, bool *r_layer_ok,
     size_t *r_col_size, size_t *r_row_size, size_t *r_layer_size,
     ssize_t *r_col_begin, ssize_t *r_row_begin, ssize_t *r_layer_begin) const;
    /* */
    int scan_along_x_goto_next() const;
    ssize_t begin_scan_along_y_common() const;
    int scan_along_y_goto_next() const;
    void scan_along_y_do_partial_transpose() const;
    ssize_t begin_scan_along_z_common() const;
    int scan_along_z_goto_next() const;
    void scan_along_z_do_partial_transpose() const;
    ssize_t begin_scan_zx_planes_common() const;
    int scan_zx_goto_next() const;
    int scan_a_cube_goto_next() const;
    /* */
    bool is_my_buffer( const void *ptr ) const;
    /* private なアクセッサー */
    void *default_rec_ptr();
    size_t *size_rec_ptr();

  protected:
    int realloc_arr_rec( size_t len_bytes );
    void free_arr_rec();

  private:
    int realloc_default_rec( size_t len_bytes );
    int realloc_size_rec( size_t len_elements );
    /* */
    void free_default_rec();
    void free_size_rec();
    /* _arr_ptr_2d_rec, _arr_ptr_3d_rec を更新する */
    /* update_length_info() の直後に呼ぶ事       */
    void update_arr_ptr_2d( bool on_2d = false );
    void update_arr_ptr_3d( bool on_3d = false );
    /* _init(..) 用低レベルメンバ関数 */
    void _update_arr_ptr_2d( bool on_2d = false );
    void _update_arr_ptr_3d( bool on_3d = false );
    /* extptr_2d_rec, extptr_3d_rec が non-NULL の時は使ってはいけない */
    void free_arr_ptr_2d();
    void free_arr_ptr_3d();
    /* inlineメンバ関数で throw するの専用 */
    void err_throw_int_arg( const char *fncname, const char *lev, 
			    const char *mes, int v ) const;

  private:
    /* dummy member functions to output error */
    mdarray( ssize_t sz_type, int naxis0 );
    mdarray( ssize_t sz_type, size_t naxis0 );
    mdarray &init( ssize_t sz_type, int naxis0 );
    mdarray &init( ssize_t sz_type, size_t naxis0 );

  private:
    /* 【注意】shallow copy の対象になるものは _ が先頭につく               */
    /*  _default_rec, _size_rec, _arr_rec, _arr_ptr_2d_rec, _arr_ptr_3d_rec */
    /* これらについては，this->_size_rec[i] のような直接読みのみ，どこで    */
    /* 使っても良い．それ以外の場合は，アクセッサー経由で使うこと．         */
    /* コードをチェックするには                                             */
    /*    [^a-z_]_size_rec[^a-z_]                                           */
    /* のように探し，cleanup_shallow_copy(true); を使っているかをチェック． */
    bool auto_resize_rec;	/* 自動リサイズをするか */
    bool auto_init_rec;		/* リサイズ時にバッファを初期化するか */
    bool rounding_rec;		/* 浮動小数点数を整数に変換する時に四捨五入 */
    void *_default_rec;		/* 1要素の初期値を保持するバッファのアドレス */
    /* 注意!! ↓これは負の値を持つ事があるので，labs() をとる必要がある． */
    ssize_t sz_type_rec;	/* 1要素のバイト数(負の場合は浮動小数点数) */
    size_t dim_size_rec;	/* Number of dimension */
    size_t *_size_rec;		/* length of dim0,dim1,dim2,... */
    void *_arr_rec;		/* データ領域のアドレス */
    size_t arr_alloc_blen_rec;	/* _arr_rec 用に確保しているバイト長 */
    int alloc_strategy_rec;	/* realloc() をどのように呼び出すかの戦略 */
    /* _arr_rec へのアドレステーブル (2d or 3dデータ参照用)                  */
    /* NULL終端のアドレステーブルであり，アドレステーブルが有効かどうかは    */
    /* _arr_ptr_2d_rec，_arr_ptr_3d_rec それぞれの値がNULLで無い事で判定する */
    /* (アドレステーブルが有効な場合，mdarrayの配列長が0なら長さ1のテーブル  */
    /* となる) */
    void **_arr_ptr_2d_rec;
    void ***_arr_ptr_3d_rec;
    /* これ以下は update_length_info() で自動更新される */
    size_t cached_length_rec;		/* 全要素数 */
    size_t cached_col_length_rec;	/* 列の長さ */
    size_t cached_row_length_rec;	/* 行の長さ */
    size_t cached_layer_length_rec;	/* レイヤの数 */
    size_t cached_bytes_rec;		/* 1要素のバイト長 */
    size_t cached_col_row_length_rec;	/* 列×行の長さ */
    size_t cached_row_layer_length_rec;	/* 行×レイヤの数 */
    /* 型変換用関数(自身の型→double)．update_length_info() で自動更新 */
    void (*func_cnv_nd_x2d)(const void *, void *, size_t, int, void *);
    /* 型変換用関数(double→自身の型)．update_length_info() で自動更新 */
    void (*func_cnv_nd_d2x)(const void *, void *, size_t, int, void *);
    /* 型変換用関数(自身の型→float)．update_length_info() で自動更新 */
    void (*func_cnv_nd_x2f)(const void *, void *, size_t, int, void *);
    /* 転置行列を求める関数．update_length_info() で自動更新 */
    void (*func_transp_mtx)(void *, size_t);
    /* move_from(), move_to() 専用 */
    ssize_t move_from_idx_rec[3];
    size_t move_from_len_rec[3];
    /* これ以下は init() や swap() で変更しない */
    void **extptr_rec;		/* 外部ポインタ変数のアドレス */
    void ***extptr_2d_rec;	/* 2d用外部ポインタ変数のアドレス */
    void ****extptr_3d_rec;	/* 3d用外部ポインタ変数のアドレス */
    /* void (*length_change_notify_func)(const mdarray *, void *); */
    /* void *length_change_notify_user_ptr; */

    /* scan_along_x() 専用 (= 等ではコピーされない) */
    heap_mem<char> scanx_ret_buf;	/* scan_along_x() の返り値用バッファ */
    size_t scanx_ret_buf_blen;		/* alignedで確保されているbyteサイズ */
    char *scanx_aligned_ret_ptr;	/* 一時buf↑のalignedな先頭アドレス */
    heap_mem<size_t> scanx_begin;	/* スキャンするバッファ領域の位置   */
    heap_mem<size_t> scanx_len;		/* スキャンするバッファ領域の大きさ */
    heap_mem<size_t> scanx_cntr;	/* カウンタ */
    size_t scanx_ndim;
    size_t scanx_ret_z;
    const char *scanx_cptr;		/* 現在のバッファ位置 */

    /* scan_along_y() 専用 (= 等ではコピーされない) */
    heap_mem<char> scany_trans_buf;	/* 部分xy-transpose用一時バッファ */
    size_t scany_trans_buf_blen;
    size_t scany_height_trans;		/* scany_trans_buf の縦幅 */
    heap_mem<char> scany_mtx_buf;	/* transpose用正方行列バッファ */
    size_t scany_mtx_buf_blen;
    size_t scany_len_block;
    heap_mem<char> scany_ret_buf;	/* scan_along_y() の返り値用バッファ */
    size_t scany_ret_buf_blen;		/* alignedで確保されているbyteサイズ */
    char *scany_aligned_ret_ptr;	/* 一時buf↑のalignedな先頭アドレス */
    void (*scany_func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    heap_mem<size_t> scany_begin;	/* スキャンするバッファ領域の位置   */
    heap_mem<size_t> scany_len;		/* スキャンするバッファ領域の大きさ */
    heap_mem<size_t> scany_cntr;	/* カウンタ */
    size_t scany_ndim;
    size_t scany_ret_z;
    const char *scany_cptr;		/* 現在のバッファ位置 */

    /* scan_along_z() 専用 (= 等ではコピーされない) */
    heap_mem<char> scanz_trans_buf;	/* 部分xz-transpose用一時バッファ */
    size_t scanz_trans_buf_blen;
    size_t scanz_height_trans;		/* scanz_trans_buf の縦幅 */
    heap_mem<char> scanz_mtx_buf;	/* transpose用正方行列バッファ */
    size_t scanz_mtx_buf_blen;
    size_t scanz_len_block;
    heap_mem<char> scanz_ret_buf;	/* scan_along_z() の返り値用バッファ */
    size_t scanz_ret_buf_blen;		/* alignedで確保されているbyteサイズ */
    char *scanz_aligned_ret_ptr;	/* 一時buf↑のalignedな先頭アドレス */
    void (*scanz_func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    heap_mem<size_t> scanz_begin;	/* スキャンするバッファ領域の位置   */
    heap_mem<size_t> scanz_len;		/* スキャンするバッファ領域の大きさ */
    heap_mem<size_t> scanz_cntr;	/* カウンタ */
    size_t scanz_ndim;
    size_t scanz_ret_z;
    const char *scanz_cptr;		/* 現在のバッファ位置 */

    /* scan_zx_planes() 専用 (= 等ではコピーされない) */
    heap_mem<char> scanzx_mtx_buf;	/* transpose用正方行列バッファ */
    size_t scanzx_mtx_buf_blen;
    size_t scanzx_len_block;
    heap_mem<char> scanzx_ret_buf;	/* scan_zx_planes() 返り値用バッファ */
    size_t scanzx_ret_buf_blen;		/* alignedで確保されているbyteサイズ */
    char *scanzx_aligned_ret_ptr;	/* 一時buf↑のalignedな先頭アドレス */
    heap_mem<size_t> scanzx_begin;	/* スキャンするバッファ領域の位置   */
    heap_mem<size_t> scanzx_len;	/* スキャンするバッファ領域の大きさ */
    heap_mem<size_t> scanzx_cntr;	/* カウンタ */
    size_t scanzx_ndim;
    size_t scanzx_ret_z;
    const char *scanzx_cptr;		/* 現在のバッファ位置 */

    /* scan_a_cube() 専用 (= 等ではコピーされない) */
    heap_mem<char> scancube_ret_buf;	/* scan_a_cube() の返り値用バッファ */
    size_t scancube_ret_buf_blen;	/* alignedで確保されているbyteサイズ */
    char *scancube_aligned_ret_ptr;	/* 一時buf↑のalignedな先頭アドレス */
    heap_mem<size_t> scancube_begin;	/* スキャンするバッファ領域の位置   */
    heap_mem<size_t> scancube_len;	/* スキャンするバッファ領域の大きさ */
    heap_mem<size_t> scancube_cntr;	/* カウンタ */
    size_t scancube_ndim;
    size_t scancube_ret_z;

    /*
     * バッファの継承(shallow copy)のためのフラグ．= 等ではコピーされない
     */
    /* コピーの src 側のフラグ */
    bool shallow_copy_ok;		/* set_scopy_flag() でセットされる */
    mdarray *shallow_copy_dest_obj;	/* コピー先: 有効なら non-NULL */

    /* コピーの dest 側のフラグ(コピー元から書き込まれる) */
    mdarray *shallow_copy_src_obj;	/* コピー元: 生きていれば non-NULL */

    /* __shallow_init(), __deep_init() を安全に行なうためのフラグ */
    bool __copying;

  protected:
    /* class level */
    int class_level_rec;				/* 継承すると +1 */
    /* ダミーバッファ: 初期化不要 */
    char junk_rec[64];		/* f() とか d() で範囲外の場合に使う */

  };


/*
 * inline member functions
 */

/**
 * @brief  型を表す整数(型種別)を取得
 *
 *  これにより，自身に設定された配列の型を調べる事ができます．
 *
 * @return  型を表す整数(型種別)
 *            
 */
inline ssize_t mdarray::size_type() const
{
    return this->sz_type_rec;
}

/**
 * @brief  配列の次元数を取得
 *
 * @return  自身が持つ配列の次元数
 *            
 */
inline size_t mdarray::dim_length() const
{
    return this->dim_size_rec;
}

/**
 * @brief  配列の全ての要素数を取得
 *
 *  全要素数(次元1 の個数×次元2 の個数× 次元3 の個数×...) を返します．
 *
 * @return    自身が持つ配列の全ての要素数
 *            
 */
inline size_t mdarray::length() const
{
    return this->cached_length_rec;
}

/**
 * @brief  配列の指定された次元の要素数を返す
 *
 *  次元番号が dim_index の次元の要素の個数を返します．<br>
 *  dim_index の開始番号は 0 です．
 *
 * @param     dim_index 次元番号(開始の値は0)
 * @return    自身が持つ配列の指定された次元の要素数
 *            
 */
inline size_t mdarray::length( size_t dim_index ) const
{
    //if ( this->dim_size_rec <= 0 ) return 0;
    //if ( dim_index < this->dim_size_rec ) return this->_size_rec[dim_index];
    //else return 1;
    return
	( (this->dim_size_rec <= 0) ?
	  0
	  :
	  ( (dim_index < this->dim_size_rec) ? this->_size_rec[dim_index] : 1)
	);
}

/**
 * @brief  配列の列(x軸)の長さを取得
 *
 * @return  自身が持つ配列の列の長さ
 *            
 */
inline size_t mdarray::col_length() const
{
    return this->cached_col_length_rec;
}

/**
 * @brief  配列の列(x軸)の長さを取得
 *
 * @return    自身が持つ配列の列の長さ
 *            
 */
inline size_t mdarray::x_length() const
{
    return this->cached_col_length_rec;
}

/**
 * @brief  配列の行(y軸)の長さを取得
 *
 * @return    自身が持つ配列の行の長さ
 *            
 */
inline size_t mdarray::row_length() const
{
    return this->cached_row_length_rec;
}

/**
 * @brief  配列の行(y軸)の長さを取得
 *
 * @return    自身が持つ配列の行の長さ
 *            
 */
inline size_t mdarray::y_length() const
{
    return this->cached_row_length_rec;
}

/**
 * @brief  配列のレイヤ数を取得
 *
 *  1次元または2次元配列の場合は，1 を返します．<br>
 *  3次元以上の場合は，配列の次元を 3次元に縮退させた場合の 3次元目(次元番号2)
 *  の長さを返します．
 *
 * @return    自身が持つ配列のレイヤ数
 *            
 */
inline size_t mdarray::layer_length() const
{
    return this->cached_layer_length_rec;
}

/**
 * @brief  配列のレイヤ数を取得
 *
 *  1次元または2次元配列の場合は，1 を返します．<br>
 *  3次元以上の場合は，配列の次元を 3次元に縮退させた場合の 3次元目(次元番号2)
 *  の長さを返します．
 *
 * @return    自身が持つ配列のレイヤ数
 *            
 */
inline size_t mdarray::z_length() const
{
    return this->cached_layer_length_rec;
}

/**
 * @brief  配列の x × y の長さを取得
 *
 * @return    自身が持つ配列の x × y の長さ
 *            
 */
inline size_t mdarray::col_row_length() const
{
    return this->cached_col_row_length_rec;
}

/**
 * @brief  配列の y × z (zは縮退)の長さを取得
 *
 * @return    自身が持つ配列の y × z (zは縮退) の長さ
 *            
 */
inline size_t mdarray::row_layer_length() const
{
    return this->cached_row_layer_length_rec;
}


/**
 * @brief  配列の1要素のバイト長を取得
 *
 * @return    自身が持つ配列の1要素のバイト長
 *            
 */
inline size_t mdarray::bytes() const
{
    return this->cached_bytes_rec;
}

/**
 * @brief  配列の総バイト長を取得
 *
 * @return    自身が持つ配列全要素のバイト長
 *            
 */
inline size_t mdarray::byte_length() const
{
    return this->cached_length_rec * this->cached_bytes_rec;
}

/**
 * @brief  配列の指定された次元の総バイト長を返す
 *
 *  次元番号が dim_index の次元の総バイト長を返します．
 *
 * @param     dim_index 次元番号(開始の値は0)
 * @return    指定次元の総バイト長
 *            
 */
inline size_t mdarray::byte_length( size_t dim_index ) const
{
    return this->length( dim_index ) * this->cached_bytes_rec;
}

/*
 * member functions for basic data I/O
 */

/**
 * @brief  dcomplex 型に変換した１要素の値を取得
 *
 *  自身が持つ配列の要素値を dcomplex 型に変換して返します．
 *  配列サイズ超えた要素を指定した場合，NAN+NAN*I を返します．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @return     変換結果
 * @retval     dcomplex型に変換した要素の値 正常終了
 * @retval     NAN+NAN*I 要素の型がサポートされない型の場合／配列サイズを
 *             超えた要素を指定した場合
 *
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline dcomplex mdarray::dxvalue( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return NAN + NAN*(__extension__ 1.0iF);
    if ( this__sz_type_rec == DCOMPLEX_ZT ) {
        return ((const dcomplex *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == FCOMPLEX_ZT ) {
        return ((const fcomplex *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == LDCOMPLEX_ZT ) {
        return ((const ldcomplex *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == DOUBLE_ZT ) {
	const double v = ((const double *)(this->_arr_rec))[idx];
	return (v + 0.0*(__extension__ 1.0iF));
    }
    else if ( this__sz_type_rec == FLOAT_ZT ) {
	const float v = ((const float *)(this->_arr_rec))[idx];
	return (v + 0.0*(__extension__ 1.0iF));
    }
    else if ( this__sz_type_rec == INT32_ZT ) {
	const int32_t v = ((const int32_t *)(this->_arr_rec))[idx];
	return (v + 0.0*(__extension__ 1.0iF));
    }
    else if ( this__sz_type_rec == UCHAR_ZT ) {
	const uint8_t v = ((const uint8_t *)(this->_arr_rec))[idx];
	return (v + 0.0*(__extension__ 1.0iF));
    }
    else if ( this__sz_type_rec == INT16_ZT ) {
	const int16_t v = ((const int16_t *)(this->_arr_rec))[idx];
	return (v + 0.0*(__extension__ 1.0iF));
    }
    else if ( this__sz_type_rec == INT64_ZT ) {
	const int64_t v = ((const int64_t *)(this->_arr_rec))[idx];
	return (v + 0.0*(__extension__ 1.0iF));
    }
    else if ( this__sz_type_rec == LDOUBLE_ZT ) {
        const long double v = ((const long double *)(this->_arr_rec))[idx];
        return (v + 0.0*(__extension__ 1.0iF));
    }
    else return NAN + NAN*(__extension__ 1.0iF);
}

/**
 * @brief  double 型に変換した１要素の値を取得
 *
 *  自身が持つ配列の要素値を double 型に変換して返します．
 *  配列サイズ超えた要素を指定した場合，NAN を返します．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @return     変換結果
 * @retval     double型に変換した要素の値 正常終了
 * @retval     NAN 要素の型がサポートされない型の場合／配列サイズを超えた
 *             要素を指定した場合
 *
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline double mdarray::dvalue( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return NAN;
    if ( this__sz_type_rec == DOUBLE_ZT ) {
	return ((const double *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == FLOAT_ZT ) {
	return ((const float *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT32_ZT ) {
	return ((const int32_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == UCHAR_ZT ) {
	return ((const uint8_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT16_ZT ) {
	return ((const int16_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT64_ZT ) {
	return ((const int64_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == LDOUBLE_ZT ) {
        return ((const long double *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == FCOMPLEX_ZT ) {
        const fcomplex cv = ((const fcomplex *)(this->_arr_rec))[idx];
        return (__real__ (cv));
    }
    else if ( this__sz_type_rec == DCOMPLEX_ZT ) {
        const dcomplex cv = ((const dcomplex *)(this->_arr_rec))[idx];
        return (__real__ (cv));
    }
    else if ( this__sz_type_rec == LDCOMPLEX_ZT ) {
        const ldcomplex cv = ((const ldcomplex *)(this->_arr_rec))[idx];
        return (__real__ (cv));
    }
    else return NAN;
}

/**
 * @brief  long 型に変換した１要素の値を取得
 *
 *  自身が持つ配列の要素値を long 型に変換して返します．<br>
 *  自身の型が浮動小数点の場合，デフォルトでは小数点以下は切り捨てられます．
 *  四捨五入したい場合は，予め set_rounding() メンバ関数を使用して，四捨五入を
 *  行う設定にします．<br>
 *  配列サイズ超えた要素を指定した場合，INDEF_LONG が返ります．
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @return     変換結果
 * @retval     long型に変換した要素の値 正常終了
 * @retval     INDEF_LONG 要素の型がサポートされない型の場合／配列サイズを
 *             超えた要素を指定した場合
 *
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline long mdarray::lvalue( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return INDEF_LONG;
    if ( this__sz_type_rec == DOUBLE_ZT ) {
	const double v = ((const double *)(this->_arr_rec))[idx];
	if ( isfinite(v) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_LONG <= v && v <= MAX_DOUBLE_ROUND_LONG )
		    return (long)((v < 0) ? v-0.5 : v+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_LONG <= v && v <= MAX_DOUBLE_TRUNC_LONG )
		    return (long)v;
	    }
	}
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == FLOAT_ZT ) {
	const float v = ((const float *)(this->_arr_rec))[idx];
	if ( isfinite(v) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_FLOAT_ROUND_LONG <= v && v <= MAX_FLOAT_ROUND_LONG )
		    return (long)((v < 0) ? v-0.5 : v+0.5);
	    }
	    else {
		if ( MIN_FLOAT_TRUNC_LONG <= v && v <= MAX_FLOAT_TRUNC_LONG )
		    return (long)v;
	    }
	}
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == INT32_ZT ) {
	const int32_t v = ((const int32_t *)(this->_arr_rec))[idx];
	if ( MIN_LONG <= v && v <= MAX_LONG ) return v;
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == UCHAR_ZT ) {
	return ((const uint8_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT16_ZT ) {
	return ((const int16_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT64_ZT ) {
	const int64_t v = ((const int64_t *)(this->_arr_rec))[idx];
	if ( MIN_LONG <= v && v <= MAX_LONG ) return v;
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == LDOUBLE_ZT ) {
	const long double v = ((const long double *)(this->_arr_rec))[idx];
	if ( isfinite(v) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_LDOUBLE_ROUND_LONG <= v && v <= MAX_LDOUBLE_ROUND_LONG )
	      return (long)((v < 0) ? v-0.5 : v+0.5);
	  }
	  else {
	    if ( MIN_LDOUBLE_TRUNC_LONG <= v && v <= MAX_LDOUBLE_TRUNC_LONG )
	      return (long)v;
	  }
	}
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == FCOMPLEX_ZT ) {
        const fcomplex *p = (const fcomplex *)(this->_arr_rec) + idx;
	const float fval = (__real__ (*p));
	if ( isfinite(fval) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_FLOAT_ROUND_LONG <= fval && fval <= MAX_FLOAT_ROUND_LONG )
	      return (long)((fval < 0) ? fval-0.5 : fval+0.5);
	  }
	  else {
	    if ( MIN_FLOAT_TRUNC_LONG <= fval && fval <= MAX_FLOAT_TRUNC_LONG )
	      return (long)fval;
	  }
	}
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == DCOMPLEX_ZT ) {
        const dcomplex *p = (const dcomplex *)(this->_arr_rec) + idx;
	const double dval = (__real__ (*p));
	if ( isfinite(dval) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_DOUBLE_ROUND_LONG <= dval && dval <= MAX_DOUBLE_ROUND_LONG )
	      return (long)((dval < 0) ? dval-0.5 : dval+0.5);
	  }
	  else {
	    if ( MIN_DOUBLE_TRUNC_LONG <= dval && dval <= MAX_DOUBLE_TRUNC_LONG )
	      return (long)dval;
	  }
	}
	return INDEF_LONG;
    }
    else if ( this__sz_type_rec == LDCOMPLEX_ZT ) {
        const ldcomplex *p = (const ldcomplex *)(this->_arr_rec) + idx;
	const long double ldval = (__real__ (*p));
	if ( isfinite(ldval) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_LDOUBLE_ROUND_LONG <= ldval && ldval <= MAX_LDOUBLE_ROUND_LONG )
	      return (long)((ldval < 0) ? ldval-0.5 : ldval+0.5);
	  }
	  else {
	    if ( MIN_LDOUBLE_TRUNC_LONG <= ldval && ldval <= MAX_LDOUBLE_TRUNC_LONG )
	      return (long)ldval;
	  }
	}
	return INDEF_LONG;
    }
    else return INDEF_LONG;
}

/**
 * @brief  long long 型に変換した１要素の値を取得
 *
 *  自身が持つ配列の要素値を long long 型に変換して返します．<br>
 *  自身の型が浮動小数点の場合，デフォルトでは小数点以下は切り捨てられます．
 *  四捨五入したい場合は，予め set_rounding() メンバ関数を使用して，四捨五入を
 *  行う設定にします．<br>
 *  配列サイズ超えた要素を指定した場合，INDEF_LLONG が返ります．
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @return     変換結果
 * @retval     longlong型に変換した要素の値 正常終了
 * @retval     INDEF_LLONG 要素の型がサポートされない型の場合／配列サイズを
 *             超えた要素を指定した場合
 *
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline long long mdarray::llvalue( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return INDEF_LLONG;
    if ( this__sz_type_rec == DOUBLE_ZT ) {
	const double v = ((const double *)(this->_arr_rec))[idx];
	if ( isfinite(v) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_DOUBLE_ROUND_LLONG <= v && v <= MAX_DOUBLE_ROUND_LLONG )
	      return (long long)((v < 0) ? v-0.5 : v+0.5);
	  }
	  else {
	    if ( MIN_DOUBLE_TRUNC_LLONG <= v && v <= MAX_DOUBLE_TRUNC_LLONG )
	      return (long long)v;
	  }
	}
	return INDEF_LLONG;
    }
    else if ( this__sz_type_rec == FLOAT_ZT ) {
	const float v = ((const float *)(this->_arr_rec))[idx];
	if ( isfinite(v) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_FLOAT_ROUND_LLONG <= v && v <= MAX_FLOAT_ROUND_LLONG )
	      return (long long)((v < 0) ? v-0.5 : v+0.5);
	  }
	  else {
	    if ( MIN_FLOAT_TRUNC_LLONG <= v && v <= MAX_FLOAT_TRUNC_LLONG )
	      return (long long)v;
	  }
	}
	return INDEF_LLONG;
    }
    else if ( this__sz_type_rec == INT32_ZT ) {
	return ((const int32_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == UCHAR_ZT ) {
	return ((const uint8_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT16_ZT ) {
	return ((const int16_t *)(this->_arr_rec))[idx];
    }
    else if ( this__sz_type_rec == INT64_ZT ) {
	const int64_t v = ((const int64_t *)(this->_arr_rec))[idx];
	if ( MIN_LLONG <= v && v <= MAX_LLONG ) return v;
	return INDEF_LLONG;
    }
    else if ( this__sz_type_rec == LDOUBLE_ZT ) {
      const long double v = ((const long double *)(this->_arr_rec))[idx];
      if ( isfinite(v) ) {
	if ( this->rounding_rec == true ) {
	  if ( MIN_LDOUBLE_ROUND_LLONG <= v && v <= MAX_LDOUBLE_ROUND_LLONG )
	    return (long long)((v < 0) ? v-0.5 : v+0.5);
	}
	else {
	  if ( MIN_LDOUBLE_TRUNC_LLONG <= v && v <= MAX_LDOUBLE_TRUNC_LLONG )
	    return (long long)v;
	}
      }
      return INDEF_LLONG;
    }
    else if ( this__sz_type_rec == FCOMPLEX_ZT ) {
        const fcomplex *p = (const fcomplex *)(this->_arr_rec) + idx;
	const float fval = (__real__ (*p));
	if ( isfinite(fval) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_FLOAT_ROUND_LONG <= fval && fval <= MAX_FLOAT_ROUND_LONG )
	      return (long long)((fval < 0) ? fval-0.5 : fval+0.5);
	  }
	  else {
	    if ( MIN_FLOAT_TRUNC_LONG <= fval && fval <= MAX_FLOAT_TRUNC_LONG )
	      return (long long)fval;
	  }
	}
	return INDEF_LLONG;
    }
    else if ( this__sz_type_rec == DCOMPLEX_ZT ) {
        const dcomplex *p = (const dcomplex *)(this->_arr_rec) + idx;
	const double dval = (__real__ (*p));
	if ( isfinite(dval) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_DOUBLE_ROUND_LONG <= dval && dval <= MAX_DOUBLE_ROUND_LONG )
	      return (long long)((dval < 0) ? dval-0.5 : dval+0.5);
	  }
	  else {
	    if ( MIN_DOUBLE_TRUNC_LONG <= dval && dval <= MAX_DOUBLE_TRUNC_LONG )
	      return (long long)dval;
	  }
	}
	return INDEF_LLONG;
    }
    else if ( this__sz_type_rec == LDCOMPLEX_ZT ) {
        const ldcomplex *p = (const ldcomplex *)(this->_arr_rec) + idx;
	const long double ldval = (__real__ (*p));
	if ( isfinite(ldval) ) {
	  if ( this->rounding_rec == true ) {
	    if ( MIN_LDOUBLE_ROUND_LONG <= ldval && ldval <= MAX_LDOUBLE_ROUND_LONG )
	      return (long long)((ldval < 0) ? ldval-0.5 : ldval+0.5);
	  }
	  else {
	    if ( MIN_LDOUBLE_TRUNC_LONG <= ldval && ldval <= MAX_LDOUBLE_TRUNC_LONG )
	      return (long long)ldval;
	  }
	}
	return INDEF_LLONG;
    }
    else return INDEF_LLONG;
}

/**
 * @brief  指定された1要素へ値を代入 (dcomplex型で指定)
 *
 *  自身の配列の，idxn で指定された1要素に値を設定します．<br>
 *  浮動小数点値を整数型の要素に代入する場合，デフォルトでは小数点以下は切り捨
 *  てます．小数点以下を四捨五入したい場合は，予め set_rounding() メンバ関数を
 *  使用して，オブジェクトを四捨五入を行う設定にします．<br>
 *  動作モードが自動リサイズモードの場合，指定された要素番号に従って配列サイズ
 *  が自動的にリサイズされます．<br>
 *  動作モードが手動リサイズモードの場合，配列サイズを超えた要素へ値を代入して
 *  も，無視されるだけでエラーとはなりません．配列サイズを超えた要素へ値の代入
 *  を行うには，予め resize() メンバ関数でサイズを拡張する必要があります．
 *
 * @param      value dcomplex 型の値
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号
 * @param      idx2 次元番号2の次元(3次元目)の要素番号
 *
 * @return     自身の参照
 *
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 *
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline mdarray &mdarray::assign( dcomplex val,	
				 ssize_t idx0, ssize_t idx1, ssize_t idx2)
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( this__sz_type_rec == DOUBLE_ZT )        /* 実数部のみ適用 */
        ((double *)(this->_arr_rec))[idx] = (__real__ (val));
    else if ( this__sz_type_rec == FLOAT_ZT )    /* 実数部のみ適用 */
        ((float *)(this->_arr_rec))[idx] = (__real__ (val));
    else if ( this__sz_type_rec == LDOUBLE_ZT )  /* 実数部のみ適用 */
        ((long double *)(this->_arr_rec))[idx] = (__real__ (val));
    else if ( this__sz_type_rec == INT64_ZT ) {  /* 実数部のみ適用 */
	const double d_val = (__real__ (val));
        int64_t i_val = INDEF_INT64;
	if ( isfinite(d_val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_INT64 <= d_val &&
		     d_val <= MAX_DOUBLE_ROUND_INT64 ) i_val = (int64_t)((d_val < 0) ? d_val-0.5 : d_val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_INT64 <= d_val &&
		     d_val <= MAX_DOUBLE_TRUNC_INT64 ) i_val = (int64_t)d_val;
	    }
	}
        ((int64_t *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == INT32_ZT ) {  /* 実数部のみ適用 */
	const double d_val = (__real__ (val));
	int32_t i_val = INDEF_INT32;
	if ( isfinite(d_val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_INT32 <= d_val &&
		     d_val <= MAX_DOUBLE_ROUND_INT32 ) i_val = (int32_t)((d_val < 0) ? d_val-0.5 : d_val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_INT32 <= d_val &&
		     d_val <= MAX_DOUBLE_TRUNC_INT32 ) i_val = (int32_t)d_val;
	    }
	}
        ((int32_t *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == INT16_ZT ) {  /* 実数部のみ適用 */
	const double d_val = (__real__ (val));
	int16_t i_val = INDEF_INT16;
	if ( isfinite(d_val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_INT16 <= d_val &&
		     d_val <= MAX_DOUBLE_ROUND_INT16 ) i_val = (int16_t)((d_val < 0) ? d_val-0.5 : d_val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_INT16 <= d_val &&
		     d_val <= MAX_DOUBLE_TRUNC_INT16 ) i_val = (int16_t)d_val;
	    }
	}
        ((int16_t *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == UCHAR_ZT ) {  /* 実数部のみ適用 */
	const double d_val = (__real__ (val));
	unsigned char i_val = INDEF_UCHAR;
	if ( isfinite(d_val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_UCHAR <= d_val &&
		     d_val <= MAX_DOUBLE_ROUND_UCHAR ) i_val = (unsigned char)((d_val < 0) ? d_val-0.5 : d_val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_UCHAR <= d_val &&
		     d_val <= MAX_DOUBLE_TRUNC_UCHAR ) i_val = (unsigned char)d_val;
	    }
	}
        ((unsigned char *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == FCOMPLEX_ZT )
        ((fcomplex *)(this->_arr_rec))[idx] = val;
    else if ( this__sz_type_rec == DCOMPLEX_ZT )
        ((dcomplex *)(this->_arr_rec))[idx] = val;
    else if ( this__sz_type_rec == LDCOMPLEX_ZT )
        ((ldcomplex *)(this->_arr_rec))[idx] = val;

    return *this;
}

/**
 * @brief  指定された 1要素へ値を代入 (double型で指定)
 *
 *  自身の配列の，idxn で指定された1要素に値を設定します．<br>
 *  浮動小数点値を整数型の要素に代入する場合，デフォルトでは小数点以下は切り捨
 *  てます．小数点以下を四捨五入したい場合は，予め set_rounding() メンバ関数を
 *  使用して，オブジェクトを四捨五入を行う設定にします．<br>
 *  動作モードが自動リサイズモードの場合，指定された要素番号に従って配列サイズ
 *  が自動的にリサイズされます．<br>
 *  動作モードが手動リサイズモードの場合，配列サイズを超えた要素へ値を代入して
 *  も，無視されるだけでエラーとはなりません．配列サイズを超えた要素へ値の代入
 *  を行うには，予め resize() メンバ関数でサイズを拡張する必要があります．
 *
 * @param      value double 型の値
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号
 * @param      idx2 次元番号2の次元(3次元目)の要素番号
 *
 * @return     自身の参照
 *
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモードの場合)
 *
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline mdarray &mdarray::assign(double val, ssize_t idx0, ssize_t idx1, ssize_t idx2)
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( this__sz_type_rec == DOUBLE_ZT )
        ((double *)(this->_arr_rec))[idx] = val;
    else if ( this__sz_type_rec == FLOAT_ZT )
        ((float *)(this->_arr_rec))[idx] = val;
    else if ( this__sz_type_rec == LDOUBLE_ZT )
        ((long double *)(this->_arr_rec))[idx] = val;
    else if ( this__sz_type_rec == INT64_ZT ) {
        int64_t i_val = INDEF_INT64;
	if ( isfinite(val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_INT64 <= val &&
		     val <= MAX_DOUBLE_ROUND_INT64 ) i_val = (int64_t)((val < 0) ? val-0.5 : val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_INT64 <= val &&
		     val <= MAX_DOUBLE_TRUNC_INT64 ) i_val = (int64_t)val;
	    }
	}
        ((int64_t *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == INT32_ZT ) {
	int32_t i_val = INDEF_INT32;
	if ( isfinite(val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_INT32 <= val &&
		     val <= MAX_DOUBLE_ROUND_INT32 ) i_val = (int32_t)((val < 0) ? val-0.5 : val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_INT32 <= val &&
		     val <= MAX_DOUBLE_TRUNC_INT32 ) i_val = (int32_t)val;
	    }
	}
        ((int32_t *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == INT16_ZT ) {
	int16_t i_val = INDEF_INT16;
	if ( isfinite(val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_INT16 <= val &&
		     val <= MAX_DOUBLE_ROUND_INT16 ) i_val = (int16_t)((val < 0) ? val-0.5 : val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_INT16 <= val &&
		     val <= MAX_DOUBLE_TRUNC_INT16 ) i_val = (int16_t)val;
	    }
	}
        ((int16_t *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == UCHAR_ZT ) {
	unsigned char i_val = INDEF_UCHAR;
	if ( isfinite(val) ) {
	    if ( this->rounding_rec == true ) {
		if ( MIN_DOUBLE_ROUND_UCHAR <= val &&
		     val <= MAX_DOUBLE_ROUND_UCHAR ) i_val = (unsigned char)((val < 0) ? val-0.5 : val+0.5);
	    }
	    else {
		if ( MIN_DOUBLE_TRUNC_UCHAR <= val &&
		     val <= MAX_DOUBLE_TRUNC_UCHAR ) i_val = (unsigned char)val;
	    }
	}
        ((unsigned char *)(this->_arr_rec))[idx] = i_val;
    }
    else if ( this__sz_type_rec == FCOMPLEX_ZT )  /* 複素数に変換して代入 */
        ((fcomplex *)(this->_arr_rec))[idx] = (val + 0.0*(__extension__ 1.0iF));
    else if ( this__sz_type_rec == DCOMPLEX_ZT )  /* 複素数に変換して代入 */
        ((dcomplex *)(this->_arr_rec))[idx] = (val + 0.0*(__extension__ 1.0iF));
    else if ( this__sz_type_rec == LDCOMPLEX_ZT ) /* 複素数に変換して代入 */
        ((ldcomplex *)(this->_arr_rec))[idx] = (val + 0.0*(__extension__ 1.0iF));

    return *this;
}

/**
 * @brief  指定された1要素値の参照 (fcomplex 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  このメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，NAN + NAN*I が
 *  返ります．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @return     fcomplex 型での値の参照
 *
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw      オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *             バイトサイズより小さい場合
 *
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline fcomplex &mdarray::fx( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(fcomplex) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		     "Too small size of type (%d-bytes) for fcomplex",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	fcomplex *p = (fcomplex *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	fcomplex *p = (fcomplex *)(this->junk_rec);
	*p = NAN + NAN*(__extension__ 1.0iF);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (dcomplex 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  このメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，NAN + NAN*I が
 *  返ります．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @return     dcomplex 型での値の参照
 *
 * @throw      オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *             バイトサイズより小さい場合
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモード)
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください
 *
 */
inline dcomplex &mdarray::dx( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(dcomplex) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		   "Too small size of type (%d-bytes) for dcomplex",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	dcomplex *p = (dcomplex *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	dcomplex *p = (dcomplex *)(this->junk_rec);
	*p = NAN + NAN*(__extension__ 1.0iF);
	return *p;
    }
}


/**
 * @brief  指定された1要素値の参照 (ldcomplex 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  このメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，NAN + NAN*I が
 *  返ります．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 * @return     ldcomplex 型での値の参照
 * @throw      オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *             バイトサイズより小さい場合
 * @throw      内部バッファの確保に失敗した場合(自動リサイズモード)
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline ldcomplex &mdarray::ldx( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(ldcomplex) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		   "Too small size of type (%d-bytes) for ldcomplex",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	ldcomplex *p = (ldcomplex *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	ldcomplex *p = (ldcomplex *)(this->junk_rec);
	*p = NAN + NAN*(__extension__ 1.0iF);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (float 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  このメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，NANが返ります．
 *
 * @param    idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param    idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param    idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return   float要素の値の参照
 * @throw    内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw    オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *           バイトサイズより小さい場合
 * @note     引数に，MDARRAY_INDEF を明示的に与えな\いでください
 */
inline float &mdarray::f( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(float) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			"Too small size of type (%d-bytes) for float",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	float *p = (float *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	float *p = (float *)(this->junk_rec);
	*p = NAN;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (double 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  このメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，NANが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    double要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline double &mdarray::d( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(double) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		       "Too small size of type (%d-bytes) for double",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	double *p = (double *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	double *p = (double *)(this->junk_rec);
	*p = NAN;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (long double 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  このメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，NANが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    long double要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline long double &mdarray::ld( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(long double) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		  "Too small size of type (%d-bytes) for long double",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	long double *p =
	    (long double *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	long double *p = (long double *)(this->junk_rec);
	*p = (long double)(NAN);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (unsigned char 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_UCHAR が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    char要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline unsigned char &mdarray::c( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(unsigned char) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		"Too small size of type (%d-bytes) for unsigned char",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	unsigned char *p =
	    (unsigned char *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	unsigned char *p = (unsigned char *)(this->junk_rec);
	*p = INDEF_UCHAR;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (short 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_SHORT が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    short要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline short &mdarray::s( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(short) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			"Too small size of type (%d-bytes) for short",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	short *p = (short *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	short *p = (short *)(this->junk_rec);
	*p = INDEF_SHORT;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (int 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_INT が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline int &mdarray::i( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			  "Too small size of type (%d-bytes) for int",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	int *p = (int *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int *p = (int *)(this->junk_rec);
	*p = INDEF_INT;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (long 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_LONG が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    long要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline long &mdarray::l( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(long) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			 "Too small size of type (%d-bytes) for long",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	long *p = (long *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	long *p = (long *)(this->junk_rec);
	*p = INDEF_LONG;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (long long型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_LLONG が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    long long要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline long long &mdarray::ll( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(long long) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		    "Too small size of type (%d-bytes) for long long",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	long long *p = (long long *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	long long *p = (long long *)(this->junk_rec);
	*p = INDEF_LLONG;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (int16_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_INT16 が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int16_t要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline int16_t &mdarray::i16( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int16_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for int16_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	int16_t *p = (int16_t *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int16_t *p = (int16_t *)(this->junk_rec);
	*p = INDEF_INT16;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (int32_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_INT32 が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int32_t要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline int32_t &mdarray::i32( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int32_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for int32_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	int32_t *p = (int32_t *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int32_t *p = (int32_t *)(this->junk_rec);
	*p = INDEF_INT32;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (int64_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，INDEF_INT64 が
 *  返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int64_t要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline int64_t &mdarray::i64( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int64_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for int64_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	int64_t *p = (int64_t *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int64_t *p = (int64_t *)(this->junk_rec);
	*p = INDEF_INT64;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (size_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，0 が返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    size_t要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline size_t &mdarray::z( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(size_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		       "Too small size of type (%d-bytes) for size_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	size_t *p = (size_t *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	size_t *p = (size_t *)(this->junk_rec);
	*p = 0;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (ssize_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると，0 が返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    ssize_t要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline ssize_t &mdarray::sz( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(ssize_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for ssize_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	ssize_t *p = (ssize_t *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	ssize_t *p = (ssize_t *)(this->junk_rec);
	*p = 0;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (bool 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予めresize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取るとfalseが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    bool要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが,メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline bool &mdarray::b( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(bool) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			 "Too small size of type (%d-bytes) for bool",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	bool *p = (bool *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	bool *p = (bool *)(this->junk_rec);
	*p = false;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の参照 (uintptr_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を設定・取得します．<br>
 *  これらのメンバ関数を使用して値を読み書きする際，動作モードが「自動リサイズ
 *  モード」の場合，配列サイズは指定された要素番号に従って自動的にリサイズされ
 *  ます．<br>
 *  動作モードが「手動リサイズモード」の場合，配列サイズを超えた要素へ値を代入
 *  しても，無視されるだけでエラーとはなりません．現在の配列サイズを超えた要素
 *  へ値の代入を行うには，予め resize() メンバ関数などで配列サイズを拡張する必
 *  要があります．<br>
 *  「手動リサイズモード」で配列サイズを超えた要素を読み取ると NULLが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    uintptr要素の値の参照
 * @throw     内部バッファの確保に失敗した場合(自動リサイズモード)
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline uintptr_t &mdarray::p( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(uintptr_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		    "Too small size of type (%d-bytes) for uintptr_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( 0 <= idx ) {
	uintptr_t *p = (uintptr_t *)((char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	uintptr_t *p = (uintptr_t *)(this->junk_rec);
	*p = (uintptr_t)(NULL);
	return *p;
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  fx_csと同様
 *
 */
inline const fcomplex &mdarray::fx( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->fx_cs(idx0, idx1, idx2);
}

/**
 * @brief  dx_csと同様
 *
 */
inline const dcomplex &mdarray::dx( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->dx_cs(idx0, idx1, idx2);
}

/**
 * @brief  ldx_csと同様
 *
 */
inline const ldcomplex &mdarray::ldx( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->ldx_cs(idx0, idx1, idx2);
}

/**
 * @brief  f_csと同様
 *
 */
inline const float &mdarray::f( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->f_cs(idx0, idx1, idx2);
}

/**
 * @brief  d_csと同様
 *
 */
inline const double &mdarray::d( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->d_cs(idx0, idx1, idx2);
}

/**
 * @brief  ld_csと同様
 *
 */
inline const long double &mdarray::ld( ssize_t idx0, ssize_t idx1,
				ssize_t idx2 ) const
{
    return this->ld_cs(idx0, idx1, idx2);
}

/**
 * @brief  c_csと同様
 *
 */
inline const unsigned char &mdarray::c( ssize_t idx0, ssize_t idx1, 
				 ssize_t idx2 ) const
{
    return this->c_cs(idx0, idx1, idx2);
}

/**
 * @brief  s_csと同様
 *
 */
inline const short &mdarray::s( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->s_cs(idx0, idx1, idx2);
}

/**
 * @brief  i_csと同様
 *
 */
inline const int &mdarray::i( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->i_cs(idx0, idx1, idx2);
}

/**
 * @brief  l_csと同様
 *
 */
inline const long &mdarray::l( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->l_cs(idx0, idx1, idx2);
}

/**
 * @brief  ll_csと同様
 *
 */
inline const long long &mdarray::ll(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->ll_cs(idx0, idx1, idx2);
}

/**
 * @brief  i16_csと同様
 *
 */
inline const int16_t &mdarray::i16(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->i16_cs(idx0, idx1, idx2);
}

/**
 * @brief  i32_csと同様
 *
 */
inline const int32_t &mdarray::i32(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->i32_cs(idx0, idx1, idx2);
}

/**
 * @brief  i64_csと同様
 *
 */
inline const int64_t &mdarray::i64(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->i64_cs(idx0, idx1, idx2);
}

/**
 * @brief  z_csと同様
 *
 */
inline const size_t &mdarray::z( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->z_cs(idx0, idx1, idx2);
}

/**
 * @brief  sz_csと同様
 *
 */
inline const ssize_t &mdarray::sz( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->sz_cs(idx0, idx1, idx2);
}

/**
 * @brief  b_csと同様
 *
 */
inline const bool &mdarray::b( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->b_cs(idx0, idx1, idx2);
}

/**
 * @brief  p_csと同様
 *
 */
inline const uintptr_t &mdarray::p(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->p_cs(idx0, idx1, idx2);
}
#endif

/* read only version */

/**
 * @brief  指定された1要素値の読み出し (fcomplex 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NAN + NAN*I が返ります．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 * @return     fcomplex 型での値の参照
 * @throw      オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *             バイトサイズより小さい場合
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline const fcomplex &mdarray::fx_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(fcomplex) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		     "Too small size of type (%d-bytes) for fcomplex",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const fcomplex *p = 
	    (const fcomplex *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	fcomplex *p = (fcomplex *)(this->junk_rec);
	*p = NAN + NAN*(__extension__ 1.0iF);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (dcomplex 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NAN + NAN*I が返ります．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 * @return     dcomplex 型での値の参照
 * @throw      オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *             バイトサイズより小さい場合
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline const dcomplex &mdarray::dx_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(dcomplex) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		     "Too small size of type (%d-bytes) for dcomplex",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const dcomplex *p = 
	    (const dcomplex *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	dcomplex *p = (dcomplex *)(this->junk_rec);
	*p = NAN + NAN*(__extension__ 1.0iF);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (ldcomplex 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NAN + NAN*I が返ります．
 *
 * @param      idx0 次元番号0の次元(1次元目)の要素番号
 * @param      idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param      idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 * @return     ldcomplex 型での値の参照
 * @throw      オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *             バイトサイズより小さい場合
 * @note       引数に，MDARRAY_INDEF を明示的に与えないでください．
 */
inline const ldcomplex &mdarray::ldx_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(ldcomplex) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		    "Too small size of type (%d-bytes) for ldcomplex",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const ldcomplex *p = 
	    (const ldcomplex *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	ldcomplex *p = (ldcomplex *)(this->junk_rec);
	*p = NAN + NAN*(__extension__ 1.0iF);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (float 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NANが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    float要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const float &mdarray::f_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(float) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			"Too small size of type (%d-bytes) for float",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const float *p = 
	    (const float *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	float *p = (float *)(this->junk_rec);
	*p = NAN;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (double 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NANが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    double要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const double &mdarray::d_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(double) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		       "Too small size of type (%d-bytes) for double",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const double *p = 
	    (const double *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	double *p = (double *)(this->junk_rec);
	*p = NAN;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (long double 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NANが返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    long double要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const long double &mdarray::ld_cs( ssize_t idx0, ssize_t idx1,
				   ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(long double) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		  "Too small size of type (%d-bytes) for long double",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const long double *p =
	 (const long double *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	long double *p = (long double *)(this->junk_rec);
	*p = (long double)(NAN);
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (unsigned char 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_UCHAR が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    char要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 *            
 */
inline const unsigned char &mdarray::c_cs( ssize_t idx0, ssize_t idx1, 
				    ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(unsigned char) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		"Too small size of type (%d-bytes) for unsigned char",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
      const unsigned char *p =
       (const unsigned char *)((const char *)(this->_arr_rec) + sz * idx);
      return *p;
    }
    else {
	unsigned char *p = (unsigned char *)(this->junk_rec);
	*p = INDEF_UCHAR;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (short 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_SHORT が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    short要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 *            
 */
inline const short &mdarray::s_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(short) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			"Too small size of type (%d-bytes) for short",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const short *p =
	    (const short *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	short *p = (short *)(this->junk_rec);
	*p = INDEF_SHORT;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (int 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_INT が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const int &mdarray::i_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			  "Too small size of type (%d-bytes) for int",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const int *p =
	    (const int *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int *p = (int *)(this->junk_rec);
	*p = INDEF_INT;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (long 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_LONG が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    long要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const long &mdarray::l_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(long) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			 "Too small size of type (%d-bytes) for long",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const long *p =
	    (const long *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	long *p = (long *)(this->junk_rec);
	*p = INDEF_LONG;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (long long型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_LLONG が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    long long要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const long long &mdarray::ll_cs(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(long long) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		    "Too small size of type (%d-bytes) for long long",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const long long *p =
	   (const long long *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	long long *p = (long long *)(this->junk_rec);
	*p = INDEF_LLONG;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (int16_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_INT16 が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int16_t要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const int16_t &mdarray::i16_cs(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int16_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for int16_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const int16_t *p = 
	    (const int16_t *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int16_t *p = (int16_t *)(this->junk_rec);
	*p = INDEF_INT16;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (int32_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_INT32 が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int32_t要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const int32_t &mdarray::i32_cs(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int32_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for int32_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const int32_t *p =
	    (const int32_t *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int32_t *p = (int32_t *)(this->junk_rec);
	*p = INDEF_INT32;
	return *p;
    }
}

/**
 * @brief  指定された1要素値の読み出し (int64_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，INDEF_INT64 が返ります．<br>
 *  INDEF の値については，各型における最小の整数値が設定されています．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    int64_t要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const int64_t &mdarray::i64_cs(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(int64_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for int64_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const int64_t *p =
	    (const int64_t *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	int64_t *p = (int64_t *)(this->junk_rec);
	*p = INDEF_INT64;
	return *p;
    }
}

/**
 * @brief 指定された1要素値の読み出し (size_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，0 が返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    size_t要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const size_t &mdarray::z_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(size_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		       "Too small size of type (%d-bytes) for size_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const size_t *p = 
	    (const size_t *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	size_t *p = (size_t *)(this->junk_rec);
	*p = 0;
	return *p;
    }
}

/**
 * @brief 指定された1要素値の読み出し (ssize_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，0 が返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    ssize_t要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const ssize_t &mdarray::sz_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(ssize_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		      "Too small size of type (%d-bytes) for ssize_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const ssize_t *p =
	    (const ssize_t *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	ssize_t *p = (ssize_t *)(this->junk_rec);
	*p = 0;
	return *p;
    }
}

/**
 * @brief 指定された1要素値の読み出し (bool 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，false が返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    bool要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const bool &mdarray::b_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(bool) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
			 "Too small size of type (%d-bytes) for bool",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const bool *p =
	    (const bool *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	bool *p = (bool *)(this->junk_rec);
	*p = false;
	return *p;
    }
}

/**
 * @brief 指定された1要素値の読み出し (uintptr_t 型)
 *
 *  idx0，idx1，idx2 で指定された配列要素の値を返します．
 *  値を設定することはできません．<br>
 *  配列サイズを超えた要素を読み取ると，NULL が返ります．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    uintptr要素の値の参照
 * @throw     オブジェクト内の要素のバイトサイズが，メンバ関数の戻り値の型の
 *            バイトサイズより小さい場合
 * @note      引数に，MDARRAY_INDEF を明示的に与えないでください
 */
inline const uintptr_t &mdarray::p_cs(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    const size_t sz = this->cached_bytes_rec;
    if ( sz < sizeof(uintptr_t) ) {
	this->err_throw_int_arg(__FUNCTION__,"FATAL",
		    "Too small size of type (%d-bytes) for uintptr_t",(int)sz);
    }
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	const uintptr_t *p =
	   (const uintptr_t *)((const char *)(this->_arr_rec) + sz * idx);
	return *p;
    }
    else {
	uintptr_t *p = (uintptr_t *)(this->junk_rec);
	*p = (uintptr_t)(NULL);
	return *p;
    }
}


/**
 * @brief  配列の先頭要素のアドレスを取得
 *
 *  自身が持つ配列の先頭要素のアドレスを返します．
 *  data_ptr() メンバ関数の場合，「const」属性なしのメンバ関数，
 *  「const」属性ありのメンバ関数のどちらが使われるかは，
 *  オブジェクトの「const」属性の有無で自動的に決まります．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @return    先頭要素のアドレス
 */
inline void *mdarray::data_ptr()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_arr_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  配列の先頭要素のアドレスを取得 (読取専用)
 *
 *  自身が持つ配列の先頭要素のアドレスを返します．
 *  data_ptr() メンバ関数の場合，「const」属性なしのメンバ関数，
 *  「const」属性ありのメンバ関数のどちらが使われるかは，
 *  オブジェクトの「const」属性の有無で自動的に決まります．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @return    先頭要素のアドレス
 * @note      読み取り専用のアドレスを取得します．
 */
inline const void *mdarray::data_ptr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_arr_rec;
}
#endif

/**
 * @brief  配列の先頭要素のアドレスを取得 (読取専用)
 *
 *  自身が持つ配列の先頭要素のアドレスを返します．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @return    先頭要素のアドレス
 * @note      読み取り専用のアドレスを取得します．
 */
inline const void *mdarray::data_ptr_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_arr_rec;
}

/* 自動リサイズはしない */
/**
 * @brief  配列の指定要素のアドレスを取得
 *
 *  自身が持つ配列の指定要素のアドレスを返します．
 *  data_ptr() メンバ関数の場合，「const」属性なしのメンバ関数，
 *  「const」属性ありのメンバ関数のどちらが使われるかは，
 *  オブジェクトの「const」属性の有無で自動的に決まります．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    指定要素のアドレス
 */
inline void *mdarray::data_ptr( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    return ( (idx < 0) ? NULL :
	    (void *)((char *)(this->_arr_rec) + this->cached_bytes_rec * idx) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  配列の指定要素のアドレスを取得 (読取専用)
 *
 *  自身が持つ配列の指定要素のアドレスを返します．
 *  「const」属性なしのメンバ関数，「const」属性ありのメンバ関数のどちらが
 *  使われるかは，オブジェクトの「const」属性の有無で自動的に決まります．<br>
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    指定要素のアドレス
 * @note      読み取り専用のアドレスを取得します． 
 */
inline const void *mdarray::data_ptr( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    return ( (idx < 0) ? NULL :
	     (const void *)((const char *)(this->_arr_rec) + this->cached_bytes_rec * idx) );
}
#endif

/**
 * @brief  配列の指定要素のアドレスを取得 (読取専用)
 *
 *  自身が持つ配列の指定要素のアドレスを返します．
 *  引数に，MDARRAY_INDEF を明示的に与えないでください．
 *
 * @param     idx0 次元番号0 の次元(1 次元目) の要素番号
 * @param     idx1 次元番号1 の次元(2 次元目) の要素番号(省略可)
 * @param     idx2 次元番号2 の次元(3 次元目) の要素番号(省略可)
 * @return    指定要素のアドレス
 * @note      読み取り専用のアドレスを取得します． 
 */
inline const void *mdarray::data_ptr_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    const ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    return ( (idx < 0) ? NULL :
	     (const void *)((const char *)(this->_arr_rec) + this->cached_bytes_rec * idx) );
}


/* acquisition of address table for 2d array */
/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 */
inline void *const *mdarray::data_ptr_2d( bool use )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    if ( use == false ) {				/* 機能 off */
	/* 外部ポインタを使っている場合はoffにできない */
	if ( this->extptr_2d_rec == NULL ) this->free_arr_ptr_2d();
    }
    else {
	if ( this->_arr_ptr_2d_rec == NULL ) {
	    this->update_arr_ptr_2d(true);		/* 機能 on */
	}
    }

    return (void *const *)(this->_arr_ptr_2d_rec);
}

/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得
 */
inline void *const *mdarray::data_ptr_2d()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (void *const *)(this->_arr_ptr_2d_rec);
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const void *const *mdarray::data_ptr_2d() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *)(this->_arr_ptr_2d_rec);
}
#endif

/**
 * @brief  内部で生成された2次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const void *const *mdarray::data_ptr_2d_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *)(this->_arr_ptr_2d_rec);
}

/* acquisition of address table for 3d array */
/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 */
inline void *const *const *mdarray::data_ptr_3d( bool use )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    if ( use == false ) {				/* 機能 off */
	/* 外部ポインタを使っている場合はoffにできない */
	if ( this->extptr_3d_rec == NULL ) this->free_arr_ptr_3d();
    }
    else {
	if ( this->_arr_ptr_3d_rec == NULL ) {
	    this->update_arr_ptr_3d(true);		/* 機能 on */
	}
    }

    return (void *const *const *)(this->_arr_ptr_3d_rec);
}

/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得
 */
inline void *const *const *mdarray::data_ptr_3d()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (void *const *const *)(this->_arr_ptr_3d_rec);
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const void *const *const *mdarray::data_ptr_3d() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *const *)(this->_arr_ptr_3d_rec);
}
#endif

/**
 * @brief  内部で生成された3次元配列用ポインタ配列のアドレスを取得 (読取専用)
 */
inline const void *const *const *mdarray::data_ptr_3d_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *const *)(this->_arr_ptr_3d_rec);
}


/**
 * @brief  配列の次元数を持つ配列のアドレスを取得
 *
 * @return    自身が持つ配列の次元数を持つ配列のアドレス
 *            
 */
inline const size_t *mdarray::cdimarray() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_size_rec;
}

/**
 * @brief  サイズ拡張時の初期値を取得
 *
 * @return    自身が持つサイズ拡張時の初期値が格納されているアドレスを返します
 *            (初期値が未設定の場合はNULL が返ります)
 */
inline const void *mdarray::default_value_ptr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_default_rec;
}

/**
 * @brief  指定された要素の位置を返す (リサイズあり)
 *
 *  配列の範囲外の要素を指定した場合リサイズを行う．
 *
 * @param   ix0 1次元目の要素番号
 * @param   ix1 2次元目の要素番号
 * @param   ix2 3次元目の要素番号
 * @return  成功した場合は位置番号<br>
 *          引数が正しくないときは-1
 * @throw   内部バッファの確保に失敗した場合
 * @note    このメンバ関数は protected です．
 */
inline ssize_t mdarray::get_idx_3d( ssize_t ix0, ssize_t ix1, ssize_t ix2 )
{
    if ( this->auto_resize_rec == true ) {
      if ( this->dim_size_rec == 0 ) this->increase_dim();
      if ( ix1 == MDARRAY_INDEF ) {
        /* 1-dim */
	if ( ix0 < 0 ) return -1;
	else if ( ix0 < (ssize_t)(this->cached_length_rec) ) return ix0;
	else {
	  /* 最高次元の軸をリサイズする */
	  size_t i, blk = 1;
	  for ( i=0 ; i+1 < this->dim_size_rec ; i++ ) {
	      blk *= this->_size_rec[i];
	  }
	  this->resize(this->dim_size_rec - 1, (ix0 / blk) + 1);
	  return ix0;
	}
      }
      else {
	/* 2-dim or 3-dim */
	if ( this->dim_size_rec == 1 ) this->increase_dim();
	if ( ix2 == MDARRAY_INDEF ) {
	  /* 2-dim */
	  if ( ix0 < 0 || ix1 < 0 ) return -1;
	  else {
	    if ( (ssize_t)(this->cached_col_length_rec) <= ix0 ) 
	      this->resize(0,ix0+1);
	    if ( (ssize_t)(this->cached_row_layer_length_rec) <= ix1 ) {
	      /* 最高次元の軸をリサイズする */
	      size_t i, blk = 1;
	      for ( i=1 ; i+1 < this->dim_size_rec ; i++ ) 
		  blk *= this->_size_rec[i];
	      this->resize(this->dim_size_rec - 1, (ix1 / blk) + 1);
	    }
	    return ix0 + this->cached_col_length_rec * ix1;
	  }
	}
	else {
	  /* 3-dim */
	  if ( this->dim_size_rec == 2 ) this->increase_dim();
	  if ( ix0 < 0 || ix1 < 0 || ix2 < 0 ) return -1;
	  else {
	    if ( (ssize_t)(this->cached_col_length_rec) <= ix0 ) 
	      this->resize(0,ix0+1);
	    if ( (ssize_t)(this->cached_row_length_rec) <= ix1 ) 
	      this->resize(1,ix1+1);
	    if ( (ssize_t)(this->cached_layer_length_rec) <= ix2 ) {
	      /* 最高次元の軸をリサイズする */
	      size_t i, blk = 1;
	      for ( i=2 ; i+1 < this->dim_size_rec ; i++ ) 
		  blk *= this->_size_rec[i];
	      this->resize(this->dim_size_rec - 1, (ix2 / blk) + 1);
	    }
	    return ix0 + this->cached_col_length_rec * ix1
		       + this->cached_col_row_length_rec * ix2;
	  }
	}
      }
    }
    else {					/* auto resize OFF */
	return this->get_idx_3d_cs(ix0,ix1,ix2);
    }
}

/** 
 * @brief  指定された要素の位置を返す (リサイズなし)
 *
 *  配列の範囲外の要素を指定してもリサイズは行わない．
 *
 * @param   ix0 1次元目の要素番号
 * @param   ix1 2次元目の要素番号
 * @param   ix2 3次元目の要素番号
 * @return  成功した場合は位置番号<br>
 *          引数が正しくないときは-1 
 * @note    このメンバ関数は protected です．
 */
inline ssize_t mdarray::get_idx_3d_cs( ssize_t ix0, ssize_t ix1, ssize_t ix2 ) const
{
    return
    ( /* if */ (ix1 == MDARRAY_INDEF) ?
      /* 1-dim */
      ( /* if */ (ix0 < 0 || (ssize_t)(this->cached_length_rec) <= ix0) ?
	-1 : ix0
      )
      :
      /* 2-dim or 3-dim */
      ( /* if */ (ix2 == MDARRAY_INDEF) ?
	/* 2-dim */
	( /* if */ (ix0 < 0 || (ssize_t)(this->cached_col_length_rec) <= ix0) ? 
	  -1
	  :
	  ( /* if */ (ix1 < 0 || (ssize_t)(this->cached_row_layer_length_rec) <= ix1) ?
	    -1
	    :
	    (ix0 + (ssize_t)(this->cached_col_length_rec) * ix1)
	  )
	)
	:
	/* 3-dim */
	( /* if */ (ix0 < 0 || (ssize_t)(this->cached_col_length_rec) <= ix0) ?
	  -1
	  :
	  ( /* if */ (ix1 < 0 || (ssize_t)(this->cached_row_length_rec) <= ix1) ?
	    -1
	    :
	    ( /* if */ (ix2 < 0 || (ssize_t)(this->cached_layer_length_rec) <= ix2) ?
	      -1
	      :
	      (ix0 + (ssize_t)(this->cached_col_length_rec) * ix1
		   + (ssize_t)(this->cached_col_row_length_rec) * ix2)
	    )
	  )
	)
      )
    );
}

/*
 * private member functions 
 */

/** 
 * @brief  default値の private 用アクセッサー
 * @note   このメンバ関数は private です．
 */
inline void *mdarray::default_rec_ptr()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    return this->_default_rec;
}

/** 
 * @brief  次元長の private 用アクセッサー
 * @note   このメンバ関数は private です．
 */
inline size_t *mdarray::size_rec_ptr()
{
    this->cleanup_shallow_copy(true);	/* バッファを直接操作するため呼ぶ */

    return this->_size_rec;
}


/**
 * @example  examples_sllib/array_digest.cc
 *           多次元配列の様々な機能のデモ・コード
 */

/**
 * @example  examples_sllib/array_basic.cc
 *           多次元配列の扱い方の基本を示したコード
 */

/**
 * @example  examples_sllib/array_edit.cc
 *           多次元配列の編集の例
 */

/**
 * @example  examples_sllib/array_fast_access.cc
 *           3次元配列までの要素にポインタ変数で高速にアクセスする例
 */

/**
 * @example  examples_sllib/array_idl.cc
 *           多次元配列を IDL 風の記法で部分選択や演算を行なう例
 */

/**
 * @example  examples_sllib/array_math.cc
 *           多次元配列に対する数学関数を使った例
 */


}


/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ(実数・複素数対応版)
 *
 * 本マクロを利用することで，全ての型変換のパターンを網羅したマクロ式の一覧に
 * 展開することができる．本マクロの用途は以下の２種類に区別される．
 *
 *  1. SEL_FUNC
 *    条件に合致したいずれかの MACRO_TO_DO のみが適用される形で，マクロが展開
 *    される．
 *    els には必ず else が指定されなければならず，MACRO_TO_DO は必ず if で
 *    始まる評価式の形をとらなければならない．このとき，
 *      if MACRO( ... ) else if MACRO( ... ) ...
 *    このように展開される．
 *
 *  2. MAKE_FUNC
 *    全てのパターンで MACRO_TO_DO を展開する．
 *    els には何も指定してはならない．このとき，
 *      MACRO( ... ) MACRO( ... ) ...
 *    このように展開される．
 *
 * @param  MACRO_TO_DO 展開するマクロ名(SEL_FUNC/MAKE_FUNC)
 * @param  f2c               float 型から  8ビット 整数型に変換するための関数名
 * @param  f2i16             float 型から 16ビット 整数型に変換するための関数名
 * @param  f2i32             float 型から 32ビット 整数型に変換するための関数名
 * @param  f2i64             float 型から 64ビット 整数型に変換するための関数名
 * @param  d2c              double 型から  8ビット 整数型に変換するための関数名
 * @param  d2i16            double 型から 16ビット 整数型に変換するための関数名
 * @param  d2i32            double 型から 32ビット 整数型に変換するための関数名
 * @param  d2i64            double 型から 64ビット 整数型に変換するための関数名
 * @param  ld2c        long double 型から  8ビット 整数型に変換するための関数名
 * @param  ld2i16      long double 型から 16ビット 整数型に変換するための関数名
 * @param  ld2i32      long double 型から 32ビット 整数型に変換するための関数名
 * @param  ld2i64      long double 型から 64ビット 整数型に変換するための関数名
 * @param  fx2c           fcomplex 型から  8ビット 整数型に変換するための関数名
 * @param  fx2i16         fcomplex 型から 16ビット 整数型に変換するための関数名
 * @param  fx2i32         fcomplex 型から 32ビット 整数型に変換するための関数名
 * @param  fx2i64         fcomplex 型から 64ビット 整数型に変換するための関数名
 * @param  dx2c           dcomplex 型から  8ビット 整数型に変換するための関数名
 * @param  dx2i16         dcomplex 型から 16ビット 整数型に変換するための関数名
 * @param  dx2i32         dcomplex 型から 32ビット 整数型に変換するための関数名
 * @param  dx2i64         dcomplex 型から 64ビット 整数型に変換するための関数名
 * @param  ldx2c         ldcomplex 型から  8ビット 整数型に変換するための関数名
 * @param  ldx2i16       ldcomplex 型から 16ビット 整数型に変換するための関数名
 * @param  ldx2i32       ldcomplex 型から 32ビット 整数型に変換するための関数名
 * @param  ldx2i64       ldcomplex 型から 64ビット 整数型に変換するための関数名
 * @param  els         MACRO_TO_DO が if else 文を求める場合，else を設定する．
 */
#define SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(MACRO_TO_DO,f2c,f2i16,f2i32,f2i64,d2c,d2i16,d2i32,d2i64,ld2c,ld2i16,ld2i32,ld2i64,fx2c,fx2i16,fx2i32,fx2i64,dx2c,dx2i16,dx2i32,dx2i64,ldx2c,ldx2i16,ldx2i32,ldx2i64,f2fx,d2dx,ld2ldx,els) \
  /* */ \
      MACRO_TO_DO(sli__md_double_to_double,   DOUBLE_ZT,double,   DOUBLE_ZT,     double,     ) \
  els MACRO_TO_DO(sli__md_double_to_float,    DOUBLE_ZT,double,    FLOAT_ZT,      float,     ) \
  els MACRO_TO_DO(sli__md_double_to_int16,    DOUBLE_ZT,double,    INT16_ZT,    int16_t,d2i16) \
  els MACRO_TO_DO(sli__md_double_to_int32,    DOUBLE_ZT,double,    INT32_ZT,    int32_t,d2i32) \
  els MACRO_TO_DO(sli__md_double_to_uint8,    DOUBLE_ZT,double,    UCHAR_ZT,    uint8_t,  d2c) \
  els MACRO_TO_DO(sli__md_double_to_int64,    DOUBLE_ZT,double,    INT64_ZT,    int64_t,d2i64) \
  els MACRO_TO_DO(sli__md_double_to_ldouble,  DOUBLE_ZT,double,  LDOUBLE_ZT,long double,     ) \
  els MACRO_TO_DO(sli__md_double_to_fcomplex, DOUBLE_ZT,double, FCOMPLEX_ZT,   fcomplex, f2fx) \
  els MACRO_TO_DO(sli__md_double_to_dcomplex, DOUBLE_ZT,double, DCOMPLEX_ZT,   dcomplex, d2dx) \
  els MACRO_TO_DO(sli__md_double_to_ldcomplex,DOUBLE_ZT,double,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_float_to_double,   FLOAT_ZT,float,   DOUBLE_ZT,     double,     ) \
  els MACRO_TO_DO(sli__md_float_to_float,    FLOAT_ZT,float,    FLOAT_ZT,      float,     ) \
  els MACRO_TO_DO(sli__md_float_to_int16,    FLOAT_ZT,float,    INT16_ZT,    int16_t,f2i16) \
  els MACRO_TO_DO(sli__md_float_to_int32,    FLOAT_ZT,float,    INT32_ZT,    int32_t,f2i32) \
  els MACRO_TO_DO(sli__md_float_to_uint8,    FLOAT_ZT,float,    UCHAR_ZT,    uint8_t,  f2c) \
  els MACRO_TO_DO(sli__md_float_to_int64,    FLOAT_ZT,float,    INT64_ZT,    int64_t,f2i64) \
  els MACRO_TO_DO(sli__md_float_to_ldouble,  FLOAT_ZT,float,  LDOUBLE_ZT,long double,     ) \
  els MACRO_TO_DO(sli__md_float_to_fcomplex, FLOAT_ZT,float, FCOMPLEX_ZT,   fcomplex, f2fx) \
  els MACRO_TO_DO(sli__md_float_to_dcomplex, FLOAT_ZT,float, DCOMPLEX_ZT,   dcomplex, d2dx) \
  els MACRO_TO_DO(sli__md_float_to_ldcomplex,FLOAT_ZT,float,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_int16_to_double,   INT16_ZT,int16_t,   DOUBLE_ZT,     double,) \
  els MACRO_TO_DO(sli__md_int16_to_float,    INT16_ZT,int16_t,    FLOAT_ZT,      float,) \
  els MACRO_TO_DO(sli__md_int16_to_int16,    INT16_ZT,int16_t,    INT16_ZT,    int16_t,) \
  els MACRO_TO_DO(sli__md_int16_to_int32,    INT16_ZT,int16_t,    INT32_ZT,    int32_t,) \
  els MACRO_TO_DO(sli__md_int16_to_uint8,    INT16_ZT,int16_t,    UCHAR_ZT,    uint8_t,) \
  els MACRO_TO_DO(sli__md_int16_to_int64,    INT16_ZT,int16_t,    INT64_ZT,    int64_t,) \
  els MACRO_TO_DO(sli__md_int16_to_ldouble,  INT16_ZT,int16_t,  LDOUBLE_ZT,long double,) \
  els MACRO_TO_DO(sli__md_int16_to_fcomplex, INT16_ZT,int16_t, FCOMPLEX_ZT,   fcomplex,  f2fx) \
  els MACRO_TO_DO(sli__md_int16_to_dcomplex, INT16_ZT,int16_t, DCOMPLEX_ZT,   dcomplex,  d2dx) \
  els MACRO_TO_DO(sli__md_int16_to_ldcomplex,INT16_ZT,int16_t,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_int32_to_double,   INT32_ZT,int32_t,   DOUBLE_ZT,     double,) \
  els MACRO_TO_DO(sli__md_int32_to_float,    INT32_ZT,int32_t,    FLOAT_ZT,      float,) \
  els MACRO_TO_DO(sli__md_int32_to_int16,    INT32_ZT,int32_t,    INT16_ZT,    int16_t,) \
  els MACRO_TO_DO(sli__md_int32_to_int32,    INT32_ZT,int32_t,    INT32_ZT,    int32_t,) \
  els MACRO_TO_DO(sli__md_int32_to_uint8,    INT32_ZT,int32_t,    UCHAR_ZT,    uint8_t,) \
  els MACRO_TO_DO(sli__md_int32_to_int64,    INT32_ZT,int32_t,    INT64_ZT,    int64_t,) \
  els MACRO_TO_DO(sli__md_int32_to_ldouble,  INT32_ZT,int32_t,  LDOUBLE_ZT,long double,) \
  els MACRO_TO_DO(sli__md_int32_to_fcomplex, INT32_ZT,int32_t, FCOMPLEX_ZT,   fcomplex,  f2fx) \
  els MACRO_TO_DO(sli__md_int32_to_dcomplex, INT32_ZT,int32_t, DCOMPLEX_ZT,   dcomplex,  d2dx) \
  els MACRO_TO_DO(sli__md_int32_to_ldcomplex,INT32_ZT,int32_t,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_uint8_to_double,   UCHAR_ZT,uint8_t,   DOUBLE_ZT,     double,) \
  els MACRO_TO_DO(sli__md_uint8_to_float,    UCHAR_ZT,uint8_t,    FLOAT_ZT,      float,) \
  els MACRO_TO_DO(sli__md_uint8_to_int16,    UCHAR_ZT,uint8_t,    INT16_ZT,    int16_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_int32,    UCHAR_ZT,uint8_t,    INT32_ZT,    int32_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_uint8,    UCHAR_ZT,uint8_t,    UCHAR_ZT,    uint8_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_int64,    UCHAR_ZT,uint8_t,    INT64_ZT,    int64_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_ldouble,  UCHAR_ZT,uint8_t,  LDOUBLE_ZT,long double,) \
  els MACRO_TO_DO(sli__md_uint8_to_fcomplex, UCHAR_ZT,uint8_t, FCOMPLEX_ZT,   fcomplex,  f2fx) \
  els MACRO_TO_DO(sli__md_uint8_to_dcomplex, UCHAR_ZT,uint8_t, DCOMPLEX_ZT,   dcomplex,  d2dx) \
  els MACRO_TO_DO(sli__md_uint8_to_ldcomplex,UCHAR_ZT,uint8_t,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_int64_to_double,   INT64_ZT,int64_t,   DOUBLE_ZT,     double,) \
  els MACRO_TO_DO(sli__md_int64_to_float,    INT64_ZT,int64_t,    FLOAT_ZT,      float,) \
  els MACRO_TO_DO(sli__md_int64_to_int16,    INT64_ZT,int64_t,    INT16_ZT,    int16_t,) \
  els MACRO_TO_DO(sli__md_int64_to_int32,    INT64_ZT,int64_t,    INT32_ZT,    int32_t,) \
  els MACRO_TO_DO(sli__md_int64_to_uint8,    INT64_ZT,int64_t,    UCHAR_ZT,    uint8_t,) \
  els MACRO_TO_DO(sli__md_int64_to_int64,    INT64_ZT,int64_t,    INT64_ZT,    int64_t,) \
  els MACRO_TO_DO(sli__md_int64_to_ldouble,  INT64_ZT,int64_t,  LDOUBLE_ZT,long double,) \
  els MACRO_TO_DO(sli__md_int64_to_fcomplex, INT64_ZT,int64_t, FCOMPLEX_ZT,   fcomplex,  f2fx) \
  els MACRO_TO_DO(sli__md_int64_to_dcomplex, INT64_ZT,int64_t, DCOMPLEX_ZT,   dcomplex,  d2dx) \
  els MACRO_TO_DO(sli__md_int64_to_ldcomplex,INT64_ZT,int64_t,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_ldouble_to_double,   LDOUBLE_ZT,long double,   DOUBLE_ZT,     double,      ) \
  els MACRO_TO_DO(sli__md_ldouble_to_float,    LDOUBLE_ZT,long double,    FLOAT_ZT,      float,      ) \
  els MACRO_TO_DO(sli__md_ldouble_to_int16,    LDOUBLE_ZT,long double,    INT16_ZT,    int16_t,ld2i16) \
  els MACRO_TO_DO(sli__md_ldouble_to_int32,    LDOUBLE_ZT,long double,    INT32_ZT,    int32_t,ld2i32) \
  els MACRO_TO_DO(sli__md_ldouble_to_uint8,    LDOUBLE_ZT,long double,    UCHAR_ZT,    uint8_t,  ld2c) \
  els MACRO_TO_DO(sli__md_ldouble_to_int64,    LDOUBLE_ZT,long double,    INT64_ZT,    int64_t,ld2i64) \
  els MACRO_TO_DO(sli__md_ldouble_to_ldouble,  LDOUBLE_ZT,long double,  LDOUBLE_ZT,long double,      ) \
  els MACRO_TO_DO(sli__md_ldouble_to_fcomplex, LDOUBLE_ZT,long double, FCOMPLEX_ZT,   fcomplex,  f2fx) \
  els MACRO_TO_DO(sli__md_ldouble_to_dcomplex, LDOUBLE_ZT,long double, DCOMPLEX_ZT,   dcomplex,  d2dx) \
  els MACRO_TO_DO(sli__md_ldouble_to_ldcomplex,LDOUBLE_ZT,long double,LDCOMPLEX_ZT,  ldcomplex,ld2ldx) \
  /* */ \
  els MACRO_TO_DO(sli__md_fcomplex_to_double,   FCOMPLEX_ZT,fcomplex,   DOUBLE_ZT,     double, creal) \
  els MACRO_TO_DO(sli__md_fcomplex_to_float,    FCOMPLEX_ZT,fcomplex,    FLOAT_ZT,      float, creal) \
  els MACRO_TO_DO(sli__md_fcomplex_to_int16,    FCOMPLEX_ZT,fcomplex,    INT16_ZT,    int16_t,fx2i16) \
  els MACRO_TO_DO(sli__md_fcomplex_to_int32,    FCOMPLEX_ZT,fcomplex,    INT32_ZT,    int32_t,fx2i32) \
  els MACRO_TO_DO(sli__md_fcomplex_to_uint8,    FCOMPLEX_ZT,fcomplex,    UCHAR_ZT,    uint8_t,  fx2c) \
  els MACRO_TO_DO(sli__md_fcomplex_to_int64,    FCOMPLEX_ZT,fcomplex,    INT64_ZT,    int64_t,fx2i64) \
  els MACRO_TO_DO(sli__md_fcomplex_to_ldouble,  FCOMPLEX_ZT,fcomplex,  LDOUBLE_ZT,long double, creal) \
  els MACRO_TO_DO(sli__md_fcomplex_to_fcomplex, FCOMPLEX_ZT,fcomplex, FCOMPLEX_ZT,   fcomplex,      ) \
  els MACRO_TO_DO(sli__md_fcomplex_to_dcomplex, FCOMPLEX_ZT,fcomplex, DCOMPLEX_ZT,   dcomplex,      ) \
  els MACRO_TO_DO(sli__md_fcomplex_to_ldcomplex,FCOMPLEX_ZT,fcomplex,LDCOMPLEX_ZT,  ldcomplex,      ) \
  /* */ \
  els MACRO_TO_DO(sli__md_dcomplex_to_double,   DCOMPLEX_ZT,dcomplex,   DOUBLE_ZT,     double, creal) \
  els MACRO_TO_DO(sli__md_dcomplex_to_float,    DCOMPLEX_ZT,dcomplex,    FLOAT_ZT,      float, creal) \
  els MACRO_TO_DO(sli__md_dcomplex_to_int16,    DCOMPLEX_ZT,dcomplex,    INT16_ZT,    int16_t,dx2i16) \
  els MACRO_TO_DO(sli__md_dcomplex_to_int32,    DCOMPLEX_ZT,dcomplex,    INT32_ZT,    int32_t,dx2i32) \
  els MACRO_TO_DO(sli__md_dcomplex_to_uint8,    DCOMPLEX_ZT,dcomplex,    UCHAR_ZT,    uint8_t,  dx2c) \
  els MACRO_TO_DO(sli__md_dcomplex_to_int64,    DCOMPLEX_ZT,dcomplex,    INT64_ZT,    int64_t,dx2i64) \
  els MACRO_TO_DO(sli__md_dcomplex_to_ldouble,  DCOMPLEX_ZT,dcomplex,  LDOUBLE_ZT,long double, creal) \
  els MACRO_TO_DO(sli__md_dcomplex_to_fcomplex, DCOMPLEX_ZT,dcomplex, FCOMPLEX_ZT,   fcomplex,      ) \
  els MACRO_TO_DO(sli__md_dcomplex_to_dcomplex, DCOMPLEX_ZT,dcomplex, DCOMPLEX_ZT,   dcomplex,      ) \
  els MACRO_TO_DO(sli__md_dcomplex_to_ldcomplex,DCOMPLEX_ZT,dcomplex,LDCOMPLEX_ZT,  ldcomplex,      ) \
  /* */ \
  els MACRO_TO_DO(sli__md_ldcomplex_to_double,   LDCOMPLEX_ZT,ldcomplex,   DOUBLE_ZT,     double,  creal) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_float,    LDCOMPLEX_ZT,ldcomplex,    FLOAT_ZT,      float,  creal) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_int16,    LDCOMPLEX_ZT,ldcomplex,    INT16_ZT,    int16_t,ldx2i16) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_int32,    LDCOMPLEX_ZT,ldcomplex,    INT32_ZT,    int32_t,ldx2i32) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_uint8,    LDCOMPLEX_ZT,ldcomplex,    UCHAR_ZT,    uint8_t,  ldx2c) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_int64,    LDCOMPLEX_ZT,ldcomplex,    INT64_ZT,    int64_t,ldx2i64) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_ldouble,  LDCOMPLEX_ZT,ldcomplex,  LDOUBLE_ZT,long double,  creal) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_fcomplex, LDCOMPLEX_ZT,ldcomplex, FCOMPLEX_ZT,   fcomplex,       ) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_dcomplex, LDCOMPLEX_ZT,ldcomplex, DCOMPLEX_ZT,   dcomplex,       ) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_ldcomplex,LDCOMPLEX_ZT,ldcomplex,LDCOMPLEX_ZT,  ldcomplex,       )

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ(実数のみ)
 *
 */
#define SLI__MDARRAY__DO_OPERATION_2TYPES(MACRO_TO_DO,f2c,f2i16,f2i32,f2i64,d2c,d2i16,d2i32,d2i64,ld2c,ld2i16,ld2i32,ld2i64,els) \
  /* */ \
      MACRO_TO_DO(sli__md_double_to_double, DOUBLE_ZT,double,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_double_to_float,  DOUBLE_ZT,double,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_double_to_int16,  DOUBLE_ZT,double, INT16_ZT,int16_t,d2i16) \
  els MACRO_TO_DO(sli__md_double_to_int32,  DOUBLE_ZT,double, INT32_ZT,int32_t,d2i32) \
  els MACRO_TO_DO(sli__md_double_to_uint8,  DOUBLE_ZT,double, UCHAR_ZT,uint8_t,d2c) \
  els MACRO_TO_DO(sli__md_double_to_int64,  DOUBLE_ZT,double, INT64_ZT,int64_t,d2i64) \
  els MACRO_TO_DO(sli__md_double_to_ldouble,DOUBLE_ZT,double,LDOUBLE_ZT,long double,) \
  /* */ \
  els MACRO_TO_DO(sli__md_float_to_double, FLOAT_ZT,float,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_float_to_float,  FLOAT_ZT,float,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_float_to_int16,  FLOAT_ZT,float, INT16_ZT,int16_t,f2i16) \
  els MACRO_TO_DO(sli__md_float_to_int32,  FLOAT_ZT,float, INT32_ZT,int32_t,f2i32) \
  els MACRO_TO_DO(sli__md_float_to_uint8,  FLOAT_ZT,float, UCHAR_ZT,uint8_t,f2c) \
  els MACRO_TO_DO(sli__md_float_to_int64,  FLOAT_ZT,float, INT64_ZT,int64_t,f2i64) \
  els MACRO_TO_DO(sli__md_float_to_ldouble,FLOAT_ZT,float,LDOUBLE_ZT,long double,) \
  /* */ \
  els MACRO_TO_DO(sli__md_int16_to_double, INT16_ZT,int16_t,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_int16_to_float,  INT16_ZT,int16_t,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_int16_to_int16,  INT16_ZT,int16_t, INT16_ZT,int16_t,) \
  els MACRO_TO_DO(sli__md_int16_to_int32,  INT16_ZT,int16_t, INT32_ZT,int32_t,) \
  els MACRO_TO_DO(sli__md_int16_to_uint8,  INT16_ZT,int16_t, UCHAR_ZT,uint8_t,) \
  els MACRO_TO_DO(sli__md_int16_to_int64,  INT16_ZT,int16_t, INT64_ZT,int64_t,) \
  els MACRO_TO_DO(sli__md_int16_to_ldouble,INT16_ZT,int16_t,LDOUBLE_ZT,long double,) \
  /* */ \
  els MACRO_TO_DO(sli__md_int32_to_double, INT32_ZT,int32_t,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_int32_to_float,  INT32_ZT,int32_t,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_int32_to_int16,  INT32_ZT,int32_t, INT16_ZT,int16_t,) \
  els MACRO_TO_DO(sli__md_int32_to_int32,  INT32_ZT,int32_t, INT32_ZT,int32_t,) \
  els MACRO_TO_DO(sli__md_int32_to_uint8,  INT32_ZT,int32_t, UCHAR_ZT,uint8_t,) \
  els MACRO_TO_DO(sli__md_int32_to_int64,  INT32_ZT,int32_t, INT64_ZT,int64_t,) \
  els MACRO_TO_DO(sli__md_int32_to_ldouble,INT32_ZT,int32_t,LDOUBLE_ZT,long double,) \
  /* */ \
  els MACRO_TO_DO(sli__md_uint8_to_double, UCHAR_ZT,uint8_t,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_uint8_to_float,  UCHAR_ZT,uint8_t,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_uint8_to_int16,  UCHAR_ZT,uint8_t, INT16_ZT,int16_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_int32,  UCHAR_ZT,uint8_t, INT32_ZT,int32_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_uint8,  UCHAR_ZT,uint8_t, UCHAR_ZT,uint8_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_int64,  UCHAR_ZT,uint8_t, INT64_ZT,int64_t,) \
  els MACRO_TO_DO(sli__md_uint8_to_ldouble,UCHAR_ZT,uint8_t,LDOUBLE_ZT,long double,) \
  /* */ \
  els MACRO_TO_DO(sli__md_int64_to_double, INT64_ZT,int64_t,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_int64_to_float,  INT64_ZT,int64_t,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_int64_to_int16,  INT64_ZT,int64_t, INT16_ZT,int16_t,) \
  els MACRO_TO_DO(sli__md_int64_to_int32,  INT64_ZT,int64_t, INT32_ZT,int32_t,) \
  els MACRO_TO_DO(sli__md_int64_to_uint8,  INT64_ZT,int64_t, UCHAR_ZT,uint8_t,) \
  els MACRO_TO_DO(sli__md_int64_to_int64,  INT64_ZT,int64_t, INT64_ZT,int64_t,) \
  els MACRO_TO_DO(sli__md_int64_to_ldouble,INT64_ZT,int64_t,LDOUBLE_ZT,long double,) \
  /* */ \
  els MACRO_TO_DO(sli__md_ldouble_to_double,LDOUBLE_ZT,long double,DOUBLE_ZT,double,) \
  els MACRO_TO_DO(sli__md_ldouble_to_float, LDOUBLE_ZT,long double,FLOAT_ZT,float,) \
  els MACRO_TO_DO(sli__md_ldouble_to_int16, LDOUBLE_ZT,long double, INT16_ZT,int16_t,ld2i16) \
  els MACRO_TO_DO(sli__md_ldouble_to_int32, LDOUBLE_ZT,long double, INT32_ZT,int32_t,ld2i32) \
  els MACRO_TO_DO(sli__md_ldouble_to_uint8, LDOUBLE_ZT,long double, UCHAR_ZT,uint8_t,ld2c) \
  els MACRO_TO_DO(sli__md_ldouble_to_int64, LDOUBLE_ZT,long double, INT64_ZT,int64_t,ld2i64) \
  els MACRO_TO_DO(sli__md_ldouble_to_ldouble,LDOUBLE_ZT,long double,LDOUBLE_ZT,long double,)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ(複素数のみ)
 *
 */
#define SLI__MDARRAY__DO_OPERATION_2TYPES_COMPLEX(MACRO_TO_DO,els) \
  /* */ \
      MACRO_TO_DO(sli__md_fcomplex_to_fcomplex, FCOMPLEX_ZT,fcomplex, FCOMPLEX_ZT,   fcomplex) \
  els MACRO_TO_DO(sli__md_fcomplex_to_dcomplex, FCOMPLEX_ZT,fcomplex, DCOMPLEX_ZT,   dcomplex) \
  els MACRO_TO_DO(sli__md_fcomplex_to_ldcomplex,FCOMPLEX_ZT,fcomplex,LDCOMPLEX_ZT,  ldcomplex) \
  /* */ \
  els MACRO_TO_DO(sli__md_dcomplex_to_fcomplex, DCOMPLEX_ZT,dcomplex, FCOMPLEX_ZT,   fcomplex) \
  els MACRO_TO_DO(sli__md_dcomplex_to_dcomplex, DCOMPLEX_ZT,dcomplex, DCOMPLEX_ZT,   dcomplex) \
  els MACRO_TO_DO(sli__md_dcomplex_to_ldcomplex,DCOMPLEX_ZT,dcomplex,LDCOMPLEX_ZT,  ldcomplex) \
  /* */ \
  els MACRO_TO_DO(sli__md_ldcomplex_to_fcomplex, LDCOMPLEX_ZT,ldcomplex, FCOMPLEX_ZT,   fcomplex) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_dcomplex, LDCOMPLEX_ZT,ldcomplex, DCOMPLEX_ZT,   dcomplex) \
  els MACRO_TO_DO(sli__md_ldcomplex_to_ldcomplex,LDCOMPLEX_ZT,ldcomplex,LDCOMPLEX_ZT,  ldcomplex)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ(1種類の型のみの場合)
 *
 */
#define SLI__MDARRAY__DO_OPERATION_1TYPE_ALL(MACRO_TO_DO,els) \
      MACRO_TO_DO(sli__md_double,DOUBLE_ZT,double) \
  els MACRO_TO_DO(sli__md_float,FLOAT_ZT,float) \
  els MACRO_TO_DO(sli__md_int16,INT16_ZT,int16_t) \
  els MACRO_TO_DO(sli__md_int32,INT32_ZT,int32_t) \
  els MACRO_TO_DO(sli__md_uchar,UCHAR_ZT,uint8_t) \
  els MACRO_TO_DO(sli__md_int64,INT64_ZT,int64_t) \
  els MACRO_TO_DO(sli__md_ldouble,LDOUBLE_ZT,long double) \
  els MACRO_TO_DO(sli__md_fcomplex,FCOMPLEX_ZT,fcomplex) \
  els MACRO_TO_DO(sli__md_dcomplex,DCOMPLEX_ZT,dcomplex) \
  els MACRO_TO_DO(sli__md_ldcomplex,LDCOMPLEX_ZT,ldcomplex)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ(複素数で1種類の型のみの場合)
 *
 */
#define SLI__MDARRAY__DO_OPERATION_1TYPE_COMPLEX(MACRO_TO_DO,els) \
      MACRO_TO_DO(sli__md_fcomplex,FCOMPLEX_ZT,fcomplex) \
  els MACRO_TO_DO(sli__md_dcomplex,DCOMPLEX_ZT,dcomplex) \
  els MACRO_TO_DO(sli__md_ldcomplex,LDCOMPLEX_ZT,ldcomplex)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ
 * (double型→何かの型の場合．逆変換は最後の引数でサポート．
 *
 */
#define SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY_ALL(MACRO_TO_DO,d2c,d2i16,d2i32,d2i64,f2fx,d2dx,ld2ldx,els) \
      MACRO_TO_DO(sli__md_double_to_double,    DOUBLE_ZT,    double,           ,) \
  els MACRO_TO_DO(sli__md_double_to_float,     FLOAT_ZT,     float,            ,) \
  els MACRO_TO_DO(sli__md_double_to_int16,     INT16_ZT,     int16_t,    d2i16 ,) \
  els MACRO_TO_DO(sli__md_double_to_int32,     INT32_ZT,     int32_t,    d2i32 ,) \
  els MACRO_TO_DO(sli__md_double_to_uint8,     UCHAR_ZT,     uint8_t,    d2c   ,) \
  els MACRO_TO_DO(sli__md_double_to_int64,     INT64_ZT,     int64_t,    d2i64 ,) \
  els MACRO_TO_DO(sli__md_double_to_ldouble,   LDOUBLE_ZT,   long double,      ,) \
  els MACRO_TO_DO(sli__md_double_to_fcomplex,  FCOMPLEX_ZT,  fcomplex,   f2fx  ,creal) \
  els MACRO_TO_DO(sli__md_double_to_dcomplex,  DCOMPLEX_ZT,  dcomplex,   d2dx  ,creal) \
  els MACRO_TO_DO(sli__md_double_to_ldcomplex, LDCOMPLEX_ZT, ldcomplex,  ld2ldx,creal)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ
 * (double型→何かの型(not複素数)の場合．逆変換は最後の引数でサポート．
 *
 */
#define SLI__MDARRAY__DO_OPERATION_DOUBLE2ANY(MACRO_TO_DO,d2c,d2i16,d2i32,d2i64,els) \
      MACRO_TO_DO(sli__md_double_to_double,    DOUBLE_ZT,    double,          ) \
  els MACRO_TO_DO(sli__md_double_to_float,     FLOAT_ZT,     float,           ) \
  els MACRO_TO_DO(sli__md_double_to_int16,     INT16_ZT,     int16_t,    d2i16) \
  els MACRO_TO_DO(sli__md_double_to_int32,     INT32_ZT,     int32_t,    d2i32) \
  els MACRO_TO_DO(sli__md_double_to_uint8,     UCHAR_ZT,     uint8_t,    d2c  ) \
  els MACRO_TO_DO(sli__md_double_to_int64,     INT64_ZT,     int64_t,    d2i64) \
  els MACRO_TO_DO(sli__md_double_to_ldouble,   LDOUBLE_ZT,   long double,     )

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ
 * (float型→何かの型の場合．逆変換は最後の引数でサポート．
 *
 */
#define SLI__MDARRAY__DO_OPERATION_FLOAT2ANY_ALL(MACRO_TO_DO,f2c,f2i16,f2i32,f2i64,f2fx,d2dx,ld2ldx,els) \
      MACRO_TO_DO(sli__md_float_to_double,    DOUBLE_ZT,    double,           ,) \
  els MACRO_TO_DO(sli__md_float_to_float,     FLOAT_ZT,     float,            ,) \
  els MACRO_TO_DO(sli__md_float_to_int16,     INT16_ZT,     int16_t,    f2i16 ,) \
  els MACRO_TO_DO(sli__md_float_to_int32,     INT32_ZT,     int32_t,    f2i32 ,) \
  els MACRO_TO_DO(sli__md_float_to_uint8,     UCHAR_ZT,     uint8_t,    f2c   ,) \
  els MACRO_TO_DO(sli__md_float_to_int64,     INT64_ZT,     int64_t,    f2i64 ,) \
  els MACRO_TO_DO(sli__md_float_to_ldouble,   LDOUBLE_ZT,   long double,      ,) \
  els MACRO_TO_DO(sli__md_float_to_fcomplex,  FCOMPLEX_ZT,  fcomplex,   f2fx  ,creal) \
  els MACRO_TO_DO(sli__md_float_to_dcomplex,  DCOMPLEX_ZT,  dcomplex,   d2dx  ,creal) \
  els MACRO_TO_DO(sli__md_float_to_ldcomplex, LDCOMPLEX_ZT, ldcomplex,  ld2ldx,creal)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC 一括展開用マクロ
 * (float型→何かの型(not複素数)の場合．逆変換は最後の引数でサポート．
 *
 */
#define SLI__MDARRAY__DO_OPERATION_FLOAT2ANY(MACRO_TO_DO,f2c,f2i16,f2i32,f2i64,els) \
      MACRO_TO_DO(sli__md_float_to_double,    DOUBLE_ZT,    double,          ) \
  els MACRO_TO_DO(sli__md_float_to_float,     FLOAT_ZT,     float,           ) \
  els MACRO_TO_DO(sli__md_float_to_int16,     INT16_ZT,     int16_t,    f2i16) \
  els MACRO_TO_DO(sli__md_float_to_int32,     INT32_ZT,     int32_t,    f2i32) \
  els MACRO_TO_DO(sli__md_float_to_uint8,     UCHAR_ZT,     uint8_t,    f2c  ) \
  els MACRO_TO_DO(sli__md_float_to_int64,     INT64_ZT,     int64_t,    f2i64) \
  els MACRO_TO_DO(sli__md_float_to_ldouble,   LDOUBLE_ZT,   long double,     )


#if 1
/* */
#include "mdarray_float.h"
#include "mdarray_double.h"
#include "mdarray_uchar.h"
#include "mdarray_short.h"
#include "mdarray_int.h"
#include "mdarray_long.h"
#include "mdarray_llong.h"
#include "mdarray_int16.h"
#include "mdarray_int32.h"
#include "mdarray_int64.h"
#include "mdarray_size.h"
#include "mdarray_ssize.h"
#include "mdarray_bool.h"
#include "mdarray_uintptr.h"
/* */
/* #include "mdarray_math.h" */
#endif

#endif	/* _SLI__MDARRAY_H */
