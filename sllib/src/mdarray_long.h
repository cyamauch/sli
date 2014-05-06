/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:54:07 cyamauch> */
 
#ifndef _SLI__MDARRAY_LONG_H
#define _SLI__MDARRAY_LONG_H 1

/**
 * @file   mdarray_long.h
 * @brief  long����¿��������򰷤� mdarray_long ���饹�Υإå��ե�����
 */

#include "sli_config.h"
#include "mdarray.h"
#if 0
#include "complex.h"
#endif

namespace sli
{

/*
 * sli::mdarray_long is an inherited class of sli::mdarray, and is a 
 * high-level class of SLLIB.  See also mdarray.h for all member functions.
 * 
 * This class can handle multidimensional arrays, and provide a lot of useful
 * member functions such as mathematical operations, image processings, etc.
 */

/**
 * @class  sli::mdarray_long
 * @brief  long����¿��������򰷤�����Υ��饹
 *
 *   mdarray_long���饹�ϡ�long����¿��������� IDL �� Python �Τ褦��
 *   ��ڤ˰�����褦�ˤ��ޤ��������Τ褦�ʵ�ǽ������ޤ���<br>
 *    - �黻�����ˤ������ȥ����顼������Ȥα黻 <br>
 *    - �����������ǿ���ưŪ���ѹ� <br>
 *    - IDL/IRAF����ɽ�� (��: "0:99,*") �ˤ��������Խ����黻 <br>
 *    - 2D��3D�ǡ����Υݥ�������μ�ư���� <br>
 *    - y������z�����ؤι�®�������Τ���ι�®transpose��ǽ <br>
 *    - ������Ф�����شؿ� (mdarray_math.h) <br>
 *    - �����Ѵؿ� (moment��median��etc.; mdarray_statistics.h) <br>
 *    - C99����ʣ�Ǵؿ� (complex.h) �� <br>
 *   ����μ������˴ط��ʤ������Ͼ��1�����Хåե��ǡ����ܻ��Ȥ��Ф��Ƥϴˤ�
 *   ������API�Ǥ��Τǡ������C�ǽ񤫤줿���إ饤�֥��Ȥ�ʻ�Ѥ��ñ�Ǥ���<br>
 *   ����η����̤�ưŪ���ѹ���������硤���쥯�饹��mdarray�򤪻Ȥ�����������
 * 
 * @note  mdarray_long���饹�� mdarray(���쥯�饹) ��Ѿ����Ƥ��ޤ���<br>
 *        SIMD̿��(SSE2)�ˤ�ꡤ����Ū����ʬ����®������Ƥ��ޤ���
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class mdarray_long : public mdarray
  {

  public:
    /* constructor                                                           */
    /* The mdarray_long class has two kinds of operating modes:            */
    /*  1. automatic resize mode                                             */
    /*  2. manual resize mode                                                */
    /* The operating mode is decided by auto_resize arg of constructor or    */
    /* init() member functions.  set_auto_resize() can also change the mode. */
    /* Basically automatic resize mode has larger overhead when calling      */
    /* member functions that can change the size of internal buffer.         */
    mdarray_long();
    mdarray_long( bool auto_resize );
    mdarray_long( bool auto_resize, long **extptr_address );
    mdarray_long( bool auto_resize, long *const **extptr2d_address );
    mdarray_long( bool auto_resize, long *const *const **extptr3d_address );
    mdarray_long( bool auto_resize, const size_t naxisx[], size_t ndim, bool init_buf );
    mdarray_long( bool auto_resize, size_t naxis0 );
    mdarray_long( bool auto_resize, size_t naxis0, size_t naxis1 );
    mdarray_long( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2 );
    mdarray_long( bool auto_resize, size_t naxis0, 
		   const long vals[] );
    mdarray_long( bool auto_resize, size_t naxis0, size_t naxis1,
		   const long vals[] );
    mdarray_long( bool auto_resize, size_t naxis0, size_t naxis1,
		   const long *const vals[] );
    mdarray_long( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
		   const long vals[] );
    mdarray_long( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
		   const long *const *const vals[] );
#if 0
    mdarray_long( bool auto_resize, const char *dim_info, long v0, ... );
#endif
    mdarray_long( const mdarray_long &obj );


    /* --------------------------------------------------------------------- */

    /* `virtual' ���Ĥ��Ƥ��Τϥ����С��饤�ɤǤϤʤ������С�����     */
    /*   mdarray_long &operator=(const mdarray_long &obj);            */
    /* �������¸�ߤ��������� (�黻�� = �����ϡ����̤ʵ�§������餷��) */

    /* substitute an array (copy the attribute, too)                      */
    virtual mdarray_long &operator=(const mdarray &obj);

    /* (overridden) */
    mdarray_long &operator+=(const mdarray &obj);
    mdarray_long &operator-=(const mdarray &obj);
    mdarray_long &operator*=(const mdarray &obj);
    mdarray_long &operator/=(const mdarray &obj);

    /* (overridden) */
    mdarray_long &operator=(dcomplex v);
    mdarray_long &operator=(double v);
    mdarray_long &operator=(long long v);
    mdarray_long &operator=(long v);
    mdarray_long &operator=(int v);
    mdarray_long &operator+=(dcomplex v);
    mdarray_long &operator+=(double v);
    mdarray_long &operator+=(long long v);
    mdarray_long &operator+=(long v);
    mdarray_long &operator+=(int v);
    mdarray_long &operator-=(dcomplex v);
    mdarray_long &operator-=(double v);
    mdarray_long &operator-=(long long v);
    mdarray_long &operator-=(long v);
    mdarray_long &operator-=(int v);
    mdarray_long &operator*=(dcomplex v);
    mdarray_long &operator*=(double v);
    mdarray_long &operator*=(long long v);
    mdarray_long &operator*=(long v);
    mdarray_long &operator*=(int v);
    mdarray_long &operator/=(dcomplex v);
    mdarray_long &operator/=(double v);
    mdarray_long &operator/=(long long v);
    mdarray_long &operator/=(long v);
    mdarray_long &operator/=(int v);

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
    mdarray_long &init();

    /* <overloaded>                */
    virtual mdarray_long &init( bool auto_resize );
    virtual mdarray_long &init( bool auto_resize, const size_t naxisx[], size_t ndim, bool init_buf );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0 );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, size_t naxis1 );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2 );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, 
				 const long vals[] );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, size_t naxis1,
				 const long vals[] );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, size_t naxis1,
				 const long *const vals[] );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
				 const long vals[] );
    virtual mdarray_long &init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2,
				 const long *const *const vals[] );
#if 0
    virtual mdarray_long &init( bool auto_resize, const char *dim_info,
				 long v0, ... );
    virtual mdarray_long &vinit( bool auto_resize, const char *dim_info,
				  long v0, va_list );
#endif
    /*  mdarray_long &init( const mdarray &obj ); ������Ƥ��� */
    /*  (��mdarray���饹�Υݥ��󥿷�ͳ�ǸƤӽФ���ǽ)            */

    /* (overridden)                                           */
    mdarray_long &init( const mdarray &obj );

    /* copy properties (auto_resize, auto_init, and rounding) */
    /* (overridden)                                           */
    mdarray_long &init_properties( const mdarray &src_obj );

    /* setting of the resize mode */
    /* (overridden)               */
    mdarray_long &set_auto_resize( bool tf );

    /* setting of the initialize mode */
    /* (overridden)                   */
    mdarray_long &set_auto_init( bool tf );

    /* setting of the rounding off possibility */
    /* (overridden)                            */
    mdarray_long &set_rounding( bool tf );

    /* setting of strategy of memory allocation */
    /* "auto", "min" and "pow" can be set.      */
    /* (overridden)                             */
    mdarray_long &set_alloc_strategy( const char *strategy );

    /* convert the value of the full array element */
    /* <overloaded>                                */
    virtual mdarray_long &convert_via_udf(
		void (*func)(const long *,long *,size_t,int,void *),
		void *user_ptr );

    /* swap contents between self and another objects */
    /* (overridden)                 */
    mdarray_long &swap( mdarray &sobj );

    /* ssize_t copy( mdarray &dest ) const; ������Ƥ��롥*/
    /* (��mdarray���饹�Υݥ��󥿷�ͳ�ǸƤӽФ���ǽ)      */

    /* copy an array into another object */
    /* (overridden)                      */
    ssize_t copy( mdarray *dest ) const;
    /* not recommended */
    ssize_t copy( mdarray &dest ) const;

    /* cut all values in an array and copy them */
    /* (overridden)                             */
    mdarray_long &cut( mdarray *dest );

    /* interchange rows and columns */
    /* (overridden)                 */
    mdarray_long &transpose_xy();

    /* interchange xyz to zxy */
    /* (overridden)           */
    mdarray_long &transpose_xyz2zxy();

    /* rotate image                                              */
    /*   angle: 90,-90, or 180                                   */
    /*          (anticlockwise when image is bottom-left origin) */
    /* (overridden)                                              */
    mdarray_long &rotate_xy( int angle );


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
    virtual size_t col_length() const;		     // axis0 ��Ĺ�� //
    virtual size_t x_length() const;		     // same as col_length //

    // the length of the array's row  //
    virtual size_t row_length() const;		     // axis1 ��Ĺ�� //
    virtual size_t y_length() const;		     // same as row_length //

    // the layer number of the array //
    virtual size_t layer_length() const;	     // axis2(�ʹ�) ��Ĺ�� //
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
    mdarray_long &assign( dcomplex value,
			    ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF );
    mdarray_long &assign( double value, 
			    ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF,
			    ssize_t idx2 = MDARRAY_INDEF );

    /* a reference to the specified value of the element (1 dimension) */
    virtual long &operator[]( ssize_t idx0 );
    virtual const long &operator[]( ssize_t idx0 ) const;

    /* a reference to the specified value of the element (1-3 dimensions) */
    virtual long &operator()( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				ssize_t idx2 = MDARRAY_INDEF);
    virtual const long &operator()( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				ssize_t idx2 = MDARRAY_INDEF) const;

    /* a reference to the specified value of the element (1-3 dimensions) */
    virtual long &at( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
			  ssize_t idx2 = MDARRAY_INDEF);
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const long &at( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				 ssize_t idx2 = MDARRAY_INDEF) const;
#endif
    virtual const long &at_cs( ssize_t idx0, ssize_t idx1 = MDARRAY_INDEF, 
				 ssize_t idx2 = MDARRAY_INDEF) const;


    /* --------------------------------------------------------------------- */

    /*
     * C-like APIs to input and output data
     */

    /* set a value to an arbitrary element's point */
    /* <overloaded>                                */
    virtual mdarray_long &put( long value, ssize_t idx, size_t len );
    virtual mdarray_long &put( long value,
				size_t dim_index, ssize_t idx, size_t len );

    /* copy the array stored in object into the programmer's buffer. */
    /* <overloaded>                                                  */
    virtual ssize_t get_elements( long *dest_buf, size_t elem_size, 
				ssize_t idx0 = 0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF ) const;

    /* copy the array in the programmer's buffer into object's buffer. */
    /* <overloaded>                                                    */
    virtual ssize_t put_elements( const long *src_buf, size_t elem_size,
				ssize_t idx0 = 0, ssize_t idx1 = MDARRAY_INDEF,
				ssize_t idx2 = MDARRAY_INDEF );


    /* --------------------------------------------------------------------- */

    /*
     * member functions to update length, type, etc.
     */

    /* expansion of the dimension number */
    /* (overridden)                      */
    mdarray_long &increase_dim();

    /* reduction of the dimension number */
    /* (overridden)                      */
    mdarray_long &decrease_dim();

    /* change the length of the array */
    /* (overridden)                   */
    mdarray_long &resize( size_t len );
    mdarray_long &resize( size_t dim_index, size_t len );
    mdarray_long &resize( const mdarray &src );

    /* change the length of the 1-d array */
    /* (overridden)                       */
    mdarray_long &resize_1d( size_t x_len );

    /* change the length of the 2-d array */
    /* (overridden)                       */
    mdarray_long &resize_2d( size_t x_len, size_t y_len );

    /* change the length of the 3-d array */
    /* (overridden)                       */
    mdarray_long &resize_3d( size_t x_len, size_t y_len, size_t z_len );

    /* change the length of the array (supports n-dim) */
    /* (overridden)                                    */
    mdarray_long &resize( const size_t naxisx[], size_t ndim, 
			    bool init_buf );

    /* set a string like "3,2" using printf() style args.             */
    /* Omitting values is allowed.  For example, ",2" does not change */
    /* length of first dimension.                                     */
    /* (overridden)                                                   */
    mdarray_long &resizef( const char *exp_fmt, ... );
    mdarray_long &vresizef( const char *exp_fmt, va_list ap );

    /* change the length of the array relatively */
    /* (overridden)                              */
    mdarray_long &resizeby( ssize_t len );			/* for 1-d */
    mdarray_long &resizeby( size_t dim_index, ssize_t len );

    /* change the length of the 1-d array relatively */
    /* (overridden)                                  */
    mdarray_long &resizeby_1d( ssize_t x_len );

    /* change the length of the 2-d array relatively */
    /* (overridden)                                  */
    mdarray_long &resizeby_2d( ssize_t x_len, ssize_t y_len );

    /* change the length of the 3-d array relatively */
    /* (overridden)                                  */
    mdarray_long &resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len);

    /* change the length of the array relatively                            */
    /*   naxisx_rel:  number of elements to be increased for each dimension */
    /*   ndim:        number of dimension                                   */
    /*   init_buf:    set true to initialize new elements                   */
    /* (overridden)                                                         */
    mdarray_long &resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			       bool init_buf );

    /* set a string like "3,-2" using printf() style args.             */
    /* Omitting values is allowed.  For example, ",-2" does not change */
    /* length of first dimension.                                      */
    /* (overridden)                                                    */
    mdarray_long &resizebyf( const char *exp_fmt, ... );
    mdarray_long &vresizebyf( const char *exp_fmt, va_list ap );

    /* change length of array without adjusting buffer contents */
    /* (overridden)                                             */
    mdarray_long &reallocate( const size_t naxisx[], size_t ndim, 
				bool init_buf );

    /* free current buffer and alloc new memory */
    /* (overridden)                             */
    mdarray_long &allocate( const size_t naxisx[], size_t ndim, 
			      bool init_buf );

    /* setting of the initial value for .resize(), etc. */
    /* <overloaded>                                     */
    virtual mdarray_long &assign_default( long value );

    /* insert a blank section */
    /* (overridden)           */
    mdarray_long &insert( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_long &insert( size_t dim_index, ssize_t idx, size_t len );

    /* erase a section */
    /* (overridden)    */
    mdarray_long &erase( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_long &erase( size_t dim_index, ssize_t idx, size_t len );

    /* copy values between elements (without automatic resizing) */
    /* ��ư (�Хåե��Υ��������ѹ����ʤ�)                       */
    /* (overridden)                                              */
    mdarray_long &move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );				/* for 1-d */
    mdarray_long &move( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );

    /* copy values between elements (with automatic resizing) */
    /* ��ư (�Хåե��Υ�������ɬ�פ˱������ѹ�����)          */
    /* (overridden)                                           */
    mdarray_long &cpy( ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );				/* for 1-d */
    mdarray_long &cpy( size_t dim_index, 
			  ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );

    /* replace values between elements */
    /* (overridden)                    */
    mdarray_long &swap( ssize_t idx_src, size_t len, 
			  ssize_t idx_dst );			/* for 1-d */
    mdarray_long &swap( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst );

    /* extract a section           */
    /* see also trimf() and trim() */
    /* (overridden)                */
    mdarray_long &crop( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_long &crop( size_t dim_index, ssize_t idx, size_t len );

    /* flip a section   */
    /* see also flipf() */
    /* (overridden)     */
    mdarray_long &flip( ssize_t idx, size_t len );		/* for 1-d */
    mdarray_long &flip( size_t dim_index, ssize_t idx, size_t len );


    /* raise decimals to the next whole number in a float/double type value */
    /* (overridden)                                                         */
    mdarray_long &ceil();

    /* devalue decimals in a float/double type value */
    /* (overridden)                                  */
    mdarray_long &floor();

    /* round off decimals in a float/double type value */
    /* (overridden)                                    */
    mdarray_long &round();

    /* omit decimals in a float/double type value */
    /* (overridden)                               */
    mdarray_long &trunc();

    /* absolute value of all elements */
    /* (overridden)                   */
    mdarray_long &abs();


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
    mdarray_long &trimf( const char *exp_fmt, ... );
    mdarray_long &vtrimf( const char *exp_fmt, va_list ap );

    /* flip elements in a section */
    /* (overridden)               */
    mdarray_long &flipf( const char *exp_fmt, ... );
    mdarray_long &vflipf( const char *exp_fmt, va_list ap );

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
    mdarray_long &cleanf( const char *exp_fmt, ... );
    mdarray_long &vcleanf( const char *exp_fmt, va_list ap );

    /* rewrite element values with a value in a section */
    /* (overridden)                                     */
    mdarray_long &fillf( double value, const char *exp_fmt, ... );
    mdarray_long &vfillf( double value, const char *exp_fmt, va_list ap );

    /* <overloaded>                                     */
    virtual mdarray_long &fillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_long *,void *),
        void *user_ptr, const char *exp_fmt, ... );
    virtual mdarray_long &vfillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_long *,void *),
        void *user_ptr, const char *exp_fmt, va_list ap );

    /* add a scalar value to element values in a section */
    /* (overridden)                                      */
    mdarray_long &addf( double value, 
			  const char *exp_fmt, ... );
    mdarray_long &vaddf( double value, 
			   const char *exp_fmt, va_list ap );

    /* subtract a scalar value from element values in a section */
    /* (overridden)                                             */
    mdarray_long &subtractf( double value, 
			       const char *exp_fmt, ... );
    mdarray_long &vsubtractf( double value, 
				const char *exp_fmt, va_list ap );

    /* multiply element values in a section by a scalar value */
    /* (overridden)                                           */
    mdarray_long &multiplyf( double value, 
			       const char *exp_fmt, ... );
    mdarray_long &vmultiplyf( double value, 
				const char *exp_fmt, va_list ap );

    /* divide element values in a section by a scalar value */
    /* (overridden)                                         */
    mdarray_long &dividef( double value, 
			     const char *exp_fmt, ... );
    mdarray_long &vdividef( double value, 
			      const char *exp_fmt, va_list ap );

    /* paste up an array object                                              */

    /* paste without operation using fast method of that of .convert().      */
    /* (overridden)                                                          */
    mdarray_long &pastef( const mdarray &src,
			    const char *exp_fmt, ... );
    mdarray_long &vpastef( const mdarray &src,
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
    virtual mdarray_long &pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_long *,void *),
	void *user_ptr,
        const char *exp_fmt, ... );
    virtual mdarray_long &vpastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_long *,void *),
	void *user_ptr,
        const char *exp_fmt, va_list ap );

    /* add an array object */
    /* (overridden)        */
    mdarray_long &addf( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_long &vaddf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* subtract an array object */
    /* (overridden)             */
    mdarray_long &subtractf( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_long &vsubtractf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* multiply an array object */
    /* (overridden)             */
    mdarray_long &multiplyf( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_long &vmultiplyf( const mdarray &src_img, const char *exp_fmt, va_list ap );

    /* divide an array object */
    /* (overridden)           */
    mdarray_long &dividef( const mdarray &src_img, const char *exp_fmt, ... );
    mdarray_long &vdividef( const mdarray &src_img, const char *exp_fmt, va_list ap );


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
    mdarray_long &move_from( 
			   ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* copy a section specified by move_from() */
    /* (overridden)                            */
    mdarray_long &move_to( 
	      ssize_t dest_col, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* trim a section */
    /* (overridden)   */
    mdarray_long &trim( 
			   ssize_t col_idx, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* flip horizontal within a rectangular section */
    /* (overridden)                                 */
    mdarray_long &flip_cols( 
			   ssize_t col_idx=0, size_t col_len=MDARRAY_ALL,
			   ssize_t row_idx=0, size_t row_len=MDARRAY_ALL, 
			   ssize_t layer_idx=0, size_t layer_len=MDARRAY_ALL );

    /* flip vertical within a rectangular section */
    /* (overridden)                               */
    mdarray_long &flip_rows( 
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
    mdarray_long &clean( 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* rewrite element values with a value in a section */
    /* (overridden)                                     */
    mdarray_long &fill( double value,
			 ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
			 ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
			 ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* <overloaded>                                     */
    virtual mdarray_long &fill_via_udf( double value,
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_long *,void *),
	void *user_ptr,
	ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
	ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
	ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );


    /* add a scalar value to element values in a section */
    /* (overridden)                                      */
    mdarray_long &add( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* subtract a scalar value from element values in a section */
    /* (overridden)                                             */
    mdarray_long &subtract( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* multiply element values in a section by a scalar value */
    /* (overridden)                                           */
    mdarray_long &multiply( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* divide element values in a section by a scalar value */
    /* (overridden)                                         */
    mdarray_long &divide( double value, 
		    ssize_t col_index = 0, size_t col_size = MDARRAY_ALL,
		    ssize_t row_index = 0, size_t row_size = MDARRAY_ALL,
		    ssize_t layer_index = 0, size_t layer_size = MDARRAY_ALL );

    /* paste up an array object */
    /* (overridden)             */
    mdarray_long &paste( const mdarray &src,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* <overloaded>             */
    virtual mdarray_long &paste_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray_long *,void *),
	void *user_ptr,
	ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );


    /* add an array object */
    /* (overridden)        */
    mdarray_long &add( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* subtract an array object */
    /* (overridden)             */
    mdarray_long &subtract( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* multiply an array object */
    /* (overridden)             */
    mdarray_long &multiply( const mdarray &src_img,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* divide an array object */
    /* (overridden)           */
    mdarray_long &divide( const mdarray &src_img,
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
    virtual mdarray_long &register_extptr( long **extptr_address );
    virtual mdarray_long &register_extptr_2d( long *const **extptr2d_address );
    virtual mdarray_long &register_extptr_3d( long *const *const **extptr3d_address );

    /* Acquire the specified element's address */
    virtual const long *carray() const;
    virtual const long *carray( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;
    virtual long *array_ptr();
    virtual long *array_ptr( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const long *array_ptr() const;
    virtual const long *array_ptr( ssize_t idx0, 
	    ssize_t idx1 = MDARRAY_INDEF, ssize_t idx2 = MDARRAY_INDEF ) const;
#endif
    virtual const long *array_ptr_cs() const;
    virtual const long *array_ptr_cs( ssize_t idx0, 
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
    virtual long *const *array_ptr_2d( bool use );
    /* followings have no overhead. Execute .array_ptr_2d(true) before use. */
    virtual long *const *array_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const long *const *array_ptr_2d() const;
#endif
    virtual const long *const *array_ptr_2d_cs() const;

    /* acquisition of address table for 3d array                            */
    /*   use: set true to enable automatic update of internal address table */
    /*        for 3d array and to obtain the address table.                 */
    /*        set false to free the memory for address table and to disable */
    /*        automatic update for it.                                      */
    /* *NOTE* To minimize internal processings for array resizing,          */
    /*        programmers have to call this member function with            */
    /*        "use=false" when there is no requirement of referring         */
    /*        obtained address table.                                       */
    virtual long *const *const *array_ptr_3d( bool use );
    /* followings have no overhead. Execute .array_ptr_3d(true) before use. */
    virtual long *const *const *array_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const long *const *const *array_ptr_3d() const;
#endif
    virtual const long *const *const *array_ptr_3d_cs() const;


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
    mdarray_long &reverse_endian( bool is_little_endian );
    /* (overridden)                    */
    mdarray_long &reverse_endian( bool is_little_endian, 
				    size_t begin, size_t length );

    /* acquisition of the initial value */
    virtual long default_value() const;

    /* taken over from mdarray class

    // returns type of result of calculations between self and type of szt //
    virtual ssize_t ope_size_type( ssize_t szt ) const;

    // ������֥������Ȥ� return ��ľ���˻Ȥ���shallow copy ����Ĥ������ //
    // �Ȥ���                                                                //
    virtual void set_scopy_flag();

    // output of the object information to the stderr output //
    // (for programmer's debug)                              //
    // ���֥������Ȥξ��֤�ɽ������ (�ץ���ޤ� debug ��)  //
    virtual void dprint( const char *msg = NULL ) const;

    */

  protected:
    /* �����С��饤��(ư������) */
    ssize_t default_size_type();
    bool is_acceptable_size_type( ssize_t sz_type );

  private:
    size_t parse_dim_info( const char *dim_info );
    /* dim_info �� */
    size_t dinfo_dlen[3];

  };


/*
 * inline member functions
 */

/**  
 * @brief  �ǽ�μ����ˤĤ��Ƥλ��ꤵ�줿��֤�ѥǥ���
 *
 *  ���Ȥ���������������ֹ�idx�ˡ���value��len�Ľ񤭹��ߤޤ���
 *
 * @param      value ���åȤ�����
 * @param      idx �����ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 *
 */
inline mdarray_long &mdarray_long::put( long value, ssize_t idx, size_t len )
{
    this->mdarray::put((const void *)&value, idx, len);
    return *this;
}

/**  
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��Ƥλ��ꤵ�줿��֤�ѥǥ���
 *
 *  ���Ȥ���������������ֹ�idx�ˡ���value��len�Ľ񤭹��ߤޤ���
 *
 * @param      value ���åȤ�����
 * @param      idx �����ֹ�
 * @param      len ���ǤθĿ�
 * @param      dim_index �����ֹ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 *
 */
inline mdarray_long &mdarray_long::put( long value,
				    size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::put((const void *)&value, dim_index, idx, len);
    return *this;
}

/**
 * @brief  ���ꤵ�줿1���Ǥ��ͤ����� (dcomplex���ǻ���)
 */
inline mdarray_long &mdarray_long::assign( dcomplex value,
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    this->mdarray::assign(value, idx0, idx1, idx2);
    return *this;
}

/**  
 * @brief  ���ꤵ�줿1���Ǥ��ͤ����� (double���ǻ���)
 *
 *  ���Ȥ�����Ρ�idxn �ǻ��ꤵ�줿1 ���Ǥ��ͤ����ꤷ�ޤ���
 *
 * @param      value double ������
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 *
 */
inline mdarray_long &mdarray_long::assign( double value, 
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    this->mdarray::assign(value, idx0, idx1, idx2);
    return *this;
}


/**  
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤� (1����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @return     ���Ǥ��ͤλ���
 * @throw      ��ư�ꥵ�����⡼�ɤ������Хåե��γ��ݤ˼��Ԥ������
 *
 */
inline long &mdarray_long::operator[]( ssize_t idx0 )
{
    return ((0 <= idx0 && idx0 < (ssize_t)this->length()) ? 
	    ((long *)(this->data_ptr()))[idx0] :
	    this->at(idx0));
}

/**  
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤� (1�������ɼ�����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @return     ���Ǥ��ͤλ���
 *
 */
inline const long &mdarray_long::operator[]( ssize_t idx0 ) const
{
    return ((0 <= idx0 && idx0 < (ssize_t)this->length()) ? 
	    ((const long *)(this->data_ptr_cs()))[idx0] :
	    this->at_cs(idx0));
}

/**  
 * @brief  () �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤� (1��3����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ǥ��ͤλ���
 * @throw      ��ư�ꥵ�����⡼�ɤ������Хåե��γ��ݤ˼��Ԥ������
 *
 */
inline long &mdarray_long::operator()( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    return this->at(idx0, idx1, idx2);
}

/**  
 * @brief  () �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤� (1��3�������ɼ�����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ǥ��ͤλ���
 *
 */
inline const long &mdarray_long::operator()( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->at_cs(idx0, idx1, idx2);
}

/**  
 * @brief  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ����
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ǥ��ͤλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 * @throw      ���֥�������������Ǥη��������дؿ�������ͤη���꾮�������
 *
 */
inline long &mdarray_long::at( ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	return ((long *)(this->data_ptr()))[idx];
    }
    else {
	long *p = (long *)(this->junk_rec);
	*p = INDEF_LONG;
	return *p;
    }
}

/**  
 * @brief  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤�
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ǥ��ͤλ���
 * @throw      ���֥�������������Ǥη��������дؿ�������ͤη���꾮�������
 *
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const long &mdarray_long::at( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->at_cs(idx0, idx1, idx2);
}
#endif

/**  
 * @brief  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤�
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ǥ��ͤλ���
 * @throw      ���֥�������������Ǥη��������дؿ�������ͤη���꾮�������
 *
 */
inline const long &mdarray_long::at_cs( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    ssize_t idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( 0 <= idx ) {
	return ((const long *)(this->data_ptr()))[idx];
    }
    else {
	long *p = (long *)(this->junk_rec);
	*p = INDEF_LONG;
	return *p;
    }
}

/**  
 * @brief  �����Ǥξ��������ͤ��ڤ�夲
 *
 *  ���Ȥ�����(��ư��������) �������Ǥξ��������ͤ��ڤ�夲�ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
inline mdarray_long &mdarray_long::ceil()
{
    this->mdarray::ceil();
    return *this;
}

/**  
 * @brief  �����Ǥξ��������ͤ��ڤ겼��
 * 
 *  ���Ȥ�����(��ư��������) �������Ǥξ������򡤤��줾������Ǥ��ͤ�
 *  �ۤ��ʤ�����������ͤ��ڤ겼���ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
inline mdarray_long &mdarray_long::floor()
{
    this->mdarray::floor();
    return *this;
}

/**  
 * @brief  �����Ǥξ��������ͤ�ͼθ����������ͤ��Ѵ�
 *
 *  ���Ȥ�����(��ư��������) ���������ͤξ�������ͼθ������������ͤˤ��ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
inline mdarray_long &mdarray_long::round()
{
    this->mdarray::round();
    return *this;
}

/**
 * @brief  �����Ǥ��ͤξ��������ڤ�Τơ�0 �˶ᤤ���������ͤ��Ѵ�
 *
 *  ���Ȥ�����(��ư��������) �������Ǥ��ͤξ��������ڤ�Τơ�
 *  0 �˶ᤤ���������ͤˤ��ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
inline mdarray_long &mdarray_long::trunc()
{
    this->mdarray::trunc();
    return *this;
}

/**  
 * @brief  ���Ȥ�����������ǤˤĤ��������ͤ�Ȥ�
 *
 *  ���Ȥ�����������ǤˤĤ��������ͤ�Ȥ�ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
inline mdarray_long &mdarray_long::abs()
{
    this->mdarray::abs();
    return *this;
}

/**  
 * @brief  �������Ƭ���ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ������������Ƭ���ǤΥ��ɥ쥹
 *
 */
inline const long *mdarray_long::carray() const
{
    return (const long *)(this->data_ptr_cs());
}

/**  
 * @brief  ����λ������ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ȥ���������λ������ǤΥ��ɥ쥹
 *
 */
inline const long *mdarray_long::carray( ssize_t idx0, 
				     ssize_t idx1, ssize_t idx2 ) const
{
    return (const long *)(this->data_ptr_cs(idx0,idx1,idx2));
}

/**  
 * @brief  �������Ƭ���ǤΥ��ɥ쥹�����
 *
 * @return     ���Ȥ������������Ƭ���ǤΥ��ɥ쥹
 *
 */
inline long *mdarray_long::array_ptr()
{
    return (long *)(this->data_ptr());
}

/**  
 * @brief  ����λ������ǤΥ��ɥ쥹�����
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ȥ���������λ������ǤΥ��ɥ쥹
 *
 */
inline long *mdarray_long::array_ptr( ssize_t idx0, 
				  ssize_t idx1, ssize_t idx2 )
{
    return (long *)(this->data_ptr(idx0,idx1,idx2));
}

/**  
 * @brief  �������Ƭ���ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ������������Ƭ���ǤΥ��ɥ쥹
 *
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const long *mdarray_long::array_ptr() const
{
    return (const long *)(this->data_ptr_cs());
}

/**  
 * @brief  ����λ������ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ȥ���������λ������ǤΥ��ɥ쥹
 *
 */
inline const long *mdarray_long::array_ptr( ssize_t idx0, 
					   ssize_t idx1, ssize_t idx2 ) const
{
    return (const long *)(this->data_ptr_cs(idx0,idx1,idx2));
}
#endif

/**  
 * @brief  �������Ƭ���ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 * @return     ���Ȥ������������Ƭ���ǤΥ��ɥ쥹
 *
 */
inline const long *mdarray_long::array_ptr_cs() const
{
    return (const long *)(this->data_ptr_cs());
}

/**  
 * @brief  ����λ������ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return     ���Ȥ���������λ������ǤΥ��ɥ쥹
 *
 */
inline const long *mdarray_long::array_ptr_cs( ssize_t idx0, 
					   ssize_t idx1, ssize_t idx2 ) const
{
    return (const long *)(this->data_ptr_cs(idx0,idx1,idx2));
}

/* acquisition of address table for 2d array */
/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹�����
 *
 * @param  use ���Ѥ������true(��ǽon)��<br>
 *             ���Ѥ��ʤ�����false(��ǽoff)��
 */
inline long *const *mdarray_long::array_ptr_2d( bool use )
{
    return (long *const *)(this->mdarray::data_ptr_2d(use));
}

/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹�����
 */
inline long *const *mdarray_long::array_ptr_2d()
{
    return (long *const *)(this->mdarray::data_ptr_2d());
}

/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
#ifdef SLI__OVERLOAD_CONST_AT
inline const long *const *mdarray_long::array_ptr_2d() const
{
    return (const long *const *)(this->mdarray::data_ptr_2d_cs());
}
#endif

/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const long *const *mdarray_long::array_ptr_2d_cs() const
{
    return (const long *const *)(this->mdarray::data_ptr_2d_cs());
}

/* acquisition of address table for 3d array */
/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹�����
 *
 * @param  use ���Ѥ������true(��ǽon)��<br>
 *             ���Ѥ��ʤ�����false(��ǽoff)��
 */
inline long *const *const *mdarray_long::array_ptr_3d( bool use )
{
    return (long *const *const *)(this->mdarray::data_ptr_3d(use));
}

/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹�����
 */
inline long *const *const *mdarray_long::array_ptr_3d()
{
    return (long *const *const *)(this->mdarray::data_ptr_3d());
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const long *const *const *mdarray_long::array_ptr_3d() const
{
    return (const long *const *const *)(this->mdarray::data_ptr_3d_cs());
}
#endif

/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const long *const *const *mdarray_long::array_ptr_3d_cs() const
{
    return (const long *const *const *)(this->mdarray::data_ptr_3d_cs());
}

/**  
 * @brief  ��������ĥ���ν���ͤ����
 *
 * @return     ���Ȥ����ĥ�������ĥ���ν����
 *
 */
inline long mdarray_long::default_value() const
{
    const long *p = (const long *)(this->default_value_ptr());
    if ( p == NULL ) return 0;
    else return *p;
}


}

#endif	/* _SLI__MDARRAY_LONG_H */
