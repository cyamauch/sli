/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2014-05-08 03:08:20 cyamauch> */

#ifndef _SLI__FITS_HEADER_H
#define _SLI__FITS_HEADER_H 1

/**
 * @file   fits_header.h
 * @brief  FITS �إå����Τ�ɽ�����륯�饹 fits_header �����
 */

#include "fits_header_record.h"

#ifdef BUILD_SFITSIO
#include <sli/ctindex.h>
#include <sli/mdarray.h>
#else
#include "ctindex.h"
#include "mdarray.h"
#endif

namespace sli
{

  class fits_hdu;

  /*
   * sli::fits_header class manages an array of fits::fits_header_records
   * objects, and provide a number of member functions to manipulate FITS 
   * header records.  Objects of this class are usually managed by an object  
   * of sli::fits_hdu class.
   *
   * This class is usually used via sli::fits_hdu class, but can be used as a
   * stand-alone.  For example, creating a sli::fits_header object and using 
   * .read_stream() of it will helpful to access FITS headers only with 
   * high-speed file I/O.
   */

  /**
   * @class  sli::fits_header
   * @brief  FITS �إå����Τ�ɽ�����륯�饹
   *
   *  fits_header ���饹�ϡ�FITS �إå����Τ�ɽ�����ޤ���fits_header_record
   *  ��ɽ�������إå��쥳���ɤ�������������ݻ����ޤ���
   *
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fits_header

  {
    friend class fits_hdu;

  public:
    /* constructor & destructor */
    fits_header();
    fits_header(const fits_header &obj);
    virtual ~fits_header();

    /* same as .init(obj) */
    virtual fits_header &operator=(const fits_header &obj);

    /* complete initialization of objects */
    virtual fits_header &init();
    virtual fits_header &init( const fits_header &obj );
    virtual fits_header &init( const fits::header_def defs[] );

    /* swap contents between this object and obj */
    virtual fits_header &swap( fits_header &obj );

    /*
     * stream and buffer I/O
     */

    /* read a header unit of FITS from stream or buffer.           */
    /* Argument `max_bytes_read' is not used for general purposes. */
    virtual ssize_t read_stream( cstreamio &sref, size_t max_bytes_read );
    virtual ssize_t read_stream( cstreamio &sref );
    virtual ssize_t read_buffer( const char *buffer );

    /* skip a data block.                                 */
    /* Argument `bytes' is not used for general purposes. */
    virtual ssize_t skip_data_stream( cstreamio &sref, size_t max_bytes_skip );
    virtual ssize_t skip_data_stream( cstreamio &sref );

    /* write a header unit to stream.                                */
    /* If end_and_blank arg is true, END keyword and blank chars for */
    /* 2880-bytes blocking are added.                                */
    virtual ssize_t write_stream( cstreamio &sref, bool end_and_blank=false );

    /* discard original 80-char record, and reformat all records */
    virtual fits_header &reformat();

    /* This returns formatted 80 * N bytes string.      */
    /* Returned result is a string of 80 * n characters */
    /* without '\n' but with '\0' termination.          */
    virtual const char *formatted_string();

    /* This returns length of formatted 80 * N bytes string. */
    /* (actually internal data string is not generated)      */
    virtual size_t formatted_length();

    /*
     * manipulate some records
     */

    /* update some header records                                        */
    /* If specified keyword is not found, new record is created.         */
    /* Note that this member function cannot handle description records. */
    virtual fits_header &update_records( const fits_header &obj );

    /* append some header records */
    virtual fits_header &append_records( const fits::header_def defs[] );
    virtual fits_header &append_records( const fits::header_def defs[],
					 long num_defs, bool warn );
    virtual fits_header &append_records( const fits_header &obj, bool warn );

    /* insert some header records */
    virtual fits_header &insert_records( long index0,
					 const fits::header_def defs[] );
    virtual fits_header &insert_records( const char *keyword0,
					 const fits::header_def defs[] );
    virtual fits_header &insert_records( long index0,
					 const fits::header_def defs[],
					 long num_defs, bool warn );
    virtual fits_header &insert_records( const char *keyword0,
					 const fits::header_def defs[],
					 long num_defs, bool warn );
    virtual fits_header &insert_records( long index0,
					 const fits_header &obj, bool warn );
    virtual fits_header &insert_records( const char *keyword0,
					 const fits_header &obj, bool warn );

    /* erase some header records */
    virtual fits_header &erase_records( long index0, long num_records );
    virtual fits_header &erase_records(const char *keyword0, long num_records);

    //virtual fits_header &erase_all_sysrecords();

    /*
     * manipulate a record
     */

    /* update a record.                                          */
    /* If specified keyword is not found, new record is created. */
    /* Note that update() cannot handle description records.     */
    virtual fits_header &update( const char *keyword, const char *value, 
				 const char *comment );
    virtual fits_header &update( const fits_header_record &obj );

    /* append a record */
    virtual fits_header &append( const char *keyword );
    virtual fits_header &append( const char *keyword, const char *value, 
				 const char *comment );
    virtual fits_header &append(const char *keyword, const char *description);
    virtual fits_header &append( const fits::header_def &def );
    virtual fits_header &append( const fits_header_record &obj );

    /* insert a record */
    virtual fits_header &insert( long index0, const char *keyword );
    virtual fits_header &insert( const char *keyword0, const char *keyword );
    virtual fits_header &insert( long index0,
				 const char *k, const char *v, const char *c );
    virtual fits_header &insert( const char *keyword0,
				 const char *k, const char *v, const char *c );
    virtual fits_header &insert( long index0, 
				 const char *keywd, const char *description );
    virtual fits_header &insert( const char *keyword0, 
				 const char *keywd, const char *description );
    virtual fits_header &insert( long index0, const fits::header_def &def );
    virtual fits_header &insert( const char *keyword0, 
				 const fits::header_def &def );
    virtual fits_header &insert( long index0, const fits_header_record &obj );
    virtual fits_header &insert( const char *keyword0, 
				 const fits_header_record &obj );

    /* rename keyword of a record */
    virtual fits_header &rename( long index0, const char *new_name );
    virtual fits_header &rename( const char *keyword0, const char *new_name );

    /* erase a record */
    virtual fits_header &erase( long index0 );
    virtual fits_header &erase( const char *keyword0 );

    /* comment ��񤯤ȡ�description �ǤϤʤ��ʤ� */

    /*
     * low-level member functions
     */

    /* set keyword, raw value, and comment of a record */
    virtual fits_header &assign( long index0, const fits::header_def &def );
    virtual fits_header &assign( const char *keyword0, const fits::header_def &def );
    virtual fits_header &assign( long index0, const fits_header_record &obj );
    virtual fits_header &assign( const char *keyword0,
				 const fits_header_record &obj );
    virtual fits_header &assign( long index0, 
				 const char *keyword, const char *value, 
				 const char *comment );
    virtual fits_header &assign( const char *keyword0, 
				 const char *keyword, const char *value, 
				 const char *comment );
    virtual fits_header &assign( long index0, 
				const char *keyword, const char *description );
    virtual fits_header &assign( const char *keyword0, 
				const char *keyword, const char *description );

    /* set a raw value of a record */
    virtual fits_header &vassignf_value( long index0, const char *format, va_list ap );
    virtual fits_header &vassignf_value( const char *keyword0, 
					 const char *format, va_list ap );
    virtual fits_header &assignf_value( long index0, const char *format, ... );
    virtual fits_header &assignf_value( const char *keyword0, const char *format, ... );

    /* set a comment string of a record */
    virtual fits_header &vassignf_comment( long index0, const char *format, va_list ap);
    virtual fits_header &vassignf_comment( const char *keyword0, 
					   const char *format, va_list ap );
    virtual fits_header &assignf_comment( long index0, const char *format, ... );
    virtual fits_header &assignf_comment( const char *keyword0, 
					  const char *format, ... );

    /* This overwrites existing all comments with comment string in comment */
    /* dictionary.                                                          */
    virtual fits_header &assign_default_comments(int hdutype = FITS::ANY_HDU);

    /* This fills only blank comments with comment string in comment */
    /* dictionary.                                                   */
    virtual fits_header &fill_blank_comments(int hdutype = FITS::ANY_HDU);

    /* record() and at() return the reference to each fits_header_record */
    /* object.                                                           */
    virtual fits_header_record &record( long index0 );
    virtual fits_header_record &record( const char *keyword0 );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_header_record &record( long index0 ) const;
    virtual const fits_header_record &record( const char *keyword0 ) const;
#endif
    virtual const fits_header_record &record_cs( long index0 ) const;
    virtual const fits_header_record &record_cs( const char *keyword0 ) const;
    /* same as record() */
    virtual fits_header_record &at( long index0 );
    virtual fits_header_record &at( const char *keyword0 );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_header_record &at( long index0 ) const;
    virtual const fits_header_record &at( const char *keyword0 ) const;
#endif
    virtual const fits_header_record &at_cs( long index0 ) const;
    virtual const fits_header_record &at_cs( const char *keyword0 ) const;

    /*
     * member functions to read and search information of header records
     */

    /* index() or regmatch() searches a keyword from records which format    */
    /* is not descriptive (unlike COMMENT or HISTORY) and returns its record */
    /* index.  Negative value is returned when specified keyword or pattern  */
    /* is not found.  Set true to is_description arg to search a keyword     */
    /* from descriptive records.                                             */
    /*   keyword0: keyword name                                              */
    /*   keypat:   keyword pattern (POSIX extended regular expression)       */
    virtual long index( const char *keyword0 ) const;
    virtual long index( const char *keyword0, bool is_description ) const;
    virtual long regmatch( const char *keypat, 
			   ssize_t *rpos = NULL, size_t *rlen = NULL ) const;
    virtual long regmatch( long index0, const char *keypat, 
			   ssize_t *rpos = NULL, size_t *rlen = NULL ) const;

    /* returns length of raw value. More than 0 means NON-NULL. */
    /* Negative value is returned when a record is not found.   */
    virtual long value_length( const char *keyword ) const;
    virtual long value_length( long index ) const;

    /* returns number of records */
    virtual long length() const;
    /* not recommended */
    virtual long size() const;				/* same as length() */

    /* returns type of HDU based on header information */
    virtual int hdutype() const;

    /* returns true if the record of index has a system-reserved keyword */
    virtual bool is_sysrecord( long index ) const;

    /* get checksum of header unit only */
    virtual unsigned long checksum( unsigned long sum_in = 0 );
    virtual const char *encoded_checksum( unsigned long sum_in = 0 );

    /* Set true to supress warning messages of stdout when duplicated */
    /* keyword is found.                                              */
    /* ��ʣ������ɤ��������줿���ηٹ�� on / off                  */
    virtual bool &suppress_dupkey_warning();

    /* CONTINUE �쥳���ɤ�Ÿ��(fitscc�����Ȥ���) */
    virtual fits_header &expand_continue_records();

    /* shallow copy ����Ĥ�����˻��� (̤����)  */
    /* (������֥������Ȥ� return ��ľ���ǻ���)    */
    virtual void set_scopy_flag();

  protected:
    /* setup_sys_header() ��ƤФʤ��� */
    virtual fits_header &_init();
    virtual bool _erase_records( long index0, long num_records );
    /* */
    virtual fits_header_record *get_ptr( long index0 );
    virtual fits_header_record *get_ptr( const char *keyword0 );
    virtual fits_header &set_protections( long index0, bool keyword, bool value_type, 
					  bool value, bool comment );
    /* fits_hdu ����Ȥ� */
    virtual fits_header &register_manager( fits_hdu *ptr );
    virtual fits_header &set_sysrecords_prohibition( bool value );

  private:
    fits_header &append_header_records( const fits::header_def defs[],
					const char *formatted_str[],
					long num_defs, bool warn );
    fits_header &insert_header_records( long index0, 
					const fits::header_def defs[], 
					const char *formatted_str[],
					long num_defs, bool warn );
    ssize_t data_skip( cstreamio &sref, const size_t *max_bytes_ptr );
    ssize_t header_load( cstreamio &sref, const size_t *max_bytes_ptr );
    ssize_t header_load( const char *header_all );
    ssize_t header_save(cstreamio *sptr, void *c_sum_info, bool end_and_blank);

    /* shallow copy �ط� */
    bool request_shallow_copy( fits_header *from_obj ) const;
    void cancel_shallow_copy( fits_header *from_obj ) const;
    void cleanup_shallow_copy( bool do_deep_copy_for_this ) const;

    /* */
    long num_records_rec;
    mdarray records_rec;
    fits_header_record **records_ptr_rec;	/* records_rec �ΥХåե�ptr */
    ctindex index_rec;		      /* �ĥ꡼���� */
    tstring formatted_rec;	      /* �� 80 char �ǥե����ޥåȤ��줿str */
    char encoded_chksum_rec[32];
    /* �����ƥ�إå����ɲá��ͤ��ѹ���ػߤ������ true */
    bool sysrecords_prohibition_rec;
    /* duplicated keyword �ηٹ��Ф��ʤ����� true �򥻥å� */
    bool suppress_dupkey_warning_rec;
    /* fits_hdu �ˤ�äƴ�������Ƥ���Ф��� ptr */
    fits_hdu *manager;

    /* shallow copy �Τ���Υե饰�ࡥ= ���Ǥϥ��ԡ�����ʤ� */
    /* ���ԡ��� src ¦�Υե饰 */
    bool shallow_copy_ok;		/* set_scopy_flag() �ǥ��åȤ���� */
    fits_header *shallow_copy_dest_obj;       /* ���ԡ���: ͭ���ʤ� non-NULL */

    /* ���ԡ��� dest ¦�Υե饰(���ԡ�������񤭹��ޤ��) */
    fits_header *shallow_copy_src_obj;    /* ���ԡ���: �����Ƥ���� non-NULL */

    /* __shallow_init(), __deep_init() ������˹Ԥʤ�����Υե饰 */
    bool __copying;

  };


/**
 * @example  tools_sfitsio/fill_fits_header_comments.cc
 *           �֥�󥯥����Ȥ� SFITSIO �μ��������ġ���
 */

/**
 * @example  tools_sfitsio/hv.cc
 *           ��®��ư��� FITS �إå��α����ġ��롥<br>
 *           �ǥ������١����� FITS I/O ��Ԥʤ��������λ��ͤˡ�
 */

/**
 * @example  tools_sfitsio/fits_dataunit_md5.cc
 *           FITS �ե������ dataunit �� md5 �����ġ��롥<br>
 *           �ǥ������١����� FITS I/O ��Ԥʤ��������λ��ͤˡ�
 */


}

#endif	/* _SLI__FITS_HEADER_H */

