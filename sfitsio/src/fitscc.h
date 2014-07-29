/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2014-07-30 04:27:08 cyamauch> */

/* SFITSIO version 1.4.4 [STABLE] */

#ifndef _SLI__FITSCC_H
#define _SLI__FITSCC_H 1

/**
 * @file   fitscc.h
 * @brief  1つの FITS ファイル全体を表現するクラス fitscc の定義
 */

#include "fits_image.h"
#include "fits_table.h"

#ifdef BUILD_SFITSIO
#include <sli/ctindex.h>
#include <sli/mdarray.h>
#else
#include "ctindex.h"
#include "mdarray.h"
#endif

namespace sli
{

/*
 * sil::fitscc class expresses a FITS file, and manages sli::fits_image objects
 * and sli::fits_table objects that express image HDUs and table HDUs.
 *
 * When accessing data in a FITS file, all data in the FITS file are loaded on
 * allocated memory managed by the fitscc object and objects managed by it.  
 * Of course, the memory management is automatic, so programmers do not have to
 * use malloc(), free(), etc.  Programmers only write the code to access FITS 
 * data using provided easy-to-use APIs.
 * 
 * In SFITSIO, APIs also express the FITS structure, therefore, it is important
 * for programmers to know it and structure in fitscc object.
 *
 * Following figure shows how does fitscc class express the FITS structure.
 * In this figure, a FITS file to be accessed has Primary HDU and a binary
 * table HDU.
 *
 * +----------------------------- fitscc object ------------------------------+
 * |                                                                          |
 * | +------------------------- fits_image object --------------------------+ |
 * | | HDU index: 0      (inherited from fits_hdu class)                    | |
 * | |                                                                      | |
 * | | +------- fits_header object ------+ +------- mdarray object -------+ | |
 * | | | +- fits_header_record object -+ | |                              | | |
 * | | | +-----------------------------+ | |                              | | |
 * | | | +- fits_header_record object -+ | |                              | | |
 * | | | +-----------------------------+ | |          image data          | | |
 * | | | +- fits_header_record object -+ | |                              | | |
 * | | | +-----------------------------+ | |                              | | |
 * | | | +                             + | |                              | | |
 * | | | +                             + | |                              | | |
 * | | +---------------------------------+ +------------------------------+ | |
 * | +----------------------------------------------------------------------+ |
 * |                                                                          |
 * | +------------------------- fits_table object --------------------------+ |
 * | | HDU index: 1      (inherited from fits_hdu class)                    | |
 * | |                                                                      | |
 * | | +------- fits_header object ------+   - - - main table data - - - -  | |
 * | | | +- fits_header_record object -+ | | +---+---+---+--- - - - - - + | | |
 * | | | +-----------------------------+ |   |   |   |   |              |   | |
 * | | | +- fits_header_record object -+ | | |(*)|(*)|(*)|(*)           | | | |
 * | | | +-----------------------------+ |   |   |   |   |              |   | |
 * | | | +- fits_header_record object -+ | | |   |   |   |              | | | |
 * | | | +-----------------------------+ |   |   |   |   |              |   | |
 * | | | +                             + | | |   |   |   |              | | | |
 * | | | +                             + |   |   |   |   |              |   | |
 * | | | +                             + | | +---+---+---+--- - - - - - + | | |
 * | | | +                             + |   - - - - - - - - - - - - - - -  | |
 * | | | +                             + | +------- mdarray object -------+ | |
 * | | | +                             + | |                              | | |
 * | | | +                             + | |        heap area (#)         | | |
 * | | | +                             + | |                              | | |
 * | | +---------------------------------+ +------------------------------+ | |
 * | +----------------------------------------------------------------------+ |
 * |                                     (*): fits_table_col object           |
 *                                       (#): no guarantee of byte alignment,
 *                                            and always big-endian
 *
 * How to get the reference to each object is essential to use SFITSIO APIs.
 * Followings show important examples:
 *  fits.hdu(..)                 returns reference of fits_hdu object
 *  fits.image(..)               returns reference of fits_image object
 *  fits.table(..)               returns reference of fits_table object
 *  fits.table(..).col(..)       returns reference of fits_table_col object
 *  fits.hdu(..).header()        returns reference of fits_header object
 *  fits.hdu(..).header().at(..) returns reference of fits_header_record object
 *  fits.hdu(..).header(..)      : same as fits.hdu(..).header().at(..)
 * where `fits' is the instance of fitscc class, and fits.hdu(..).header can
 * be replaced with fits.image(..).header or fits.table(..).header, since 
 * fits_image class and fits_table class are inherited from fits_hdu class.
 *
 * Note that APIs of SFITSIO always take 0-indexed number.
 */

  /**
   * @class  sli::fitscc
   * @brief  1つの FITS ファイル全体を表現するクラス
   * @author Chisato YAMAUCHI
   * @date   2013-03-26 00:00:00
   */

  class fitscc

  {
    friend class fits_hdu;

  public:
    /* constructor & destructor */
    fitscc();
    fitscc(const fitscc &obj);
    virtual ~fitscc();

    /* complete initialization */
    virtual fitscc &operator=(const fitscc &obj);
    virtual fitscc &init();
    virtual fitscc &init( const fitscc &obj );

    /*
     * I/O of stream
     */

    /* read a FITS file from stream (only for power users) */
    virtual ssize_t read_stream( cstreamio &sref, size_t max_bytes_read );
    virtual ssize_t read_stream( cstreamio &sref );

    /* write a FITS file to stream (only for power users) */
    virtual ssize_t write_stream( cstreamio &sref );

    /* read a FITS file from stream                                          */
    /* These member functions accepts URL (http:// or ftp://) and compressed */
    /* files. (.gz or .bzip2)                                                */
    /* `path' arg can take string like "ftp://username:password@ftp.com/".   */
    virtual ssize_t read_stream( const char *path );
    virtual ssize_t vreadf_stream( const char *path_fmt, va_list ap );
    virtual ssize_t readf_stream( const char *path_fmt, ... );

    /* write a FITS file to stream                                           */
    /* These member functions accepts URL (http:// or ftp://) and compressed */
    /* files. (.gz or .bzip2)                                                */
    /* `path' arg can take string like "ftp://username:password@ftp.com/".   */
    virtual ssize_t write_stream( const char *path );
    virtual ssize_t vwritef_stream( const char *path_fmt, va_list ap );
    virtual ssize_t writef_stream( const char *path_fmt, ... );

    /* read or write a FITS file                                     */
    /* These member functions can accept Perl-like argument to setup */
    /* input/output stream.                                          */
    /*   Examples: (read and write, respectively)                    */
    /*     fits.access_stream("cat hoge.fits.bz2 | lbzip2 -d |");    */
    /*     fits.access_stream("| lbzip2 > hoge.fits.bz2");           */
    virtual ssize_t access_stream( const char *path );
    virtual ssize_t vaccessf_stream( const char *path_fmt, va_list ap );
    virtual ssize_t accessf_stream( const char *path_fmt, ... );

    /* read a SFITSIO template and create new FITS contents */
    /* having blank data in self.                           */
    /* See official manual for details of SFITSIO template. */
    virtual ssize_t read_template( int flags, const char *path );
    virtual ssize_t read_template( const char *path );
    virtual ssize_t vreadf_template( const char *path_fmt, va_list ap );
    virtual ssize_t readf_template( const char *path_fmt, ... );

    /* returns class level                                    */
    /* Class level is used when inherited classes are defined */
    virtual int classlevel() const;

    /* This returns byte length of FITS file that will be written */
    virtual ssize_t stream_length();
    /* not recommended */
    virtual ssize_t stream_size();		/* same as stream_length() */

    /* returns length of HDUs */
    virtual long length() const;
    /* not recommended */
    virtual long size() const;			/* same as length() */

    /*
     * member functions to manipulate image or table HDUs.
     *
     * Note that integer argument `index' of each member function takes
     * a 0-indexed serial number through all HDUs.
     */

    /* NOT recommended : do not use! */
    virtual fitscc &create();
    virtual fitscc &create( const char *fmttype, long long ftypever );
    virtual fitscc &create_image( int type, long naxisx[], long ndim, 
				  bool init_buf );
    virtual fitscc &create_image( const char *fmttype, long long ftypever,
			   int type, long naxisx[], long ndim, bool init_buf );
    virtual fitscc &create_image( int type,
			       long naxis0, long naxis1 = 0, long naxis2 = 0 );
    virtual fitscc &create_image( const char *fmttype, long long ftypever,
		     int type, long naxis0, long naxis1 = 0, long naxis2 = 0 );
    virtual fitscc &create_image( const fits_image &src );
    virtual fitscc &create_image( const char *fmttype, long long ftypever,
				  const fits_image &src );

    /*
     * append an image HDU 
     */
    virtual fitscc &append_image( const char *extname, long long extver );
    virtual fitscc &append_image( const char *extname, long long extver,
			   int type, long naxisx[], long ndim, bool init_buf );
    virtual fitscc &append_image( const char *extname, long long extver,
		 int type, long naxis0 = 0, long naxis1 = 0, long naxis2 = 0 );
    virtual fitscc &append_image( const char *extname, long long extver,
				  const fits_image &src );
    virtual fitscc &append_image( const fits_image &src );

    /*
     * append a binary table HDU or an ASCII table HDU 
     */
    virtual fitscc &append_table( const char *extname, long long extver,
				  bool ascii = false );
    virtual fitscc &append_table( const char *extname, long long extver,
			    const fits::table_def defs[], bool ascii = false );
    virtual fitscc &append_table( const char *extname, long long extver,
			const fits::table_def_all defs[], bool ascii = false );
    virtual fitscc &append_table( const char *extname, long long extver,
				  const fits_table &src );
    virtual fitscc &append_table( const fits_table &src );

    /*
     * insert an image HDU
     */
    virtual fitscc &insert_image( long index0, 
				  const char *extname, long long extver );
    virtual fitscc &insert_image( const char *extname0,
				  const char *extname, long long extver );
    virtual fitscc &insert_image( long index0,
			   const char *extname, long long extver,
			   int type, long naxisx[], long ndim, bool init_buf );
    virtual fitscc &insert_image( long index0,
		 const char *extname, long long extver,
		 int type, long naxis0 = 0, long naxis1 = 0, long naxis2 = 0 );
    virtual fitscc &insert_image( const char *extname0,
			   const char *extname, long long extver,
			   int type, long naxisx[], long ndim, bool init_buf );
    virtual fitscc &insert_image( const char *extname0,
		 const char *extname, long long extver,
		 int type, long naxis0 = 0, long naxis1 = 0, long naxis2 = 0 );
    virtual fitscc &insert_image( long index0,
				  const char *extname, long long extver, 
				  const fits_image &src );
    virtual fitscc &insert_image( const char *extname0,
				  const char *extname, long long extver,
				  const fits_image &src );

    virtual fitscc &insert_image( long index0, const fits_image &src );
    virtual fitscc &insert_image(const char *extname0, const fits_image &src);

    /*
     * insert a binary table HDU or an ASCII table HDU 
     */
    virtual fitscc &insert_table( long index0, 
                     const char *extname, long long extver, bool ascii=false );
    virtual fitscc &insert_table( const char *extname0, 
		     const char *extname, long long extver, bool ascii=false );
    virtual fitscc &insert_table( long index0,
				  const char *extname, long long extver,
				  const fits::table_def defs[],
				  bool ascii = false );
    virtual fitscc &insert_table( long index0,
				  const char *extname, long long extver,
				  const fits::table_def_all defs[],
				  bool ascii = false );
    virtual fitscc &insert_table( const char *extname0,
				  const char *extname, long long extver,
				  const fits::table_def defs[],
				  bool ascii = false );
    virtual fitscc &insert_table( const char *extname0,
				  const char *extname, long long extver,
				  const fits::table_def_all defs[],
				  bool ascii = false );
    virtual fitscc &insert_table( long index0,
				  const char *extname, long long extver,
				  const fits_table &src );
    virtual fitscc &insert_table( const char *extname0,
				  const char *extname, long long extver,
				  const fits_table &src );
    virtual fitscc &insert_table( long index0, const fits_table &src );
    virtual fitscc &insert_table(const char *extname0, const fits_table &src);

    /* erase an HDU */
    virtual fitscc &erase( long index0 );
    virtual fitscc &erase( const char *extname0 );

    /* obtain number of HDU from an HDU name */
    virtual long index( const char *extname ) const;
    virtual long indexf( const char *fmt, ... ) const;
    virtual long vindexf( const char *fmt, va_list ap ) const;

    /* .hdu() returns reference of an object of fits_hdu class. */
    /* This can be used to access FITS header of any HDU types. */
    virtual fits_hdu &hdu( long index );
    virtual fits_hdu &hdu( const char *extname );
    virtual fits_hdu &hduf( const char *fmt, ... );
    virtual fits_hdu &vhduf( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_hdu &hdu( long index ) const;
    virtual const fits_hdu &hdu( const char *extname ) const;
    virtual const fits_hdu &hduf( const char *fmt, ... ) const;
    virtual const fits_hdu &vhduf( const char *fmt, va_list ap ) const;
#endif
    virtual const fits_hdu &hdu_cs( long index ) const;
    virtual const fits_hdu &hdu_cs( const char *extname ) const;
    virtual const fits_hdu &hduf_cs( const char *fmt, ... ) const;
    virtual const fits_hdu &vhduf_cs( const char *fmt, va_list ap ) const;

    /* .image() returns reference of an object of fits_image class */
    /* Programmers can access FITS image via this reference.       */
    virtual fits_image &image( long index );
    virtual fits_image &image( const char *extname );
    virtual fits_image &imagef( const char *fmt, ... );
    virtual fits_image &vimagef( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_image &image( long index ) const;
    virtual const fits_image &image( const char *extname ) const;
    virtual const fits_image &imagef( const char *fmt, ... ) const;
    virtual const fits_image &vimagef( const char *fmt, va_list ap ) const;
#endif
    virtual const fits_image &image_cs( long index ) const;
    virtual const fits_image &image_cs( const char *extname ) const;
    virtual const fits_image &imagef_cs( const char *fmt, ... ) const;
    virtual const fits_image &vimagef_cs( const char *fmt, va_list ap ) const;

    /* .table() returns reference of an object of fits_table class */
    /* Programmers can access FITS binary table or ASCII table via */
    /* this reference.                                             */
    virtual fits_table &table( long index );
    virtual fits_table &table( const char *extname );
    virtual fits_table &tablef( const char *fmt, ... );
    virtual fits_table &vtablef( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const fits_table &table( long index ) const;
    virtual const fits_table &table( const char *extname ) const;
    virtual const fits_table &tablef( const char *fmt, ... ) const;
    virtual const fits_table &vtablef( const char *fmt, va_list ap ) const;
#endif
    virtual const fits_table &table_cs( long index ) const;
    virtual const fits_table &table_cs( const char *extname ) const;
    virtual const fits_table &tablef_cs( const char *fmt, ... ) const;
    virtual const fits_table &vtablef_cs( const char *fmt, va_list ap ) const;

    /*
     * get or set basic HDU information
     */

    /* returns HDU type;                                                */
    /* FITS::IMAGE_HDU, FITS::BINARY_TABLE_HDU or FITS::ASCII_TABLE_HDU */
    virtual int hdutype( long index ) const;
    virtual int hdutype( const char *hduname ) const;
    /* same as hdutype() */
    virtual int exttype( long index ) const;
    virtual int exttype( const char *extname ) const;

    /* returns HDU name (value of EXTNAME in FITS header) */
    virtual const char *hduname( long index ) const;
    /* same as hduname() */
    virtual const char *extname( long index ) const;

    /* returns HDU version (value of EXTVER in FITS header) */
    virtual long long hduver( long index ) const;
    virtual long long hduver( const char *hduname ) const;
    /* same as hduver() */
    virtual long long extver( long index ) const;
    virtual long long extver( const char *extname ) const;

    /* returns HDU level (value of EXTLEVEL in FITS header) */
    virtual long long hdulevel( long index ) const;
    virtual long long hdulevel( const char *hduname ) const;
    /* same as hduver() */
    virtual long long extlevel( long index ) const;
    virtual long long extlevel( const char *extname ) const;

    /* returns true if HDU version is set */
    virtual bool hduver_is_set( long index ) const;
    virtual bool hduver_is_set( const char *hduname ) const;
    /* same as hduver_is_set() */
    virtual bool extver_is_set( long index ) const;
    virtual bool extver_is_set( const char *extname ) const;

    /* returns true if HDU level is set */
    virtual bool hdulevel_is_set( long index ) const;
    virtual bool hdulevel_is_set( const char *hduname ) const;
    /* same as hdulevel_is_set() */
    virtual bool extlevel_is_set( long index ) const;
    virtual bool extlevel_is_set( const char *extname ) const;

    /* set HDU name */
    virtual fitscc &assign_hduname( long index, const char *hduname );
    /* same as assign_hduname() */
    virtual fitscc &assign_extname( long index, const char *extname );

    /* set HDU version */
    virtual fitscc &assign_hduver( long index, long long hduver );
    /* same as assign_hduver() */
    virtual fitscc &assign_extver( long index, long long extver );

    /* set HDU level */
    virtual fitscc &assign_hdulevel( long index, long long hduver );
    /* same as assign_hdulevel() */
    virtual fitscc &assign_extlevel( long index, long long extver );

    /* returns value of FMTTYPE in FITS header */
    virtual const char *fmttype() const;

    /* returns numeric value of FTYPEVER in FITS header */
    virtual long long ftypever() const;

    /* returns string value of FTYPEVER in FITS header */
    virtual const char *ftypever_value() const;

    /* set value of FMTTYPE, FTYPEVER in FITS header */
    virtual fitscc &assign_fmttype( const char *fmttype, long long ftypever );
    virtual fitscc &assign_fmttype( const char *fmttype );
    virtual fitscc &assign_ftypever( long long ftypever );

  protected:
    virtual fitscc &_init();
    /* 継承クラスのコンストラクタで呼ぶ */
    virtual int increase_classlevel();
    /* */
    virtual ssize_t fits_load( cstreamio &sref, const size_t *max_bytes_ptr );
    virtual int template_load_recursively( int flags, const char *path,
					   const char *base_dir );
    virtual int template_load( int flags, const char *templ );
    virtual int interpolate_templhdr_and_append_hdu( fits_header *templhdr );
    virtual fitscc &append( int hdutype );
    virtual fitscc &insert( long index0, int hdutype );
    virtual fitscc &insert( const char *extname0, int hdutype );
    virtual void **table_data_ptr_mng_ptr( long hdu_index, long col_index );

  protected:
    mdarray hdus_rec;
    fits_hdu **hdus_ptr_rec;
    mdarray hdu_is_stack_rec;

  private:
    /* これは，コンストラクタ内だけで更新される */
    int classlevel_rec;
    /* HDUの個数，HDU名の辞書 */
    long num_hdus_rec;
    ctindex index_rec;
    /* FMTTYPE, FTYPEVER */
    tstring fmttype_rec;
    tstring ftypever_rec;
    /* read_stream() での HDU と画像・テーブルの部分読みの指定 */
    tarray_tstring hdus_to_read_rec;
    tarray_tstring section_to_read_rec;
    /* 以下は template 再帰読み込み用に一時的に使うのみ */
    tstring template_buffer;
    size_t template_buffer_idx;

  };


/**
 * @example  examples_sfitsio/read_and_write_fits.cc
 *           FITS ファイルの読み書きを行なう最も基本的なコード
 */

/**
 * @example  examples_sfitsio/read_fits_header.cc
 *           FITS ファイルのヘッダを読み取るコード．<br>
 *           速度を求める場合は， tools/hv.cc を参照．
 */

/**
 * @example  tools_sfitsio/create_fits_from_template.cc
 *           FITS テンプレートから新規 FITS ファイルを作成するコード
 */

}

#endif	/* _SLI__FITSCC_H */
