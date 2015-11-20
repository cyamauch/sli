/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-20 13:21:44 cyamauch> */

#ifndef _SLI__MDARRAY_H
#define _SLI__MDARRAY_H 1

/**
 * @file   mdarray.h
 * @brief  ¿��������򰷤�����δ��쥯�饹 mdarray �������inline���дؿ�
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
 * @brief  ¿��������򰷤���������٥�ʥ��饹(���쥯�饹)
 *
 *   mdarray ���饹�ϡ�SLLIB �����٥�ʥ��饹�ǡ��ץ�ߥƥ��ַ��乽¤�Τ�
 *   ¿��������� IDL �� Python �Τ褦�˼�ڤ˰�����褦�ˤ��ޤ��������Τ褦��
 *   ��ǽ������ޤ���<br>
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
 *   ���쥯�饹��mdarray�Ǥϡ�����η����̤�ưŪ���ѹ���������Ǥ��ޤ�����
 *   []�黻�Ҥʤɤ����Ĥ��Ȥ��ʤ�API������ޤ�������Ū�����Ӥˤϡ������̤�
 *   ��ޤäƤ��ư����� mdarray_float ���ηѾ����饹�򤪻Ȥ�����������
 * 
 * @note  SIMD̿��(SSE2)�ˤ�ꡤ����Ū����ʬ����®������Ƥ��ޤ���
 * @attention  C++ ɸ��饤�֥��� <cmath> ��Ȥ���硤�ޥ��� SLI__USE_CMATH
 *             ��������Ƥ��� mdarray.h �� include ���Ƥ���������
 * @attention  long double���˴ؤ��Ƥϼ����������ǤϤ���ޤ���
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */
 
/*
 * ��������ư����������¤�� (�ץ�ߥƥ��ַ�) �� multi-dimensional array ��
 * ��������Υ��饹(���饹��Ĥä�����ϤǤ��ʤ�)
 *
 * ����ա�long double �ˤĤ��Ƥϡ�����������Ⱦü�ʤΤǻȤ�ʤ�����
 *         (Cygwin �������� long double �б������饵�ݡ��Ȥ���ͽ��)
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
    /* ���: ���� = �� init() ��Ƥ�                 */
    virtual mdarray &operator=(const mdarray &obj);
    /* �ʲ��α黻�Ҥ�Ʊ�����񤤤� = ��Ȥ�������硤 */
    /* obj = 0.0; obj += src; �Ȥ���                 */

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
    /* ����Ū���ڥ졼���Ȥ�����֤���                           */
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
    virtual ssize_t size_type() const;		       /* sz_type_rec ���֤� */

    /* number of dimensions of an array */
    virtual size_t dim_length() const;		       /* �����ο� */

    /* number of all elements */
    virtual size_t length() const;		       /* �����ǤθĿ� */

    /* number of elements in a dimension */
    virtual size_t length( size_t dim_index ) const;   /* �Ƽ��ˤ��������ǿ� */

    /* the length of the array's column */
    virtual size_t col_length() const;		     /* axis0 ��Ĺ��       */
    virtual size_t x_length() const;		     /* same as col_length */

    /* the length of the array's row  */
    virtual size_t row_length() const;		     /* axis1 ��Ĺ��       */
    virtual size_t y_length() const;		     /* same as row_length */

    /* the layer number of the array */
    virtual size_t layer_length() const;	     /* axis2(�ʹ�) ��Ĺ��   */
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
    /* ��ư(�Хåե��Υ��������ѹ����ʤ�)                        */
    virtual mdarray &move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );				/* for 1-d */
    virtual mdarray &move( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr );

    /* copy values between elements (with automatic resizing) */
    /* ��ư(�Хåե��Υ�������ɬ�פ˱������ѹ�����)           */
    virtual mdarray &cpy( ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );				/* for 1-d */
    virtual mdarray &cpy( size_t dim_index, 
			  ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr );

    /* replace values between elements */
    /* �Хåե���Ǥ����촹�� */
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
    /* �黻�ʤ��ڡ�����: convert() ��Ʊ����ˡ���Ѵ�������̤��®�˳�Ǽ�� */
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
    /* �黻����ڡ�����: ��ö double �����Ѵ�����Ƥ���黻������̤��Ǽ��  */
    /* ���������α黻�ʤ��Ǥ���٤��٤���                                    */
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
    /* �黻�ʤ��ڡ�����: convert() ��Ʊ����ˡ���Ѵ�������̤��®�˳�Ǽ�� */
    virtual mdarray &paste( const mdarray &src,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* for SFITSIO */
    virtual mdarray &paste_via_udf( const mdarray &src,
	  void (*func)(const void *,void *,size_t,int,void *), void *user_ptr,
	  ssize_t dest_col = 0, ssize_t dest_row = 0, ssize_t dest_layer = 0 );

    /* paste with operation: all elements are converted into double type, so */
    /* the performance is inferior to above paste().                         */
    /* �黻����ڡ�����: ��ö double �����Ѵ�����Ƥ���黻������̤��Ǽ��  */
    /* ���������α黻�ʤ��Ǥ���٤��٤���                                    */
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
    /* ���Ȥη��ȷ� szt �α黻���Ԥʤ�줿���η����֤�                     */
    virtual ssize_t ope_size_type( ssize_t szt ) const;

    /* ������֥������Ȥ� return ��ľ���˻Ȥ���shallow copy ����Ĥ������ */
    /* �Ȥ���                                                                */
    virtual void set_scopy_flag();

    /* output of the object information to the stderr output */
    /* (for programmer's debug)                              */
    /* ���֥������Ȥξ��֤�ɽ������ (�ץ���ޤ� debug ��)  */
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
    /* �Ƽ�Ĺ��������������ɬ���ƤӤ�����롥�Ѿ��������饹�ǥ����Х饤�� */
    /* ����ȡ��Ƽ�Ĺ������Υߥ顼����ǽ                                   */
    virtual void update_length_info();

  private:
    /* constructor �Ѥ����̤ʥ��˥���饤�� */
    void __force_init( ssize_t sz_type, bool is_constructor );
    /* destructor �Ѥ����̤ʥ��곫���ѥ��дؿ� */
    void __force_free();
    /* shallow copy �ط� */
    bool request_shallow_copy( mdarray *from_obj ) const;
    void cancel_shallow_copy( mdarray *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;
    /* ���̤ʥ��˥���饤�� */
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
    /* private �ʥ������å��� */
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
    /* _arr_ptr_2d_rec, _arr_ptr_3d_rec �򹹿����� */
    /* update_length_info() ��ľ��˸Ƥֻ�       */
    void update_arr_ptr_2d( bool on_2d = false );
    void update_arr_ptr_3d( bool on_3d = false );
    /* _init(..) �����٥���дؿ� */
    void _update_arr_ptr_2d( bool on_2d = false );
    void _update_arr_ptr_3d( bool on_3d = false );
    /* extptr_2d_rec, extptr_3d_rec �� non-NULL �λ��ϻȤäƤϤ����ʤ� */
    void free_arr_ptr_2d();
    void free_arr_ptr_3d();
    /* inline���дؿ��� throw ��������� */
    void err_throw_int_arg( const char *fncname, const char *lev, 
			    const char *mes, int v ) const;

  private:
    /* dummy member functions to output error */
    mdarray( ssize_t sz_type, int naxis0 );
    mdarray( ssize_t sz_type, size_t naxis0 );
    mdarray &init( ssize_t sz_type, int naxis0 );
    mdarray &init( ssize_t sz_type, size_t naxis0 );

  private:
    /* ����ա�shallow copy ���оݤˤʤ��Τ� _ ����Ƭ�ˤĤ�               */
    /*  _default_rec, _size_rec, _arr_rec, _arr_ptr_2d_rec, _arr_ptr_3d_rec */
    /* �����ˤĤ��Ƥϡ�this->_size_rec[i] �Τ褦��ľ���ɤߤΤߡ��ɤ���    */
    /* �ȤäƤ��ɤ�������ʳ��ξ��ϡ��������å�����ͳ�ǻȤ����ȡ�         */
    /* �����ɤ�����å�����ˤ�                                             */
    /*    [^a-z_]_size_rec[^a-z_]                                           */
    /* �Τ褦��õ����cleanup_shallow_copy(true); ��ȤäƤ��뤫������å��� */
    bool auto_resize_rec;	/* ��ư�ꥵ�����򤹤뤫 */
    bool auto_init_rec;		/* �ꥵ�������˥Хåե����������뤫 */
    bool rounding_rec;		/* ��ư�����������������Ѵ�������˻ͼθ��� */
    void *_default_rec;		/* 1���Ǥν���ͤ��ݻ�����Хåե��Υ��ɥ쥹 */
    /* ���!! �����������ͤ���Ļ�������Τǡ�labs() ��Ȥ�ɬ�פ����롥 */
    ssize_t sz_type_rec;	/* 1���ǤΥХ��ȿ�(��ξ�����ư��������) */
    size_t dim_size_rec;	/* Number of dimension */
    size_t *_size_rec;		/* length of dim0,dim1,dim2,... */
    void *_arr_rec;		/* �ǡ����ΰ�Υ��ɥ쥹 */
    size_t arr_alloc_blen_rec;	/* _arr_rec �Ѥ˳��ݤ��Ƥ���Х���Ĺ */
    int alloc_strategy_rec;	/* realloc() ��ɤΤ褦�˸ƤӽФ�������ά */
    /* _arr_rec �ؤΥ��ɥ쥹�ơ��֥� (2d or 3d�ǡ���������)                  */
    /* NULL��ü�Υ��ɥ쥹�ơ��֥�Ǥ��ꡤ���ɥ쥹�ơ��֥뤬ͭ�����ɤ�����    */
    /* _arr_ptr_2d_rec��_arr_ptr_3d_rec ���줾����ͤ�NULL��̵������Ƚ�ꤹ�� */
    /* (���ɥ쥹�ơ��֥뤬ͭ���ʾ�硤mdarray������Ĺ��0�ʤ�Ĺ��1�Υơ��֥�  */
    /* �Ȥʤ�) */
    void **_arr_ptr_2d_rec;
    void ***_arr_ptr_3d_rec;
    /* ����ʲ��� update_length_info() �Ǽ�ư��������� */
    size_t cached_length_rec;		/* �����ǿ� */
    size_t cached_col_length_rec;	/* ���Ĺ�� */
    size_t cached_row_length_rec;	/* �Ԥ�Ĺ�� */
    size_t cached_layer_length_rec;	/* �쥤��ο� */
    size_t cached_bytes_rec;		/* 1���ǤΥХ���Ĺ */
    size_t cached_col_row_length_rec;	/* ��߹Ԥ�Ĺ�� */
    size_t cached_row_layer_length_rec;	/* �ԡߥ쥤��ο� */
    /* ���Ѵ��Ѵؿ�(���Ȥη���double)��update_length_info() �Ǽ�ư���� */
    void (*func_cnv_nd_x2d)(const void *, void *, size_t, int, void *);
    /* ���Ѵ��Ѵؿ�(double�����Ȥη�)��update_length_info() �Ǽ�ư���� */
    void (*func_cnv_nd_d2x)(const void *, void *, size_t, int, void *);
    /* ���Ѵ��Ѵؿ�(���Ȥη���float)��update_length_info() �Ǽ�ư���� */
    void (*func_cnv_nd_x2f)(const void *, void *, size_t, int, void *);
    /* ž�ֹ�������ؿ���update_length_info() �Ǽ�ư���� */
    void (*func_transp_mtx)(void *, size_t);
    /* move_from(), move_to() ���� */
    ssize_t move_from_idx_rec[3];
    size_t move_from_len_rec[3];
    /* ����ʲ��� init() �� swap() ���ѹ����ʤ� */
    void **extptr_rec;		/* �����ݥ����ѿ��Υ��ɥ쥹 */
    void ***extptr_2d_rec;	/* 2d�ѳ����ݥ����ѿ��Υ��ɥ쥹 */
    void ****extptr_3d_rec;	/* 3d�ѳ����ݥ����ѿ��Υ��ɥ쥹 */
    /* void (*length_change_notify_func)(const mdarray *, void *); */
    /* void *length_change_notify_user_ptr; */

    /* scan_along_x() ���� (= ���Ǥϥ��ԡ�����ʤ�) */
    heap_mem<char> scanx_ret_buf;	/* scan_along_x() ���֤����ѥХåե� */
    size_t scanx_ret_buf_blen;		/* aligned�ǳ��ݤ���Ƥ���byte������ */
    char *scanx_aligned_ret_ptr;	/* ���buf����aligned����Ƭ���ɥ쥹 */
    heap_mem<size_t> scanx_begin;	/* ������󤹤�Хåե��ΰ�ΰ���   */
    heap_mem<size_t> scanx_len;		/* ������󤹤�Хåե��ΰ���礭�� */
    heap_mem<size_t> scanx_cntr;	/* ������ */
    size_t scanx_ndim;
    size_t scanx_ret_z;
    const char *scanx_cptr;		/* ���ߤΥХåե����� */

    /* scan_along_y() ���� (= ���Ǥϥ��ԡ�����ʤ�) */
    heap_mem<char> scany_trans_buf;	/* ��ʬxy-transpose�Ѱ���Хåե� */
    size_t scany_trans_buf_blen;
    size_t scany_height_trans;		/* scany_trans_buf �ν��� */
    heap_mem<char> scany_mtx_buf;	/* transpose����������Хåե� */
    size_t scany_mtx_buf_blen;
    size_t scany_len_block;
    heap_mem<char> scany_ret_buf;	/* scan_along_y() ���֤����ѥХåե� */
    size_t scany_ret_buf_blen;		/* aligned�ǳ��ݤ���Ƥ���byte������ */
    char *scany_aligned_ret_ptr;	/* ���buf����aligned����Ƭ���ɥ쥹 */
    void (*scany_func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    heap_mem<size_t> scany_begin;	/* ������󤹤�Хåե��ΰ�ΰ���   */
    heap_mem<size_t> scany_len;		/* ������󤹤�Хåե��ΰ���礭�� */
    heap_mem<size_t> scany_cntr;	/* ������ */
    size_t scany_ndim;
    size_t scany_ret_z;
    const char *scany_cptr;		/* ���ߤΥХåե����� */

    /* scan_along_z() ���� (= ���Ǥϥ��ԡ�����ʤ�) */
    heap_mem<char> scanz_trans_buf;	/* ��ʬxz-transpose�Ѱ���Хåե� */
    size_t scanz_trans_buf_blen;
    size_t scanz_height_trans;		/* scanz_trans_buf �ν��� */
    heap_mem<char> scanz_mtx_buf;	/* transpose����������Хåե� */
    size_t scanz_mtx_buf_blen;
    size_t scanz_len_block;
    heap_mem<char> scanz_ret_buf;	/* scan_along_z() ���֤����ѥХåե� */
    size_t scanz_ret_buf_blen;		/* aligned�ǳ��ݤ���Ƥ���byte������ */
    char *scanz_aligned_ret_ptr;	/* ���buf����aligned����Ƭ���ɥ쥹 */
    void (*scanz_func_gencpy2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    heap_mem<size_t> scanz_begin;	/* ������󤹤�Хåե��ΰ�ΰ���   */
    heap_mem<size_t> scanz_len;		/* ������󤹤�Хåե��ΰ���礭�� */
    heap_mem<size_t> scanz_cntr;	/* ������ */
    size_t scanz_ndim;
    size_t scanz_ret_z;
    const char *scanz_cptr;		/* ���ߤΥХåե����� */

    /* scan_zx_planes() ���� (= ���Ǥϥ��ԡ�����ʤ�) */
    heap_mem<char> scanzx_mtx_buf;	/* transpose����������Хåե� */
    size_t scanzx_mtx_buf_blen;
    size_t scanzx_len_block;
    heap_mem<char> scanzx_ret_buf;	/* scan_zx_planes() �֤����ѥХåե� */
    size_t scanzx_ret_buf_blen;		/* aligned�ǳ��ݤ���Ƥ���byte������ */
    char *scanzx_aligned_ret_ptr;	/* ���buf����aligned����Ƭ���ɥ쥹 */
    heap_mem<size_t> scanzx_begin;	/* ������󤹤�Хåե��ΰ�ΰ���   */
    heap_mem<size_t> scanzx_len;	/* ������󤹤�Хåե��ΰ���礭�� */
    heap_mem<size_t> scanzx_cntr;	/* ������ */
    size_t scanzx_ndim;
    size_t scanzx_ret_z;
    const char *scanzx_cptr;		/* ���ߤΥХåե����� */

    /* scan_a_cube() ���� (= ���Ǥϥ��ԡ�����ʤ�) */
    heap_mem<char> scancube_ret_buf;	/* scan_a_cube() ���֤����ѥХåե� */
    size_t scancube_ret_buf_blen;	/* aligned�ǳ��ݤ���Ƥ���byte������ */
    char *scancube_aligned_ret_ptr;	/* ���buf����aligned����Ƭ���ɥ쥹 */
    heap_mem<size_t> scancube_begin;	/* ������󤹤�Хåե��ΰ�ΰ���   */
    heap_mem<size_t> scancube_len;	/* ������󤹤�Хåե��ΰ���礭�� */
    heap_mem<size_t> scancube_cntr;	/* ������ */
    size_t scancube_ndim;
    size_t scancube_ret_z;

    /*
     * �Хåե��ηѾ�(shallow copy)�Τ���Υե饰��= ���Ǥϥ��ԡ�����ʤ�
     */
    /* ���ԡ��� src ¦�Υե饰 */
    bool shallow_copy_ok;		/* set_scopy_flag() �ǥ��åȤ���� */
    mdarray *shallow_copy_dest_obj;	/* ���ԡ���: ͭ���ʤ� non-NULL */

    /* ���ԡ��� dest ¦�Υե饰(���ԡ�������񤭹��ޤ��) */
    mdarray *shallow_copy_src_obj;	/* ���ԡ���: �����Ƥ���� non-NULL */

    /* __shallow_init(), __deep_init() ������˹Ԥʤ�����Υե饰 */
    bool __copying;

  protected:
    /* class level */
    int class_level_rec;				/* �Ѿ������ +1 */
    /* ���ߡ��Хåե�: ��������� */
    char junk_rec[64];		/* f() �Ȥ� d() ���ϰϳ��ξ��˻Ȥ� */

  };


/*
 * inline member functions
 */

/**
 * @brief  ����ɽ������(������)�����
 *
 *  ����ˤ�ꡤ���Ȥ����ꤵ�줿����η���Ĵ�٤�����Ǥ��ޤ���
 *
 * @return  ����ɽ������(������)
 *            
 */
inline ssize_t mdarray::size_type() const
{
    return this->sz_type_rec;
}

/**
 * @brief  ����μ����������
 *
 * @return  ���Ȥ���������μ�����
 *            
 */
inline size_t mdarray::dim_length() const
{
    return this->dim_size_rec;
}

/**
 * @brief  ��������Ƥ����ǿ������
 *
 *  �����ǿ�(����1 �θĿ��߼���2 �θĿ��� ����3 �θĿ���...) ���֤��ޤ���
 *
 * @return    ���Ȥ�������������Ƥ����ǿ�
 *            
 */
inline size_t mdarray::length() const
{
    return this->cached_length_rec;
}

/**
 * @brief  ����λ��ꤵ�줿���������ǿ����֤�
 *
 *  �����ֹ椬 dim_index �μ��������ǤθĿ����֤��ޤ���<br>
 *  dim_index �γ����ֹ�� 0 �Ǥ���
 *
 * @param     dim_index �����ֹ�(���Ϥ��ͤ�0)
 * @return    ���Ȥ���������λ��ꤵ�줿���������ǿ�
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
 * @brief  �������(x��)��Ĺ�������
 *
 * @return  ���Ȥ�������������Ĺ��
 *            
 */
inline size_t mdarray::col_length() const
{
    return this->cached_col_length_rec;
}

/**
 * @brief  �������(x��)��Ĺ�������
 *
 * @return    ���Ȥ�������������Ĺ��
 *            
 */
inline size_t mdarray::x_length() const
{
    return this->cached_col_length_rec;
}

/**
 * @brief  ����ι�(y��)��Ĺ�������
 *
 * @return    ���Ȥ���������ιԤ�Ĺ��
 *            
 */
inline size_t mdarray::row_length() const
{
    return this->cached_row_length_rec;
}

/**
 * @brief  ����ι�(y��)��Ĺ�������
 *
 * @return    ���Ȥ���������ιԤ�Ĺ��
 *            
 */
inline size_t mdarray::y_length() const
{
    return this->cached_row_length_rec;
}

/**
 * @brief  ����Υ쥤��������
 *
 *  1�����ޤ���2��������ξ��ϡ�1 ���֤��ޤ���<br>
 *  3�����ʾ�ξ��ϡ�����μ����� 3�����˽��व�������� 3������(�����ֹ�2)
 *  ��Ĺ�����֤��ޤ���
 *
 * @return    ���Ȥ���������Υ쥤���
 *            
 */
inline size_t mdarray::layer_length() const
{
    return this->cached_layer_length_rec;
}

/**
 * @brief  ����Υ쥤��������
 *
 *  1�����ޤ���2��������ξ��ϡ�1 ���֤��ޤ���<br>
 *  3�����ʾ�ξ��ϡ�����μ����� 3�����˽��व�������� 3������(�����ֹ�2)
 *  ��Ĺ�����֤��ޤ���
 *
 * @return    ���Ȥ���������Υ쥤���
 *            
 */
inline size_t mdarray::z_length() const
{
    return this->cached_layer_length_rec;
}

/**
 * @brief  ����� x �� y ��Ĺ�������
 *
 * @return    ���Ȥ���������� x �� y ��Ĺ��
 *            
 */
inline size_t mdarray::col_row_length() const
{
    return this->cached_col_row_length_rec;
}

/**
 * @brief  ����� y �� z (z�Ͻ���)��Ĺ�������
 *
 * @return    ���Ȥ���������� y �� z (z�Ͻ���) ��Ĺ��
 *            
 */
inline size_t mdarray::row_layer_length() const
{
    return this->cached_row_layer_length_rec;
}


/**
 * @brief  �����1���ǤΥХ���Ĺ�����
 *
 * @return    ���Ȥ����������1���ǤΥХ���Ĺ
 *            
 */
inline size_t mdarray::bytes() const
{
    return this->cached_bytes_rec;
}

/**
 * @brief  �������Х���Ĺ�����
 *
 * @return    ���Ȥ��������������ǤΥХ���Ĺ
 *            
 */
inline size_t mdarray::byte_length() const
{
    return this->cached_length_rec * this->cached_bytes_rec;
}

/**
 * @brief  ����λ��ꤵ�줿��������Х���Ĺ���֤�
 *
 *  �����ֹ椬 dim_index �μ�������Х���Ĺ���֤��ޤ���
 *
 * @param     dim_index �����ֹ�(���Ϥ��ͤ�0)
 * @return    ���꼡������Х���Ĺ
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
 * @brief  dcomplex �����Ѵ����������Ǥ��ͤ����
 *
 *  ���Ȥ���������������ͤ� dcomplex �����Ѵ������֤��ޤ���
 *  ���󥵥���Ķ�������Ǥ���ꤷ����硤NAN+NAN*I ���֤��ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 *
 * @return     �Ѵ����
 * @retval     dcomplex�����Ѵ��������Ǥ��� ���ｪλ
 * @retval     NAN+NAN*I ���Ǥη������ݡ��Ȥ���ʤ����ξ�硿���󥵥�����
 *             Ķ�������Ǥ���ꤷ�����
 *
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  double �����Ѵ����������Ǥ��ͤ����
 *
 *  ���Ȥ���������������ͤ� double �����Ѵ������֤��ޤ���
 *  ���󥵥���Ķ�������Ǥ���ꤷ����硤NAN ���֤��ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 *
 * @return     �Ѵ����
 * @retval     double�����Ѵ��������Ǥ��� ���ｪλ
 * @retval     NAN ���Ǥη������ݡ��Ȥ���ʤ����ξ�硿���󥵥�����Ķ����
 *             ���Ǥ���ꤷ�����
 *
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  long �����Ѵ����������Ǥ��ͤ����
 *
 *  ���Ȥ���������������ͤ� long �����Ѵ������֤��ޤ���<br>
 *  ���Ȥη�����ư�������ξ�硤�ǥե���ȤǤϾ������ʲ����ڤ�ΤƤ��ޤ���
 *  �ͼθ������������ϡ�ͽ�� set_rounding() ���дؿ�����Ѥ��ơ��ͼθ�����
 *  �Ԥ�����ˤ��ޤ���<br>
 *  ���󥵥���Ķ�������Ǥ���ꤷ����硤INDEF_LONG ���֤�ޤ���
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 *
 * @return     �Ѵ����
 * @retval     long�����Ѵ��������Ǥ��� ���ｪλ
 * @retval     INDEF_LONG ���Ǥη������ݡ��Ȥ���ʤ����ξ�硿���󥵥�����
 *             Ķ�������Ǥ���ꤷ�����
 *
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  long long �����Ѵ����������Ǥ��ͤ����
 *
 *  ���Ȥ���������������ͤ� long long �����Ѵ������֤��ޤ���<br>
 *  ���Ȥη�����ư�������ξ�硤�ǥե���ȤǤϾ������ʲ����ڤ�ΤƤ��ޤ���
 *  �ͼθ������������ϡ�ͽ�� set_rounding() ���дؿ�����Ѥ��ơ��ͼθ�����
 *  �Ԥ�����ˤ��ޤ���<br>
 *  ���󥵥���Ķ�������Ǥ���ꤷ����硤INDEF_LLONG ���֤�ޤ���
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 *
 * @return     �Ѵ����
 * @retval     longlong�����Ѵ��������Ǥ��� ���ｪλ
 * @retval     INDEF_LLONG ���Ǥη������ݡ��Ȥ���ʤ����ξ�硿���󥵥�����
 *             Ķ�������Ǥ���ꤷ�����
 *
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  ���ꤵ�줿1���Ǥ��ͤ����� (dcomplex���ǻ���)
 *
 *  ���Ȥ�����Ρ�idxn �ǻ��ꤵ�줿1���Ǥ��ͤ����ꤷ�ޤ���<br>
 *  ��ư�������ͤ������������Ǥ����������硤�ǥե���ȤǤϾ������ʲ����ڤ��
 *  �Ƥޤ����������ʲ���ͼθ������������ϡ�ͽ�� set_rounding() ���дؿ���
 *  ���Ѥ��ơ����֥������Ȥ�ͼθ�����Ԥ�����ˤ��ޤ���<br>
 *  ư��⡼�ɤ���ư�ꥵ�����⡼�ɤξ�硤���ꤵ�줿�����ֹ�˽��ä����󥵥���
 *  ����ưŪ�˥ꥵ��������ޤ���<br>
 *  ư��⡼�ɤ���ư�ꥵ�����⡼�ɤξ�硤���󥵥�����Ķ�������Ǥ��ͤ���������
 *  �⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ������󥵥�����Ķ�������Ǥ��ͤ�����
 *  ��Ԥ��ˤϡ�ͽ�� resize() ���дؿ��ǥ��������ĥ����ɬ�פ�����ޤ���
 *
 * @param      value dcomplex ������
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�
 *
 * @return     ���Ȥλ���
 *
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 *
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 */
inline mdarray &mdarray::assign( dcomplex val,	
				 ssize_t idx0, ssize_t idx1, ssize_t idx2)
{
    const ssize_t this__sz_type_rec = this->sz_type_rec;
    const ssize_t idx = this->get_idx_3d(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    if ( this__sz_type_rec == DOUBLE_ZT )        /* �¿����Τ�Ŭ�� */
        ((double *)(this->_arr_rec))[idx] = (__real__ (val));
    else if ( this__sz_type_rec == FLOAT_ZT )    /* �¿����Τ�Ŭ�� */
        ((float *)(this->_arr_rec))[idx] = (__real__ (val));
    else if ( this__sz_type_rec == LDOUBLE_ZT )  /* �¿����Τ�Ŭ�� */
        ((long double *)(this->_arr_rec))[idx] = (__real__ (val));
    else if ( this__sz_type_rec == INT64_ZT ) {  /* �¿����Τ�Ŭ�� */
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
    else if ( this__sz_type_rec == INT32_ZT ) {  /* �¿����Τ�Ŭ�� */
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
    else if ( this__sz_type_rec == INT16_ZT ) {  /* �¿����Τ�Ŭ�� */
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
    else if ( this__sz_type_rec == UCHAR_ZT ) {  /* �¿����Τ�Ŭ�� */
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
 * @brief  ���ꤵ�줿 1���Ǥ��ͤ����� (double���ǻ���)
 *
 *  ���Ȥ�����Ρ�idxn �ǻ��ꤵ�줿1���Ǥ��ͤ����ꤷ�ޤ���<br>
 *  ��ư�������ͤ������������Ǥ����������硤�ǥե���ȤǤϾ������ʲ����ڤ��
 *  �Ƥޤ����������ʲ���ͼθ������������ϡ�ͽ�� set_rounding() ���дؿ���
 *  ���Ѥ��ơ����֥������Ȥ�ͼθ�����Ԥ�����ˤ��ޤ���<br>
 *  ư��⡼�ɤ���ư�ꥵ�����⡼�ɤξ�硤���ꤵ�줿�����ֹ�˽��ä����󥵥���
 *  ����ưŪ�˥ꥵ��������ޤ���<br>
 *  ư��⡼�ɤ���ư�ꥵ�����⡼�ɤξ�硤���󥵥�����Ķ�������Ǥ��ͤ���������
 *  �⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ������󥵥�����Ķ�������Ǥ��ͤ�����
 *  ��Ԥ��ˤϡ�ͽ�� resize() ���дؿ��ǥ��������ĥ����ɬ�פ�����ޤ���
 *
 * @param      value double ������
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�
 *
 * @return     ���Ȥλ���
 *
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼�ɤξ��)
 *
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
    else if ( this__sz_type_rec == FCOMPLEX_ZT )  /* ʣ�ǿ����Ѵ��������� */
        ((fcomplex *)(this->_arr_rec))[idx] = (val + 0.0*(__extension__ 1.0iF));
    else if ( this__sz_type_rec == DCOMPLEX_ZT )  /* ʣ�ǿ����Ѵ��������� */
        ((dcomplex *)(this->_arr_rec))[idx] = (val + 0.0*(__extension__ 1.0iF));
    else if ( this__sz_type_rec == LDCOMPLEX_ZT ) /* ʣ�ǿ����Ѵ��������� */
        ((ldcomplex *)(this->_arr_rec))[idx] = (val + 0.0*(__extension__ 1.0iF));

    return *this;
}

/**
 * @brief  ���ꤵ�줿1�����ͤλ��� (fcomplex ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  ���Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN + NAN*I ��
 *  �֤�ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 *
 * @return     fcomplex ���Ǥ��ͤλ���
 *
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw      ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *             �Х��ȥ�������꾮�������
 *
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (dcomplex ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  ���Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN + NAN*I ��
 *  �֤�ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 *
 * @return     dcomplex ���Ǥ��ͤλ���
 *
 * @throw      ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *             �Х��ȥ�������꾮�������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (ldcomplex ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  ���Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN + NAN*I ��
 *  �֤�ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 * @return     ldcomplex ���Ǥ��ͤλ���
 * @throw      ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *             �Х��ȥ�������꾮�������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (float ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  ���Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN���֤�ޤ���
 *
 * @param    idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param    idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param    idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return   float���Ǥ��ͤλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw    ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *           �Х��ȥ�������꾮�������
 * @note     �����ˡ�MDARRAY_INDEF ������Ū��Ϳ����\���Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (double ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  ���Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    double���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (long double ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  ���Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    long double���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (unsigned char ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_UCHAR ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    char���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (short ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_SHORT ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    short���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (int ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (long ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_LONG ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    long���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (long long��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_LLONG ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    long long���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (int16_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT16 ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int16_t���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (int32_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT32 ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int32_t���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (int64_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT64 ��
 *  �֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int64_t���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (size_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�0 ���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    size_t���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (ssize_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�0 ���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    ssize_t���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (bool ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ��resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼���false���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    bool���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ�������,���дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤλ��� (uintptr_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ����ꡦ�������ޤ���<br>
 *  �����Υ��дؿ�����Ѥ����ͤ��ɤ߽񤭤���ݡ�ư��⡼�ɤ��ּ�ư�ꥵ����
 *  �⡼�ɡפξ�硤���󥵥����ϻ��ꤵ�줿�����ֹ�˽��äƼ�ưŪ�˥ꥵ��������
 *  �ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ�硤���󥵥�����Ķ�������Ǥ��ͤ�����
 *  ���Ƥ⡤̵�뤵�������ǥ��顼�ȤϤʤ�ޤ��󡥸��ߤ����󥵥�����Ķ��������
 *  ���ͤ�������Ԥ��ˤϡ�ͽ�� resize() ���дؿ��ʤɤ����󥵥������ĥ����ɬ
 *  �פ�����ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����󥵥�����Ķ�������Ǥ��ɤ߼��� NULL���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    uintptr���Ǥ��ͤλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������(��ư�ꥵ�����⡼��)
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  fx_cs��Ʊ��
 *
 */
inline const fcomplex &mdarray::fx( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->fx_cs(idx0, idx1, idx2);
}

/**
 * @brief  dx_cs��Ʊ��
 *
 */
inline const dcomplex &mdarray::dx( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->dx_cs(idx0, idx1, idx2);
}

/**
 * @brief  ldx_cs��Ʊ��
 *
 */
inline const ldcomplex &mdarray::ldx( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->ldx_cs(idx0, idx1, idx2);
}

/**
 * @brief  f_cs��Ʊ��
 *
 */
inline const float &mdarray::f( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->f_cs(idx0, idx1, idx2);
}

/**
 * @brief  d_cs��Ʊ��
 *
 */
inline const double &mdarray::d( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->d_cs(idx0, idx1, idx2);
}

/**
 * @brief  ld_cs��Ʊ��
 *
 */
inline const long double &mdarray::ld( ssize_t idx0, ssize_t idx1,
				ssize_t idx2 ) const
{
    return this->ld_cs(idx0, idx1, idx2);
}

/**
 * @brief  c_cs��Ʊ��
 *
 */
inline const unsigned char &mdarray::c( ssize_t idx0, ssize_t idx1, 
				 ssize_t idx2 ) const
{
    return this->c_cs(idx0, idx1, idx2);
}

/**
 * @brief  s_cs��Ʊ��
 *
 */
inline const short &mdarray::s( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->s_cs(idx0, idx1, idx2);
}

/**
 * @brief  i_cs��Ʊ��
 *
 */
inline const int &mdarray::i( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->i_cs(idx0, idx1, idx2);
}

/**
 * @brief  l_cs��Ʊ��
 *
 */
inline const long &mdarray::l( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->l_cs(idx0, idx1, idx2);
}

/**
 * @brief  ll_cs��Ʊ��
 *
 */
inline const long long &mdarray::ll(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->ll_cs(idx0, idx1, idx2);
}

/**
 * @brief  i16_cs��Ʊ��
 *
 */
inline const int16_t &mdarray::i16(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->i16_cs(idx0, idx1, idx2);
}

/**
 * @brief  i32_cs��Ʊ��
 *
 */
inline const int32_t &mdarray::i32(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->i32_cs(idx0, idx1, idx2);
}

/**
 * @brief  i64_cs��Ʊ��
 *
 */
inline const int64_t &mdarray::i64(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->i64_cs(idx0, idx1, idx2);
}

/**
 * @brief  z_cs��Ʊ��
 *
 */
inline const size_t &mdarray::z( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->z_cs(idx0, idx1, idx2);
}

/**
 * @brief  sz_cs��Ʊ��
 *
 */
inline const ssize_t &mdarray::sz( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->sz_cs(idx0, idx1, idx2);
}

/**
 * @brief  b_cs��Ʊ��
 *
 */
inline const bool &mdarray::b( ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->b_cs(idx0, idx1, idx2);
}

/**
 * @brief  p_cs��Ʊ��
 *
 */
inline const uintptr_t &mdarray::p(ssize_t idx0, ssize_t idx1, ssize_t idx2) const
{
    return this->p_cs(idx0, idx1, idx2);
}
#endif

/* read only version */

/**
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (fcomplex ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN + NAN*I ���֤�ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 * @return     fcomplex ���Ǥ��ͤλ���
 * @throw      ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *             �Х��ȥ�������꾮�������
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (dcomplex ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN + NAN*I ���֤�ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 * @return     dcomplex ���Ǥ��ͤλ���
 * @throw      ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *             �Х��ȥ�������꾮�������
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (ldcomplex ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN + NAN*I ���֤�ޤ���
 *
 * @param      idx0 �����ֹ�0�μ���(1������)�������ֹ�
 * @param      idx1 �����ֹ�1�μ���(2������)�������ֹ�(��ά��)
 * @param      idx2 �����ֹ�2�μ���(3������)�������ֹ�(��ά��)
 * @return     ldcomplex ���Ǥ��ͤλ���
 * @throw      ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *             �Х��ȥ�������꾮�������
 * @note       �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (float ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    float���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (double ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    double���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (long double ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NAN���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    long double���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (unsigned char ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_UCHAR ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    char���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (short ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_SHORT ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    short���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (int ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (long ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_LONG ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    long���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (long long��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_LLONG ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    long long���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (int16_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT16 ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int16_t���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (int32_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT32 ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int32_t���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  ���ꤵ�줿1�����ͤ��ɤ߽Ф� (int64_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�INDEF_INT64 ���֤�ޤ���<br>
 *  INDEF ���ͤˤĤ��Ƥϡ��Ʒ��ˤ�����Ǿ��������ͤ����ꤵ��Ƥ��ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    int64_t���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief ���ꤵ�줿1�����ͤ��ɤ߽Ф� (size_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�0 ���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    size_t���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief ���ꤵ�줿1�����ͤ��ɤ߽Ф� (ssize_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�0 ���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    ssize_t���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief ���ꤵ�줿1�����ͤ��ɤ߽Ф� (bool ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�false ���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    bool���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief ���ꤵ�줿1�����ͤ��ɤ߽Ф� (uintptr_t ��)
 *
 *  idx0��idx1��idx2 �ǻ��ꤵ�줿�������Ǥ��ͤ��֤��ޤ���
 *  �ͤ����ꤹ�뤳�ȤϤǤ��ޤ���<br>
 *  ���󥵥�����Ķ�������Ǥ��ɤ߼��ȡ�NULL ���֤�ޤ���
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    uintptr���Ǥ��ͤλ���
 * @throw     ���֥�������������ǤΥХ��ȥ������������дؿ�������ͤη���
 *            �Х��ȥ�������꾮�������
 * @note      �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ�������
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
 * @brief  �������Ƭ���ǤΥ��ɥ쥹�����
 *
 *  ���Ȥ������������Ƭ���ǤΥ��ɥ쥹���֤��ޤ���
 *  data_ptr() ���дؿ��ξ�硤��const��°���ʤ��Υ��дؿ���
 *  ��const��°������Υ��дؿ��Τɤ��餬�Ȥ��뤫�ϡ�
 *  ���֥������ȤΡ�const��°����̵ͭ�Ǽ�ưŪ�˷�ޤ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @return    ��Ƭ���ǤΥ��ɥ쥹
 */
inline void *mdarray::data_ptr()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_arr_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  �������Ƭ���ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 *  ���Ȥ������������Ƭ���ǤΥ��ɥ쥹���֤��ޤ���
 *  data_ptr() ���дؿ��ξ�硤��const��°���ʤ��Υ��дؿ���
 *  ��const��°������Υ��дؿ��Τɤ��餬�Ȥ��뤫�ϡ�
 *  ���֥������ȤΡ�const��°����̵ͭ�Ǽ�ưŪ�˷�ޤ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @return    ��Ƭ���ǤΥ��ɥ쥹
 * @note      �ɤ߼�����ѤΥ��ɥ쥹��������ޤ���
 */
inline const void *mdarray::data_ptr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_arr_rec;
}
#endif

/**
 * @brief  �������Ƭ���ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 *  ���Ȥ������������Ƭ���ǤΥ��ɥ쥹���֤��ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @return    ��Ƭ���ǤΥ��ɥ쥹
 * @note      �ɤ߼�����ѤΥ��ɥ쥹��������ޤ���
 */
inline const void *mdarray::data_ptr_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_arr_rec;
}

/* ��ư�ꥵ�����Ϥ��ʤ� */
/**
 * @brief  ����λ������ǤΥ��ɥ쥹�����
 *
 *  ���Ȥ���������λ������ǤΥ��ɥ쥹���֤��ޤ���
 *  data_ptr() ���дؿ��ξ�硤��const��°���ʤ��Υ��дؿ���
 *  ��const��°������Υ��дؿ��Τɤ��餬�Ȥ��뤫�ϡ�
 *  ���֥������ȤΡ�const��°����̵ͭ�Ǽ�ưŪ�˷�ޤ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    �������ǤΥ��ɥ쥹
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
 * @brief  ����λ������ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 *  ���Ȥ���������λ������ǤΥ��ɥ쥹���֤��ޤ���
 *  ��const��°���ʤ��Υ��дؿ�����const��°������Υ��дؿ��Τɤ��餬
 *  �Ȥ��뤫�ϡ����֥������ȤΡ�const��°����̵ͭ�Ǽ�ưŪ�˷�ޤ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    �������ǤΥ��ɥ쥹
 * @note      �ɤ߼�����ѤΥ��ɥ쥹��������ޤ��� 
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
 * @brief  ����λ������ǤΥ��ɥ쥹����� (�ɼ�����)
 *
 *  ���Ȥ���������λ������ǤΥ��ɥ쥹���֤��ޤ���
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param     idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�
 * @param     idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(��ά��)
 * @param     idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(��ά��)
 * @return    �������ǤΥ��ɥ쥹
 * @note      �ɤ߼�����ѤΥ��ɥ쥹��������ޤ��� 
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
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹�����
 *
 * @param  use ���Ѥ������true(��ǽon)��<br>
 *             ���Ѥ��ʤ�����false(��ǽoff)��
 */
inline void *const *mdarray::data_ptr_2d( bool use )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    if ( use == false ) {				/* ��ǽ off */
	/* �����ݥ��󥿤�ȤäƤ������off�ˤǤ��ʤ� */
	if ( this->extptr_2d_rec == NULL ) this->free_arr_ptr_2d();
    }
    else {
	if ( this->_arr_ptr_2d_rec == NULL ) {
	    this->update_arr_ptr_2d(true);		/* ��ǽ on */
	}
    }

    return (void *const *)(this->_arr_ptr_2d_rec);
}

/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹�����
 */
inline void *const *mdarray::data_ptr_2d()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (void *const *)(this->_arr_ptr_2d_rec);
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const void *const *mdarray::data_ptr_2d() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *)(this->_arr_ptr_2d_rec);
}
#endif

/**
 * @brief  �������������줿2���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const void *const *mdarray::data_ptr_2d_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *)(this->_arr_ptr_2d_rec);
}

/* acquisition of address table for 3d array */
/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹�����
 *
 * @param  use ���Ѥ������true(��ǽon)��<br>
 *             ���Ѥ��ʤ�����false(��ǽoff)��
 */
inline void *const *const *mdarray::data_ptr_3d( bool use )
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    if ( use == false ) {				/* ��ǽ off */
	/* �����ݥ��󥿤�ȤäƤ������off�ˤǤ��ʤ� */
	if ( this->extptr_3d_rec == NULL ) this->free_arr_ptr_3d();
    }
    else {
	if ( this->_arr_ptr_3d_rec == NULL ) {
	    this->update_arr_ptr_3d(true);		/* ��ǽ on */
	}
    }

    return (void *const *const *)(this->_arr_ptr_3d_rec);
}

/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹�����
 */
inline void *const *const *mdarray::data_ptr_3d()
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (void *const *const *)(this->_arr_ptr_3d_rec);
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const void *const *const *mdarray::data_ptr_3d() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *const *)(this->_arr_ptr_3d_rec);
}
#endif

/**
 * @brief  �������������줿3���������ѥݥ�������Υ��ɥ쥹����� (�ɼ�����)
 */
inline const void *const *const *mdarray::data_ptr_3d_cs() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);

    return (const void *const *const *)(this->_arr_ptr_3d_rec);
}


/**
 * @brief  ����μ��������������Υ��ɥ쥹�����
 *
 * @return    ���Ȥ���������μ��������������Υ��ɥ쥹
 *            
 */
inline const size_t *mdarray::cdimarray() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_size_rec;
}

/**
 * @brief  ��������ĥ���ν���ͤ����
 *
 * @return    ���Ȥ����ĥ�������ĥ���ν���ͤ���Ǽ����Ƥ��륢�ɥ쥹���֤��ޤ�
 *            (����ͤ�̤����ξ���NULL ���֤�ޤ�)
 */
inline const void *mdarray::default_value_ptr() const
{
    if ( this->shallow_copy_dest_obj != NULL ||
	 this->shallow_copy_src_obj != NULL ) this->cleanup_shallow_copy(true);
    return this->_default_rec;
}

/**
 * @brief  ���ꤵ�줿���Ǥΰ��֤��֤� (�ꥵ��������)
 *
 *  ������ϰϳ������Ǥ���ꤷ�����ꥵ������Ԥ���
 *
 * @param   ix0 1�����ܤ������ֹ�
 * @param   ix1 2�����ܤ������ֹ�
 * @param   ix2 3�����ܤ������ֹ�
 * @return  �����������ϰ����ֹ�<br>
 *          �������������ʤ��Ȥ���-1
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
	  /* �ǹ⼡���μ���ꥵ�������� */
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
	      /* �ǹ⼡���μ���ꥵ�������� */
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
	      /* �ǹ⼡���μ���ꥵ�������� */
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
 * @brief  ���ꤵ�줿���Ǥΰ��֤��֤� (�ꥵ�����ʤ�)
 *
 *  ������ϰϳ������Ǥ���ꤷ�Ƥ�ꥵ�����ϹԤ�ʤ���
 *
 * @param   ix0 1�����ܤ������ֹ�
 * @param   ix1 2�����ܤ������ֹ�
 * @param   ix2 3�����ܤ������ֹ�
 * @return  �����������ϰ����ֹ�<br>
 *          �������������ʤ��Ȥ���-1 
 * @note    ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  default�ͤ� private �ѥ������å���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
inline void *mdarray::default_rec_ptr()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    return this->_default_rec;
}

/** 
 * @brief  ����Ĺ�� private �ѥ������å���
 * @note   ���Υ��дؿ��� private �Ǥ���
 */
inline size_t *mdarray::size_rec_ptr()
{
    this->cleanup_shallow_copy(true);	/* �Хåե���ľ�����뤿��Ƥ� */

    return this->_size_rec;
}


/**
 * @example  examples_sllib/array_digest.cc
 *           ¿����������͡��ʵ�ǽ�Υǥ⡦������
 */

/**
 * @example  examples_sllib/array_basic.cc
 *           ¿��������ΰ������δ��ܤ򼨤���������
 */

/**
 * @example  examples_sllib/array_edit.cc
 *           ¿����������Խ�����
 */

/**
 * @example  examples_sllib/array_fast_access.cc
 *           3��������ޤǤ����Ǥ˥ݥ����ѿ��ǹ�®�˥�������������
 */

/**
 * @example  examples_sllib/array_idl.cc
 *           ¿��������� IDL ���ε�ˡ����ʬ�����黻��Ԥʤ���
 */

/**
 * @example  examples_sllib/array_math.cc
 *           ¿����������Ф�����شؿ���Ȥä���
 */


}


/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���(�¿���ʣ�ǿ��б���)
 *
 * �ܥޥ�������Ѥ��뤳�Ȥǡ����Ƥη��Ѵ��Υѥ���������夷���ޥ����ΰ�����
 * Ÿ�����뤳�Ȥ��Ǥ��롥�ܥޥ�������Ӥϰʲ��Σ�����˶��̤���롥
 *
 *  1. SEL_FUNC
 *    ���˹��פ��������줫�� MACRO_TO_DO �Τߤ�Ŭ�Ѥ������ǡ��ޥ���Ÿ��
 *    ����롥
 *    els �ˤ�ɬ�� else �����ꤵ��ʤ���Фʤ餺��MACRO_TO_DO ��ɬ�� if ��
 *    �Ϥޤ�ɾ�����η���Ȥ�ʤ���Фʤ�ʤ������ΤȤ���
 *      if MACRO( ... ) else if MACRO( ... ) ...
 *    ���Τ褦��Ÿ������롥
 *
 *  2. MAKE_FUNC
 *    ���ƤΥѥ������ MACRO_TO_DO ��Ÿ�����롥
 *    els �ˤϲ�����ꤷ�ƤϤʤ�ʤ������ΤȤ���
 *      MACRO( ... ) MACRO( ... ) ...
 *    ���Τ褦��Ÿ������롥
 *
 * @param  MACRO_TO_DO Ÿ������ޥ���̾(SEL_FUNC/MAKE_FUNC)
 * @param  f2c               float ������  8�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  f2i16             float ������ 16�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  f2i32             float ������ 32�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  f2i64             float ������ 64�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  d2c              double ������  8�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  d2i16            double ������ 16�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  d2i32            double ������ 32�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  d2i64            double ������ 64�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ld2c        long double ������  8�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ld2i16      long double ������ 16�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ld2i32      long double ������ 32�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ld2i64      long double ������ 64�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  fx2c           fcomplex ������  8�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  fx2i16         fcomplex ������ 16�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  fx2i32         fcomplex ������ 32�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  fx2i64         fcomplex ������ 64�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  dx2c           dcomplex ������  8�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  dx2i16         dcomplex ������ 16�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  dx2i32         dcomplex ������ 32�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  dx2i64         dcomplex ������ 64�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ldx2c         ldcomplex ������  8�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ldx2i16       ldcomplex ������ 16�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ldx2i32       ldcomplex ������ 32�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  ldx2i64       ldcomplex ������ 64�ӥå� ���������Ѵ����뤿��δؿ�̾
 * @param  els         MACRO_TO_DO �� if else ʸ������硤else �����ꤹ�롥
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���(�¿��Τ�)
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���(ʣ�ǿ��Τ�)
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���(1����η��Τߤξ��)
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���(ʣ�ǿ���1����η��Τߤξ��)
 *
 */
#define SLI__MDARRAY__DO_OPERATION_1TYPE_COMPLEX(MACRO_TO_DO,els) \
      MACRO_TO_DO(sli__md_fcomplex,FCOMPLEX_ZT,fcomplex) \
  els MACRO_TO_DO(sli__md_dcomplex,DCOMPLEX_ZT,dcomplex) \
  els MACRO_TO_DO(sli__md_ldcomplex,LDCOMPLEX_ZT,ldcomplex)

/* NOT for doxygen style
 *
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���
 * (double���������η��ξ�硥���Ѵ��ϺǸ�ΰ����ǥ��ݡ��ȡ�
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���
 * (double���������η�(notʣ�ǿ�)�ξ�硥���Ѵ��ϺǸ�ΰ����ǥ��ݡ��ȡ�
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���
 * (float���������η��ξ�硥���Ѵ��ϺǸ�ΰ����ǥ��ݡ��ȡ�
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
 * @brief SEL_FUNC/MAKE_FUNC ���Ÿ���ѥޥ���
 * (float���������η�(notʣ�ǿ�)�ξ�硥���Ѵ��ϺǸ�ΰ����ǥ��ݡ��ȡ�
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
