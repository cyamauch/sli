/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 13:20:29 cyamauch> */

#ifndef _SLI__FITS_IMAGE_H
#define _SLI__FITS_IMAGE_H 1

/**
 * @file   fits_image.h
 * @brief  FITS の Image HDU を表現するクラス fits_image の定義
 */

#include "fits_hdu.h"

#ifdef BUILD_SFITSIO
#include <sli/numeric_minmax.h>
#include <sli/mdarray.h>
#else
#include "numeric_minmax.h"
#include "mdarray.h"
#endif

namespace sli
{

#ifdef SLI__USE_CMATH
  using std::isfinite;
#endif

  /*
   * fits_image class
   */

  class fitscc;

  /*
   * sil::fits_image class (inherited class of sli::fits_hdu) expresses an
   * image HDU of FITS data structure, and manages multidimensional array using
   * sli::mdarray class.  Objects of sil::fits_image class are usually managed
   * by an object of sli::fitscc class.
   *
   * Programmers usually use APIs for Image HDU like this:
   *   fits.image("Primary").trim( ... );
   * When a number of member functions are used, create a reference:
   *   fits_image &pri = fits.image("Primary");
   *   pri.trimf( ... );
   *   pri. ...
   *
   */

  /**
   * @class  sli::fits_image
   * @brief  FITS の Image HDU を表現するクラス
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fits_image : public fits_hdu

  {
    friend class fitscc;

  public:
    /* constructor & destructor */
    fits_image();
    fits_image(const fits_image &obj);
    ~fits_image();

    /* complete initialization */
    fits_image &operator=(const fits_image &obj);
    fits_image &init();					/* overridden */
    fits_image &init( const fits_image &obj );		/* overridden */

    /* complete initialization (added) */
    virtual fits_image &init( int type, 
			    long naxis0 = 0, long naxis1 = 0, long naxis2 = 0);
    virtual fits_image &init( int type, const long naxisx[], long ndim,
			      bool buf_init );

    /* conversion of type, BZERO, BSCALE and BLANK                     */
    /* All pixels are converted, therefore, a byte length of internal  */
    /* buffer is changed if necessary.                                 */
    /* These member functions are useful for direct access of internal */
    /* buffer, when FITS files have BZERO, etc.                        */
    virtual fits_image &convert_type( int new_type );
    virtual fits_image &convert_type( int new_type, double new_zero );
    virtual fits_image &convert_type( int new_type, double new_zero,
				      double new_scale);
    virtual fits_image &convert_type( int new_type, double new_zero, 
				      double new_scale, long long new_blank );

    /* virtual mdarray converted_array( int sz_type ) const; */

    /* update BZERO, BSCALE and BLANK                    */
    /* These member function do not change image buffer. */
    virtual fits_image &assign_bzero( double zero, int prec = 15 );
    virtual fits_image &assign_bscale( double scale, int prec = 15 );
    virtual fits_image &assign_blank( long long blank );
    virtual fits_image &assign_bunit( const char *unit );
    virtual fits_image &erase_bzero();
    virtual fits_image &erase_bscale();
    virtual fits_image &erase_blank();
    virtual fits_image &erase_bunit();

    /* swap contents between self and obj */
    fits_image &swap( fits_image &obj );		/* overridden */

    /* copy all to another fits_image object */
    virtual void copy( fits_image *dest_img ) const;
    /* not recommended */
    virtual void copy( fits_image &dest_img ) const;

    /* move all contents of an image into another object. */
    /* an array of this object becomes empty.             */
    /* object's attributes are also copied.               */
    virtual fits_image &cut( fits_image *dest_img );

    /* interchange rows and columns */
    virtual fits_image &transpose_xy();

    /* interchange xyz to zxy */
    virtual fits_image &transpose_xyz2zxy();

    /* rotate image                            */
    /*   angle: 90,-90, or 180 (anticlockwise) */
    virtual fits_image &rotate_xy( int angle );


    /*
     * obtain basic information of this image.
     *
     * Note that argument `axis' is 0-indexed.
     */

    /* returns type.                */
    /* (e.g., FITS::DOUBLE_T, etc.) */
    virtual int type() const;

    /* number of axes */
    virtual long dim_length() const;
    virtual long axis_length() const;		/* same as dim_length() */

    /* number of all pixels */
    virtual long length() const;

    /* number of pixels in axis               */
    /* (i.e., number of NAXIS1, NAXIS2, etc.) */
    virtual long length( long axis ) const;

    /* NOTE: layer_length() returns (NAXIS3 * NAXIS4 * ...) when NAXIS = 3 */
    /*       or more.                                                      */
    virtual long col_length() const;		/* length of columns */
    virtual long x_length() const;		/* same as col_length() */
    virtual long row_length() const;		/* length of rows */
    virtual long y_length() const;		/* same as row_length() */
    virtual long layer_length() const;		/* length of layers */
    virtual long z_length() const;		/* same as layer_length() */

    /* the length of x * y */
    virtual long col_row_length() const;

    /* the length of y * z (degenerated z) */
    virtual long row_layer_length() const;

    /* returns byte length of a pixel. */
    /* (e.g., sizeof(fits::double_t))  */
    virtual long bytes() const;


    /* obtain information of BZERO, BSCALE and BLANK values of FITS header. */
    virtual double bzero() const;
    virtual double bscale() const;
    virtual long long blank() const;
    virtual const char *bunit() const;

    /* returns true when the value of header record exists */
    virtual bool bzero_is_set() const;
    virtual bool bscale_is_set() const;
    virtual bool blank_is_set() const;
    virtual bool bunit_is_set() const;

    /* returns pixel offset from original FITS image. */
    /* useful to fix CRPIXx when cropping an image using [...] expression */
    virtual long coord_offset( long axis ) const;

    /* returns true if loaded image is flipped by [...] expression  */
    virtual bool is_flipped( long axis ) const;


    /*
     * member functions to read a pixel
     *
     * Note that these functions have some overhead of calling functions.
     * Typical purposes are following cases:
     *   1. processings for smaller number of pixels such as a bad pixel mask,
     *      displaying a pixel value pointed by mouse cursor in GUI, etc.
     *   2. performing heavy calculation such as WCS per a pixel.
     *      (In this case, overhead of dvalue() is negligible.)
     *
     */

    /* high-level access: applies BZERO and BSCALE */
    virtual double dvalue( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual long lvalue( long idx0,
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual long long llvalue( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;

    /* low-level access: does not apply BZERO and BSCALE */
    virtual double double_value( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual float float_value( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual long long longlong_value( long idx0,
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual long long_value( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual short short_value( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual unsigned char byte_value( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;


    /* 
     * member functions to write a pixel
     *
     * Note that these functions have some overhead of calling functions.
     * Main purposes are following cases:
     *   1. smaller number of pixels to be processed.
     *   2. performing heavy calculation such as WCS per a pixel.
     *      (In this case, overhead of dvalue() is negligible.)
     */

    /* high-level access: applies BZERO and BSCALE */
    virtual fits_image &assign( double value,
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );

    /* low-level access: does not apply BZERO and BSCALE */
    virtual fits_image &assign_double( double value,
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
    virtual fits_image &assign_float( float value, 
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
    virtual fits_image &assign_longlong( long long value,
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
    virtual fits_image &assign_long( long value, 
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
    virtual fits_image &assign_short( short value, 
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
    virtual fits_image &assign_byte( unsigned char value,
	      long idx0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );


    /*
     * C-like APIs to input and output data
     */

    /* low-level I/O of traditional style of arguments              */
    /* Note that these member functions cannot check type validity. */
    virtual ssize_t get_data( void *dest_buf, size_t buf_size, long idx0 = 0,
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    virtual ssize_t put_data( const void *src_buf, size_t buf_size,
	  long idx0 = 0, long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );


    /*
     * member functions to update length, type, etc.
     */

    /* increase dimension */
    virtual fits_image &increase_dim();
    virtual fits_image &increase_axis();	/* same as increase_dim() */

    /* decrease dimension */
    virtual fits_image &decrease_dim();
    virtual fits_image &decrease_axis();	/* same as decrease_dim() */

    /* update image size                            */
    /*   axis: an axis for modification (0-indexed) */
    /*   size: number of pixels to be set           */
    virtual fits_image &resize( long axis, long size );

    /* change the length of the 1-d array */
    virtual fits_image &resize_1d( long x_len );
    /* change the length of the 2-d array */
    virtual fits_image &resize_2d( long x_len, long y_len );
    /* change the length of the 3-d array */
    virtual fits_image &resize_3d( long x_len, long y_len, long z_len );

    /* update image size                                 */
    /*   naxisx:   number of elements for each dimension */
    /*   ndim:     number of dimension                   */
    /*   buf_init: set true to initialize new elements   */
    virtual fits_image &resize( const long naxisx[], long ndim,
				bool buf_init );

    /* update image size                                     */
    /* specify a string like "3,2" using printf() style args */
    virtual fits_image &resizef( const char *exp_fmt, ... );
    virtual fits_image &vresizef( const char *exp_fmt, va_list ap );

    /* update image size relatively                     */
    /*   axis:     an axis for modification (0-indexed) */
    /*   size_rel: number of pixels to be increased     */
    virtual fits_image &resizeby( long axis, long size_rel );

    /* change the length of the 1-d array relatively */
    virtual fits_image &resizeby_1d( long x_len );
    /* change the length of the 2-d array relatively */
    virtual fits_image &resizeby_2d( long x_len, long y_len );
    /* change the length of the 3-d array relatively */
    virtual fits_image &resizeby_3d(long x_len, long y_len, long z_len);

    /* update image size relatively                                         */
    /*   naxisx_rel:  number of elements to be increased for each dimension */
    /*   ndim:        number of dimension                                   */
    /*   buf_init:    set true to initialize new elements                   */
    virtual fits_image &resizeby( const long naxisx_rel[], long ndim,
				  bool buf_init );

    /* update image size relatively                           */
    /* specify a string like "3,-2" using printf() style args */
    virtual fits_image &resizebyf( const char *exp_fmt, ... );
    virtual fits_image &vresizebyf( const char *exp_fmt, va_list ap );

    /* change length of image array without adjusting buffer contents */
    /*   naxisx:   number of elements for each dimension              */
    /*   ndim:     number of dimension                                */
    /*   buf_init: set true to initialize new elements                */
    virtual fits_image &reallocate( const long naxisx[], long ndim,
				    bool buf_init );

    /* free current image buffer and alloc new memory    */
    /*   naxisx:   number of elements for each dimension */
    /*   ndim:     number of dimension                   */
    /*   buf_init: set true to initialize new elements   */
    virtual fits_image &allocate( const long naxisx[], long ndim,
				  bool buf_init );

    /* setup default value when resizing */
    /* high-level */
    virtual fits_image &assign_default( double value );
    /* low-level */
    virtual fits_image &assign_default_value( const void *value_ptr );

    /* insert a blank section */
    virtual fits_image &insert( size_t dim_index, long idx, size_t len );

    /* erase a section */
    virtual fits_image &erase( size_t dim_index, long idx, size_t len );

    /* copy values between elements (without automatic resizing) */
    /* 移動(バッファのサイズは変更しない)                        */
    virtual fits_image &move( long dim_index, 
			      long idx_src, long len, long idx_dst, bool clr );

    /* copy values between elements (with automatic resizing) */
    /* 移動(バッファのサイズは必要に応じて変更する)           */
    virtual fits_image &cpy( long dim_index, 
			     long idx_src, long len, long idx_dst, bool clr );

    /* replace values between elements */
    /* バッファ内での入れ換え */
    virtual fits_image &swap( long dim_index, 
			      long idx_src, long len, long idx_dst );

    /* extract a section           */
    /* see also trimf() and trim() */
    virtual fits_image &crop( long dim_index, long idx, long len );

    /* flip a section   */
    /* see also flipf() */
    virtual fits_image &flip( long dim_index, long idx, long len );

    /*
     * following member functions for numeric conversions of raw pixel values
     * provided by mdarray class can be used via the reference .data_array()
     *
     * Example:
     *   - round off decimals of raw values in all pixels
     *   fits.image("Primary").data_array().round();
     */
    /*
       - raise decimals to the next whole number in a float/double type value
         .data_array().ceil();
      
       - devalue decimals in a float/double type value
         .data_array().floor();
      
       - round off decimals in a float/double type value
         .data_array().round();
      
       - omit decimals in a float/double type value
         .data_array().trunc();
      
       - absolute value of all elements
         .data_array().abs();

     */

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
    virtual fits_image sectionf( const char *exp_fmt, ... ) const;
    virtual fits_image vsectionf( const char *exp_fmt, va_list ap ) const;

    /* copy all or a section to another fits_image object */
    /* Flipping elements is supported                     */
    virtual void copyf( fits_image *dest, const char *exp_fmt, ... ) const;
    virtual void vcopyf( fits_image *dest, 
			 const char *exp_fmt, va_list ap ) const;

    /* trim a section                 */
    /* Flipping elements is supported */
    virtual fits_image &trimf( const char *exp_fmt, ... );
    virtual fits_image &vtrimf( const char *exp_fmt, va_list ap );

    /* flip elements in a section */
    virtual fits_image &flipf( const char *exp_fmt, ... );
    virtual fits_image &vflipf( const char *exp_fmt, va_list ap );

    /* interchange rows and columns and copy */
    /* Flipping elements is supported        */
    virtual void transposef_xy_copy( fits_image *dest, 
				     const char *exp_fmt, ... ) const;
    virtual void vtransposef_xy_copy( fits_image *dest, 
				      const char *exp_fmt, va_list ap ) const;

    /* interchange xyz to zxy and copy */
    /* Flipping elements is supported  */
    virtual void transposef_xyz2zxy_copy( fits_image *dest, 
					const char *exp_fmt, ... ) const;
    virtual void vtransposef_xyz2zxy_copy( fits_image *dest, 
				       const char *exp_fmt, va_list ap ) const;

    /* padding existing values in an array */
    virtual fits_image &cleanf( const char *exp_fmt, ... );
    virtual fits_image &vcleanf( const char *exp_fmt, va_list ap );

    /* rewrite element values with a value in a section */
    virtual fits_image &fillf( double value, const char *exp_fmt, ... );
    virtual fits_image &vfillf( double value, 
				const char *exp_fmt, va_list ap );

    virtual fits_image &fillf_via_udf( double value, 
			     void (*func)(double [],double,long, 
					  long,long,long,fits_image *,void *),
			     void *user_ptr, const char *exp_fmt, ... );
    virtual fits_image &vfillf_via_udf( double value, 
			     void (*func)(double [],double,long, 
					  long,long,long,fits_image *,void *),
			     void *user_ptr, const char *exp_fmt, va_list ap );

    /* add a scalar value to element values in a section */
    virtual fits_image &addf( double value, 
			      const char *exp_fmt, ... );
    virtual fits_image &vaddf( double value, 
			       const char *exp_fmt, va_list ap );

    /* subtract a scalar value from element values in a section */
    virtual fits_image &subtractf( double value, 
				   const char *exp_fmt, ... );
    virtual fits_image &vsubtractf( double value, 
				    const char *exp_fmt, va_list ap );

    /* multiply element values in a section by a scalar value */
    virtual fits_image &multiplyf( double value, 
				   const char *exp_fmt, ... );
    virtual fits_image &vmultiplyf( double value, 
				    const char *exp_fmt, va_list ap );

    /* divide element values in a section by a scalar value */
    virtual fits_image &dividef( double value, 
				 const char *exp_fmt, ... );
    virtual fits_image &vdividef( double value, 
				  const char *exp_fmt, va_list ap );

    /* paste up an array object                                           */

    /* paste without operation using fast method of that of .convert().   */
    /* 演算なしペースト: convert() と同じ手法で変換した結果を高速に格納． */
    virtual fits_image &pastef( const fits_image &src,
				const char *exp_fmt, ... );
    virtual fits_image &vpastef( const fits_image &src,
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
    virtual fits_image &pastef_via_udf( const fits_image &src,
			      void (*func)(double [],double [],long, 
					   long,long,long,fits_image *,void *),
			      void *user_ptr, const char *exp_fmt, ... );
    virtual fits_image &vpastef_via_udf( const fits_image &src,
			     void (*func)(double [],double [],long, 
					  long,long,long,fits_image *,void *),
			     void *user_ptr, const char *exp_fmt, va_list ap );

    /* add an array object */
    virtual fits_image &addf( const fits_image &src_img, 
			      const char *exp_fmt, ... );
    virtual fits_image &vaddf( const fits_image &src_img, 
			       const char *exp_fmt, va_list ap );

    /* subtract an array object */
    virtual fits_image &subtractf( const fits_image &src_img, 
				   const char *exp_fmt, ... );
    virtual fits_image &vsubtractf( const fits_image &src_img, 
				    const char *exp_fmt, va_list ap );

    /* multiply an array object */
    virtual fits_image &multiplyf( const fits_image &src_img, 
				   const char *exp_fmt, ... );
    virtual fits_image &vmultiplyf( const fits_image &src_img, 
				    const char *exp_fmt, va_list ap );

    /* divide an array object */
    virtual fits_image &dividef( const fits_image &src_img, 
				 const char *exp_fmt, ... );
    virtual fits_image &vdividef( const fits_image &src_img, 
				  const char *exp_fmt, va_list ap );


    /* --------------------------------------------------------------------- */

    /* 
     * member functions for image manipulation and operations.
     */

    /* returns trimmed array */
    virtual fits_image section( 
		     long col_index, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* copy all or a region to another fits_image object */
    virtual void copy( fits_image *dest_img,
		     long col_index, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* set a section to be copied by move_to() */
    virtual fits_image &move_from( 
				long col_idx, long col_len = FITS::ALL,
				long row_idx=0, long row_len = FITS::ALL, 
				long layer_idx=0, long layer_len = FITS::ALL );

    /* copy a section specified by move_from() */
    virtual fits_image &move_to( 
		       long dest_col, long dest_row = 0, long dest_layer = 0 );

    /* trim a section */
    virtual fits_image &trim( long col_idx, long col_len = FITS::ALL,
			      long row_idx=0, long row_len = FITS::ALL, 
			      long layer_idx=0, long layer_len = FITS::ALL );

    /* flip horizontal within the specified section */
    virtual fits_image &flip_cols( 
				 long col_idx=0, long col_len = FITS::ALL,
				 long row_idx=0, long row_len = FITS::ALL, 
				 long layer_idx=0, long layer_len = FITS::ALL);

    /* flip vertical within the specified section */
    virtual fits_image &flip_rows( 
				long col_idx=0, long col_len = FITS::ALL,
				long row_idx=0, long row_len = FITS::ALL, 
				long layer_idx=0, long layer_len = FITS::ALL );

    /* interchange rows and columns and copy */
    virtual void transpose_xy_copy( fits_image *dest_img, 
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* interchange xyz to zxy and copy */
    virtual void transpose_xyz2zxy_copy( fits_image *dest_img, 
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* rotate and copy a region to another fits_image object */
    /*   angle: 90,-90, or 180 (anticlockwise)               */
    virtual void rotate_xy_copy( fits_image *dest_img, int angle, 
		     long col_index = 0, long col_size = FITS::ALL, 
		     long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;

    /* padding existing elements with default value within specified section */
    virtual fits_image &clean( long col_idx=0, long col_len = FITS::ALL,
			       long row_idx=0, long row_len = FITS::ALL,
			       long layer_idx=0, long layer_len = FITS::ALL );

    /* 数値をいれたり．引数は value だけで十分なのかもしれん…             */
    /* 特定の領域だけ計算したい場合，コピーバッファ側でやればいいので      */

    /* rewrite element values with a scalar value in a rectangular section */
    virtual fits_image &fill( double value, 
			   long col_index = 0, long col_size = FITS::ALL, 
			   long row_index = 0, long row_size = FITS::ALL,
			   long layer_index = 0, long layer_size = FITS::ALL );
    virtual fits_image &fill_via_udf( double value, 
	      void (*func)(double [],double,long, long,long,long,fits_image *,void *),
	      void *user_ptr,
	      long col_index = 0, long col_size = FITS::ALL, 
	      long row_index = 0, long row_size = FITS::ALL,
	      long layer_index = 0, long layer_size = FITS::ALL );

    /* add a scalar value to element values in a rectangular section */
    virtual fits_image &add( double value, 
			  long col_index = 0, long col_size = FITS::ALL, 
			  long row_index = 0, long row_size = FITS::ALL,
			  long layer_index = 0, long layer_size = FITS::ALL );

    /* subtract a scalar value from element values in a rectangular section */
    virtual fits_image &subtract( double value, 
			  long col_index = 0, long col_size = FITS::ALL, 
			  long row_index = 0, long row_size = FITS::ALL,
			  long layer_index = 0, long layer_size = FITS::ALL );

    /* multiply element values in a rectangular section by a scalar value */
    virtual fits_image &multiply( double value, 
			  long col_index = 0, long col_size = FITS::ALL, 
			  long row_index = 0, long row_size = FITS::ALL,
			  long layer_index = 0, long layer_size = FITS::ALL );

    /* divide element values in a rectangular section by a scalar value */
    virtual fits_image &divide( double value, 
			  long col_index = 0, long col_size = FITS::ALL, 
			  long row_index = 0, long row_size = FITS::ALL,
			  long layer_index = 0, long layer_size = FITS::ALL );

    /* member functions to paste up an image */

    /* paste without operation using fast method of that of .convert(). */
    virtual fits_image &paste( const fits_image &src_img,
		   long dest_col = 0, long dest_row = 0, long dest_layer = 0 );

    /* paste with operation: all elements are converted into double type, so */
    /* the performance is inferior to simple version of paste().             */
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
    virtual fits_image &paste_via_udf( const fits_image &src_img,
	      void (*func)(double [],double [],long, long,long,long,fits_image *,void *),
	      void *user_ptr,
	      long dest_col = 0, long dest_row = 0, long dest_layer = 0 );

    /* add an image */
    virtual fits_image &add( const fits_image &src_img,
		   long dest_col = 0, long dest_row = 0, long dest_layer = 0 );

    /* subtract an image */
    virtual fits_image &subtract( const fits_image &src_img,
		   long dest_col = 0, long dest_row = 0, long dest_layer = 0 );

    /* multiply an image */
    virtual fits_image &multiply( const fits_image &src_img,
		   long dest_col = 0, long dest_row = 0, long dest_layer = 0 );

    /* divide an image */
    virtual fits_image &divide( const fits_image &src_img,
		   long dest_col = 0, long dest_row = 0, long dest_layer = 0 );


    /* --------------------------------------------------------------------- */

    /* 
     * member functions for image analysis.
     */

    /* test argument that expresses a rectangular section, and fix them */
    /* if positions are out of range or sizes are too large.            */
    virtual int fix_section_args( long *r_col_index, long *r_col_size,
				  long *r_row_index, long *r_row_size,
				  long *r_layer_index, long *r_layer_size
				  ) const;


    /* horizontally scans the specified section.  A temporary buffer of 1-D */
    /* array is prepared and scan_along_x() returns it.                     */
    /* A scan order is displayed in pseudocode:                             */
    /*  for(...) {      <- layer                                            */
    /*    for(...) {    <- row                                              */
    /*      for(...) {  <- column                                           */
    virtual long beginf_scan_along_x( const char *exp_fmt, ... ) const;
    virtual long vbeginf_scan_along_x( const char *exp_fmt, va_list ap ) const;
    virtual long begin_scan_along_x( 
                     long col_index = 0, long col_size = FITS::ALL,
	             long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;
    virtual double *scan_along_x( long *n, long *x, long *y, long *z ) const;
    virtual float *scan_along_x_f( long *n, long *x, long *y, long *z ) const;
    virtual void end_scan_along_x() const;

    /* vertically scans the specified section.  A temporary buffer of 1-D */
    /* array is prepared and scan_along_y() returns it.                   */
    /* scan order is displayed by pseudocode:                             */
    /*  for(...) {      <- layer                                          */
    /*    for(...) {    <- column                                         */
    /*      for(...) {  <- row                                            */
    virtual long beginf_scan_along_y( const char *exp_fmt, ... ) const;
    virtual long vbeginf_scan_along_y( const char *exp_fmt, va_list ap ) const;
    virtual long begin_scan_along_y( 
                     long col_index = 0, long col_size = FITS::ALL,
	             long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;
    virtual double *scan_along_y( long *n, long *x, long *y, long *z ) const;
    virtual float *scan_along_y_f( long *n, long *x, long *y, long *z ) const;
    virtual void end_scan_along_y() const;

    /* scan the specified section along z-axis.  A temporary buffer of 1-D */
    /* array is prepared and scan_along_z() returns it.                    */
    /* scan order is displayed by pseudocode:                              */
    /*  for(...) {      <- row                                             */
    /*    for(...) {    <- column                                          */
    /*      for(...) {  <- layer                                           */
    virtual long beginf_scan_along_z( const char *exp_fmt, ... ) const;
    virtual long vbeginf_scan_along_z( const char *exp_fmt, va_list ap ) const;
    virtual long begin_scan_along_z( 
                     long col_index = 0, long col_size = FITS::ALL,
	             long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;
    virtual double *scan_along_z( long *n, long *x, long *y, long *z ) const;
    virtual float *scan_along_z_f( long *n, long *x, long *y, long *z ) const;
    virtual void end_scan_along_z() const;

    /* scan the specified 3-D section with plane by plane (zx plane). */
    /* A temporary buffer of 2-D array is prepared.                   */
    /* scan order is displayed by pseudocode:                         */
    /*  for(...) {      <- row                                        */
    /*    for(...) {    <- column                                     */
    /*      for(...) {  <- layer                                      */
    virtual long beginf_scan_zx_planes( const char *exp_fmt, ... ) const;
    virtual long vbeginf_scan_zx_planes( const char *exp_fmt, va_list ap ) const;
    virtual long begin_scan_zx_planes( 
		   long col_index = 0, long col_size = MDARRAY_ALL,
		   long row_index = 0, long row_size = MDARRAY_ALL,
		   long layer_index = 0, long layer_size = MDARRAY_ALL ) const;
    virtual double *scan_zx_planes( 
		       long *n_z, long *n_x, long *x, long *y, long *z ) const;
    virtual float *scan_zx_planes_f( 
		       long *n_z, long *n_x, long *x, long *y, long *z ) const;
    virtual void end_scan_zx_planes() const;

    /* scan the specified 3-D section.  A temporary buffer of 1-D array is */
    /* prepared and scan_a_cube() returns it.                              */
    /*  for(...) {      <- layer                                           */
    /*    for(...) {    <- row                                             */
    /*      for(...) {  <- column                                          */
    virtual long beginf_scan_a_cube( const char *exp_fmt, ... ) const;
    virtual long vbeginf_scan_a_cube( const char *exp_fmt, va_list ap ) const;
    virtual long begin_scan_a_cube( 
                     long col_index = 0, long col_size = FITS::ALL,
	             long row_index = 0, long row_size = FITS::ALL,
		     long layer_index = 0, long layer_size = FITS::ALL ) const;
    /* double version */
    virtual double *scan_a_cube( long *n_x, long *n_y, long *n_z, 
				 long *x, long *y, long *z ) const;
    /* float version */
    virtual float *scan_a_cube_f( long *n_x, long *n_y, long *n_z, 
				  long *x, long *y, long *z ) const;
    virtual void end_scan_a_cube() const;


    /* --------------------------------------------------------------------- */

    /*
     * member functions for direct access to internal buffer
     */

    /* BYTE_T (unsigned 8-bit) */
    virtual fits::byte_t *byte_t_ptr();
    virtual fits::byte_t *byte_t_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::byte_t *byte_t_ptr() const;
    virtual const fits::byte_t *byte_t_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const fits::byte_t *byte_t_ptr_cs() const;
    virtual const fits::byte_t *byte_t_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    /* SHORT_T (16-bit) */
    virtual fits::short_t *short_t_ptr();
    virtual fits::short_t *short_t_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::short_t *short_t_ptr() const;
    virtual const fits::short_t *short_t_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const fits::short_t *short_t_ptr_cs() const;
    virtual const fits::short_t *short_t_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    /* LONG_T (32-bit) */
    virtual fits::long_t *long_t_ptr();
    virtual fits::long_t *long_t_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::long_t *long_t_ptr() const;
    virtual const fits::long_t *long_t_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const fits::long_t *long_t_ptr_cs() const;
    virtual const fits::long_t *long_t_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    /* LONGLONG_T (64-bit) */
    virtual fits::longlong_t *longlong_t_ptr();
    virtual fits::longlong_t *longlong_t_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longlong_t *longlong_t_ptr() const;
    virtual const fits::longlong_t *longlong_t_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const fits::longlong_t *longlong_t_ptr_cs() const;
    virtual const fits::longlong_t *longlong_t_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    /* FLOAT_T */
    virtual fits::float_t *float_t_ptr();
    virtual fits::float_t *float_t_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::float_t *float_t_ptr() const;
    virtual const fits::float_t *float_t_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const fits::float_t *float_t_ptr_cs() const;
    virtual const fits::float_t *float_t_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
    /* DOUBLE_T */
    virtual fits::double_t *double_t_ptr();
    virtual fits::double_t *double_t_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::double_t *double_t_ptr() const;
    virtual const fits::double_t *double_t_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const fits::double_t *double_t_ptr_cs() const;
    virtual const fits::double_t *double_t_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;

    /*
     * member functions that return address table for direct access to 2d image
     *
     * Example:
     *   fits_image &primary = fits.image("Primary");
     *   fits::float_t *const *img_ptr = primary.float_t_ptr_2d();
     *   for ( i=0 ; i < primary.y_length() ; i++ ) {
     *     for ( j=0 ; j < primary.x_length() ; j++ ) {
     *       img_ptr[i][j] = ...;
     *     }
     *   }
     */

    /* BYTE_T (unsigned 8-bit) */
    virtual fits::byte_t *const *byte_t_ptr_2d( bool use );
    virtual fits::byte_t *const *byte_t_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::byte_t *const *byte_t_ptr_2d() const;
#endif
    virtual const fits::byte_t *const *byte_t_ptr_2d_cs() const;
    /* SHORT_T (16-bit) */
    virtual fits::short_t *const *short_t_ptr_2d( bool use );
    virtual fits::short_t *const *short_t_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::short_t *const *short_t_ptr_2d() const;
#endif
    virtual const fits::short_t *const *short_t_ptr_2d_cs() const;
    /* LONG_T (32-bit) */
    virtual fits::long_t *const *long_t_ptr_2d( bool use );
    virtual fits::long_t *const *long_t_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::long_t *const *long_t_ptr_2d() const;
#endif
    virtual const fits::long_t *const *long_t_ptr_2d_cs() const;
    /* LONGLONG_T (64-bit) */
    virtual fits::longlong_t *const *longlong_t_ptr_2d( bool use );
    virtual fits::longlong_t *const *longlong_t_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longlong_t *const *longlong_t_ptr_2d() const;
#endif
    virtual const fits::longlong_t *const *longlong_t_ptr_2d_cs() const;
    /* FLOAT_T */
    virtual fits::float_t *const *float_t_ptr_2d( bool use );
    virtual fits::float_t *const *float_t_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::float_t *const *float_t_ptr_2d() const;
#endif
    virtual const fits::float_t *const *float_t_ptr_2d_cs() const;
    /* DOUBLE_T */
    virtual fits::double_t *const *double_t_ptr_2d( bool use );
    virtual fits::double_t *const *double_t_ptr_2d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::double_t *const *double_t_ptr_2d() const;
#endif
    virtual const fits::double_t *const *double_t_ptr_2d_cs() const;

    /*
     * member functions that return address table for direct access to 3d image
     *
     * Example:
     *   fits_image &primary = fits.image("Primary");
     *   fits::float_t *const *const *img_ptr = primary.float_t_ptr_3d();
     *   for ( i=0 ; i < primary.z_length() ; i++ ) {
     *     for ( j=0 ; j < primary.y_length() ; j++ ) {
     *       for ( k=0 ; k < primary.x_length() ; k++ ) {
     *         img_ptr[i][j][k] = ...;
     *       }
     *     }
     *   }
     */

    /* BYTE_T (unsigned 8-bit) */
    virtual fits::byte_t *const *const *byte_t_ptr_3d( bool use );
    virtual fits::byte_t *const *const *byte_t_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::byte_t *const *const *byte_t_ptr_3d() const;
#endif
    virtual const fits::byte_t *const *const *byte_t_ptr_3d_cs() const;
    /* SHORT_T (16-bit) */
    virtual fits::short_t *const *const *short_t_ptr_3d( bool use );
    virtual fits::short_t *const *const *short_t_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::short_t *const *const *short_t_ptr_3d() const;
#endif
    virtual const fits::short_t *const *const *short_t_ptr_3d_cs() const;
    /* LONG_T (32-bit) */
    virtual fits::long_t *const *const *long_t_ptr_3d( bool use );
    virtual fits::long_t *const *const *long_t_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::long_t *const *const *long_t_ptr_3d() const;
#endif
    virtual const fits::long_t *const *const *long_t_ptr_3d_cs() const;
    /* LONGLONG_T (64-bit) */
    virtual fits::longlong_t *const *const *longlong_t_ptr_3d( bool use );
    virtual fits::longlong_t *const *const *longlong_t_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longlong_t *const *const *longlong_t_ptr_3d() const;
#endif
    virtual const fits::longlong_t *const *const *longlong_t_ptr_3d_cs() const;
    /* FLOAT_T */
    virtual fits::float_t *const *const *float_t_ptr_3d( bool use );
    virtual fits::float_t *const *const *float_t_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::float_t *const *const *float_t_ptr_3d() const;
#endif
    virtual const fits::float_t *const *const *float_t_ptr_3d_cs() const;
    /* DOUBLE_T */
    virtual fits::double_t *const *const *double_t_ptr_3d( bool use );
    virtual fits::double_t *const *const *double_t_ptr_3d();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::double_t *const *const *double_t_ptr_3d() const;
#endif
    virtual const fits::double_t *const *const *double_t_ptr_3d_cs() const;


    /* direct access for any types                                  */
    /* Note that these member functions cannot check type validity. */
    virtual void *data_ptr();
    virtual void *data_ptr( long idx0, 
			  long idx1 = FITS::INDEF, long idx2 = FITS::INDEF );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *data_ptr() const;
    virtual const void *data_ptr( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;
#endif
    virtual const void *data_ptr_cs() const;
    virtual const void *data_ptr_cs( long idx0, 
		    long idx1 = FITS::INDEF, long idx2 = FITS::INDEF ) const;


    /* --------------------------------------------------------------------- */

    /*
     * etc. 
     */

    /* to access image data using mdarray class: mdarray class provides */
    /* fast and useful member functions to manipulate image data.       */

    /* uchar */
    virtual mdarray_uchar &uchar_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray_uchar &uchar_array() const;
#endif
    virtual const mdarray_uchar &uchar_array_cs() const;

    /* int16 */
    virtual mdarray_int16 &int16_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray_int16 &int16_array() const;
#endif
    virtual const mdarray_int16 &int16_array_cs() const;

    /* int32 */
    virtual mdarray_int32 &int32_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray_int32 &int32_array() const;
#endif
    virtual const mdarray_int32 &int32_array_cs() const;

    /* int64 */
    virtual mdarray_int64 &int64_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray_int64 &int64_array() const;
#endif
    virtual const mdarray_int64 &int64_array_cs() const;

    /* float */
    virtual mdarray_float &float_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray_float &float_array() const;
#endif
    virtual const mdarray_float &float_array_cs() const;

    /* double */
    virtual mdarray_double &double_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray_double &double_array() const;
#endif
    virtual const mdarray_double &double_array_cs() const;

    /* any type */
    virtual mdarray &data_array();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const mdarray &data_array() const;
#endif
    virtual const mdarray &data_array_cs() const;


    /* shallow copy を許可する場合に使用 (配列データに対して有効) */
    /* (一時オブジェクトの return の直前で使用)                   */
    /* (fits_hdu::set_scopy_flag() のオーバライド)                           */
    void set_scopy_flag();

    /* --------------------------------------------------------------------- */

  protected:
    /* */
    virtual long get_idx_3d_cs( long ix0, long ix1, long ix2 ) const;
    /* steram io */
    /* overridden member functions */
    ssize_t read_stream( cstreamio &sref, size_t max_bytes_read );
    ssize_t read_stream( cstreamio &sref );
    ssize_t write_stream( cstreamio &sref );
    ssize_t stream_length();
    /* overridden */
    ssize_t save_or_check_data( cstreamio *sptr, void *c_sum_info );
    /* 追加 */
    virtual ssize_t read_stream( fits_hdu *objp, cstreamio &sref );
    virtual ssize_t read_stream( fits_hdu *objp, cstreamio &sref,
			       const char *section_to_read, bool is_last_hdu );
    virtual ssize_t read_stream( fits_hdu *objp, cstreamio &sref, 
			       const char *section_to_read, bool is_last_hdu,
			       size_t max_bytes_read );
    //fits_image img_buffer;	/* copy & paste 用 image バッファ */
    fits_image &setup_sys_header();			/* overridden */

  private:
    void _init_all_data_recs( int sz_type );
    void _setup_data_rec( int sz_type );
    void init_properties( const fits_image &obj );
    long find_sysheader_insert_point( const char *search_kwds[] ) const;
    void set_blank( const long long *new_blank_ptr );
    long get_degenerated_zindex( long num_axisx, long axis_z, 
				 va_list ap ) const;
    void update_zero_of_data_rec();
    void fix_indices( long *col_index, long *row_index, 
		      long *layer_index ) const;
    int read_image_section( cstreamio &sref, ssize_t sz_type,
			   const size_t *n_axisx_org, size_t ndim_org,
			   void *_sect_info, bool is_last_hdu, 
			   const size_t *max_bytes_ptr, size_t *len_read_all );
    ssize_t image_load( const fits_hdu *objp, cstreamio &sref, 
			const char *section_to_read, bool is_last_hdu,
			const size_t *max_bytes_ptr );
    fits_image &convert_image_type( int new_type, 
				    const double *new_zero_ptr, 
				    const double *new_scale_ptr,
				    const long long *new_blank_ptr );
    void prepare_image_fill_paste_fast( const fits_image *src_img,
		ssize_t src_size_type,
		void *prms_p,
		void (**func_cnv_ptr_p)(const void *,void *,size_t,int,void *),
		bool *simple_cnv_ok_p ) const;
    fits_image &image_vfillf_fast( double value,
				   const char *exp_fmt, va_list ap );
    fits_image &image_fill_fast( double value,
		  long col_index, long col_size, long row_index, long row_size,
		  long layer_index, long layer_size );
    void prepare_image_fill( 
	  void *prms_p,
	  void (**func_dst2d_p)(const void *,void *,size_t,int,void *),
	  void (**func_d2dst_p)(const void *,void *,size_t,int,void *) ) const;
    fits_image &image_vfillf( double value, 
       void (*func)(double [],double,long, long,long,long,fits_image *,void *),
       void *user_ptr, const char *exp_fmt, va_list ap );
    fits_image &image_fill( double value,
       void (*func)(double [],double,long, long,long,long,fits_image *,void *),
		  void *user_ptr, 
		  long col_index, long col_size, long row_index, long row_size,
		  long layer_index, long layer_size );
    fits_image &image_vpastef_fast( const fits_image &src_img,
				    const char *exp_fmt, va_list ap );
    fits_image &image_paste_fast( const fits_image &src_img,
			       long dest_col, long dest_row, long dest_layer );
    void prepare_image_paste( const fits_image &src_img,
	  void *p_src_p, void *p_dst_p,
	  void (**func_src2d_p)(const void *,void *,size_t,int,void *),
	  void (**func_dst2d_p)(const void *,void *,size_t,int,void *),
	  void (**func_d2dst_p)(const void *,void *,size_t,int,void *) ) const;
    fits_image &image_vpastef( const fits_image &src_img,
    void (*func)(double [],double [],long, long,long,long,fits_image *,void *),
	      void *user_ptr, const char *exp_fmt, va_list ap );
    fits_image &image_paste( const fits_image &src_img,
    void (*func)(double [],double [],long, long,long,long,fits_image *,void *),
	      void *user_ptr,
	      long dest_col, long dest_row, long dest_layer );
    void prepare_scan_a_section() const;
    void *err_throw_void_p( const char *fnc, const char *lv, const char *mes);
    const void *err_throw_const_void_p( const char *fnc, 
				  const char *lv, const char *mes) const;

  private:
    int type_rec;			/* 型を保持 (FITS::SHORT_T, etc.) */
    mdarray_uchar uchar_data_rec;	/* 画像dataはこれらのいずれかに入る */
    mdarray_int16 int16_data_rec;
    mdarray_int32 int32_data_rec;
    mdarray_int64 int64_data_rec;
    mdarray_float float_data_rec;
    mdarray_double double_data_rec;
    mdarray *data_rec;			/* 上記のいずれかに対するアドレス */
    /* bzero, bscale, blank and bunit */
    tstring bzero_rec;
    tstring bscale_rec;
    tstring blank_rec;
    tstring bunit_rec;
    /* cached bzero, bscale, blank */
    double bzero_double_rec;
    double bscale_double_rec;
    long long blank_r_rec;	/* ヘッダの値をそのまま保存するとこ */
    long long blank_w_rec;	/* 書き出し時に使う．型によって自動制限あり */
    long long blank_longlong_rec;
    long blank_long_rec;
    short blank_short_rec;
    unsigned char blank_byte_rec;
    bool blank_is_set_rec;
    /* information of cropping using foo.fits[...] */
    mdarray_long coord_offset_rec;
    mdarray_bool is_flipped_rec;
    /* scan系のwork: = などではコピーされない */
    heap_mem<char> scan_prms_rec;
    unsigned int scan_use_flags;
    void (*scan_func_cnv_nd_x2d)(const void *,void *,size_t,int,void *);
    void (*scan_func_cnv_nd_x2f)(const void *,void *,size_t,int,void *);
    void (*scan_func_gencpy2d_x2d)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
    void (*scan_func_gencpy2d_x2f)(const void *, void *, 
		       size_t, size_t, size_t, size_t, size_t, size_t, void *);
  };

/*
 * inline member functions
 */

/**
 * @brief  画像データ配列の型種別(FITS::SHORT_T，FITS::FLOAT_T 等)を取得
 */
inline int fits_image::type() const
{
    return this->type_rec;
}

/**
 * @brief  次元数を取得
 */
inline long fits_image::dim_length() const
{
    return this->data_rec->dim_length();
}

/**
 * @brief  次元数を取得
 * @note   fits_image::dim_length() との違いはありません．
 */
inline long fits_image::axis_length() const
{
    return this->data_rec->dim_length();
}

/**
 * @brief  画像データ配列の全要素数を取得
 */
inline long fits_image::length() const
{
    return this->data_rec->length();
}

/**
 * @brief  指定された次元の画像データ配列の長さを返す
 * @param  axis 次元番号 (0-indexed)
 */
inline long fits_image::length( long axis ) const
{
    return this->data_rec->length(axis);
}

/**
 * @brief  画像データ配列の横(カラム)方向の長さを取得
 */
inline long fits_image::col_length() const
{
    return this->data_rec->col_length();
}

/**
 * @brief  画像データ配列の横(カラム)方向の長さを取得
 * @note   fits_image::col_length() との違いはありません．
 */
inline long fits_image::x_length() const
{
    return this->data_rec->col_length();
}

/**
 * @brief  画像データ配列の縦(ロウ)方向の長さを取得
 */
inline long fits_image::row_length() const
{
    return this->data_rec->row_length();
}

/**
 * @brief  画像データ配列の縦(ロウ)方向の長さを取得
 * @note   fits_image::row_length() との違いはありません．
 */
inline long fits_image::y_length() const
{
    return this->data_rec->row_length();
}

/**
 * @brief  画像データ配列のレイヤ方向の長さを取得
 * @note   3時限目以降の(縮退した)長さを返します．
 */
inline long fits_image::layer_length() const
{
    return this->data_rec->layer_length();
}

/**
 * @brief  画像データ配列のレイヤ方向の長さを取得
 * @note   fits_image::layer_length() との違いはありません．
 */
inline long fits_image::z_length() const
{
    return this->data_rec->layer_length();
}

/**
 * @brief  画像データ配列の横×縦の大きさを取得
 */
inline long fits_image::col_row_length() const
{
    return this->data_rec->col_row_length();
}

/**
 * @brief  画像データ配列の縦×レイヤの大きさを取得
 */
inline long fits_image::row_layer_length() const
{
    return this->data_rec->row_layer_length();
}

/**
 * @brief  画像データ配列の1要素のバイト長を取得
 */
inline long fits_image::bytes() const
{
    return this->data_rec->bytes();
}

/**
 * @brief  FITSのBZERO値を取得
 * @note   内部でキャッシュされた値を返すため，高速です．ループ中で使用可能．
 */
inline double fits_image::bzero() const
{
    return this->bzero_double_rec;
}

/**
 * @brief  FITSのBSCALE値を取得
 * @note   内部でキャッシュされた値を返すため，高速です．ループ中で使用可能．
 */
inline double fits_image::bscale() const
{
    return this->bscale_double_rec;
}

/**
 * @brief  FITSのBLANK値を取得
 * @note   内部でキャッシュされた値を返すため，高速です．ループ中で使用可能．
 */
inline long long fits_image::blank() const
{
    return this->blank_r_rec;
}

/**
 * @brief  FITSのBZERO値設定の有無を取得
 */
inline bool fits_image::bzero_is_set() const
{
    return ( 0 < this->bzero_rec.length() ) ? true : false;
}

/**
 * @brief  FITSのBSCALE値設定の有無を取得
 */
inline bool fits_image::bscale_is_set() const
{
    return ( 0 < this->bscale_rec.length() ) ? true : false;
}

/**
 * @brief  FITSのBLANK値設定の有無を取得
 */
inline bool fits_image::blank_is_set() const
{
    return this->blank_is_set_rec;
}

/**
 * @brief  read_stream() で部分読みされた場合の本来の座標からのオフセットを取得
 * @param  axis 次元番号 (0-indexed)
 */
inline long fits_image::coord_offset( long axis ) const
{
    const long n_rec = this->coord_offset_rec.length();
    if ( axis < 0 ) return 0;
    else if ( axis < n_rec ) return this->coord_offset_rec.at_cs(axis);
    else return 0;
}

/**
 * @brief  各次元について read_stream() で反転読みされたかどうかをテスト
 * @param  axis 次元番号 (0-indexed)
 */
inline bool fits_image::is_flipped( long axis ) const
{
    const long n_rec = this->is_flipped_rec.length();
    if ( axis < 0 ) return false;
    else if ( axis < n_rec ) return this->is_flipped_rec.at_cs(axis);
    else return false;
}

/* HIGH LEVEL */
/**
 * @brief  1つのピクセルの値を取得 (高レベル・double型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO，BSCALE，BLANK についての計算・変換が行なわれます．<br>
 *         fixpix のように少数のピクセルを扱う時に使います．パフォーマンスを
 *         求める用途には向いていません．
 */
inline double fits_image::dvalue( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    double v;
    if ( idx < 0 ) return NAN;
    else {
	const void *data_ptr = this->data_rec->data_ptr_cs();
	if ( this_type == FITS::DOUBLE_T ) {
	    v = ((const fits::double_t *)(data_ptr))[idx];
	}
	else if ( this_type == FITS::FLOAT_T ) {
	    v = ((const fits::float_t *)(data_ptr))[idx];
	}
	else if ( this_type == FITS::SHORT_T ) {
	    const fits::short_t v0 = ((const fits::short_t *)(data_ptr))[idx];
	    if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 ) v=NAN;
	    else v = v0;
	}
	else if ( this_type == FITS::LONG_T ) {
	    const fits::long_t v0 = ((const fits::long_t *)(data_ptr))[idx];
	    if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 ) v=NAN;
	    else v = v0;
	}
	else if ( this_type == FITS::BYTE_T ) {
	    const fits::byte_t v0 = ((const fits::byte_t *)(data_ptr))[idx];
	    if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 ) v=NAN;
	    else v = v0;
	}
	else if ( this_type == FITS::LONGLONG_T ) {
	    const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	    if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 ) v=NAN;
	    else v = v0;
	}
	else v = NAN;
    }
    return this->bzero_double_rec + v * this->bscale_double_rec;
}

/**
 * @brief  1つのピクセルの値を取得 (高レベル・long型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO，BSCALE，BLANK についての計算・変換が行なわれます．<br>
 *         fixpix のように少数のピクセルを扱う時に使います．パフォーマンスを
 *         求める用途には向いていません．
 */
inline long fits_image::lvalue(long idx0, long idx1, long idx2) const
{
    const double dv = this->dvalue(idx0,idx1,idx2);
    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LONG <= dv && dv <= MAX_DOUBLE_ROUND_LONG )
	return (long)((dv < 0) ? dv-0.5 : dv+0.5);
    else return INDEF_LONG;
}

/**
 * @brief  1つのピクセルの値を取得 (高レベル・long long型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO，BSCALE，BLANK についての計算・変換が行なわれます．<br>
 *         fixpix のように少数のピクセルを扱う時に使います．パフォーマンスを
 *         求める用途には向いていません．
 */
inline long long fits_image::llvalue(long idx0, long idx1, long idx2) const
{
    if ( this->type_rec == FITS::LONGLONG_T ) {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	if ( idx < 0 ) return INDEF_LLONG;
	const void *data_ptr = this->data_rec->data_ptr_cs();
	const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 ) {
	    return INDEF_LLONG;
	}
	else if ( this->bscale_double_rec == 1.0 && this->bzero_double_rec == 0.0 ) {
	    return v0;
	}
	else {
	    const double dv = this->bzero_double_rec + v0 * this->bscale_double_rec;
	    if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
		return (long long)((dv < 0) ? dv-0.5 : dv+0.5);
	    else return INDEF_LLONG;
	}
    }
    else {
	const double dv = this->dvalue(idx0,idx1,idx2);
	if ( isfinite(dv) && MIN_DOUBLE_ROUND_LLONG <= dv && dv <= MAX_DOUBLE_ROUND_LLONG )
	    return (long long)((dv < 0) ? dv-0.5 : dv+0.5);
	else return INDEF_LLONG;
    }
}

/* DOUBLE */
/**
 * @brief  1つのピクセルの値を取得 (低レベル・double型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が返されます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline double fits_image::double_value( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return NAN;
    const void *data_ptr = this->data_rec->data_ptr_cs();
    if ( this_type == FITS::DOUBLE_T ) {
	return ((const fits::double_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::FLOAT_T ) {
	return ((const fits::float_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::SHORT_T ) {
	const fits::short_t v0 = ((const fits::short_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this_type == FITS::LONG_T ) {
	const fits::long_t v0 = ((const fits::long_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this_type == FITS::BYTE_T ) {
	const fits::byte_t v0 = ((const fits::byte_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else return NAN;
}

/* FLOAT */
/**
 * @brief  1つのピクセルの値を取得 (低レベル・float型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が返されます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline float fits_image::float_value( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return NAN;
    const void *data_ptr = this->data_rec->data_ptr_cs();
    if ( this_type == FITS::FLOAT_T ) {
	return ((const fits::float_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	return ((const fits::double_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::SHORT_T ) {
	const fits::short_t v0 = ((const fits::short_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this_type == FITS::LONG_T ) {
	const fits::long_t v0 = ((const fits::long_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this_type == FITS::BYTE_T ) {
	const fits::byte_t v0 = ((const fits::byte_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	if ( this->blank_is_set_rec == true && this->blank_r_rec == v0 )
	    return NAN;
	else return v0;
    }
    else return NAN;
}

/* LONGLONG */
/**
 * @brief  1つのピクセルの値を取得 (低レベル・long long型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が返されます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline long long fits_image::longlong_value( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return this->blank_longlong_rec;
    const void *data_ptr = this->data_rec->data_ptr_cs();
    if ( this_type == FITS::LONGLONG_T ) {
	return ((const fits::longlong_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::LONG_T ) {
	return ((const fits::long_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::SHORT_T ) {
	return ((const fits::short_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	const double v0 = ((const fits::double_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LLONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LLONG ) 
	    return (long long)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_longlong_rec;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	const float v0 = ((const fits::float_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LLONG <= v0 && v0 <= MAX_FLOAT_ROUND_LLONG ) 
	    return (long long)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_longlong_rec;
    }
    else if ( this_type == FITS::BYTE_T ) {
	return ((const fits::byte_t *)(data_ptr))[idx];
    }
    else {
	return this->blank_longlong_rec;
    }
}

/* LONG */
/**
 * @brief  1つのピクセルの値を取得 (低レベル・long型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が返されます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline long fits_image::long_value( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return this->blank_long_rec;
    const void *data_ptr = this->data_rec->data_ptr_cs();
    if ( this_type == FITS::LONG_T ) {
	return ((const fits::long_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::SHORT_T ) {
	return ((const fits::short_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::BYTE_T ) {
	return ((const fits::byte_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::FLOAT_T ) {
	const float v0 = ((const fits::float_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_FLOAT_ROUND_LONG <= v0 && v0 <= MAX_FLOAT_ROUND_LONG ) 
	    return (long)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_long_rec;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	const double v0 = ((const fits::double_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_LONG <= v0 && v0 <= MAX_DOUBLE_ROUND_LONG ) 
	    return (long)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_long_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	if ( MIN_LONG <= v0 && v0 <= MAX_LONG ) return v0;
	else return this->blank_long_rec;
    }
    else {
	return this->blank_long_rec;
    }
}

/* SHORT */
/**
 * @brief  1つのピクセルの値を取得 (低レベル・short型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が返されます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline short fits_image::short_value( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return this->blank_short_rec;
    const void *data_ptr = this->data_rec->data_ptr_cs();
    if ( this_type == FITS::SHORT_T ) {
	return ((const fits::short_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::BYTE_T ) {
	return ((const fits::byte_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::FLOAT_T ) {
	const float v0 = ((const fits::float_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_FLOAT_ROUND_SHORT <= v0 && v0 <= MAX_FLOAT_ROUND_SHORT ) 
	    return (short)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_short_rec;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	const double v0 = ((const fits::double_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_SHORT <= v0 && v0 <= MAX_DOUBLE_ROUND_SHORT ) 
	    return (short)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_short_rec;
    }
    else if ( this_type == FITS::LONG_T ) {
	const fits::long_t v0 = ((const fits::long_t *)(data_ptr))[idx];
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->blank_short_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	if ( MIN_SHORT <= v0 && v0 <= MAX_SHORT ) return v0;
	else return this->blank_short_rec;
    }
    else {
	return this->blank_short_rec;
    }
}

/* BYTE */
/**
 * @brief  1つのピクセルの値を取得 (低レベル・unsigned char型)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が返されます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline unsigned char fits_image::byte_value( long idx0, long idx1, long idx2 ) const
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return this->blank_byte_rec;
    const void *data_ptr = this->data_rec->data_ptr_cs();
    if ( this_type == FITS::BYTE_T ) {
	return ((const fits::byte_t *)(data_ptr))[idx];
    }
    else if ( this_type == FITS::SHORT_T ) {
	const fits::short_t v0 = ((const fits::short_t *)(data_ptr))[idx];
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->blank_byte_rec;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	const float v0 = ((const fits::float_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_FLOAT_ROUND_UCHAR <= v0 && v0 <= MAX_FLOAT_ROUND_UCHAR ) 
	    return (unsigned char)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_byte_rec;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	const double v0 = ((const fits::double_t *)(data_ptr))[idx];
	if ( isfinite(v0) && MIN_DOUBLE_ROUND_UCHAR <= v0 && v0 <= MAX_DOUBLE_ROUND_UCHAR ) 
	    return (unsigned char)((v0 < 0) ? v0-0.5 : v0+0.5);
	else return this->blank_byte_rec;
    }
    else if ( this_type == FITS::LONG_T ) {
	const fits::long_t v0 = ((const fits::long_t *)(data_ptr))[idx];
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->blank_byte_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	const fits::longlong_t v0 = ((const fits::longlong_t *)(data_ptr))[idx];
	if ( MIN_UCHAR <= v0 && v0 <= MAX_UCHAR ) return v0;
	else return this->blank_byte_rec;
    }
    else {
	return this->blank_byte_rec;
    }
}

/* HIGH LEVEL */
/**
 * @brief  1つのピクセルの値を設定 (高レベル・double型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO，BSCALE，BLANK についての計算・変換が行なわれます．<br>
 *         fixpix のように少数のピクセルを扱う時に使います．パフォーマンスを
 *         求める用途には向いていません．
 */
inline fits_image &fits_image::assign( double value, long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    value -= this->bzero_double_rec;
    value /= this->bscale_double_rec;
    if ( this_type == FITS::DOUBLE_T ) {
	((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	((fits::float_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::SHORT_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_INT16 <= value && value <= MAX_DOUBLE_ROUND_INT16 )
	  ((fits::short_t *)(data_ptr))[idx] = (fits::short_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::short_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONG_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_INT32 <= value && value <= MAX_DOUBLE_ROUND_INT32 )
	  ((fits::long_t *)(data_ptr))[idx] = (fits::long_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::long_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::BYTE_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_UCHAR <= value && value <= MAX_DOUBLE_ROUND_UCHAR )
	  ((fits::byte_t *)(data_ptr))[idx] = (fits::byte_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::byte_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_INT64 <= value && value <= MAX_DOUBLE_ROUND_INT64 )
	  ((fits::longlong_t *)(data_ptr))[idx] = (fits::longlong_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::longlong_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    return *this;
}

/* DOUBLE */
/**
 * @brief  1つのピクセルの値を設定 (低レベル・double型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が書き込まれます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline fits_image &fits_image::assign_double( double value, long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    if ( this_type == FITS::DOUBLE_T ) {
       ((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::FLOAT_T ) {
       ((fits::float_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::SHORT_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_INT16 <= value && value <= MAX_DOUBLE_ROUND_INT16 )
	  ((fits::short_t *)(data_ptr))[idx] = (fits::short_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::short_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONG_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_INT32 <= value && value <= MAX_DOUBLE_ROUND_INT32 )
	  ((fits::long_t *)(data_ptr))[idx] = (fits::long_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::long_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::BYTE_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_UCHAR <= value && value <= MAX_DOUBLE_ROUND_UCHAR )
	  ((fits::byte_t *)(data_ptr))[idx] = (fits::byte_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::byte_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
       if ( isfinite(value) && MIN_DOUBLE_ROUND_INT64 <= value && value <= MAX_DOUBLE_ROUND_INT64 )
	  ((fits::longlong_t *)(data_ptr))[idx] = (fits::longlong_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::longlong_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    return *this;
}

/* FLOAT */
/**
 * @brief  1つのピクセルの値を設定 (低レベル・float型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が書き込まれます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline fits_image &fits_image::assign_float( float value, long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    if ( this_type == FITS::FLOAT_T ) {
	((fits::float_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::SHORT_T ) {
       if ( isfinite(value) && MIN_FLOAT_ROUND_INT16 <= value && value <= MAX_FLOAT_ROUND_INT16 )
	  ((fits::short_t *)(data_ptr))[idx] = (fits::short_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::short_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONG_T ) {
       if ( isfinite(value) && MIN_FLOAT_ROUND_INT32 <= value && value <= MAX_FLOAT_ROUND_INT32 )
	  ((fits::long_t *)(data_ptr))[idx] = (fits::long_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::long_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::BYTE_T ) {
       if ( isfinite(value) && MIN_FLOAT_ROUND_UCHAR <= value && value <= MAX_FLOAT_ROUND_UCHAR )
	  ((fits::byte_t *)(data_ptr))[idx] = (fits::byte_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::byte_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
       if ( isfinite(value) && MIN_FLOAT_ROUND_INT64 <= value && value <= MAX_FLOAT_ROUND_INT64 )
	  ((fits::longlong_t *)(data_ptr))[idx] = (fits::longlong_t)((value < 0) ? value-0.5 : value+0.5);
       else ((fits::longlong_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    return *this;
}

/* LONGLONG */
/**
 * @brief  1つのピクセルの値を設定 (低レベル・long long型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が書き込まれます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline fits_image &fits_image::assign_longlong( long long value, 
					   long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    if ( this_type == FITS::LONGLONG_T ) {
	if ( MIN_INT64 <= value && value <= MAX_INT64 ) 
	    ((fits::longlong_t *)(data_ptr))[idx] = value;
	else ((fits::longlong_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	/* ほとんど裏技．浮動小数点なのに blank をセットした場合だけ有効 */
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::double_t *)(data_ptr))[idx] = NAN;
	else ((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	/* ほとんど裏技．浮動小数点なのに blank をセットした場合だけ有効 */
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::float_t *)(data_ptr))[idx] = NAN;
	else ((fits::float_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::LONG_T ) {
	if ( MIN_INT32 <= value && value <= MAX_INT32 )
	    ((fits::long_t *)(data_ptr))[idx] = value;
	else ((fits::long_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::SHORT_T ) {
	if ( MIN_INT16 <= value && value <= MAX_INT16 ) 
	    ((fits::short_t *)(data_ptr))[idx] = value;
	else ((fits::short_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::BYTE_T ) {
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) 
	    ((fits::byte_t *)(data_ptr))[idx] = value;
	else ((fits::byte_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    return *this;
}

/* LONG */
/**
 * @brief  1つのピクセルの値を設定 (低レベル・long型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が書き込まれます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline fits_image &fits_image::assign_long( long value,
					   long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    if ( this_type == FITS::LONG_T ) {
	if ( MIN_INT32 <= value && value <= MAX_INT32 )
	    ((fits::long_t *)(data_ptr))[idx] = value;
	else ((fits::long_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	((fits::longlong_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::double_t *)(data_ptr))[idx] = NAN;
	else ((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::float_t *)(data_ptr))[idx] = NAN;
	else ((fits::float_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::SHORT_T ) {
	if ( MIN_INT16 <= value && value <= MAX_INT16 ) 
	    ((fits::short_t *)(data_ptr))[idx] = value;
	else ((fits::short_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    else if ( this_type == FITS::BYTE_T ) {
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR ) 
	    ((fits::byte_t *)(data_ptr))[idx] = value;
	else ((fits::byte_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    return *this;
}

/* SHORT */
/**
 * @brief  1つのピクセルの値を設定 (低レベル・short型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が書き込まれます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline fits_image &fits_image::assign_short( short value, 
					   long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    if ( this_type == FITS::SHORT_T ) {
	((fits::short_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::LONG_T ) {
	((fits::long_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	((fits::longlong_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::double_t *)(data_ptr))[idx] = NAN;
	else ((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::float_t *)(data_ptr))[idx] = NAN;
	else ((fits::float_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::BYTE_T ) {
	if ( MIN_UCHAR <= value && value <= MAX_UCHAR )
	    ((fits::byte_t *)(data_ptr))[idx] = value;
	else ((fits::byte_t *)(data_ptr))[idx] = this->blank_w_rec;
    }
    return *this;
}

/* BYTE */
/**
 * @brief  1つのピクセルの値を設定 (低レベル・unsigned char型)
 *
 * @param  value 書き込まれる値
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @note   BZERO等は計算されず，そのままの値が書き込まれます．<br>
 *         パフォーマンスを求める用途にはあまり向いていません．
 */
inline fits_image &fits_image::assign_byte( unsigned char value,
					   long idx0, long idx1, long idx2 )
{
    const int this_type = this->type_rec;
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    if ( idx < 0 ) return *this;				/* invalid */
    void *data_ptr = this->data_rec->data_ptr();
    if ( this_type == FITS::BYTE_T ) {
	((fits::byte_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::SHORT_T ) {
	((fits::short_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::LONG_T ) {
	((fits::long_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::LONGLONG_T ) {
	((fits::longlong_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::DOUBLE_T ) {
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::double_t *)(data_ptr))[idx] = NAN;
	else ((fits::double_t *)(data_ptr))[idx] = value;
    }
    else if ( this_type == FITS::FLOAT_T ) {
	if ( this->blank_is_set_rec == true && this->blank_r_rec == value )
	    ((fits::float_t *)(data_ptr))[idx] = NAN;
	else ((fits::float_t *)(data_ptr))[idx] = value;
    }
    return *this;
}


/*
 * BYTE_T
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::byte_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *fits_image::byte_t_ptr()
{
    return (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *)(this->err_throw_void_p("fits_image::byte_t_ptr()",
					      "ERROR","does not match type."))
      :
      (fits::byte_t *)(this->data_rec->data_ptr()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_image::byte_t_ptr() const
{
    return (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *)(this->err_throw_const_void_p(
		 "fits_image::byte_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::byte_t *)(this->data_rec->data_ptr()) ;
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_image::byte_t_ptr_cs() const
{
    return (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *)(this->err_throw_const_void_p(
		 "fits_image::byte_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::byte_t *)(this->data_rec->data_ptr_cs()) ;
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::byte_t)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *fits_image::byte_t_ptr( long idx0, long idx1, long idx2 )
{
    if ( this->type_rec != FITS::BYTE_T ) {
	return (fits::byte_t *)(this->err_throw_void_p(
		   "fits_image::byte_t_ptr()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((fits::byte_t *)(this->data_rec->data_ptr()) + idx) );
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_image::byte_t_ptr( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::BYTE_T ) {
	return (const fits::byte_t *)(this->err_throw_const_void_p(
		"fits_image::byte_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::byte_t *)(this->data_rec->data_ptr()) + idx) );
    }
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_image::byte_t_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::BYTE_T ) {
	return (const fits::byte_t *)(this->err_throw_const_void_p(
		"fits_image::byte_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::byte_t *)(this->data_rec->data_ptr_cs()) + idx) );
    }
}

/*
 * SHORT_T
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::short_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *fits_image::short_t_ptr()
{
    return (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *)(this->err_throw_void_p("fits_image::short_t_ptr()",
					      "ERROR","does not match type."))
      :
      (fits::short_t *)(this->data_rec->data_ptr()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_image::short_t_ptr() const
{
    return (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *)(this->err_throw_const_void_p(
		"fits_image::short_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::short_t *)(this->data_rec->data_ptr()) ;
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_image::short_t_ptr_cs() const
{
    return (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *)(this->err_throw_const_void_p(
		"fits_image::short_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::short_t *)(this->data_rec->data_ptr_cs()) ;
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::short_t)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *fits_image::short_t_ptr( long idx0, long idx1, long idx2 )
{
    if ( this->type_rec != FITS::SHORT_T ) {
	return (fits::short_t *)(this->err_throw_void_p(
		  "fits_image::short_t_ptr()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((fits::short_t *)(this->data_rec->data_ptr()) + idx) );
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_image::short_t_ptr( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::SHORT_T ) {
	return (const fits::short_t *)(this->err_throw_const_void_p(
	       "fits_image::short_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::short_t *)(this->data_rec->data_ptr()) + idx) );
    }
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_image::short_t_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::SHORT_T ) {
	return (const fits::short_t *)(this->err_throw_const_void_p(
	       "fits_image::short_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::short_t *)(this->data_rec->data_ptr_cs()) + idx) );
    }
}

/*
 * LONG_T
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::long_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *fits_image::long_t_ptr()
{
    return (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *)(this->err_throw_void_p("fits_image::long_t_ptr()",
					      "ERROR","does not match type."))
      :
      (fits::long_t *)(this->data_rec->data_ptr()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_image::long_t_ptr() const
{
    return (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *)(this->err_throw_const_void_p(
		 "fits_image::long_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::long_t *)(this->data_rec->data_ptr()) ;
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_image::long_t_ptr_cs() const
{
    return (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *)(this->err_throw_const_void_p(
		 "fits_image::long_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::long_t *)(this->data_rec->data_ptr_cs()) ;
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::long_t)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *fits_image::long_t_ptr( long idx0, long idx1, long idx2 )
{
    if ( this->type_rec != FITS::LONG_T ) {
	return (fits::long_t *)(this->err_throw_void_p(
		   "fits_image::long_t_ptr()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((fits::long_t *)(this->data_rec->data_ptr()) + idx) );
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_image::long_t_ptr( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::LONG_T ) {
	return (const fits::long_t *)(this->err_throw_const_void_p(
		"fits_image::long_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::long_t *)(this->data_rec->data_ptr()) + idx) );
    }
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_image::long_t_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::LONG_T ) {
	return (const fits::long_t *)(this->err_throw_const_void_p(
		"fits_image::long_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::long_t *)(this->data_rec->data_ptr_cs()) + idx) );
    }
}

/*
 * LONGLONG_T
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::longlong_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *fits_image::longlong_t_ptr()
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *)(this->err_throw_void_p(
		"fits_image::longlong_t_ptr()","ERROR","does not match type."))
      :
      (fits::longlong_t *)(this->data_rec->data_ptr()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_image::longlong_t_ptr() const
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *)(this->err_throw_const_void_p(
	     "fits_image::longlong_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::longlong_t *)(this->data_rec->data_ptr()) ;
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_image::longlong_t_ptr_cs() const
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *)(this->err_throw_const_void_p(
	     "fits_image::longlong_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::longlong_t *)(this->data_rec->data_ptr_cs()) ;
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::longlong_t)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *fits_image::longlong_t_ptr( long idx0, long idx1, long idx2 )
{
    if ( this->type_rec != FITS::LONGLONG_T ) {
	return (fits::longlong_t *)(this->err_throw_void_p(
	       "fits_image::longlong_t_ptr()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((fits::longlong_t *)(this->data_rec->data_ptr()) + idx) );
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_image::longlong_t_ptr( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::LONGLONG_T ) {
	return (const fits::longlong_t *)(this->err_throw_const_void_p(
	    "fits_image::longlong_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::longlong_t *)(this->data_rec->data_ptr()) + idx) );
    }
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_image::longlong_t_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::LONGLONG_T ) {
	return (const fits::longlong_t *)(this->err_throw_const_void_p(
	    "fits_image::longlong_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::longlong_t *)(this->data_rec->data_ptr_cs()) + idx) );
    }
}

/*
 * FLOAT_T
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::float_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *fits_image::float_t_ptr()
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *)(this->err_throw_void_p("fits_image::float_t_ptr()",
					      "ERROR","does not match type."))
      :
      (fits::float_t *)(this->data_rec->data_ptr()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_image::float_t_ptr() const
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *)(this->err_throw_const_void_p(
		"fits_image::float_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::float_t *)(this->data_rec->data_ptr()) ;
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_image::float_t_ptr_cs() const
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *)(this->err_throw_const_void_p(
		"fits_image::float_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::float_t *)(this->data_rec->data_ptr_cs()) ;
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::float_t)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *fits_image::float_t_ptr( long idx0, long idx1, long idx2 )
{
    if ( this->type_rec != FITS::FLOAT_T ) {
	return (fits::float_t *)(this->err_throw_void_p(
		  "fits_image::float_t_ptr()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((fits::float_t *)(this->data_rec->data_ptr()) + idx) );
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_image::float_t_ptr( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::FLOAT_T ) {
	return (const fits::float_t *)(this->err_throw_const_void_p(
	       "fits_image::float_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::float_t *)(this->data_rec->data_ptr()) + idx) );
    }
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_image::float_t_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::FLOAT_T ) {
	return (const fits::float_t *)(this->err_throw_const_void_p(
	       "fits_image::float_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::float_t *)(this->data_rec->data_ptr_cs()) + idx) );
    }
}

/*
 * DOUBLE_T
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::double_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *fits_image::double_t_ptr()
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *)(this->err_throw_void_p("fits_image::double_t_ptr()",
					      "ERROR","does not match type."))
      :
      (fits::double_t *)(this->data_rec->data_ptr()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_image::double_t_ptr() const
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *)(this->err_throw_const_void_p(
	       "fits_image::double_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::double_t *)(this->data_rec->data_ptr()) ;
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_image::double_t_ptr_cs() const
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *)(this->err_throw_const_void_p(
	       "fits_image::double_t_ptr_cs()","ERROR","does not match type."))
      :
      (const fits::double_t *)(this->data_rec->data_ptr_cs()) ;
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::double_t)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *fits_image::double_t_ptr( long idx0, long idx1, long idx2 )
{
    if ( this->type_rec != FITS::DOUBLE_T ) {
	return (fits::double_t *)(this->err_throw_void_p(
		 "fits_image::double_t_ptr()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((fits::double_t *)(this->data_rec->data_ptr()) + idx) );
    }
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_image::double_t_ptr( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::DOUBLE_T ) {
	return (const fits::double_t *)(this->err_throw_const_void_p(
	      "fits_image::double_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::double_t *)(this->data_rec->data_ptr()) + idx) );
    }
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_image::double_t_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    if ( this->type_rec != FITS::DOUBLE_T ) {
	return (const fits::double_t *)(this->err_throw_const_void_p(
	      "fits_image::double_t_ptr_cs()","ERROR","does not match type."));
    }
    else {
	const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
	return ( (idx < 0) ? NULL :
		 ((const fits::double_t *)(this->data_rec->data_ptr_cs()) + idx) );
    }
}

/*
 *
 *  ADDRESS TABLE for 2-D
 *
 */

/*
 * BYTE_T
 */

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::byte_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *const *fits_image::byte_t_ptr_2d( bool use )
{
    return (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *const *)(this->err_throw_void_p(
		 "fits_image::byte_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::byte_t *const *)(this->data_rec->data_ptr_2d(use)) ;
}

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::byte_t)
 *
 *  fits_image::byte_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *const *fits_image::byte_t_ptr_2d()
{
    return (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *const *)(this->err_throw_void_p(
		 "fits_image::byte_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::byte_t *const *)(this->data_rec->data_ptr_2d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::byte_t・読取専用)
 *
 *  fits_image::byte_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *const *fits_image::byte_t_ptr_2d() const
{
    return (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *const *)(this->err_throw_const_void_p(
	      "fits_image::byte_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::byte_t *const *)(this->data_rec->data_ptr_2d()) ;
}
#endif

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::byte_t・読取専用)
 *
 *  fits_image::byte_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *const *fits_image::byte_t_ptr_2d_cs() const
{
    return (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *const *)(this->err_throw_const_void_p(
	      "fits_image::byte_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::byte_t *const *)(this->data_rec->data_ptr_2d()) ;
}

/*
 * SHORT_T
 */

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::short_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *const *fits_image::short_t_ptr_2d( bool use )
{
    return (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *const *)(this->err_throw_void_p(
		"fits_image::short_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::short_t *const *)(this->data_rec->data_ptr_2d(use)) ;
}

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::short_t)
 *
 *  fits_image::short_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *const *fits_image::short_t_ptr_2d()
{
    return (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *const *)(this->err_throw_void_p(
		"fits_image::short_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::short_t *const *)(this->data_rec->data_ptr_2d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::short_t・読取専用)
 *
 *  fits_image::short_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *const *fits_image::short_t_ptr_2d() const
{
    return (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *const *)(this->err_throw_const_void_p(
	     "fits_image::short_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::short_t *const *)(this->data_rec->data_ptr_2d()) ;
}
#endif

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::short_t・読取専用)
 *
 *  fits_image::short_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *const *fits_image::short_t_ptr_2d_cs() const
{
    return (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *const *)(this->err_throw_const_void_p(
	     "fits_image::short_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::short_t *const *)(this->data_rec->data_ptr_2d()) ;
}

/*
 * LONG_T
 */

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::long_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *const *fits_image::long_t_ptr_2d( bool use )
{
    return (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *const *)(this->err_throw_void_p(
		 "fits_image::long_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::long_t *const *)(this->data_rec->data_ptr_2d(use)) ;
}

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::long_t)
 *
 *  fits_image::long_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *const *fits_image::long_t_ptr_2d()
{
    return (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *const *)(this->err_throw_void_p(
		 "fits_image::long_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::long_t *const *)(this->data_rec->data_ptr_2d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::long_t・読取専用)
 *
 *  fits_image::long_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *const *fits_image::long_t_ptr_2d() const
{
    return (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *const *)(this->err_throw_const_void_p(
	      "fits_image::long_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::long_t *const *)(this->data_rec->data_ptr_2d()) ;
}
#endif

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::long_t・読取専用)
 *
 *  fits_image::long_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *const *fits_image::long_t_ptr_2d_cs() const
{
    return (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *const *)(this->err_throw_const_void_p(
	      "fits_image::long_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::long_t *const *)(this->data_rec->data_ptr_2d()) ;
}

/*
 * LONGLONG_T
 */

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::longlong_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *const *fits_image::longlong_t_ptr_2d( bool use )
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *const *)(this->err_throw_void_p(
	     "fits_image::longlong_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::longlong_t *const *)(this->data_rec->data_ptr_2d(use)) ;
}

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::longlong_t)
 *
 *  fits_image::longlong_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *const *fits_image::longlong_t_ptr_2d()
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *const *)(this->err_throw_void_p(
	     "fits_image::longlong_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::longlong_t *const *)(this->data_rec->data_ptr_2d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::longlong_t・読取専用)
 *
 *  fits_image::longlong_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *const *fits_image::longlong_t_ptr_2d() const
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *const *)(this->err_throw_const_void_p(
	  "fits_image::longlong_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::longlong_t *const *)(this->data_rec->data_ptr_2d()) ;
}
#endif

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::longlong_t・読取専用)
 *
 *  fits_image::longlong_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *const *fits_image::longlong_t_ptr_2d_cs() const
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *const *)(this->err_throw_const_void_p(
	  "fits_image::longlong_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::longlong_t *const *)(this->data_rec->data_ptr_2d()) ;
}

/*
 * FLOAT_T
 */

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::float_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *const *fits_image::float_t_ptr_2d( bool use )
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *const *)(this->err_throw_void_p(
		"fits_image::float_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::float_t *const *)(this->data_rec->data_ptr_2d(use)) ;
}

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::float_t)
 *
 *  fits_image::float_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *const *fits_image::float_t_ptr_2d()
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *const *)(this->err_throw_void_p(
		"fits_image::float_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::float_t *const *)(this->data_rec->data_ptr_2d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::float_t・読取専用)
 *
 *  fits_image::float_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *const *fits_image::float_t_ptr_2d() const
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *const *)(this->err_throw_const_void_p(
	     "fits_image::float_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::float_t *const *)(this->data_rec->data_ptr_2d()) ;
}
#endif

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::float_t・読取専用)
 *
 *  fits_image::float_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *const *fits_image::float_t_ptr_2d_cs() const
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *const *)(this->err_throw_const_void_p(
	     "fits_image::float_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::float_t *const *)(this->data_rec->data_ptr_2d()) ;
}

/*
 * DOUBLE_T
 */

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::double_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *const *fits_image::double_t_ptr_2d( bool use )
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *const *)(this->err_throw_void_p(
	       "fits_image::double_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::double_t *const *)(this->data_rec->data_ptr_2d(use)) ;
}

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::double_t)
 *
 *  fits_image::double_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *const *fits_image::double_t_ptr_2d()
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *const *)(this->err_throw_void_p(
	       "fits_image::double_t_ptr_2d()","ERROR","does not match type."))
      :
      (fits::double_t *const *)(this->data_rec->data_ptr_2d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::double_t・読取専用)
 *
 *  fits_image::double_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *const *fits_image::double_t_ptr_2d() const
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *const *)(this->err_throw_const_void_p(
	    "fits_image::double_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::double_t *const *)(this->data_rec->data_ptr_2d()) ;
}
#endif

/**
 * @brief  2次元画像データ配列のポインタ配列を取得 (fits::double_t・読取専用)
 *
 *  fits_image::double_t_ptr_2d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *const *fits_image::double_t_ptr_2d_cs() const
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *const *)(this->err_throw_const_void_p(
	    "fits_image::double_t_ptr_2d_cs()","ERROR","does not match type."))
      :
      (const fits::double_t *const *)(this->data_rec->data_ptr_2d()) ;
}


/*
 *
 *  ADDRESS TABLE for 3-D
 *
 */

/*
 * BYTE_T
 */

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::byte_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *const *const *fits_image::byte_t_ptr_3d( bool use )
{
    return (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *const *const *)(this->err_throw_void_p(
		 "fits_image::byte_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::byte_t *const *const *)(this->data_rec->data_ptr_3d(use)) ;
}

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::byte_t)
 *
 *  fits_image::byte_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *const *const *fits_image::byte_t_ptr_3d()
{
    return (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *const *const *)(this->err_throw_void_p(
		 "fits_image::byte_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::byte_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::byte_t・読取専用)
 *
 *  fits_image::byte_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *const *const *fits_image::byte_t_ptr_3d() const
{
    return (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *const *const *)(this->err_throw_const_void_p(
	      "fits_image::byte_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::byte_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}
#endif

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::byte_t・読取専用)
 *
 *  fits_image::byte_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *const *const *fits_image::byte_t_ptr_3d_cs() const
{
    return (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *const *const *)(this->err_throw_const_void_p(
	      "fits_image::byte_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::byte_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

/*
 * SHORT_T
 */

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::short_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *const *const *fits_image::short_t_ptr_3d( bool use )
{
    return (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *const *const *)(this->err_throw_void_p(
		"fits_image::short_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::short_t *const *const *)(this->data_rec->data_ptr_3d(use)) ;
}

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::short_t)
 *
 *  fits_image::short_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *const *const *fits_image::short_t_ptr_3d()
{
    return (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *const *const *)(this->err_throw_void_p(
		"fits_image::short_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::short_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::short_t・読取専用)
 *
 *  fits_image::short_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *const *const *fits_image::short_t_ptr_3d() const
{
    return (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *const *const *)(this->err_throw_const_void_p(
	     "fits_image::short_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::short_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}
#endif

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::short_t・読取専用)
 *
 *  fits_image::short_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *const *const *fits_image::short_t_ptr_3d_cs() const
{
    return (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *const *const *)(this->err_throw_const_void_p(
	     "fits_image::short_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::short_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

/*
 * LONG_T
 */

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::long_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *const *const *fits_image::long_t_ptr_3d( bool use )
{
    return (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *const *const *)(this->err_throw_void_p(
		 "fits_image::long_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::long_t *const *const *)(this->data_rec->data_ptr_3d(use)) ;
}

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::long_t)
 *
 *  fits_image::long_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *const *const *fits_image::long_t_ptr_3d()
{
    return (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *const *const *)(this->err_throw_void_p(
		 "fits_image::long_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::long_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::long_t・読取専用)
 *
 *  fits_image::long_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *const *const *fits_image::long_t_ptr_3d() const
{
    return (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *const *const *)(this->err_throw_const_void_p(
	      "fits_image::long_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::long_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}
#endif

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::long_t・読取専用)
 *
 *  fits_image::long_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *const *const *fits_image::long_t_ptr_3d_cs() const
{
    return (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *const *const *)(this->err_throw_const_void_p(
	      "fits_image::long_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::long_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

/*
 * LONGLONG_T
 */

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::longlong_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *const *const *fits_image::longlong_t_ptr_3d( bool use )
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *const *const *)(this->err_throw_void_p(
	     "fits_image::longlong_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::longlong_t *const *const *)(this->data_rec->data_ptr_3d(use)) ;
}

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::longlong_t)
 *
 *  fits_image::longlong_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *const *const *fits_image::longlong_t_ptr_3d()
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *const *const *)(this->err_throw_void_p(
	     "fits_image::longlong_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::longlong_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::longlong_t・読取専用)
 *
 *  fits_image::longlong_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *const *const *fits_image::longlong_t_ptr_3d() const
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *const *const *)(this->err_throw_const_void_p(
	  "fits_image::longlong_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::longlong_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}
#endif

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::longlong_t・読取専用)
 *
 *  fits_image::longlong_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *const *const *fits_image::longlong_t_ptr_3d_cs() const
{
    return (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *const *const *)(this->err_throw_const_void_p(
	  "fits_image::longlong_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::longlong_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

/*
 * FLOAT_T
 */

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::float_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *const *const *fits_image::float_t_ptr_3d( bool use )
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *const *const *)(this->err_throw_void_p(
		"fits_image::float_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::float_t *const *const *)(this->data_rec->data_ptr_3d(use)) ;
}

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::float_t)
 *
 *  fits_image::float_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *const *const *fits_image::float_t_ptr_3d()
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *const *const *)(this->err_throw_void_p(
		"fits_image::float_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::float_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::float_t・読取専用)
 *
 *  fits_image::float_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *const *const *fits_image::float_t_ptr_3d() const
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *const *const *)(this->err_throw_const_void_p(
	     "fits_image::float_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::float_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}
#endif

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::float_t・読取専用)
 *
 *  fits_image::float_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *const *const *fits_image::float_t_ptr_3d_cs() const
{
    return (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *const *const *)(this->err_throw_const_void_p(
	     "fits_image::float_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::float_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

/*
 * DOUBLE_T
 */

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::double_t)
 *
 * @param  use 使用する場合はtrue(機能on)．<br>
 *             使用しない場合はfalse(機能off)．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *const *const *fits_image::double_t_ptr_3d( bool use )
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *const *const *)(this->err_throw_void_p(
	       "fits_image::double_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::double_t *const *const *)(this->data_rec->data_ptr_3d(use)) ;
}

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::double_t)
 *
 *  fits_image::double_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *const *const *fits_image::double_t_ptr_3d()
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *const *const *)(this->err_throw_void_p(
	       "fits_image::double_t_ptr_3d()","ERROR","does not match type."))
      :
      (fits::double_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::double_t・読取専用)
 *
 *  fits_image::double_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *const *const *fits_image::double_t_ptr_3d() const
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *const *const *)(this->err_throw_const_void_p(
	    "fits_image::double_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::double_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}
#endif

/**
 * @brief  3次元画像データ配列のポインタ配列を取得 (fits::double_t・読取専用)
 *
 *  fits_image::double_t_ptr_3d(true) を実行してから使用します．
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *const *const *fits_image::double_t_ptr_3d_cs() const
{
    return (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *const *const *)(this->err_throw_const_void_p(
	    "fits_image::double_t_ptr_3d_cs()","ERROR","does not match type."))
      :
      (const fits::double_t *const *const *)(this->data_rec->data_ptr_3d()) ;
}


/*
 * ANY TYPE
 */

/**
 * @brief  画像データ配列のバッファアドレスを取得 (void *)
 *
 * @attention  型チェックは行なわれません．fits_image::float_t_ptr() などの各型
 *             専用のメンバ関数を検討してください．
 */
inline void *fits_image::data_ptr()
{
    return this->data_rec->data_ptr();
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (void *・読取専用)
 *
 * @attention  型チェックは行なわれません．fits_image::float_t_ptr() などの各型
 *             専用のメンバ関数を検討してください．
 */
inline const void *fits_image::data_ptr() const
{
    return this->data_rec->data_ptr();
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (void *・読取専用)
 *
 * @attention  型チェックは行なわれません．fits_image::float_t_ptr() などの各型
 *             専用のメンバ関数を検討してください．
 */
inline const void *fits_image::data_ptr_cs() const
{
    return this->data_rec->data_ptr_cs();
}

/**
 * @brief  画像データ配列のバッファアドレスを取得 (void *)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @attention  型チェックは行なわれません．fits_image::float_t_ptr() などの各型
 *             専用のメンバ関数を検討してください．
 */
inline void *fits_image::data_ptr( long idx0, long idx1, long idx2 )
{
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    return
    ( (idx < 0) ? 
      NULL
      :
      ((void *)((char *)(this->data_rec->data_ptr()) + this->bytes() * idx))
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列のバッファアドレスを取得 (void *・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @attention  型チェックは行なわれません．fits_image::float_t_ptr() などの各型
 *             専用のメンバ関数を検討してください．
 */
inline const void *fits_image::data_ptr( long idx0, long idx1, long idx2 ) const
{
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    return
    ( (idx < 0) ? 
      NULL
      :
      ((const void *)((const char *)(this->data_rec->data_ptr()) + this->bytes() * idx))
    );
}
#endif

/**
 * @brief  画像データ配列のバッファアドレスを取得 (void *・読取専用)
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号(省略可)
 * @param  idx2 次元番号2の次元(3次元目)の要素番号(省略可)
 *
 * @attention  型チェックは行なわれません．fits_image::float_t_ptr() などの各型
 *             専用のメンバ関数を検討してください．
 */
inline const void *fits_image::data_ptr_cs( long idx0, long idx1, long idx2 ) const
{
    const long idx = this->get_idx_3d_cs(idx0,idx1,idx2);
    return
    ( (idx < 0) ? 
      NULL
      :
      ((const void *)((const char *)(this->data_rec->data_ptr_cs()) + this->bytes() * idx))
    );
}

/* to access image data using mdarray class: mdarray class provides */
/* fast and useful member functions to manipulate image data.       */

/* uchar */
/**
 * @brief  画像データ配列の参照を取得 (mdarray_uchar)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline mdarray_uchar &fits_image::uchar_array()
{
    if ( this->data_array().size_type() != UCHAR_ZT )
	this->err_throw_void_p("fits_image::uchar_array()",
			       "ERROR","does not match type.");
    return this->uchar_data_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray_uchar・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_uchar &fits_image::uchar_array() const
{
    if ( this->data_array().size_type() != UCHAR_ZT )
	this->err_throw_const_void_p("fits_image::uchar_array()",
				     "ERROR","does not match type.");
    return this->uchar_data_rec;
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray_uchar・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_uchar &fits_image::uchar_array_cs() const
{
    if ( this->data_array().size_type() != UCHAR_ZT )
	this->err_throw_const_void_p("fits_image::uchar_array()",
				     "ERROR","does not match type.");
    return this->uchar_data_rec;
}

/* int16 */
/**
 * @brief  画像データ配列の参照を取得 (mdarray_int16)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline mdarray_int16 &fits_image::int16_array()
{
    if ( this->data_array().size_type() != INT16_ZT )
	this->err_throw_void_p("fits_image::int16_array()",
			       "ERROR","does not match type.");
    return this->int16_data_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray_int16・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_int16 &fits_image::int16_array() const
{
    if ( this->data_array().size_type() != INT16_ZT )
	this->err_throw_const_void_p("fits_image::int16_array()",
				     "ERROR","does not match type.");
    return this->int16_data_rec;
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray_int16・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_int16 &fits_image::int16_array_cs() const
{
    if ( this->data_array().size_type() != INT16_ZT )
	this->err_throw_const_void_p("fits_image::int16_array()",
				     "ERROR","does not match type.");
    return this->int16_data_rec;
}

/* int32 */
/**
 * @brief  画像データ配列の参照を取得 (mdarray_int32)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline mdarray_int32 &fits_image::int32_array()
{
    if ( this->data_array().size_type() != INT32_ZT )
	this->err_throw_void_p("fits_image::int32_array()",
			       "ERROR","does not match type.");
    return this->int32_data_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray_int32・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_int32 &fits_image::int32_array() const
{
    if ( this->data_array().size_type() != INT32_ZT )
	this->err_throw_const_void_p("fits_image::int32_array()",
				     "ERROR","does not match type.");
    return this->int32_data_rec;
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray_int32・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_int32 &fits_image::int32_array_cs() const
{
    if ( this->data_array().size_type() != INT32_ZT )
	this->err_throw_const_void_p("fits_image::int32_array()",
				     "ERROR","does not match type.");
    return this->int32_data_rec;
}

/* int64 */
/**
 * @brief  画像データ配列の参照を取得 (mdarray_int64)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline mdarray_int64 &fits_image::int64_array()
{
    if ( this->data_array().size_type() != INT64_ZT )
	this->err_throw_void_p("fits_image::int64_array()",
			       "ERROR","does not match type.");
    return this->int64_data_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray_int64・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_int64 &fits_image::int64_array() const
{
    if ( this->data_array().size_type() != INT64_ZT )
	this->err_throw_const_void_p("fits_image::int64_array()",
				     "ERROR","does not match type.");
    return this->int64_data_rec;
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray_int64・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_int64 &fits_image::int64_array_cs() const
{
    if ( this->data_array().size_type() != INT64_ZT )
	this->err_throw_const_void_p("fits_image::int64_array()",
				     "ERROR","does not match type.");
    return this->int64_data_rec;
}

/* float */
/**
 * @brief  画像データ配列の参照を取得 (mdarray_float)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline mdarray_float &fits_image::float_array()
{
    if ( this->data_array().size_type() != FLOAT_ZT )
	this->err_throw_void_p("fits_image::float_array()",
			       "ERROR","does not match type.");
    return this->float_data_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray_float・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_float &fits_image::float_array() const
{
    if ( this->data_array().size_type() != FLOAT_ZT )
	this->err_throw_const_void_p("fits_image::float_array()",
				     "ERROR","does not match type.");
    return this->float_data_rec;
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray_float・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_float &fits_image::float_array_cs() const
{
    if ( this->data_array().size_type() != FLOAT_ZT )
	this->err_throw_const_void_p("fits_image::float_array()",
				     "ERROR","does not match type.");
    return this->float_data_rec;
}

/* double */
/**
 * @brief  画像データ配列の参照を取得 (mdarray_double)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline mdarray_double &fits_image::double_array()
{
    if ( this->data_array().size_type() != DOUBLE_ZT )
	this->err_throw_void_p("fits_image::double_array()",
			       "ERROR","does not match type.");
    return this->double_data_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray_double・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_double &fits_image::double_array() const
{
    if ( this->data_array().size_type() != DOUBLE_ZT )
	this->err_throw_const_void_p("fits_image::double_array()",
				     "ERROR","does not match type.");
    return this->double_data_rec;
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray_double・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const mdarray_double &fits_image::double_array_cs() const
{
    if ( this->data_array().size_type() != DOUBLE_ZT )
	this->err_throw_const_void_p("fits_image::double_array()",
				     "ERROR","does not match type.");
    return this->double_data_rec;
}

/* any type */
/**
 * @brief  画像データ配列の参照を取得 (mdarray)
 */
inline mdarray &fits_image::data_array()
{
    return *(this->data_rec);
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  画像データ配列の参照を取得 (mdarray・読取専用)
 */
inline const mdarray &fits_image::data_array() const
{
    return *(this->data_rec);
}
#endif

/**
 * @brief  画像データ配列の参照を取得 (mdarray・読取専用)
 */
inline const mdarray &fits_image::data_array_cs() const
{
    return *(this->data_rec);
}

/* obtain 1-d index for internal buffer from (x,y,z) */
/**
 * @brief  3次元の要素番号から1次元の要素番号への変換
 *
 * @param  idx0 次元番号0の次元(1次元目)の要素番号
 * @param  idx1 次元番号1の次元(2次元目)の要素番号
 * @param  idx2 次元番号2の次元(3次元目)の要素番号
 *
 * @note   このメンバ関数は private です．
 */
inline long fits_image::get_idx_3d_cs( long ix0, long ix1, long ix2 ) const
{
    return
    ( /* if */ (ix1 == FITS::INDEF) ?
      /* 1-dim */
      ( /* if */ (ix0 < 0 || this->length() <= ix0) ?
	-1 : ix0
      )
      :
      /* 2-dim or 3-dim */
      ( /* if */ (ix2 == FITS::INDEF) ?
	/* 2-dim */
	( /* if */ (ix0 < 0 || this->col_length() <= ix0) ? 
	  -1
	  :
	  ( /* if */ (ix1 < 0 || this->row_layer_length() <= ix1) ?
	    -1
	    :
	    (ix0 + this->col_length() * ix1)
	  )
	)
	:
	/* 3-dim */
	( /* if */ (ix0 < 0 || this->col_length() <= ix0) ?
	  -1
	  :
	  ( /* if */ (ix1 < 0 || this->row_length() <= ix1) ?
	    -1
	    :
	    ( /* if */ (ix2 < 0 || this->layer_length() <= ix2) ?
	      -1
	      :
	      (ix0 + this->col_length() * ix1
		   + this->col_row_length() * ix2)
	    )
	  )
	)
      )
    );
}


/**
 * @example  examples_sfitsio/create_fits_image.cc
 *           新規の FITS イメージを作る簡単な例
 */

/**
 * @example  examples_sfitsio/create_fits_image_and_header.cc
 *           新規の FITS イメージを作る例．少し長いヘッダを作る場合の参考に
 */

/**
 * @example  examples_sfitsio/stat_fits_image_pixels_md.cc
 *           画像の統計値を取得するコード
 */

/**
 * @example  examples_sfitsio/combine_fits_images_md.cc
 *           画像のコンバインを行なうコード
 */

/**
 * @example  tools_sfitsio/conv_fits_image_bitpix.cc
 *           FITS 画像の BITPIX を変換するツール
 */

/**
 * @example  tools_sfitsio/rotate_fits_image.cc
 *           FITS 画像の回転(90度単位)を行なうツール
 */

/**
 * @example  tools_sfitsio/transpose_fits_image.cc
 *           FITS 画像の (x,y) を入れ替えるツール
 */


}

#endif	/* _SLI__FITS_IMAGE_H */

