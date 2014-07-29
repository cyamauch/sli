/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2014-07-30 00:38:45 cyamauch> */

#ifndef _SLI__FITS_TABLE_COL_H
#define _SLI__FITS_TABLE_COL_H 1

/**
 * @file   fits_table_col.h
 * @brief  ASCII or Binary Tableの1カラムを表現するクラス fits_table_col の定義
 */

#include "fits.h"

#ifdef BUILD_SFITSIO
#include <sli/tarray_tstring.h>
#include <sli/ctindex.h>
#include <sli/tstring.h>
#include <sli/mdarray.h>
#else
#include "tarray_tstring.h"
#include "ctindex.h"
#include "tstring.h"
#include "mdarray.h"
#endif

namespace sli
{
  class fits_table;

  /*
   * sli::fits_table_col class expresses a column of a FITS binary table or 
   * ASCII table.   Instances of this class are usually managed by an instance
   * of sli::fits_table class that expresses a table, therefore, programmers
   * use this class via APIs provided in sli::fits_table class in major cases.
   *
   * Both high-level APIs for cost-effective developments and low-level APIs
   * for best performance are provided.  In high-level APIs, conversions for
   * TZEROn, TSCALn and TNULLn are supported, and minimize programmer's
   * effort.  APIs are common to both ASCII table and binary table.
   *
   * The heap area of binary table is managed by sli::fits_table class.
   */

  /**
   * @class  sli::fits_table_col
   * @brief  ASCII or Binary Table の 1 つのカラム(フィールド)を表現するクラス
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fits_table_col
  {
    friend class fits_table;

  public:
    /* constructor & destructor */
    fits_table_col();
    fits_table_col(const fits_table_col &obj);
    virtual ~fits_table_col();

    /* 
     * Member functions to obtain column information
     */

    /* for future extensions */
    virtual bool is_protected() const;

    /* these member functions return all definition of a column */
    virtual const fits::table_def &definition() const;
    virtual const fits::table_def_all &definition_all() const;

    /* get/set column name (TTYPEn) */
    virtual const char *name() const;
    virtual fits_table_col &assign_name( const char *new_name );

    /*
     * obtain type, byte-length, etc.
     *
     * When .type() == FITS::STRING_T, member functions such as .bytes()
     * return special values:
     *                           .bytes()  .dcol_len  .drow_len  .elem_length()
     * TFORMn=120A                 120         1           1          1
     * TFORMn=120A10                10        12           1         12
     * TFORMn=120A10,TDIMn=(6,2)    10         6           2         12
     * TFORMn=120A,TDIMn=(10,6,2)   10         6           2         12
     *
     * .type() == FITS::BIT_T is also special, i.e., .bytes() returns 1.
     *
     * See fits.h for type code definition.
     */
    virtual int type() const;		/* returns FITS::DOUBLE_T for 16D    */
    virtual long bytes() const;		/* sizeof(double). returns 1 for 1X  */
    virtual long elem_length() const;	/* return 16 for 16D                 */
    virtual long elem_byte_length() const;	/* .bytes() * .elem_length() */
    virtual long dcol_length() const;	/* returns 8 when TDIM = (8,2)       */
    virtual long drow_length() const;	/* returns 2 when TDIM = (8,2)       */
    /* not recommended */
    virtual long elem_size() const;	/* same as elem_length() */
    virtual long dcol_size() const;	/* same as dcol_length() */
    virtual long drow_size() const;	/* same as drow_length() */

    /*
     * obtain heap information
     */
    virtual bool heap_is_used() const;	/* returns true when TFORM is ?P?    */
    virtual int heap_type() const;	/* returns FITS::DOUBLE_T for 1PD    */
    virtual long heap_bytes() const;	/* returns sizeof(double) when 1PD   */
    virtual long max_array_length() const;	/* returns 999 when 1PD(999) */

    /* returns length of variable length array at specified row */
    virtual long array_length( long row_idx, long elem_idx = 0 ) const;

    /* returns address offset for specified row in heap area */
    virtual long array_heap_offset( long row_idx, long elem_idx = 0 ) const;

    /*
     * Member functions to obtain NULL and scaling information
     */

    /* returns true if TUNITn, TNULLn, TZEROn, and TSCALn */
    /* are set, respectively.                             */
    virtual bool tunit_is_set() const;
    virtual bool tnull_is_set() const;
    virtual bool tzero_is_set() const;
    virtual bool tscal_is_set() const;

    /* returns value of TUNITn */
    virtual const char *tunit() const;

    /* This returns integer value of TNULLn.                */
    /*   tnull_ptr: address for TNULLn string is returned.  */
    virtual long long tnull( const char **tnull_ptr = NULL ) const;

    /* returns value of TZEROn */
    virtual double tzero() const;

    /* returns value of TSCALn */
    virtual double tscal() const;

    /*
     * Member functions to convert numeric columns
     */

    /* 'B', 'I', 'J', 'K', 'E' and 'D' types are supported. */
    virtual fits_table_col &convert_type( int new_type );
    virtual fits_table_col &convert_type( int new_type, double new_zero );
    virtual fits_table_col &convert_type( int new_type, double new_zero,
					  double new_scale);
    virtual fits_table_col &convert_type( int new_type, double new_zero, 
					double new_scale, long long new_null );

    /*
     * Member functions to set/erase TUNITn, TNULLn, and scaling parameters
     */

    /* for TUNITn */
    virtual fits_table_col &assign_tunit( const char *unit );
    virtual fits_table_col &erase_tunit();

    /* for TNULLn */
    virtual fits_table_col &assign_tnull( long long null );
    virtual fits_table_col &assign_tnull( const char *null );
    virtual fits_table_col &erase_tnull();

    /* for TZEROn */
    virtual fits_table_col &assign_tzero( double zero, int prec = 15 );
    virtual fits_table_col &erase_tzero();

    /* for TSCALn */
    virtual fits_table_col &assign_tscal( double scal, int prec = 15 );
    virtual fits_table_col &erase_tscal();

    /*
     * Member functions to set or get NULL value string for high-level
     * APIs: .assign() and .svalue().
     * Default NULL value string is "NULL".
     */

    /* set NULL value string */
    virtual fits_table_col &assign_null_svalue( const char *snull );

    /* obtain NULL value string */
    virtual const char *null_svalue() const;

    /*
     * Member functions to set default value when resizing.
     * Note: default value will be reseted when changing attributes of column.
     */

    /* high-level */
    virtual fits_table_col &assign_default( double value );
    virtual fits_table_col &assign_default( float value );
    virtual fits_table_col &assign_default( long long value );
    virtual fits_table_col &assign_default( long value );
    virtual fits_table_col &assign_default( int value );
    virtual fits_table_col &assign_default( const char *value );

    /* low-level                                                  */
    /* Note that this member function cannot check type validity. */
    virtual fits_table_col &assign_default_value( const void *value );

    /*
     * Member functions to obtain length of rows. 
     */
    virtual long length() const;
    /* not recommended */
    virtual long size() const;				/* same as length() */

    /*
     * These member functions return address of internal data area.
     * (only for purposes that requires highest performance)
     */

    /* BIT_T */
    virtual fits::bit_t *bit_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::bit_t *bit_t_ptr() const;
#endif
    virtual const fits::bit_t *bit_t_ptr_cs() const;
    /* */
    virtual fits::bit_t *bit_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::bit_t *bit_t_ptr( long row_idx ) const;
#endif
    virtual const fits::bit_t *bit_t_ptr_cs( long row_idx ) const;
    /* BYTE_T */
    virtual fits::byte_t *byte_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::byte_t *byte_t_ptr() const;
#endif
    virtual const fits::byte_t *byte_t_ptr_cs() const;
    /* */
    virtual fits::byte_t *byte_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::byte_t *byte_t_ptr( long row_idx ) const;
#endif
    virtual const fits::byte_t *byte_t_ptr_cs( long row_idx ) const;
    /* LOGICAL_T */
    virtual fits::logical_t *logical_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::logical_t *logical_t_ptr() const;
#endif
    virtual const fits::logical_t *logical_t_ptr_cs() const;
    /* */
    virtual fits::logical_t *logical_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::logical_t *logical_t_ptr( long row_idx ) const;
#endif
    virtual const fits::logical_t *logical_t_ptr_cs( long row_idx ) const;
    /* ASCII_T */
    virtual fits::ascii_t *ascii_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::ascii_t *ascii_t_ptr() const;
#endif
    virtual const fits::ascii_t *ascii_t_ptr_cs() const;
    /* */
    virtual fits::ascii_t *ascii_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::ascii_t *ascii_t_ptr( long row_idx ) const;
#endif
    virtual const fits::ascii_t *ascii_t_ptr_cs( long row_idx ) const;
    /* SHORT_T */
    virtual fits::short_t *short_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::short_t *short_t_ptr() const;
#endif
    virtual const fits::short_t *short_t_ptr_cs() const;
    /* */
    virtual fits::short_t *short_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::short_t *short_t_ptr( long row_idx ) const;
#endif
    virtual const fits::short_t *short_t_ptr_cs( long row_idx ) const;
    /* LONG_T */
    virtual fits::long_t *long_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::long_t *long_t_ptr() const;
#endif
    virtual const fits::long_t *long_t_ptr_cs() const;
    /* */
    virtual fits::long_t *long_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::long_t *long_t_ptr( long row_idx ) const;
#endif
    virtual const fits::long_t *long_t_ptr_cs( long row_idx ) const;
    /* LONGLONG_T */
    virtual fits::longlong_t *longlong_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longlong_t *longlong_t_ptr() const;
#endif
    virtual const fits::longlong_t *longlong_t_ptr_cs() const;
    /* */
    virtual fits::longlong_t *longlong_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longlong_t *longlong_t_ptr( long row_idx ) const;
#endif
    virtual const fits::longlong_t *longlong_t_ptr_cs( long row_idx ) const;
    /* FLOAT_T */
    virtual fits::float_t *float_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::float_t *float_t_ptr() const;
#endif
    virtual const fits::float_t *float_t_ptr_cs() const;
    /* */
    virtual fits::float_t *float_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::float_t *float_t_ptr( long row_idx ) const;
#endif
    virtual const fits::float_t *float_t_ptr_cs( long row_idx ) const;
    /* DOUBLE_T */
    virtual fits::double_t *double_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::double_t *double_t_ptr() const;
#endif
    virtual const fits::double_t *double_t_ptr_cs() const;
    /* */
    virtual fits::double_t *double_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::double_t *double_t_ptr( long row_idx ) const;
#endif
    virtual const fits::double_t *double_t_ptr_cs( long row_idx ) const;
    /* COMPLEX_T */
    virtual fits::complex_t *complex_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::complex_t *complex_t_ptr() const;
#endif
    virtual const fits::complex_t *complex_t_ptr_cs() const;
    /* */
    virtual fits::complex_t *complex_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::complex_t *complex_t_ptr( long row_idx ) const;
#endif
    virtual const fits::complex_t *complex_t_ptr_cs( long row_idx ) const;
    /* DOUBLECOMPLEX_T */
    virtual fits::doublecomplex_t *doublecomplex_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::doublecomplex_t *doublecomplex_t_ptr() const;
#endif
    virtual const fits::doublecomplex_t *doublecomplex_t_ptr_cs() const;
    /* */
    virtual fits::doublecomplex_t *doublecomplex_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::doublecomplex_t *doublecomplex_t_ptr( long row_idx ) const;
#endif
    virtual const fits::doublecomplex_t *doublecomplex_t_ptr_cs( long row_idx ) const;
    /* LONGARRDESC_T */
    virtual fits::longarrdesc_t *longarrdesc_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longarrdesc_t *longarrdesc_t_ptr() const;
#endif
    virtual const fits::longarrdesc_t *longarrdesc_t_ptr_cs() const;
    /* */
    virtual fits::longarrdesc_t *longarrdesc_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::longarrdesc_t *longarrdesc_t_ptr( long row_idx ) const;
#endif
    virtual const fits::longarrdesc_t *longarrdesc_t_ptr_cs( long row_idx ) const;
    /* LLONGARRDESC_T */
    virtual fits::llongarrdesc_t *llongarrdesc_t_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::llongarrdesc_t *llongarrdesc_t_ptr() const;
#endif
    virtual const fits::llongarrdesc_t *llongarrdesc_t_ptr_cs() const;
    /* */
    virtual fits::llongarrdesc_t *llongarrdesc_t_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits::llongarrdesc_t *llongarrdesc_t_ptr( long row_idx ) const;
#endif
    virtual const fits::llongarrdesc_t *llongarrdesc_t_ptr_cs( long row_idx ) const;
    /* RAW: Note that these member functions cannot check type validity. */
    virtual void *data_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *data_ptr() const;
#endif
    virtual const void *data_ptr_cs() const;
    /* */
    virtual void *data_ptr( long row_idx );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *data_ptr( long row_idx ) const;
#endif
    virtual const void *data_ptr_cs( long row_idx ) const;

    /* Reserved member functions for future extensions.                   */
    /* Do not use these member functions.  See fits_table class to access */
    /* heap area.                                                         */
    /* 注意: ヒープエリアは，fits_table クラスのバッファを利用する事．    */
    /* fits_table_col クラスにもヒープバッファが用意されているが，        */
    /* これは fits_table_col 単独で利用するための予備的なものである．     */
    virtual void *heap_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *heap_ptr() const;
#endif
    virtual const void *heap_ptr_cs() const;

    /* Member functions to get raw data using C-like args.          */
    /* Note that these member functions cannot check type validity. */
    virtual ssize_t get_data( void *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_data( long row_idx, 
			      void *dest_buf, size_t buf_size ) const;

    /* Member functions to set raw data using C-like args.          */
    /* Note that these member functions cannot check type validity. */
    virtual ssize_t put_data( const void *src_buf, size_t buf_size );
    virtual ssize_t put_data( long row_idx, const void *src_buf, size_t buf_size );

    /* This returns reference of internal mdarray object. (read only) */
    virtual const mdarray &data_array_cs() const;

    /* Reserved member functions for future extensions.                   */
    /* Do not use these member functions.  See fits_table class to access */
    /* heap area.                                                         */
    virtual ssize_t get_heap( void *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_heap( long offset, 
			      void *dest_buf, size_t buf_size ) const;
    virtual ssize_t put_heap( const void *buf, size_t buf_size );
    virtual ssize_t put_heap( long offset, const void *buf, size_t buf_size );
    virtual const mdarray &heap_array_cs() const;
    virtual fits_table_col &resize_heap( size_t sz );
    virtual size_t heap_length();

    /*
     * Member functions to edit rows
     */

    /* copy row into row */
    virtual fits_table_col &move( long src_index, long num_rows, 
				  long dest_index );
    /* swap rows */
    virtual fits_table_col &swap( long index0, long num_rows, 
				  long index1 );

    /* flip rows */
    virtual fits_table_col &flip( long index, long num_rows );

    /* import from particular column.                                        */
    /* The column type of `from` and column type of self does not need to be */
    /* identical.  If two column types are not identical, it converts the    */
    /* type and imports.                                                     */
    /* Note that complex type and variable length array are not supported.   */
    virtual fits_table_col &import( long dest_index, 
				const fits_table_col &from, long idx_begin = 0,
				long num_rows = FITS::ALL );

    /* fill all or part of rows using default value */
    virtual fits_table_col &clean();			/* for all rows */
    virtual fits_table_col &clean( long index, long num_rows );

    /*
     * high-level APIs to read a cell.
     *  - TSCAL and TZERO are applied.
     *  - NULL value is handled.
     */

    /* dvalue() member functions return NAN when the cell has NULL value.    */
    /* Programmers can write the code to detect NULL of any types like this: */
    /*   if ( ! isfinite( tbl.col("FOO").dvalue(i)) ) printf("It is NULL");  */
    virtual double dvalue( long row_index ) const;
    virtual double dvalue( long row_index,
			  const char *elem_nm, long repetiti_idx = 0 ) const;
    virtual double dvalue( long row_index,
			  long elem_index, long repetition_idx = 0 ) const;

    /* return value of lvalue(), llvalue() and bvalue() cannot be used */
    /* to test NULL or non-NULL value.                                 */
    virtual long lvalue( long row_index ) const;
    virtual long lvalue( long row_index,
			 const char *elem_nm, long repetiti_idx = 0 ) const;
    virtual long lvalue( long row_index,
			 long elem_index, long repetition_idx = 0 ) const;
    virtual long long llvalue( long row_index ) const;
    virtual long long llvalue( long row_index,
			    const char *elem_nm, long repetiti_idx = 0 ) const;
    virtual long long llvalue( long row_index,
			      long elem_index, long repetition_idx = 0 ) const;
    virtual bool bvalue( long row_index ) const;
    virtual bool bvalue( long row_index,
			 const char *elem_nm, long repetiti_idx = 0 ) const;
    virtual bool bvalue( long row_index,
			 long elem_index, long repetition_idx = 0 ) const;

    /* svalue() member functions return NULL value string when the */
    /* cell has NULL value.  The NULL value string can be defined  */
    /* by assign_null_svalue().                                    */
    virtual const char *svalue( long row_index );
    virtual const char *svalue( long row_index,
				const char *el_nm, long repeti_idx = 0 );
    virtual const char *svalue( long row_index,
				long elem_idx, long repetition_idx = 0 );

    /* same as svalue(), but copy a string into programmer's object */
    virtual const char *get_svalue( long row_index, 
				    tstring *dest ) const;
    virtual const char *get_svalue( long row_index,
				    const char *el_nm,
				    tstring *dest ) const;
    virtual const char *get_svalue( long row_index,
				    const char *el_nm, long repeti_idx,
				    tstring *dest ) const;
    virtual const char *get_svalue( long row_index,
				    long elem_idx, 
				    tstring *dest ) const;
    virtual const char *get_svalue( long row_index,
				    long elem_idx, 
				    long repetition_idx,
				    tstring *dest ) const;
    /* not recommended */
    virtual const char *get_svalue( long row_index, 
				    tstring &dest ) const;
    virtual const char *get_svalue( long row_index,
				    const char *el_nm,
				    tstring &dest ) const;
    virtual const char *get_svalue( long row_index,
				    const char *el_nm, long repeti_idx,
				    tstring &dest ) const;
    virtual const char *get_svalue( long row_index,
				    long elem_idx, 
				    tstring &dest ) const;
    virtual const char *get_svalue( long row_index,
				    long elem_idx, 
				    long repetition_idx,
				    tstring &dest ) const;

    /* same as svalue(), but copy a string into programmer's buffer */
    virtual ssize_t get_svalue( long row_index, 
				char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_svalue( long row_index,
				const char *elem_name, 
				char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_svalue( long row_index,
				const char *elem_name, long repetition_idx,
				char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_svalue( long row_index,
				long elem_index, 
				char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_svalue( long row_index,
				long elem_index, long repetition_idx,
				char *dest_buf, size_t buf_size ) const;

    /*
     * low-level APIs to read a cell.
     */

    /* LOGICAL_T                                       */
    /* returns 'T', 'F' or '\0'. ('\0' indicates NULL) */
    virtual int logical_value( long row_index ) const;
    virtual int logical_value( long row_index,
			  const char *elem_nm, long repetition_idx = 0 ) const;
    virtual int logical_value( long row_index,
			      long elem_index, long repetition_idx = 0 ) const;
    /* SHORT_T */
    virtual short short_value( long row_index ) const;
    virtual short short_value( long row_index,
			  const char *elem_nm, long repetition_idx = 0 ) const;
    virtual short short_value( long row_index,
			      long elem_index, long repetition_idx = 0 ) const;
    /* LONG_T */
    virtual long long_value( long row_index ) const;
    virtual long long_value( long row_index,
			const char *elem_name, long repetition_idx = 0 ) const;
    virtual long long_value( long row_index,
			     long elem_index, long repetition_idx = 0 ) const;
    /* LONGLONG_T */
    virtual long long longlong_value( long row_index ) const;
    virtual long long longlong_value( long row_index,
				const char *elem_nm, long repe_idx = 0 ) const;
    virtual long long longlong_value( long row_index,
				long elem_idx, long repetition_idx = 0 ) const;
    /* FLOAT_T */
    virtual float float_value( long row_index ) const;
    virtual float float_value( long row_index,
			  const char *elem_nm, long repetition_idx = 0 ) const;
    virtual float float_value( long row_index,
			      long elem_index, long repetition_idx = 0 ) const;
    /* DOUBLE_T */
    virtual double double_value( long row_index ) const;
    virtual double double_value( long row_index,
			    const char *elem_nm, long repetiti_idx = 0 ) const;
    virtual double double_value( long row_index,
			      long elem_index, long repetition_idx = 0 ) const;
    /* BYTE_T */
    virtual unsigned char byte_value( long row_index ) const;
    virtual unsigned char byte_value( long row_index,
				const char *elem_nm, long repe_idx = 0 ) const;
    virtual unsigned char byte_value( long row_index,
				long elem_idx, long repetition_idx = 0 ) const;
    /* BIT_T */
    virtual long long bit_value( long row_index ) const;
    virtual long long bit_value( long row_index,
	  const char *elem_name, long repetition_idx = 0, int nbit = 0 ) const;
    virtual long long bit_value( long row_index,
			    long elem_index, long repetition_idx = 0, 
			    int nbit = 1 ) const;
    /* STRING_T */
    virtual const char *string_value( long row_index );
    virtual const char *string_value( long row_index,
				      const char *el_nm, long repeti_idx = 0 );
    virtual const char *string_value( long row_index,
				      long elem_idx, long repetition_idx = 0 );
    /* get a string into programmer's object */
    virtual const char *get_string_value( long row_index, 
					  tstring *dest ) const;
    virtual const char *get_string_value( long row_index,
					  const char *elem_name, 
					  tstring *dest ) const;
    virtual const char *get_string_value( long row_index,
				    const char *elem_name, long repetition_idx,
				    tstring *dest ) const;
    virtual const char *get_string_value( long row_index,
					  long elem_index, 
					  tstring *dest ) const;
    virtual const char *get_string_value( long row_index,
					  long elem_index, long repetition_idx,
					  tstring *dest ) const;
    /* not recommended */
    virtual const char *get_string_value( long row_index, 
					  tstring &dest ) const;
    virtual const char *get_string_value( long row_index,
					  const char *elem_name, 
					  tstring &dest ) const;
    virtual const char *get_string_value( long row_index,
				    const char *elem_name, long repetition_idx,
				    tstring &dest ) const;
    virtual const char *get_string_value( long row_index,
					  long elem_index, 
					  tstring &dest ) const;
    virtual const char *get_string_value( long row_index,
					  long elem_index, long repetition_idx,
					  tstring &dest ) const;
    /* get a string into programmer's buffer */
    virtual ssize_t get_string_value( long row_index, 
				      char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_string_value( long row_index,
				      const char *elem_name, 
				      char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_string_value( long row_index,
				    const char *elem_name, long repetition_idx,
				    char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_string_value( long row_index,
				      long elem_index, 
				      char *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_string_value( long row_index,
				      long elem_index, long repetition_idx,
				      char *dest_buf, size_t buf_size ) const;

    /*
     * high-level APIs to write a cell.
     *  - TSCAL and TZERO are applied.
     *  - NULL value is handled.
     */

    /* assign(NAN) sets NULL value to a cell of a columns of any types. */
    virtual fits_table_col &assign( double value, long row_index );
    virtual fits_table_col &assign( double value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( double value, long row_index,
			       long elem_index, long repetition_idx = 0 );
    virtual fits_table_col &assign( float value, long row_index );
    virtual fits_table_col &assign( float value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( float value, long row_index,
			       long elem_index, long repetition_idx = 0 );
    virtual fits_table_col &assign( long long value, long row_index );
    virtual fits_table_col &assign( long long value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( long long value, long row_index,
			       long elem_index, long repetition_idx = 0 );
    virtual fits_table_col &assign( long value, long row_index );
    virtual fits_table_col &assign( long value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( long value, long row_index,
			       long elem_index, long repetition_idx = 0 );
    virtual fits_table_col &assign( int value, long row_index );
    virtual fits_table_col &assign( int value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( int value, long row_index,
			       long elem_index, long repetition_idx = 0 );
#if 0
    virtual fits_table_col &assign( short value, long row_index );
    virtual fits_table_col &assign( short value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( short value, long row_index,
			       long elem_index, long repetition_idx = 0 );
#endif

    /* Giving NULL value string to `value' arg will set NULL     */
    /* value to a cell.  The NULL value string can be defined by */
    /* assign_null_svalue().                                     */
    virtual fits_table_col &assign( const char *value, long row_index );
    virtual fits_table_col &assign( const char *value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign( const char *value, long row_index,
			       long elem_index, long repetition_idx = 0 );

    /*
     * low-level APIs to write a cell.
     */

    /* LOGICAL_T                                                      */
    /* argument `value' takes 'T', 'F' or '\0'. ('\0' indicates NULL) */
    virtual fits_table_col &assign_logical( int value, long row_index );
    virtual fits_table_col &assign_logical( int value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign_logical( int value, long row_index,
				long elem_index, long repetition_idx = 0 );
    /* SHORT_T */
    virtual fits_table_col &assign_short( short value, long row_index );
    virtual fits_table_col &assign_short( short value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign_short( short value, long row_index,
			      long elem_index, long repetition_idx = 0 );
    /* LONG_T */
    virtual fits_table_col &assign_long( long value, long row_index );
    virtual fits_table_col &assign_long( long value, long row_index,
			     const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign_long( long value, long row_index,
			     long elem_index, long repetition_idx = 0 );
    /* LONGLONG_T */
    virtual fits_table_col &assign_longlong( long long value, long row_index );
    virtual fits_table_col &assign_longlong( long long value, long row_index,
			       const char *elem_nam, long repetition_idx = 0 );
    virtual fits_table_col &assign_longlong( long long value, long row_index,
				 long elem_index, long repetition_idx = 0 );
    /* BYTE_T */
    virtual fits_table_col &assign_byte( unsigned char value, long row_index );
    virtual fits_table_col &assign_byte( unsigned char value, long row_index,
			     const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign_byte( unsigned char value, long row_index,
			     long elem_index, long repetition_idx = 0 );
    /* FLOAT_T */
    virtual fits_table_col &assign_float( float value, long row_index );
    virtual fits_table_col &assign_float( float value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign_float( float value, long row_index,
			      long elem_index, long repetition_idx = 0 );
    /* DOUBLE_T */
    virtual fits_table_col &assign_double( double value, long row_index );
    virtual fits_table_col &assign_double( double value, long row_index,
			      const char *elem_name, long repetition_idx = 0 );
    virtual fits_table_col &assign_double( double value, long row_index,
			       long elem_index, long repetition_idx = 0 );
    /* STRING_T */
    virtual fits_table_col &assign_string( const char *value, long row_index );
    virtual fits_table_col &assign_string( const char *value, long row_index,
			       const char *elem_nam, long repetition_idx = 0 );
    virtual fits_table_col &assign_string( const char *value, long row_index,
			       long elem_index, long repetition_idx = 0 );
    /* BIT_T */
    virtual fits_table_col &assign_bit( long long value, long row_index );
    virtual fits_table_col &assign_bit( long long value, long row_index,
		const char *elem_name, long repetition_idx = 0, int nbit = 0 );
    virtual fits_table_col &assign_bit( long long value, long row_index,
		      long elem_index, long repetition_idx = 0, int nbit = 1 );
    /* LONGARRDESC_T or LLONGARRDESC_T */
    virtual fits_table_col &assign_arrdesc( long length, long offset,
					   long row_index, long elem_idx = 0 );

    /* 
     * These member functions send a message to fits_table object to update 
     * column information in fits_table object, when managed by it.
     */

    /* initialize self */
    virtual fits_table_col &operator=(const fits_table_col &obj);
    virtual fits_table_col &init();
    virtual fits_table_col &init(const fits_table_col &obj);

    /* swap internal data between self and obj */
    virtual fits_table_col &swap(fits_table_col &obj);

    /* change the definition of a column                                  */
    /* define() updates column properties only for non-NULL elements of   */
    /* given fits::table_def structure, i.e., set NULL to elements of the */
    /* structure to keep previous properties.                             */
    virtual fits_table_col &define( const fits::table_def &def );
    virtual fits_table_col &define( const fits::table_def_all &def );

    /* resize number of rows */
    virtual fits_table_col &resize( long num_rows );

    /* erase rows */
    virtual fits_table_col &erase( long index, long num_rows );

    /* insert blank rows */
    virtual fits_table_col &insert( long index, long num_rows );

    /* setting of strategy of memory allocation for rows */
    /* "auto" (default), "min" and "pow" can be set.     */
    virtual fits_table_col &set_alloc_strategy( const char *strategy );

    /* shallow copy を許可する場合に使用 (未実装)  */
    /* (一時オブジェクトの return の直前で使用)    */
    virtual void set_scopy_flag();

  protected:
    /* fits_table で使用 */
    virtual fits_table_col &_define( const fits::table_def_all &def );
    virtual fits_table_col &_resize( long num_rows );
    virtual fits_table_col &_erase( long index, long num_rows );
    virtual fits_table_col &_insert( long index, long num_rows );
    virtual void **data_ptr_mng_ptr();
    virtual void **heap_ptr_mng_ptr();
    /* */
    virtual fits_table_col &set_protection( bool protect );
    virtual fits_table_col &register_manager( fits_table *ptr );

  private:
    void set_tnull( const long long *new_tnull_ptr );
    void update_def_rec_ptr();
    void update_zero_of_data_rec();
    void update_zero_of_data_rec( int new_type, bool is_heap );
    fits_table_col &convert_col_type( int new_type, 
				      const double *new_zero_ptr, 
				      const double *new_scale_ptr,
				      const long long *new_null_ptr );
    /* shallow copy 関係 */
    bool request_shallow_copy( fits_table_col *from_obj ) const;
    void cancel_shallow_copy( fits_table_col *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;

    void *err_throw_void_p( const char *fnc, const char *lv, const char *mes);
    const void *err_throw_const_void_p( const char *fnc, 
				  const char *lv, const char *mes) const;

  private:
    fits::table_def_all def_all_rec;	/* ↓へのポインタ */
    fits::table_def def_rec;		/* ↓へのポインタ */
    tarray_tstring tany;		/* ttype,ttype_comment, ... */
    tarray_tstring talas;
    tarray_tstring telem;	/* ↓とは異なり，ビットフィールドは展開済み */
    tarray_tstring telem_def;	/* foo:2 のようなビットフィールドで記述 */
    ctindex elem_index_rec;	/* telem へのインデックス */
    /* */
    mdarray bit_size_telem;	/* 同じ telem が何個続いているか malloc() */
    tstring *str_buf;		/* return で返す文字列 new and delete */
    tstring *tmp_str_buf;
    tstring fmt_str;		/* svalue() で返す文字列のフォーマット */
    tstring fmt_nullstr;	/* ↑で "NULL" を表示する時のフォーマット */
    tstring null_svalue_rec;	/* svalue()，assign() でのヌル文字列 */
    /* */
    int type_rec;
    long bytes_rec;
    long elem_size_rec;
    long dcol_size_rec;
    long full_bytes_rec;
    /* heap info */
    int heap_type_rec;
    long vl_max_length_rec;
    /* tzero, tscale */
    double tzero_rec;
    double tscal_rec;
    bool tnull_is_set_rec;
    /* tnull 各種 */
    long long tnull_r_rec;	/* ヘッダの値をそのまま保存するとこ */
    long long tnull_w_rec;	/* 書き出し時に使う．型によって自動制限あり */
    long long tnull_longlong_rec;
    long tnull_long_rec;
    short tnull_short_rec;
    unsigned char tnull_byte_rec;
    /* manager が使う */
    bool protected_rec;
    fits_table *manager;
    /* */
    long row_size_rec;	/* this->data_rec.row_length() のコピーを保持する */
    mdarray data_rec;
    void *data_ptr_rec;	/* data_rec のバッファのアドレスを保持．自動更新 */
    /* このバッファは fits_table の管理下に無い場合に使う */
    mdarray heap_rec;
    void *heap_ptr_rec;	/* heap_rec のバッファのアドレスを保持．自動更新 */

    /* shallow copy のためのフラグ類．= 等ではコピーされない */
    /* コピーの src 側のフラグ */
    bool shallow_copy_ok;		/* set_scopy_flag() でセットされる */
    fits_table_col *shallow_copy_dest_obj;    /* コピー先: 有効なら non-NULL */

    /* コピーの dest 側のフラグ(コピー元から書き込まれる) */
    fits_table_col *shallow_copy_src_obj; /* コピー元: 生きていれば non-NULL */

    /* __shallow_init(), __deep_init() を安全に行なうためのフラグ */
    bool __copying;

  };

/*
 * inline member functions
 */

/* type, byte-length, etc. */
/**
 * @brief  カラムデータの型種別(FITS::SHORT_T，FITS::FLOAT_T 等)を取得
 *
 * @note  可変長配列の場合，FITS::LONGARRDESC_T または FITS::LLONGARRDESC_T が
 *        返ります．
 */
inline int fits_table_col::type() const
{
    return this->type_rec;
}

/**
 * @brief  カラムの型のバイト数を取得
 *
 *  カラムの型が FITS::ASCII_T 以外の場合，カラムの型のバイト数を返します．
 *  例えば，カラムの型が FITS::DOUBLE_T の場合，sizeof(fits::double_t) が
 *  返ります．カラムの型が FITS::BIT_T の場合は，1 が返ります．<br>
 *  カラムの型が FITS::ASCII_T の場合は，TFORMn と TDIMn 指定によるカラム中の
 *  最小要素の文字列長を返します．<br>
 *  具体的には次の表のようになります．<br>
 *  TFORMn・TDIMn   bytes()   dcol_length()   drow_length()   elem_length()<br>
 *  TFORMn='120A'      120               1               1               1 <br>
 *  TFORMn='120A10'     10              12               1              12 <br>
 *  TFORMn='120A10'                                                        <br>
 *  + TDIMn='(6,2)'     10               6               2              12 <br>
 *  TFORMn='120A'                                                          <br>
 *  + TDIMn='(10,6,2)'  10               6               2              12 <br>
 *  可変長配列の場合は，配列記述子(構造体)のバイトサイズ，すなわち
 *  sizeof(fits::longarrdesc_t) または sizeof(fits::llongarrdesc_t) が
 *  返ります．
 *
 * @return  型のバイト数
 */
inline long fits_table_col::bytes() const
{
    return this->bytes_rec;
}

/**
 * @brief  カラムの1行あたりの要素の個数を取得
 *
 *  カラムの型が FITS::ASCII_T 以外の場合，例えば TTYPEn が 16D の場合，
 *  16 が返ります(TDIMn とは無関係)．<br>
 *  カラムの型が FITS::ASCII_T の場合は，fits_table_col::bytes() メンバ関数
 *  の項目にある表を参照してください．<br>
 *  可変長配列の場合は，1行あたりの配列記述子(構造体)の個数が返ります．例えば
 *  TFORM の定義が '4PE(999)' なら 4 が返ります．
 *
 * @return 1行あたりの要素の個数
 */
inline long fits_table_col::elem_length() const
{
    return this->elem_size_rec;
}

/**
 * @brief  カラムの1行あたりの要素の個数を取得
 *
 * @note  fits_table_col::elem_length() と同様．そちらの記述をご覧ください．
 */
inline long fits_table_col::elem_size() const
{
    return this->elem_size_rec;
}

/**
 * @brief  カラムの1行あたりのバイト数を取得
 *
 *  例えば，TTYPEn が 16D の場合，sizeof(fits::double_t) × 16 が返ります．<br>
 *  可変長配列の場合は，「配列記述子(構造体)のバイト長×1行あたりの配列記述子の
 *  個数」が返ります．例えば TFORM の定義が '4PE(999)' なら 
 *  sizeof(fits::longarrdesc_t) × 4 が返ります．
 *
 * @return  1行あたりのバイト数
 */
inline long fits_table_col::elem_byte_length() const
{
    return this->full_bytes_rec;
}

/**
 * @brief  TDIMn 指定における 1次元目の要素数(横方向の長さ)を取得
 *
 *  カラムの型が FITS::ASCII_T 以外の場合，例えば TDIMn が '(8,2)' の場合，
 *  8 が返ります．<br>
 *  カラムの型が FITS::ASCII_T の場合は，fits_table_col::bytes() メンバ関数
 *  の項目にある表を参照してください．
 *
 * @return  TDIMn 指定における 1次元目の要素数
 * @attention  可変長配列はサポートされていません．
 */
inline long fits_table_col::dcol_length() const
{
    return this->dcol_size_rec;
}

/**
 * @brief  TDIMn 指定における 1次元目の要素数(横方向の長さ)を取得
 *
 * @note  fits_table_col::dcol_length() と同様．そちらの記述をご覧ください．
 */
inline long fits_table_col::dcol_size() const
{
    return this->dcol_size_rec;
}

/**
 * @brief  TDIMn 指定における，2次元目以降の要素数(縦方向の長さ)を取得
 *
 *  カラムの型が FITS::ASCII_T 以外の場合，例えば TDIMn が '(8,2)' の場合，
 *  2 が返ります．<br>
 *  カラムの型が FITS::ASCII_T の場合は，fits_table_col::bytes() メンバ関数
 *  の項目にある表を参照してください．
 *
 * @return  TDIMn 指定における 2次元目以降の要素数
 * @attention  可変長配列はサポートされていません．
 */
inline long fits_table_col::drow_length() const
{
    return this->elem_size_rec / this->dcol_size_rec;
}

/**
 * @brief  TDIMn 指定における，2次元目以降の要素数(縦方向の長さ)を取得
 *
 * @note  fits_table_col::drow_size() と同様．そちらの記述をご覧ください．
 */
inline long fits_table_col::drow_size() const
{
    return this->elem_size_rec / this->dcol_size_rec;
}

/* heap info */
/**
 * @brief  可変長配列かどうかを判定
 *
 *  可変長配列である場合(ヒープを使っている場合)は true を，そうでない場合は
 *  false を返します．
 *
 * @return  可変長配列なら true
 */
inline bool fits_table_col::heap_is_used() const
{
    return ( (this->heap_type_rec == FITS::ANY_T) ? false : true );
}

/**
 * @brief  カラムのヒープ(可変長配列)のデータ型を取得
 *
 * @return  FITS::DOUBLE_T，FITS::FLOAT_T，FITS::LONGLONG_T，FITS::LONG_T，
 *          FITS::SHORT_T，FITS::BYTE_T，FITS::BIT_T，FITS::LOGICAL_T，
 *          FITS::COMPLEX_T，FITS::DOUBLECOMPLEX_T，FITS::ASCII_T のいずれか
 */
inline int fits_table_col::heap_type() const
{
    return this->heap_type_rec;
}

/**
 * @brief  ヒープ(可変長配列)の型のバイト数を取得
 *
 *  例えば，ヒープの型が FITS::DOUBLE_T の場合，sizeof(fits::double_t) が
 *  返ります．ヒープの型が FITS::BIT_T の場合は，1 が返ります．
 *
 * @return  ヒープの型のバイト数
 */
inline long fits_table_col::heap_bytes() const
{
    return this->heap_rec.bytes();
}

/**
 * @brief  可変長配列の最大の長さを取得
 *
 *  返される値はオブジェクト内部でキャッシュされている値で，この値は 
 *  fits_table_col::assign_arrdesc() で書き込むと自動的に更新されます．<br>
 *  可変長配列ではない場合は，fits_table_col::elem_length() と同様の値が
 *  返されます．
 */
inline long fits_table_col::max_array_length() const
{
    return ( (this->heap_type_rec == FITS::ANY_T) ? 
	     this->elem_size_rec : this->vl_max_length_rec );
}

/*
 * Functions to obtain NULL and scaling infornation
 */

/**
 * @brief  TUNITn の定義の有無を返す
 */
inline bool fits_table_col::tunit_is_set() const
{
    return ( (this->def_rec.tunit != NULL && this->def_rec.tunit[0] != '\0') ?
	     true : false );
}

/**
 * @brief  TNULLn の定義の有無を返す
 */
inline bool fits_table_col::tnull_is_set() const
{
    return this->tnull_is_set_rec;
    //if ( 0 < this->tany.length(TNULL_IDX) ) return true;
    //else return false;
}

/**
 * @brief  TZEROn の定義の有無を返す
 */
inline bool fits_table_col::tzero_is_set() const
{
    return ( (this->def_rec.tzero != NULL && this->def_rec.tzero[0] != '\0') ?
	     true : false );
}

/**
 * @brief  TSCALn の定義の有無を返す
 */
inline bool fits_table_col::tscal_is_set() const
{
    return ( (this->def_rec.tscal != NULL && this->def_rec.tscal[0] != '\0') ?
	     true : false );
}

/**
 * @brief  TUNITn の値を取得
 */
inline const char *fits_table_col::tunit() const
{
    return ( (this->def_rec.tunit != NULL) ? this->def_rec.tunit : "" );
}

/**
 * @brief  TNULLn の値を取得
 *
 *  Ascii Table の場合など，文字列の TNULLn 値が必要な場合は，tnull_ptr で内部
 *  バッファ上にある NULL文字列のアドレスを取得する事ができます．
 *
 * @param  tnull_ptr プログラマのポインタ変数のアドレス (省略可)
 * @return  整数型の TNULLn 値
 */
inline long long fits_table_col::tnull( const char **tnull_ptr ) const
{
    if ( tnull_ptr != NULL ) *tnull_ptr = this->def_rec.tnull;
    return this->tnull_r_rec;
}

/**
 * @brief  TZEROn の値を取得
 * @note   内部でキャッシュされた値を返すため，高速です．ループ中で使用可能．
 */
inline double fits_table_col::tzero() const
{
    return this->tzero_rec;
}

/**
 * @brief  TSCALn の値を取得
 * @note   内部でキャッシュされた値を返すため，高速です．ループ中で使用可能．
 */
inline double fits_table_col::tscal() const
{
    return this->tscal_rec;
}

/*
 * Functions to obtain length of rows. 
 */

/**
 * @brief  テーブルの行数を取得
 */
inline long fits_table_col::length() const
{
    return this->row_size_rec;
}

/**
 * @brief  テーブルの行数を取得
 *
 * @note  fits_table_col::length() との違いはありません．
 */
inline long fits_table_col::size() const
{
    return this->row_size_rec;
}

/*
 * This returns top address of internal data area
 * (only for purposes that requires highest performance)
 */

/* BIT_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::bit_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::bit_t *fits_table_col::bit_t_ptr()
{
    return ( (this->type_rec != FITS::BIT_T) ?
      (fits::bit_t *)(this->err_throw_void_p(
		  "fits_table_col::bit_t_ptr()","ERROR","type does not match"))
      :
      (fits::bit_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::bit_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::bit_t *fits_table_col::bit_t_ptr() const
{
    return ( (this->type_rec != FITS::BIT_T) ?
      (const fits::bit_t *)(this->err_throw_const_void_p(
	       "fits_table_col::bit_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::bit_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::bit_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::bit_t *fits_table_col::bit_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::BIT_T) ?
      (const fits::bit_t *)(this->err_throw_const_void_p(
	       "fits_table_col::bit_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::bit_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::bit_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::bit_t *fits_table_col::bit_t_ptr( long row_idx )
{
    return 
    ( (this->type_rec != FITS::BIT_T) ?
      (fits::bit_t *)(this->err_throw_void_p(
		  "fits_table_col::bit_t_ptr()","ERROR","type does not match"))
      :
      ( (this->full_bytes_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	(fits::bit_t *)((char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::bit_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::bit_t *fits_table_col::bit_t_ptr( long row_idx ) const
{
    return 
    ( (this->type_rec != FITS::BIT_T) ?
      (const fits::bit_t *)(this->err_throw_const_void_p(
	       "fits_table_col::bit_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->full_bytes_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	(const fits::bit_t *)((const char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::bit_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::bit_t *fits_table_col::bit_t_ptr_cs( long row_idx ) const
{
    return 
    ( (this->type_rec != FITS::BIT_T) ?
      (const fits::bit_t *)(this->err_throw_const_void_p(
	       "fits_table_col::bit_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->full_bytes_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	(const fits::bit_t *)((const char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
      )
    );
}

/* BYTE_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::byte_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *fits_table_col::byte_t_ptr()
{
    return ( (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *)(this->err_throw_void_p(
		 "fits_table_col::byte_t_ptr()","ERROR","type does not match"))
      :
      (fits::byte_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_table_col::byte_t_ptr() const
{
    return ( (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *)(this->err_throw_const_void_p(
	      "fits_table_col::byte_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::byte_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_table_col::byte_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *)(this->err_throw_const_void_p(
	      "fits_table_col::byte_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::byte_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::byte_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::byte_t *fits_table_col::byte_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::BYTE_T) ?
      (fits::byte_t *)(this->err_throw_void_p(
		 "fits_table_col::byte_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::byte_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_table_col::byte_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *)(this->err_throw_const_void_p(
	      "fits_table_col::byte_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::byte_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::byte_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::byte_t *fits_table_col::byte_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::BYTE_T) ?
      (const fits::byte_t *)(this->err_throw_const_void_p(
	      "fits_table_col::byte_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::byte_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* LOGICAL_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::logical_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::logical_t *fits_table_col::logical_t_ptr()
{
    return ( (this->type_rec != FITS::LOGICAL_T) ?
      (fits::logical_t *)(this->err_throw_void_p(
	      "fits_table_col::logical_t_ptr()","ERROR","type does not match"))
      :
      (fits::logical_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::logical_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::logical_t *fits_table_col::logical_t_ptr() const
{
    return ( (this->type_rec != FITS::LOGICAL_T) ?
      (const fits::logical_t *)(this->err_throw_const_void_p(
	   "fits_table_col::logical_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::logical_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::logical_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::logical_t *fits_table_col::logical_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::LOGICAL_T) ?
      (const fits::logical_t *)(this->err_throw_const_void_p(
	   "fits_table_col::logical_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::logical_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::logical_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::logical_t *fits_table_col::logical_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::LOGICAL_T) ?
      (fits::logical_t *)(this->err_throw_void_p(
	      "fits_table_col::logical_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::logical_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::logical_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::logical_t *fits_table_col::logical_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LOGICAL_T) ?
      (const fits::logical_t *)(this->err_throw_const_void_p(
	   "fits_table_col::logical_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::logical_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::logical_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::logical_t *fits_table_col::logical_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LOGICAL_T) ?
      (const fits::logical_t *)(this->err_throw_const_void_p(
	   "fits_table_col::logical_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::logical_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* ASCII_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::ascii_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::ascii_t *fits_table_col::ascii_t_ptr()
{
    return ( (this->type_rec != FITS::ASCII_T) ?
      (fits::ascii_t *)(this->err_throw_void_p(
		"fits_table_col::ascii_t_ptr()","ERROR","type does not match"))
      :
      (fits::ascii_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::ascii_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::ascii_t *fits_table_col::ascii_t_ptr() const
{
    return ( (this->type_rec != FITS::ASCII_T) ?
      (const fits::ascii_t *)(this->err_throw_const_void_p(
	     "fits_table_col::ascii_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::ascii_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::ascii_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::ascii_t *fits_table_col::ascii_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::ASCII_T) ?
      (const fits::ascii_t *)(this->err_throw_const_void_p(
	     "fits_table_col::ascii_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::ascii_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::ascii_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::ascii_t *fits_table_col::ascii_t_ptr( long row_idx )
{
    return 
    ( (this->type_rec != FITS::ASCII_T) ?
      (fits::ascii_t *)(this->err_throw_void_p(
		"fits_table_col::ascii_t_ptr()","ERROR","type does not match"))
      :
      ( (this->full_bytes_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	(fits::ascii_t *)((char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::ascii_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::ascii_t *fits_table_col::ascii_t_ptr( long row_idx ) const
{
    return 
    ( (this->type_rec != FITS::ASCII_T) ?
      (const fits::ascii_t *)(this->err_throw_const_void_p(
	     "fits_table_col::ascii_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->full_bytes_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	(const fits::ascii_t *)((const char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::ascii_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::ascii_t *fits_table_col::ascii_t_ptr_cs( long row_idx ) const
{
    return 
    ( (this->type_rec != FITS::ASCII_T) ?
      (const fits::ascii_t *)(this->err_throw_const_void_p(
	     "fits_table_col::ascii_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->full_bytes_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	(const fits::ascii_t *)((const char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
      )
    );
}

/* SHORT_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::short_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *fits_table_col::short_t_ptr()
{
    return ( (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *)(this->err_throw_void_p(
		"fits_table_col::short_t_ptr()","ERROR","type does not match"))
      :
      (fits::short_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_table_col::short_t_ptr() const
{
    return ( (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *)(this->err_throw_const_void_p(
	     "fits_table_col::short_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::short_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_table_col::short_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *)(this->err_throw_const_void_p(
	     "fits_table_col::short_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::short_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::short_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::short_t *fits_table_col::short_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::SHORT_T) ?
      (fits::short_t *)(this->err_throw_void_p(
		"fits_table_col::short_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::short_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_table_col::short_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *)(this->err_throw_const_void_p(
	     "fits_table_col::short_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::short_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::short_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::short_t *fits_table_col::short_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::SHORT_T) ?
      (const fits::short_t *)(this->err_throw_const_void_p(
	     "fits_table_col::short_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::short_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* LONG_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::long_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *fits_table_col::long_t_ptr()
{
    return ( (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *)(this->err_throw_void_p(
		 "fits_table_col::long_t_ptr()","ERROR","type does not match"))
      :
      (fits::long_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_table_col::long_t_ptr() const
{
    return ( (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *)(this->err_throw_const_void_p(
	      "fits_table_col::long_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::long_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_table_col::long_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *)(this->err_throw_const_void_p(
	      "fits_table_col::long_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::long_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::long_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::long_t *fits_table_col::long_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::LONG_T) ?
      (fits::long_t *)(this->err_throw_void_p(
		 "fits_table_col::long_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::long_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_table_col::long_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *)(this->err_throw_const_void_p(
	      "fits_table_col::long_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::long_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::long_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::long_t *fits_table_col::long_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LONG_T) ?
      (const fits::long_t *)(this->err_throw_const_void_p(
	      "fits_table_col::long_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::long_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* LONGLONG_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longlong_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *fits_table_col::longlong_t_ptr()
{
    return ( (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *)(this->err_throw_void_p(
	     "fits_table_col::longlong_t_ptr()","ERROR","type does not match"))
      :
      (fits::longlong_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_table_col::longlong_t_ptr() const
{
    return ( (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *)(this->err_throw_const_void_p(
	  "fits_table_col::longlong_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::longlong_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_table_col::longlong_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *)(this->err_throw_const_void_p(
	  "fits_table_col::longlong_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::longlong_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longlong_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longlong_t *fits_table_col::longlong_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::LONGLONG_T) ?
      (fits::longlong_t *)(this->err_throw_void_p(
	     "fits_table_col::longlong_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::longlong_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_table_col::longlong_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *)(this->err_throw_const_void_p(
	  "fits_table_col::longlong_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::longlong_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longlong_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longlong_t *fits_table_col::longlong_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LONGLONG_T) ?
      (const fits::longlong_t *)(this->err_throw_const_void_p(
	  "fits_table_col::longlong_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::longlong_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* FLOAT_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::float_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *fits_table_col::float_t_ptr()
{
    return ( (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *)(this->err_throw_void_p(
		"fits_table_col::float_t_ptr()","ERROR","type does not match"))
      :
      (fits::float_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_table_col::float_t_ptr() const
{
    return ( (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *)(this->err_throw_const_void_p(
	     "fits_table_col::float_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::float_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_table_col::float_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *)(this->err_throw_const_void_p(
	     "fits_table_col::float_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::float_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::float_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::float_t *fits_table_col::float_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::FLOAT_T) ?
      (fits::float_t *)(this->err_throw_void_p(
		"fits_table_col::float_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::float_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_table_col::float_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *)(this->err_throw_const_void_p(
	     "fits_table_col::float_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::float_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::float_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::float_t *fits_table_col::float_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::FLOAT_T) ?
      (const fits::float_t *)(this->err_throw_const_void_p(
	     "fits_table_col::float_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::float_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* DOUBLE_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::double_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *fits_table_col::double_t_ptr()
{
    return ( (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *)(this->err_throw_void_p(
	       "fits_table_col::double_t_ptr()","ERROR","type does not match"))
      :
      (fits::double_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_table_col::double_t_ptr() const
{
    return ( (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *)(this->err_throw_const_void_p(
	    "fits_table_col::double_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::double_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_table_col::double_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *)(this->err_throw_const_void_p(
	    "fits_table_col::double_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::double_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::double_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::double_t *fits_table_col::double_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::DOUBLE_T) ?
      (fits::double_t *)(this->err_throw_void_p(
	       "fits_table_col::double_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::double_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_table_col::double_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *)(this->err_throw_const_void_p(
	    "fits_table_col::double_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::double_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::double_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::double_t *fits_table_col::double_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::DOUBLE_T) ?
      (const fits::double_t *)(this->err_throw_const_void_p(
	    "fits_table_col::double_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::double_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* COMPLEX_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::complex_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::complex_t *fits_table_col::complex_t_ptr()
{
    return ( (this->type_rec != FITS::COMPLEX_T) ?
      (fits::complex_t *)(this->err_throw_void_p(
	      "fits_table_col::complex_t_ptr()","ERROR","type does not match"))
      :
      (fits::complex_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::complex_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::complex_t *fits_table_col::complex_t_ptr() const
{
    return ( (this->type_rec != FITS::COMPLEX_T) ?
      (const fits::complex_t *)(this->err_throw_const_void_p(
	   "fits_table_col::complex_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::complex_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::complex_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::complex_t *fits_table_col::complex_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::COMPLEX_T) ?
      (const fits::complex_t *)(this->err_throw_const_void_p(
	   "fits_table_col::complex_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::complex_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::complex_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::complex_t *fits_table_col::complex_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::COMPLEX_T) ?
      (fits::complex_t *)(this->err_throw_void_p(
	      "fits_table_col::complex_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::complex_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::complex_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::complex_t *fits_table_col::complex_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::COMPLEX_T) ?
      (const fits::complex_t *)(this->err_throw_const_void_p(
	   "fits_table_col::complex_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::complex_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::complex_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::complex_t *fits_table_col::complex_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::COMPLEX_T) ?
      (const fits::complex_t *)(this->err_throw_const_void_p(
	   "fits_table_col::complex_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::complex_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* DOUBLECOMPLEX_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::doublecomplex_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::doublecomplex_t *fits_table_col::doublecomplex_t_ptr()
{
    return ( (this->type_rec != FITS::DOUBLECOMPLEX_T) ?
      (fits::doublecomplex_t *)(this->err_throw_void_p(
	"fits_table_col::doublecomplex_t_ptr()","ERROR","type does not match"))
      :
      (fits::doublecomplex_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::doublecomplex_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::doublecomplex_t *fits_table_col::doublecomplex_t_ptr() const
{
    return ( (this->type_rec != FITS::DOUBLECOMPLEX_T) ?
      (const fits::doublecomplex_t *)(this->err_throw_const_void_p(
     "fits_table_col::doublecomplex_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::doublecomplex_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::doublecomplex_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::doublecomplex_t *fits_table_col::doublecomplex_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::DOUBLECOMPLEX_T) ?
      (const fits::doublecomplex_t *)(this->err_throw_const_void_p(
     "fits_table_col::doublecomplex_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::doublecomplex_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::doublecomplex_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::doublecomplex_t *fits_table_col::doublecomplex_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::DOUBLECOMPLEX_T) ?
      (fits::doublecomplex_t *)(this->err_throw_void_p(
	"fits_table_col::doublecomplex_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::doublecomplex_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::doublecomplex_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::doublecomplex_t *fits_table_col::doublecomplex_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::DOUBLECOMPLEX_T) ?
      (const fits::doublecomplex_t *)(this->err_throw_const_void_p(
     "fits_table_col::doublecomplex_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::doublecomplex_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::doublecomplex_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::doublecomplex_t *fits_table_col::doublecomplex_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::DOUBLECOMPLEX_T) ?
      (const fits::doublecomplex_t *)(this->err_throw_const_void_p(
     "fits_table_col::doublecomplex_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::doublecomplex_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* LONGARRDESC_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longarrdesc_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longarrdesc_t *fits_table_col::longarrdesc_t_ptr()
{
    return ( (this->type_rec != FITS::LONGARRDESC_T) ?
      (fits::longarrdesc_t *)(this->err_throw_void_p(
	  "fits_table_col::longarrdesc_t_ptr()","ERROR","type does not match"))
      :
      (fits::longarrdesc_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longarrdesc_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longarrdesc_t *fits_table_col::longarrdesc_t_ptr() const
{
    return ( (this->type_rec != FITS::LONGARRDESC_T) ?
      (const fits::longarrdesc_t *)(this->err_throw_const_void_p(
       "fits_table_col::longarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::longarrdesc_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longarrdesc_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longarrdesc_t *fits_table_col::longarrdesc_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::LONGARRDESC_T) ?
      (const fits::longarrdesc_t *)(this->err_throw_const_void_p(
       "fits_table_col::longarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::longarrdesc_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longarrdesc_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::longarrdesc_t *fits_table_col::longarrdesc_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::LONGARRDESC_T) ?
      (fits::longarrdesc_t *)(this->err_throw_void_p(
	  "fits_table_col::longarrdesc_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::longarrdesc_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longarrdesc_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longarrdesc_t *fits_table_col::longarrdesc_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LONGARRDESC_T) ?
      (const fits::longarrdesc_t *)(this->err_throw_const_void_p(
       "fits_table_col::longarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::longarrdesc_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::longarrdesc_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::longarrdesc_t *fits_table_col::longarrdesc_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LONGARRDESC_T) ?
      (const fits::longarrdesc_t *)(this->err_throw_const_void_p(
       "fits_table_col::longarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::longarrdesc_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* LLONGARRDESC_T */

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::llongarrdesc_t)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::llongarrdesc_t *fits_table_col::llongarrdesc_t_ptr()
{
    return ( (this->type_rec != FITS::LLONGARRDESC_T) ?
      (fits::llongarrdesc_t *)(this->err_throw_void_p(
	 "fits_table_col::llongarrdesc_t_ptr()","ERROR","type does not match"))
      :
      (fits::llongarrdesc_t *)(this->data_ptr_rec) );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::llongarrdesc_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::llongarrdesc_t *fits_table_col::llongarrdesc_t_ptr() const
{
    return ( (this->type_rec != FITS::LLONGARRDESC_T) ?
      (const fits::llongarrdesc_t *)(this->err_throw_const_void_p(
      "fits_table_col::llongarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::llongarrdesc_t *)(this->data_ptr_rec) );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::llongarrdesc_t・読取専用)
 *
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::llongarrdesc_t *fits_table_col::llongarrdesc_t_ptr_cs() const
{
    return ( (this->type_rec != FITS::LLONGARRDESC_T) ?
      (const fits::llongarrdesc_t *)(this->err_throw_const_void_p(
      "fits_table_col::llongarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      (const fits::llongarrdesc_t *)(this->data_ptr_rec) );
}

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::llongarrdesc_t)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline fits::llongarrdesc_t *fits_table_col::llongarrdesc_t_ptr( long row_idx )
{
    return
    ( (this->type_rec != FITS::LLONGARRDESC_T) ?
      (fits::llongarrdesc_t *)(this->err_throw_void_p(
	 "fits_table_col::llongarrdesc_t_ptr()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((fits::llongarrdesc_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (fits::llongarrdesc_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::llongarrdesc_t *fits_table_col::llongarrdesc_t_ptr( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LLONGARRDESC_T) ?
      (const fits::llongarrdesc_t *)(this->err_throw_const_void_p(
      "fits_table_col::llongarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::llongarrdesc_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (fits::llongarrdesc_t・読取専用)
 *
 * @param  行インデックス
 * @throw  呼び出されたメンバ関数が，自身のデータ型に不適切な場合
 */
inline const fits::llongarrdesc_t *fits_table_col::llongarrdesc_t_ptr_cs( long row_idx ) const
{
    return
    ( (this->type_rec != FITS::LLONGARRDESC_T) ?
      (const fits::llongarrdesc_t *)(this->err_throw_const_void_p(
      "fits_table_col::llongarrdesc_t_ptr_cs()","ERROR","type does not match"))
      :
      ( (this->elem_size_rec == 0 ||
	 row_idx < 0 || this->row_size_rec <= row_idx) ?
	NULL
	:
	((const fits::llongarrdesc_t *)(this->data_ptr_rec) + this->elem_size_rec * row_idx)
      )
    );
}

/* RAW */

/**
 * @brief  セルデータのバッファアドレスを取得 (void *)
 *
 * @attention  型チェックは行なわれません．fits_table_col::float_t_ptr() などの
 *             各型専用のメンバ関数を検討してください．
 */
inline void *fits_table_col::data_ptr()
{
    return this->data_ptr_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (void *・読取専用)
 *
 * @attention  型チェックは行なわれません．fits_table_col::float_t_ptr() などの
 *             各型専用のメンバ関数を検討してください．
 */
inline const void *fits_table_col::data_ptr() const
{
    return this->data_ptr_rec;
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (void *・読取専用)
 *
 * @attention  型チェックは行なわれません．fits_table_col::float_t_ptr() などの
 *             各型専用のメンバ関数を検討してください．
 */
inline const void *fits_table_col::data_ptr_cs() const
{
    return this->data_ptr_rec;
}

/**
 * @brief  セルデータのバッファアドレスを取得 (void *)
 *
 * @param  行インデックス
 * @attention  型チェックは行なわれません．fits_table_col::float_t_ptr() などの
 *             各型専用のメンバ関数を検討してください．
 */
inline void *fits_table_col::data_ptr( long row_idx )
{
    return ( (this->full_bytes_rec == 0 /* not defined */ ||
	      row_idx < 0 || this->row_size_rec <= row_idx) ?
	     NULL
	     :
	     (void *)((char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
	   );
}

#ifdef SLI__OVERLOAD_CONST_AT
/**
 * @brief  セルデータのバッファアドレスを取得 (void *・読取専用)
 *
 * @param  行インデックス
 * @attention  型チェックは行なわれません．fits_table_col::float_t_ptr() などの
 *             各型専用のメンバ関数を検討してください．
 */
inline const void *fits_table_col::data_ptr( long row_idx ) const
{
    return ( (this->full_bytes_rec == 0 /* not defined */ ||
	      row_idx < 0 || this->row_size_rec <= row_idx) ?
	     NULL
	     :
	     (const void *)((const char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
	   );
}
#endif

/**
 * @brief  セルデータのバッファアドレスを取得 (void *・読取専用)
 *
 * @param  行インデックス
 * @attention  型チェックは行なわれません．fits_table_col::float_t_ptr() などの
 *             各型専用のメンバ関数を検討してください．
 */
inline const void *fits_table_col::data_ptr_cs( long row_idx ) const
{
    return ( (this->full_bytes_rec == 0 /* not defined */ ||
	      row_idx < 0 || this->row_size_rec <= row_idx) ?
	     NULL
	     :
	     (const void *)((const char *)(this->data_ptr_rec) + this->full_bytes_rec * row_idx)
	   );
}


}

#endif	/* _SLI__FITS_TABLE_COL_H */
