/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-29 12:03:40 cyamauch> */

#ifndef _SLI__FITS_HDU_H
#define _SLI__FITS_HDU_H 1

/**
 * @file   fits_hdu.h
 * @brief  FITS の HDU を表現する基底クラス fits_hdu の定義
 */

#include "fits_header.h"

#ifdef BUILD_SFITSIO
#include <sli/cstreamio.h>
#include <sli/tstring.h>
#else
#include "cstreamio.h"
#include "tstring.h"
#endif

namespace sli
{

  class fitscc;

  /*
   * sil::fits_hdu class expresses an HDU of FITS data structure, and manages
   * objects of sli::fits_header.  An inherited class of this class (fits_image
   * or fits_table) is used for general purposes, but reference of object of
   * this class itself can be used for HDUs whose type are unknown.  Objects of
   * this class and inherited class are usually managed by an object of 
   * sli::fitscc class.
   */

  /**
   * @class  sli::fits_hdu
   * @brief  FITS の HDU を表現する基底クラス
   *
   *  fits_hdu クラスは，FITS の HDU を表現します．これを継承したクラスが
   *  具体的な HDU (つまり Image か Table か) を表現します．
   *
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fits_hdu

  {

    friend class fitscc;
    friend class fits_header;

  public:
    /* constructor & destructor */
    fits_hdu();
    fits_hdu(const fits_hdu &obj);
    virtual ~fits_hdu();

    /* complete initialization of objects */
    virtual fits_hdu &init();

    /* returns type of HDU */
    virtual int hdutype() const;

    /* returns class level */
    virtual int classlevel() const;

    /* ヘッダまわり．ここからは header_rec の wrapper */

    /* complete initialization of header */
    virtual fits_hdu &header_init();
    virtual fits_hdu &header_init( const fits_header &obj );
    virtual fits_hdu &header_init( const fits::header_def defs[] );

    /* swap contents between fits_header object of self and obj */
    virtual fits_hdu &header_swap( fits_header &obj );

    /*
     * manipulate some header records
     */

    /* update some header records                                        */
    /* If specified keyword is not found, new record is created.         */
    /* Note that this member function cannot handle description records. */
    virtual fits_hdu &header_update_records( const fits_header &obj );

    /* append some header records */
    virtual fits_hdu &header_append_records( const fits::header_def defs[] );
    virtual fits_hdu &header_append_records( const fits_header &obj );

    /* insert some header records */
    virtual fits_hdu &header_insert_records( long index0,
					     const fits::header_def defs[] );
    virtual fits_hdu &header_insert_records( const char *keyword0,
					     const fits::header_def defs[] );
    virtual fits_hdu &header_insert_records( long index0,
					     const fits_header &obj );
    virtual fits_hdu &header_insert_records( const char *keyword0,
					     const fits_header &obj );

    /* erase some header records */
    virtual fits_hdu &header_erase_records( long index0, long num_records );
    virtual fits_hdu &header_erase_records( const char *keyword0, long num_records );

    /*
     * manipulate a header record
     */

    /* update a header record.                                      */
    /* If specified keyword is not found, new record is created.    */
    /* Note that .header_upate() cannot handle description records. */
    virtual fits_hdu &header_update( const char *keyword, const char *value, 
				     const char *comment );
    virtual fits_hdu &header_update( const fits_header_record &obj );

    /* append a header record */
    virtual fits_hdu &header_append( const char *keyword );
    virtual fits_hdu &header_append( const char *keyword, const char *value, 
				     const char *comment );
    virtual fits_hdu &header_append( const char *keyword, const char *description );
    virtual fits_hdu &header_append( const fits::header_def &def );
    virtual fits_hdu &header_append( const fits_header_record &obj );

    /* insert a header record */
    virtual fits_hdu &header_insert( long index0, const char *kwd );
    virtual fits_hdu &header_insert( const char *keyword0, const char *kwd );
    virtual fits_hdu &header_insert( long index0,
				 const char *k, const char *v, const char *c );
    virtual fits_hdu &header_insert( const char *keyword0,
				 const char *k, const char *v, const char *c );
    virtual fits_hdu &header_insert( long index0, 
				  const char *keywd, const char *description );
    virtual fits_hdu &header_insert( const char *keyword0, 
				  const char *keywd, const char *description );
    virtual fits_hdu &header_insert( long index0, 
				     const fits::header_def &def );
    virtual fits_hdu &header_insert( const char *keyword0, 
				     const fits::header_def &def );
    virtual fits_hdu &header_insert( long index0, 
				     const fits_header_record &obj );
    virtual fits_hdu &header_insert( const char *keyword0, 
				     const fits_header_record &obj );

    /* rename keyword of a header record */
    virtual fits_hdu &header_rename( long index0, const char *new_name );
    virtual fits_hdu &header_rename( const char *keyword0, const char *new_name );

    /* erase a header record */
    virtual fits_hdu &header_erase( long index0 );
    virtual fits_hdu &header_erase( const char *keyword0 );

    /*
     * low-level member functions for header
     */

    /* set keyword, raw value, and comment of a header record */
    virtual fits_hdu &header_assign(long index0, const fits::header_def &def);
    virtual fits_hdu &header_assign( const char *keyword0, 
				     const fits::header_def &def );
    virtual fits_hdu &header_assign( long index0,
				     const fits_header_record &obj );
    virtual fits_hdu &header_assign( const char *keyword0, 
				     const fits_header_record &obj );
    virtual fits_hdu &header_assign( long index0, 
				     const char *keyword, const char *value, 
				     const char *comment );
    virtual fits_hdu &header_assign( const char *keyword0, 
				     const char *keyword, const char *value, 
				     const char *comment );
    virtual fits_hdu &header_assign( long index0, 
				const char *keyword, const char *description );
    virtual fits_hdu &header_assign( const char *keyword0, 
				const char *keyword, const char *description );

    /* set a raw value of a header record */
    virtual fits_hdu &header_vassignf_value( long index0, 
					     const char *format, va_list ap );
    virtual fits_hdu &header_vassignf_value( const char *keyword0, 
					     const char *format, va_list ap );
    virtual fits_hdu &header_assignf_value( long index0, 
					    const char *format, ... );
    virtual fits_hdu &header_assignf_value( const char *keyword0, 
					    const char *format, ... );

    /* set a comment string of a header record */
    virtual fits_hdu &header_vassignf_comment( long index0, 
					      const char *format, va_list ap );
    virtual fits_hdu &header_vassignf_comment( const char *keyword0, 
					      const char *format, va_list ap );
    virtual fits_hdu &header_assignf_comment( long index0, 
					      const char *format, ... );
    virtual fits_hdu &header_assignf_comment( const char *keyword0, 
					      const char *format, ... );

    /* This overwrites existing all header comments with comment string in */
    /* comment dictionary.                                                 */
    virtual fits_hdu &header_assign_default_comments( int hdutype = FITS::ANY_HDU );

    /* This fills only blank header comments with comment string in comment */
    /* dictionary.                                                          */
    virtual fits_hdu &header_fill_blank_comments(int hdutype = FITS::ANY_HDU);

    /* These member functions returns reference of managed fits_header */
    /* object.                                                         */
    virtual fits_header &header();
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_header &header() const;
#endif
    virtual const fits_header &header_cs() const;

    /* These member functions returns reference of managed */
    /* fits_header_record object.                          */
    /* i.e., same as .header().at(index) ...               */
    virtual fits_header_record &header( long index0 );
    virtual fits_header_record &header( const char *keyword0 );
    virtual fits_header_record &headerf( const char *fmt, ... );
    virtual fits_header_record &vheaderf( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_header_record &header( long index0 ) const;
    virtual const fits_header_record &header( const char *keyword0 ) const;
    virtual const fits_header_record &headerf( const char *fmt, ... ) const;
    virtual const fits_header_record &vheaderf( const char *fmt, va_list ap ) const;
#endif
    virtual const fits_header_record &header_cs( long index0 ) const;
    virtual const fits_header_record &header_cs( const char *keyword0 ) const;
    virtual const fits_header_record &headerf_cs( const char *fmt, ... ) const;
    virtual const fits_header_record &vheaderf_cs( const char *fmt, va_list ap ) const;

    /*
     * member functions to read and search information of header records
     */

    /* header_index() or header_regmatch() searches a keyword from header  */
    /* records which format is not descriptive (unlike COMMENT or HISTORY) */
    /* and returns its record index.  Negative value is returned when      */
    /* specified keyword or pattern is not found.  Set true to             */
    /* is_description arg to search a keyword from descriptive records.    */
    /*   keyword0: keyword name                                            */
    /*   keypat:   keyword pattern (POSIX extended regular expression)     */
    virtual long header_index( const char *keyword0 ) const;
    virtual long header_index(const char *keyword0, bool is_description) const;
    virtual long header_regmatch( const char *keypat, 
			     ssize_t *rpos = NULL, size_t *rlen = NULL );
    virtual long header_regmatch( long index0, const char *keypat, 
			     ssize_t *rpos = NULL, size_t *rlen = NULL );

    /* returns length of raw value. More than 0 means NON-NULL. */
    /* Negative value is returned when a record is not found.   */
    virtual long header_value_length( const char *keyword ) const;
    virtual long header_value_length( long index ) const;

    /* returns number of header records */
    virtual long header_length() const;
    /* not recommended */
    virtual long header_size() const;		/* same as header_length() */

    /* discard original 80-char record, and reformat all records */
    virtual fits_hdu &header_reformat();

    /* This returns formatted 80 * N bytes string of all header records. */
    /* Returned result is a string of 80 * n characters without '\n' but */
    /* with '\0' termination.                                            */
    virtual const char *header_formatted_string();

    /* only for backward compatibility; do not use.                  */
    /* システムヘッダ読み取り(互換を保つためにあるだけ:使わないこと) */
    virtual long sysheader_length() const;
    virtual long sysheader_size() const;
    virtual long sysheader_index( const char *keyword0 ) const;
    virtual const char *sysheader_keyword( long index ) const;
    virtual const char *sysheader_value( long index ) const;
    virtual const char *sysheader_value( const char *keyword ) const;
    virtual const char *sysheader_comment( long index ) const;
    virtual const char *sysheader_comment( const char *keyword ) const;
    virtual const char *sysheader_formatted_string();

    /* change a HDU name */
    virtual fits_hdu &assign_hduname( const char *hduname );
    /* same as assign_hduname() */
    virtual fits_hdu &assign_extname( const char *extname );

    /* change a HDU version number */
    virtual fits_hdu &assign_hduver( long long hduver );
    /* same as assign_hduver() */
    virtual fits_hdu &assign_extver( long long extver );

    /* change a HDU level number */
    virtual fits_hdu &assign_hdulevel( long long hdulevel );
    /* same as assign_hdulevel() */
    virtual fits_hdu &assign_extlevel( long long extlevel );

    /* returns HDU name */
    virtual const char *hduname() const;
    virtual const char *extname() const;	/* same as hduname() */

    /* returns HDU version */
    virtual long long hduver() const;
    virtual long long extver() const;		/* same as hduver() */

    /* returns HDU version (string) */
    virtual const char *hduver_value() const;
    virtual const char *extver_value() const;	/* same as hduver_value() */

    /* returns HDU level */
    virtual long long hdulevel() const;
    virtual long long extlevel() const;		/* same as hdulevel() */

    /* returns HDU level (string) */
    virtual const char *hdulevel_value() const;
    virtual const char *extlevel_value() const;	/* same as hdulevel_value() */

    /* check HDU version and level are set or not */
    virtual bool hduver_is_set() const;
    virtual bool extver_is_set() const;
    virtual bool hdulevel_is_set() const;
    virtual bool extlevel_is_set() const;

    /* not implemented */
    virtual bool checksum_error() const;
    virtual bool datasum_error() const;

    /* returns type of HDU based on header information */
    virtual int hdutype_on_header();

    /* only for backward compatibility; do not use. */
    /* (互換を保つためにあるだけ:使わないこと)      */
    virtual const char *allheader_formatted_string();

  protected:
    /* 初期化など */
    virtual fits_hdu &operator=(const fits_hdu &obj);
    virtual fits_hdu &init( const fits_hdu &obj );
    virtual fits_hdu &swap( fits_hdu &obj );
    /* fits_header object からの入力 */
    virtual int read_header_object( const fits_header &header_all );
    /* ストリーム入出力 */
    virtual ssize_t read_stream( cstreamio &sref, size_t max_bytes_read );
    virtual ssize_t read_stream( cstreamio &sref );
    virtual ssize_t write_stream( cstreamio &sref );
    virtual ssize_t skip_data_stream( cstreamio &sref, size_t max_bytes_skip );
    virtual ssize_t skip_data_stream( cstreamio &sref );
    /* データ部保存のためのメンバ関数(継承クラスでオーバーライドする事) */
    /* オーバーライドされた関数は，fits_hdu::write_stream() から呼び出される */
    virtual ssize_t save_or_check_data( cstreamio *sptr, void *c_sum_info );
    /* 書き込まれるであろうバイト数 */
    virtual ssize_t stream_length();
    /* */
    virtual fits_hdu &register_manager( fitscc *ptr );
    virtual fits_hdu &_assign_extname( const char *extname );
    /* 継承クラスのコンストラクタで呼ぶ */
    virtual int increase_classlevel();
    /* 継承クラスのコンストラクタで呼ぶ */
    virtual fits_hdu &set_hdutype( int hdutype );
    /* 継承クラスから HDU 番号等を問い合わせる時に使う */
    virtual long hdu_index() const;
    //virtual const char *fmttype() const;
    //virtual long long ftypever() const;
    /* 保存する直前に呼ばれる */
    //virtual int set_primary_hdu( bool is_primary_hdu );
    /* setup_basic_sys_header() はファイルへ保存する直前に呼ばれる */
    /* extension 用 */
    virtual fits_hdu &setup_sys_header();
    /* 継承クラスでシステムキーワードを書き込む場合に使用 */
    virtual fits_hdu &header_set_sysrecords_prohibition( bool flag );
    /* ユーザの拡張クラスで使用を想定 */
    /* ポインタを張る場合に必要 */
    virtual fits_header_record *header_record_ptr( long index );
    /* ポインタを張った時，消えないように保護するために必要 */
    virtual fits_hdu &set_header_protections( long index0, 
					      bool keyword, bool value_type,
					      bool value, bool comment );

    /* shallow copy を許可する場合に使用．      */
    /* (一時オブジェクトの return の直前で使用) */
    virtual void set_scopy_flag();

  private:
    int header_setup_top_records( fits::header_def defs[],
				  const char *removed_keywords[] );
    /* 読み込みを行なう */
    ssize_t header_load( cstreamio &sref, const size_t *max_bytes_ptr );
    ssize_t header_load( const char *header_all );
    ssize_t header_load( const fits_header &header_all );
    /* 1つのヘッダをフォーマットする */
    ssize_t header_format_record( fits_header *target_header,
				  long index, tstring *result );

  protected:
    fits_header header_rec;
    bool checksum_error_rec;
    bool datasum_error_rec;

  private:
    /* これは，コンストラクタ内だけで更新される */
    int classlevel_rec;
    //bool is_primary_rec;	/* これは fits_hdu.cc 内で管理 */
    /* fits_hdu では初期化はするが，値の代入は継承クラスで行なう */
    int hdutype_rec;
    /* ヘッダすべて */
    tstring extname_rec;
    tstring extver_rec;
    bool extver_is_set_rec;
    tstring extlevel_rec;
    bool extlevel_is_set_rec;
    fitscc *manager;		/* fitcc によって管理されていればその ptr */

  };
}

#endif	/* _SLI__FITS_HDU_H */

