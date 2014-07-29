/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2014-07-30 00:49:32 cyamauch> */

#ifndef _SLI__FITS_TABLE_H
#define _SLI__FITS_TABLE_H 1

/**
 * @file   fits_table.h
 * @brief  ASCII or Binary Table を持つ HDU を表現するクラス fits_table の定義
 */

#include "fits_hdu.h"
#include "fits_table_col.h"

#ifdef BUILD_SFITSIO
#include <sli/ctindex.h>
#else
#include "ctindex.h"
#endif

namespace sli
{

  class fitscc;

  /*
   * sil::fits_table class (inherited class of sli::fits_hdu) expresses a table
   * HDU of FITS data structure, and manages objects of sli::fits_table_col.
   * Objects of this class are usually managed by an object of sli::fitscc 
   * class.
   *
   * Programmers usually use APIs for binary table or ASCII table like this:
   *   fits.table("TABLE_A").col("COLUMN_A"). ...
   *   fits.table("TABLE_A"). ...
   */

  /**
   * @class  sli::fits_table
   * @brief  FITS の ASCII Table か Binary Table を持つ HDU を表現するクラス
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fits_table : public fits_hdu

  {

    friend class fitscc;
    friend class fits_table_col;

  public:
    /* constructor & destructor */
    fits_table();
    fits_table(const fits_table &obj);
    ~fits_table();

    /* complete initialization */
    fits_table &operator=(const fits_table &obj);
    fits_table &init();					/* overridden */
    fits_table &init( const fits_table &obj );		/* overridden */

    /* swap contents between self and obj */
    fits_table &swap( fits_table &obj );		/* overridden */
    
    /* complete initialization (added) */
    virtual fits_table &init( const fits::table_def defs[] );
    virtual fits_table &init( const fits::table_def_all defs[] );

    /* convert from ASCII table to binary table */
    virtual fits_table &ascii_to_binary();

    /* returns column number */
    virtual long col_index( const char *col_name ) const;

    /* returns column name (TTYPEn) */
    virtual const char *col_name( long col_index ) const;

    /* returns number of columns */
    virtual long col_length() const;
    /* not recommended */
    virtual long col_size() const;		/* same as col_length() */

    /* set/rename column name (TTYPEn) */
    virtual fits_table &assign_col_name( long col_index, const char *newname );
    virtual fits_table &assign_col_name( const char *col_name, 
					 const char *newname );

    /* This sets NULL value string given by snull to all columns. */
    /* See also fits_table_col::assign_null_svalue().             */
    virtual fits_table &assign_null_svalue( const char *snull );

    /* This sets NULL value string given by snull for next initialization */
    /* of columns. (i.e., init() is called)                               */
    virtual fits_table &assign_default_null_svalue( const char *snull );

    /* returns the value set by assign_default_null_svalue() */
    virtual const char *default_null_svalue() const;

    //int resize_cols( long num_cols );
    //int copy_cols( long src_index, long dest_index, long num_cols );

    /* swap columns */
    virtual fits_table &swap_cols( long index0, long num_cols, long index1  );
    virtual fits_table &swap_cols( const char *col_name0, long num_cols,
				    const char *col_name1 );

    /* erase columns */
    virtual fits_table &erase_cols( long index0, long num_cols );
    virtual fits_table &erase_cols( const char *col_name, long num_cols );

    //int insert_cols( long index, long num_cols );
    //virtual int import_cols( long record_index,
    //			     fits_hdu *from, long index_begin, long num_cols );

    /* returns length of rows */
    virtual long row_length() const;
    /* not recommended */
    virtual long row_size() const;		/* same as row_length() */

    /* appends blank rows */
    virtual fits_table &append_rows( long num_rows );

    /* changes length of rows */
    virtual fits_table &resize_rows( long num_rows );

    /* copy row into row */
    virtual fits_table &move_rows( long src_index, long num_rows,
				   long dest_index );

    /* swap rows */
    virtual fits_table &swap_rows( long index0, long num_rows,
				   long index1 );

    /* flip rows */
    virtual fits_table &flip_rows( long index, long num_rows );

    /* Note that heavy use of erase_rows() or insert_rows() might cause    */
    /* performance problems, since this needs memory copy for each column. */

    /* erase rows */
    virtual fits_table &erase_rows( long index, long num_rows );

    /* fill all or part of rows using default value */
    virtual fits_table &clean_rows();			/* for all rows */
    virtual fits_table &clean_rows( long row_index, long num_rows );

    /* insert blank rows */
    virtual fits_table &insert_rows( long index, long num_rows );

    /* import table                                                        */
    /* How to allocate each of columns on 'from' to the object is decided  */
    /* by `match_by_name'.  When `match_by_name' is true, it searches the  */
    /* column of which the names are identical and imports the column.     */
    /* When `match_by_name' is false, it imports `from' the 0th column     */
    /* in order.                                                           */
    /* The column type of `from` table and column type of the object does  */
    /* not need to be identical.  If two column types are not identical,   */
    /* it converts the type and imports.                                   */
    /* Note that complex type and variable length array are not supported. */
    virtual fits_table &import_rows( long dest_index, bool match_by_name,
				     const fits_table &from, 
				     long idx_begin = 0, 
				     long num_rows = FITS::ALL );

    /* Note that heavy use of erase_a_rows() or insert_a_rows() might cause */
    /* performance problems, since this needs memory copy for each column.  */

    /* append a blank row */
    virtual fits_table &append_a_row();

    /* insert a blank row */
    virtual fits_table &insert_a_row( long index );

    /* erase a row */
    virtual fits_table &erase_a_row( long index );

    /* fill a row using default value */
    virtual fits_table &clean_a_row( long index );

    /* setting of strategy of memory allocation for rows of all columns */
    /* "auto" (default), "min" and "pow" can be set.                    */
    virtual fits_table &set_alloc_strategy_of_rows( const char *strategy );


    /* append columns */
    virtual fits_table &append_cols( const fits::table_def defs[] );
    virtual fits_table &append_cols( const fits::table_def_all defs[] );
    virtual fits_table &append_cols( const fits_table &src );

    /* insert columns */
    virtual fits_table &insert_cols( long index0, 
				     const fits::table_def defs[] );
    virtual fits_table &insert_cols( long index0, 
				     const fits::table_def_all defs[] );
    virtual fits_table &insert_cols( const char *col_name, 
				     const fits::table_def defs[] );
    virtual fits_table &insert_cols( const char *col_name, 
				     const fits::table_def_all defs[] );
    virtual fits_table &insert_cols( long index0, const fits_table &src );
    virtual fits_table &insert_cols( const char *col_name, 
				     const fits_table &src );

    /* append a column */
    virtual fits_table &append_a_col( const fits_table_col &src );

    /* insert a column */
    virtual fits_table &insert_a_col( long col_index, 
				      const fits_table_col &src );
    virtual fits_table &insert_a_col( const char *col_name, 
				      const fits_table_col &src );

    /* Do not use; use put_a_col() instead */
    virtual fits_table &paste_a_col( long col_index, 
				     const fits_table_col &src );
    virtual fits_table &paste_a_col( const char *col_name, 
				     const fits_table_col &src );

    /* append a column */
    virtual fits_table &append_a_col( const fits::table_def &def );
    virtual fits_table &append_a_col( const fits::table_def_all &def );

    /* insert a column */
    virtual fits_table &insert_a_col( long col_index, 
				      const fits::table_def &def );
    virtual fits_table &insert_a_col( long col_index, 
				      const fits::table_def_all &def );
    virtual fits_table &insert_a_col( const char *col_name, 
				      const fits::table_def &def );
    virtual fits_table &insert_a_col( const char *col_name, 
				      const fits::table_def_all &def );

    /* erase a column */
    virtual fits_table &erase_a_col( long col_index );
    virtual fits_table &erase_a_col( const char *col_name );

    /* change the definition of a column                                   */
    /* define_a_col() updates column properties only for non-NULL elements */
    /* of given fits::table_def structure, i.e., set NULL to elements of   */
    /* the structure to keep previous properties.                          */
    virtual fits_table &define_a_col( long col_index, 
				      const fits::table_def &def );
    virtual fits_table &define_a_col( long col_index, 
				      const fits::table_def_all &def );
    virtual fits_table &define_a_col( const char *col_name, 
				      const fits::table_def &def );
    virtual fits_table &define_a_col( const char *col_name, 
				      const fits::table_def_all &def );

    /*
     * Programmers can define new column keywords undefined in FITS standard
     * using following APIs.  New column keywords are saved in the header 
     * record of TXFLDKWD keyword.
     */

    /* returns array of programmer's new column keywords */
    virtual const char *const *user_col_keywords() const;
    virtual const tarray_tstring &user_col_keywords_cs() const;

    /* returns true if specified keyword is registered as new column keyword */
    virtual bool is_user_col_keyword( const char *kwd ) const;

    /* append a new column keyword */
    virtual fits_table &append_a_user_col_keyword( const char *kwd );

    /* erase a new column keyword */
    virtual fits_table &erase_a_user_col_keyword( const char *kwd );

    /* append new column keywords */
    virtual fits_table &append_user_col_keywords( const char *const kwds[] );

    /* erase new column keywords */
    virtual fits_table &erase_user_col_keywords( const char *const kwds[] );
    virtual fits_table &erase_user_col_keywords();		/* erase all */

    /* returns element number of a header record   */
    /* having specified column and keyword.        */
    /* For example, col_index = 0L, kwd = "TFORM". */
    virtual long col_header_index( const char *col_name, 
				   const char *kwd ) const;
    virtual long col_header_index( long col_index, 
				   const char *kwd ) const;

    /* returns reference of a header record */
    /* having specified column and keyword. */
    virtual fits_header_record &col_header( const char *col_name, 
    					    const char *kwd );
    virtual fits_header_record &col_header( long col_index, 
    					    const char *kwd );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_header_record &col_header( const char *col_name, 
    						  const char *kwd ) const;
    virtual const fits_header_record &col_header( long col_index, 
    						  const char *kwd ) const;
#endif
    virtual const fits_header_record &col_header_cs( const char *col_name, 
    						     const char *kwd ) const;
    virtual const fits_header_record &col_header_cs( long col_index, 
    						     const char *kwd ) const;

    /*
     * update_col_header() and erase_col_header() can be used for both 
     * FITS standard keyword such as TDIMn and programmer's new keywords.
     */

    /* update a header record having specified column and keyword. */
    virtual fits_table &update_col_header( const char *col_name, 
     			   const char *kwd, const char *val, const char *com );
    virtual fits_table &update_col_header( long col_index, 
    			   const char *kwd, const char *val, const char *com );

    /* erase a header record having specified column and keyword. */
    virtual fits_table &erase_col_header( const char *col_name, 
					  const char *kwd );
    virtual fits_table &erase_col_header( long col_index, 
					  const char *kwd );

    /* rename all column header records having specified keyword. */
    /* This can be used only for programmer's new keywords.       */
    virtual fits_table &rename_col_header( const char *old_kwd, 
					   const char *new_kwd );

    /* sort all column header records                             */
    /* This will be useful to fix FITS header written by MWRFITS. */
    virtual fits_table &sort_col_header();

    /* copy all or specified rows to another fits_table object */
    virtual void copy( fits_table *dest ) const;
    virtual void copy( long idx_begin, long num_rows, fits_table *dest ) const;
    /* not recommended */
    virtual void copy( fits_table &dest ) const;
    /* not recommended */
    virtual void copy( long idx_begin, long num_rows, fits_table &dest ) const;

  protected:
    /* bte_load() から使用する．ヘッダは更新しない */
    virtual fits_table &_append_cols( const fits::table_def_all defs[] );
    /* */
    virtual void copy( const char *name_begin, long num_rows, 
		       fits_table *dest ) const;
    virtual void copy( long idx_begin, fits_table *dest ) const;
    virtual void copy_cols( const char *const col_names[], 
			    fits_table *dest ) const;
    virtual void copy_cols( fits_table *dest,
			    const char *col0, ... ) const;
    virtual void va_copy_cols( fits_table *dest,
			       const char *col0, va_list ap ) const;
    virtual void copy_cols( const long col_idxs[], long num_cols,
			    fits_table *dest ) const;
    virtual void copy_cols( fits_table *dest,
			    long num_cols, ... ) const;
    virtual void va_copy_cols( fits_table *dest,
			       long num_cols, va_list ap ) const;

  public:
    /* These member functions return reference of specified column object. */
    virtual fits_table_col &col( long col_index );
    virtual fits_table_col &col( const char *col_name );
    virtual fits_table_col &colf( const char *fmt, ... );
    virtual fits_table_col &vcolf( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_table_col &col( long col_index ) const;
    virtual const fits_table_col &col( const char *col_name ) const;
    virtual const fits_table_col &colf( const char *fmt, ... ) const;
    virtual const fits_table_col &vcolf( const char *fmt, va_list ap ) const;
#endif
    virtual const fits_table_col &col_cs( long col_index ) const;
    virtual const fits_table_col &col_cs( const char *col_name ) const;
    virtual const fits_table_col &colf_cs( const char *fmt, ... ) const;
    virtual const fits_table_col &vcolf_cs( const char *fmt, va_list ap ) const;
    /* overwrite a column with specified column data */
    virtual fits_table &put_a_col( long col_index, const fits_table_col &col );
    virtual fits_table &put_a_col( const char *col_name, 
				   const fits_table_col &col );

    /* low-level member functions for heap area                        */
    /* Note that there are no guarantee of byte alignment in heap area */
    /* and data in it are stored in big-endian.                        */

    /* ヒープエリアは，データは big-endian で格納され，バイト境界での  */
    /* アライメントも一切考慮されていない．                            */
    /* 以下のメンバ関数を使ってデータにアクセスする時は，              */
    /*  unsigned char *heap = ...;                                     */
    /*  short val = (heap[off + 0] << 8) | (heap[off + 1]);            */
    /* のようにする必要がある．                                        */

    /* return address of heap area */
    virtual void *heap_ptr();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const void *heap_ptr() const;
#endif
    virtual const void *heap_ptr_cs() const;

    /* obtain raw data of heap area */
    /*   offset: byte offset        */
    virtual ssize_t get_heap( void *dest_buf, size_t buf_size ) const;
    virtual ssize_t get_heap( long offset, 
			      void *dest_buf, size_t buf_size ) const;

    /* set raw data of heap area */
    /*   offset: byte offset     */
    virtual ssize_t put_heap( const void *src_buf, size_t buf_size );
    virtual ssize_t put_heap( long offset, const void *src_buf, size_t buf_size );

    /* reverse endian of specified part of heap area if required */
    /* No endian adjustment on big-endian machines.              */
    /*   offset: byte offset                                     */
    virtual fits_table &reverse_heap_endian( long offset, 
					     int type, long length );

    /* change length of heap area */
    virtual fits_table &resize_heap( size_t sz );

    /* returns length of heap area */
    virtual size_t heap_length();

    /* returns reference of internal object that manages heap area */
    virtual const mdarray &heap_array_cs() const;

    /* returns length of reserved area before heap */
    virtual long long reserved_area_length() const;

    /* set length of reserved area before heap       */
    /* This will change the value of THEAP on header */
    virtual fits_table &resize_reserved_area( long long val );

    /* setting of strategy of memory allocation for heap */
    /* "auto" (default), "min" and "pow" can be set.     */
    virtual fits_table &set_alloc_strategy_of_heap( const char *strategy );

    /* shallow copy を許可する場合に使用 (未実装)  */
    /* (一時オブジェクトの return の直前で使用)    */
    /* (fits_hdu::set_scopy_flag() のオーバライド) */
    void set_scopy_flag();

  protected:
    /* steram io */
    /* overridden member functions */
    ssize_t read_stream( cstreamio &sref, size_t max_bytes_read );
    ssize_t read_stream( cstreamio &sref );
    ssize_t write_stream( cstreamio &sref );
    ssize_t stream_length();
    /* 追加 */
    virtual ssize_t read_stream( fits_hdu *objp, cstreamio &sref );
    virtual ssize_t read_stream( fits_hdu *objp, cstreamio &sref,
			       const char *section_to_read, bool is_last_hdu );
    virtual ssize_t read_stream( fits_hdu *objp, cstreamio &sref, 
				 const char *section_to_read, bool is_last_hdu,
				 size_t max_bytes_read );
    /* overwritten */
    ssize_t save_or_check_data( cstreamio *sptr, void *c_sum_info );
    /* */
    virtual fits_table &set_protection( long col_index, bool protect );
    fits_table &setup_sys_header();			/* overridden */
    virtual void **data_ptr_mng_ptr( long col_index );
    virtual void **heap_ptr_mng_ptr();

  private:
    void reverse_endian();
    ssize_t bte_load( const fits_hdu *objp, cstreamio &sref, 
		      const char *section_to_read, bool is_last_hdu,
		      const size_t *max_bytes_ptr );
    long find_txfldkwd_insert_point() const;
    long find_theap_insert_point() const;
    long find_colhdr_insert_point( long column_idx ) const;
    fits_table &update_hdr_of_a_col( long col_index, 
				     long *ret_idx0, long *ret_tbcol_idx );
    fits_table &update_tbcol_hdr();
    fits_table &erase_hdr_of_a_col( long col_idx );
    fits_table &renumber_hdr_of_a_col( long old_col_idx, long new_col_idx );
    fits_table &update_hdr_of_col_props();
    fits_table &update_hdr_of_row_props();
    fits_table &setup_txfldkwd_hdr();

    /* shallow copy 関係 */
    bool request_shallow_copy( fits_table *from_obj ) const;
    void cancel_shallow_copy( fits_table *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;

  private:
    long row_size_rec;
    long col_size_rec;
    mdarray cols_rec;
    fits_table_col **cols_ptr_rec;	/* cols_rec のバッファptr(自動更新) */
    ctindex index_rec;			/* カラム名用のインデックス */
    tstring default_null_svalue_rec;	/* fits_table_colクラスの */
					/* null_svalue_rec のデフォルト値 */
    tarray_tstring user_fld_kwd_rec;	/* ユーザ定義のカラムキーワード */
    mdarray heap_rec;			/* 可変長配列用のヒープバッファ */
    void *heap_ptr_rec;	/* heap_rec のバッファのアドレスを保持．自動更新 */
    long long reserve_length_rec;

    /* shallow copy のためのフラグ類．= 等ではコピーされない */
    /* コピーの src 側のフラグ */
    bool shallow_copy_ok;		/* set_scopy_flag() でセットされる */
    fits_table *shallow_copy_dest_obj;	/* コピー先: 有効なら non-NULL */

    /* コピーの dest 側のフラグ(コピー元から書き込まれる) */
    fits_table *shallow_copy_src_obj;	/* コピー元: 生きていれば non-NULL */

    /* __shallow_init(), __deep_init() を安全に行なうためのフラグ */
    bool __copying;

  };

/*
 * inline member functions
 */

inline long fits_table::col_length() const
{
    return this->col_size_rec;
}

inline long fits_table::col_size() const
{
    return this->col_size_rec;
}

inline long fits_table::row_length() const
{
    return this->row_size_rec;
}

inline long fits_table::row_size() const
{
    return this->row_size_rec;
}

inline void *fits_table::heap_ptr()
{
    return this->heap_ptr_rec;
}

#ifdef SLI__OVERLOAD_CONST_AT
inline const void *fits_table::heap_ptr() const
{
    return this->heap_ptr_rec;
}
#endif

inline const void *fits_table::heap_ptr_cs() const
{
    return this->heap_ptr_rec;
}

inline size_t fits_table::heap_length()
{
    return this->heap_rec.length();
}

inline const mdarray &fits_table::heap_array_cs() const
{
    return this->heap_rec;
}


/**
 * @example  examples_sfitsio/create_fits_bintable.cc
 *           バイナリテーブルを含む新規 FITS ファイルを作る例
 */

/**
 * @example  examples_sfitsio/create_fits_asciitable.cc
 *           ASCIIテーブルを含む新規 FITS ファイルを作る例
 */

/**
 * @example  examples_sfitsio/dump_fits_table.cc
 *           バイナリテーブルまたはASCIIテーブルをテキストで出力する．
 *           可変長配列には対応していない．
 */

/**
 * @example  examples_sfitsio/create_fits_vl_array.cc
 *           可変長配列を含むバイナリテーブル FITS を作る例．
 *           低レベル API を使っている．
 */


}

#endif	/* _SLI__FITS_TABLE_H */

