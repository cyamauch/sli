/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-29 13:15:53 cyamauch> */

#ifndef _SLI__FITS_HEADER_RECORD_H
#define _SLI__FITS_HEADER_RECORD_H 1

/**
 * @file   fits_header_record.h
 * @brief  FITSヘッダの 1つのレコードを表現するクラス fits_header_record の定義
 */

#include "fits.h"

#ifdef BUILD_SFITSIO
#include <sli/tarray_tstring.h>
#include <sli/asarray_tstring.h>
#else
#include "tarray_tstring.h"
#include "asarray_tstring.h"
#endif

namespace sli
{

  namespace fits
  {
    /* Use this function to update the built-in dictionary of header  */
    /* comments.                                                      */
    /* Set FITS::ANY_HDU, FITS::IMAGE_HDU, FITS::BINARY_TABLE_HDU, or */
    /* FITS::ASCII_TABLE_HDU to `hdutype' arg.                        */
    const asarray_tstring &update_comment_dictionary( int hdutype, 
					 const char *const *new_kwd_comments );
  }

  class fits_header;

  /*
   * sil::fits_header_record class expresses a record of FITS header, and its
   * object stores keyword, value and comment strings.  Objects of this class
   * are usually managed by an object of sli::fits_header class.
   *
   * Long keyword (54-chars in maximum), long value string (no limit), and long
   * comments (no limit) are supported, so this class can be used for purposes
   * independent of FITS.  CONTINUE keyword is automatically set for long value
   * string or long comments when length of a formatted record exceeds 80.
   */

  /**
   * @class  sli::fits_header_record
   * @brief  FITS ヘッダの 1 つのレコードを表現するクラス
   *
   *  fits_header_record クラスは，FITS ヘッダの 1 つのレコードを表現します．
   *  1つのレコードは基本的には 80 文字からなるものですが，CONTINUE キーワード
   *  が元の FITS にあった場合，80文字×n を 1 つのレコードとみなし，データを
   *  格納します．
   *
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fits_header_record

  {
    friend class fits_header;

  public:
    /* constructor & destructor */
    fits_header_record();
    fits_header_record( const fits_header_record &obj );
    virtual ~fits_header_record();

    /* same as .init(obj) */
    virtual fits_header_record &operator=(const fits_header_record &obj);

    /* complete initialization of objects */
    virtual fits_header_record &init();
    virtual fits_header_record &init(const fits_header_record &obj);

    /* Set a pair of [keyword, value, comment] or [keyword, description]. */
    /* Description is used for HISTORY or COMMENT records.                */
    virtual fits_header_record &assign( const fits::header_def &def );
    virtual fits_header_record &assign( const fits_header_record &obj );
    virtual fits_header_record &assign( const char *keyword, const char *value,
					const char *comment );
    virtual fits_header_record &assign( const char *keyword,
					const char *description );

    /* low-level member functions to set a raw value.    */
    /* To set a string for a type of header record, give */
    /* an argument for value like "'ABC'".               */
    virtual fits_header_record &assignf_value( const char *format, ... );
    virtual fits_header_record &vassignf_value(const char *format, va_list ap);
    virtual fits_header_record &assign_value( const char *str );

    /* member functions to set a comment */
    virtual fits_header_record &vassignf_comment( const char *format, va_list ap );
    virtual fits_header_record &assignf_comment( const char *format, ... );
    virtual fits_header_record &assign_comment( const char *str );

    /* This sets a comment string registered in comment dictionary. */
    virtual fits_header_record &assign_default_comment( int hdutype = FITS::ANY_HDU );

    /* high-level member functions to set a value.   */
    /* Single quotations of a string such as "'ABC'" */
    /* are not required.                             */
    /*   prec: precision (number of digit)           */
    virtual fits_header_record &assignf( const char *format, ... );
    virtual fits_header_record &vassignf( const char *format, va_list ap );
    virtual fits_header_record &assign( const char *str );
    virtual fits_header_record &assign( bool tf );
    virtual fits_header_record &assign( int val );
    virtual fits_header_record &assign( long val );
    virtual fits_header_record &assign( long long val );
    virtual fits_header_record &assign( double val, int prec = 15 );
    virtual fits_header_record &assign( float val, int prec = 6 );

    /* This sets the system date string [yyyy-mm-ddThh:mm:ss]. */
    virtual fits_header_record &assign_system_time();

    /* This checks the record whether it is normal format (in the format of */
    /* A = B), description format (COMMENT or HISTORY) or NULL format (no   */
    /* keyword nor value exist), and returns FITS::NORMAL_RECORD,           */
    /* FITS::DESCRIPTION_RECORD or FITS::NULL_RECORD, respectively.         */
    virtual int status() const;

    /* This returns FITS::DOUBLE_T in case of real number, FITS::LONGLONG_T */
    /* in case of integer number, FITS::DOUBLECOMPLEX_T in case of complex  */
    /* number, FITS::BOOL_T in case of boolean number, and FITS::STRING_T   */
    /* in case of others.                                                   */
    virtual int type() const;

    /* member functions to return keyword string */
    virtual const char *keyword() const;
    virtual ssize_t get_keyword( char *dest_buf, size_t buf_size ) const;
    virtual const tstring &keyword_cs() const;

    /* This returns length of keyword string */
    virtual size_t keyword_length() const;

    /* low-level member functions to return a raw value string.          */
    /* Single quotations of a string such as "'ABC'" are not eliminated. */
    virtual const char *value() const;
    virtual ssize_t get_value( char *dest_buf, size_t buf_size ) const;
    virtual const tstring &value_cs() const;

    /* This returns length of raw value string. */
    virtual size_t value_length() const;

    /* member functions to return comment string */
    virtual const char *comment() const;
    virtual ssize_t get_comment( char *dest_buf, size_t buf_size ) const;
    virtual const tstring &comment_cs() const;

    /* This returns length of comment string. */
    virtual size_t comment_length() const;

    /* This returns set of keyword, raw value, and comment string. */
    virtual const fits::header_def &raw_record() const;

    /* high-level member functions to return a value for string type. */
    /* Single quotations and left- and right-side of blank characters */
    /* are eliminated for returned string.                            */
    virtual const char *svalue() const;
    virtual const tstring &svalue_cs() const;
    /* This returns length of string returned by svalue().            */
    virtual size_t svalue_length() const;
    virtual ssize_t get_svalue( char *dest_buf, size_t buf_size ) const;

    /* high-level member functions to return a value for each type */
    virtual bool bvalue() const;
    virtual long lvalue() const;
    virtual long long llvalue() const;
    virtual double dvalue() const;

    /* This parses IRAF-style section info (e.g., BIASSEC, TRIMSEC, etc.)  */
    /* that expresses a box region, and returns integer positions and      */
    /* length.                                                             */
    /* Starting positions, length and flip flag are returned to ret_begin, */
    /* ret_length, and ret_flip_flag, respectively.  NULL can be set for   */
    /* 1st, 2nd, and 3rd args.                                             */
    /* This member function returns required buffer length to store the    */
    /* section information of all dimensions.  Negative value is returned  */
    /* for error.                                                          */
    /* Note that positions in header values are 1-indexed, but returned x  */
    /* and y are 0-indexed positions.                                      */
    virtual int get_section_info( long ret_begin[], long ret_length[],
				  bool ret_flip_flag[], int buf_len ) const;
    /* Four values [x, len_x, y, len_y] are returned to dest_buf.          */
    virtual int get_section_info( long dest_buf[], int buf_len ) const;

    /* discard original 80-char record, and reformat this record */
    virtual fits_header_record &reformat();

    /* This returns formatted header string or its length. */
    /* Returned result is a string of 80 * n characters    */
    /* without '\n' but with '\0' termination.             */
    virtual const char *formatted_string() const;
    virtual size_t formatted_length() const;

    /* virtual const char *comment_unit(); */
    /* virtual const char *comment_body(); */

    /* return attributes (for future extensions) */
    virtual bool keyword_protected() const;
    virtual bool value_protected() const;
    virtual bool value_type_protected() const;
    virtual bool comment_protected() const;

    /* shallow copy を許可する場合に使用 (未実装)  */
    /* (一時オブジェクトの return の直前で使用)    */
    virtual void set_scopy_flag();

  protected:
    /* Special member functions */
    virtual fits_header_record &_init();
    virtual fits_header_record &_init(const fits_header_record &obj);
    /* comment を書くと，description ではなくなる */
    /* low-level member functions */
    virtual fits_header_record &assign_any( const fits::header_def &def );
#if 1
    virtual fits_header_record &update_value_recs();
#else
    virtual fits_header_record &update_svalue_rec();
    virtual fits_header_record &update_bvalue_rec();
    virtual fits_header_record &update_dvalue_rec();
    virtual fits_header_record &update_llvalue_rec();
#endif
    virtual fits_header_record &vassignf_keyword( const char *format, va_list ap );
    virtual fits_header_record &assignf_keyword( const char *format, ... );
    /* */
    virtual fits_header_record &set_protections( bool keyword, bool value_type, bool value, 
						 bool comment );
    virtual tarray_tstring &arr();
    virtual const tarray_tstring &arr_cs() const;
    virtual fits_header_record &register_manager( fits_header *ptr );

  private:
    bool keyword_protected_rec;	/* read only の場合は trueになる */
    bool value_type_protected_rec;
    bool value_protected_rec;
    bool comment_protected_rec;
    tarray_tstring arr_rec;
    fits::header_def rec;	/* raw_record() 用．ポインタを代入するだけ */
    tstring svalue_rec;		/* ' とスペースを消したもの */
    bool bvalue_rec;
    double dvalue_rec;
    long long llvalue_rec;
    tstring a_formatted_rec;	/* formatted_string() 用のバッファ領域 */
    fits_header *manager;

  };
}

#endif	/* _SLI__FITS_HEADER_RECORD_H */
