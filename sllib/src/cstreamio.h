/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-20 15:03:28 cyamauch> */

/* SLLIB version 1.4.5 [STABLE] */

#ifndef _SLI__CSTREAMIO_H
#define _SLI__CSTREAMIO_H 1

/**
 * @file   cstreamio.h
 * @brief  ���ȥ꡼�������ϤΤ������ݴ��쥯�饹 cstreamio �����
 */

#include <stddef.h>
#include <sys/types.h>
#include <stdarg.h>

#undef rewind
#undef open
#undef close
#undef read
#undef write

#include "sli_eof.h"
#include "sli_seek.h"

#include "slierr.h"

namespace sli
{

/*
 * sli::cstreamio is abstract base class to handle various stream in SLLIB
 *
 * available concrete derived classes are:
 *  - sli::stdstreamio    ... for stdin, stdout, stderr and I/O of local files.
 *  - sli::gzstreamio     ... for gzip files and stream.
 *  - sli::bzstreamio     ... for bzip2 files and stream.
 *  - sli::httpstreamio   ... to access HTTP server.
 *  - sli::ftpstreamio    ... to access FTP server.
 *  - sli::pipestreamio   ... to read or write using pipe connections.
 *  - sli::digeststreamio ... to handle various streams.
 *  - sli::termlineio     ... wrapper for GNU readline
 *  - sli::termscreenio   ... execute pager for write, execute editor for read.
 *  - sli::inetstreamio   ... low-level class to access Internet.
 *
 */

/**
 * @class  sli::cstreamio
 * @brief  ���ȥ꡼�������ϤΤ������ݴ��쥯�饹
 *
 *   cstreamio ���饹�Ȥ��ηѾ����饹�ϡ��͡��ʼ���Υ��ȥ꡼����Ф��������Ϥ�
 *   ����� LIBC ���� API ���󶡤��ޤ���LIBC �Ȥ��ä���ʴ���Ū�� API �˲ä���
 *   �ƥ����ȥե������1�Ԥ����ɤ���������� getline() �䡤����������ǽ�ʻ���
 *   ��®���ɤ����Ф�����ǽ�� rskip() �Τ褦�ˡ������������ API ���ɲä����
 *   ���ޤ���<br>
 *   �ץ���ޤ��Ȥ���Ѿ����饹�ϲ����ΤȤ���Ǥ���<br>
 *    - sli::stdstreamio    ... ɸ�������Ϥȥ�����ե��������Υ��ȥ꡼�� <br>
 *    - sli::gzstreamio     ... gzip���̥��ȥ꡼��������� <br>
 *    - sli::bzstreamio     ... bzip2���̥��ȥ꡼��������� <br>
 *    - sli::httpstreamio   ... HTTP�����ФؤΥ������� <br>
 *    - sli::ftpstreamio    ... FTP�����ФؤΥ������� <br>
 *    - sli::pipestreamio   ... �ѥ��פ�Ȥä����ȥ꡼�������� <br>
 *    - sli::digeststreamio ... �嵭���٤ƤΥ��ȥ꡼���ޤ���ǽ���Υ��饹 <br>
 *    - sli::termlineio     ... GNU readline���Ф���wrapper <br>
 *    - sli::termscreenio   ... ���ϻ���pager�����ϻ���editor��ư <br>
 *    - sli::inetstreamio   ... ���٥�ʥ��󥿡��ͥåȥ�������(telnet) <br>
 *   cstreamio ���饹�� public �� API �ϡ�����Ū�˾嵭���٤ƤΥ��饹�ǻ��Ѳ�ǽ
 *   �Ǥ����������äơ�SLLIB �Υ��ȥ꡼��˴ؤ�����ܻ��ͤ��Τ�ˤϡ�cstreamio
 *   ���饹�� API �����򤹤�н�ʬ�Ǥ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤cstreamio.h �������� stdio.h
 *             �� include ���Ƥ���������
 * @attention  ��ݥ��饹�Τ��ᡤ�ץ���ޤ�ľ�����Ѥ�����ϤǤ��ޤ���
 *
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  /* used for bread() and bwrite() */
  typedef struct {
    ssize_t sz_type;
    ssize_t length;
  } bstream_info;

  class cstreamio 
  {

  public:
    /* constructor & destructor */
    cstreamio();
    virtual ~cstreamio();

    /*
     * provided; these member functions use this->open(), this->getstr(),
     *           this->write() and this->read(), so generally not be overridden
     */
    //virtual const char *operator=(const char *s);

    /* open a file with printf() style argument */
    virtual int openf( const char *mode, const char *format, ... );
    virtual int vopenf( const char *mode, const char *format, va_list ap );

    /* same as scanf() of LIBC, however, scanf() always calls getline() */
    /* first. i.e., a scan is always done from beginning of a line when */
    /* nchars is not set.                                               */
    /*   nchars:  limit value for the number of characters to be read   */
    virtual int scanf( const char *format, ... );
    virtual int vscanf( const char *format, va_list ap );
    virtual int scanf( size_t nchars, const char *format, ... );
    virtual int vscanf( size_t nchars, const char *format, va_list ap );

    /* same as printf() of LIBC */
    virtual int printf( const char *format, ... );
    virtual int vprintf( const char *format, va_list ap );

    /* read strings up to the newline character from a stream opened by  */
    /* open() member functions in the buffer inside the object, and then */
    /* returns the address of that internal buffer.                      */
    /*   nchars:  limit value for the number of characters to be read    */
    virtual const char *getline();
    virtual const char *getline( size_t nchars );

    /* bread() functions read binary data and adjust endian.               */
    /* bwrite() functions adjust endian of binary data and write them.     */
    /*   sz_type: type of element and word length.                         */
    /*            positive number is set for integer type.                 */
    /*            negative number is set for float type.                   */
    /*            (e.g., 4 indicates int32_t, -4 indicates 4-byte float)   */
    /*   binfo:   definition of the structure of a block of binary data    */
    /*   n:       number of data or data blocks                            */
    /*   little_endian: set true if byte order of stream is little endian. */
    virtual ssize_t bread( void *buf, ssize_t sz_type,
			   size_t n, bool little_endian );
    virtual ssize_t bwrite( const void *buf, ssize_t sz_type,
			    size_t n, bool little_endian );
    virtual ssize_t bread( void *buf, const bstream_info binfo[],
			   size_t n, bool little_endian );
    virtual ssize_t bwrite( const void *buf, const bstream_info binfo[],
			    size_t n, bool little_endian );

    /*
     * provided; these member functions call this->raw_read() and 
     *           this->raw_write() when this->fp == NULL.
     *           If this->fp is set, fgetc(), fgets(), etc. are called.
     */
    /*
     * ���Υ֥�å��Υ��дؿ��ϡ�stdstreamio �ˤƥ����С��饤�ɤ���ɬ��
     * �����롥this->fp �� NULL �ǤϤʤ���硤LIBC �δؿ���Ƥ֤���stdstreamio
     * �Ǥϡ�stdin, stdout �ˤĤ��Ƥϡ��������� this->fp ���ڤ��ؤ��ƻȤ������
     * ���롥
     */

    /* same as fgetc(), fgets(), and fread() of LIBC */
    virtual int getchr();
    virtual char *getstr( char *s, size_t size );
    virtual ssize_t read( void *buf, size_t size );

    /* same as fputc(), fputs(), and fwrite() of LIBC */
    virtual int putchr( int c );
    virtual int putstr( const char *s );
    virtual ssize_t write( const void *buf, size_t size );

    /* skip n bytes using seek if possible (for stream to be read) */
    virtual ssize_t rskip( size_t n );

    /* skip n bytes using seek if possible (for stream to be written) */
    /* `ch' is written when seek is not available.                    */
    virtual ssize_t wskip( size_t n, int ch = 0 );

    /* provided: returns this->fp_seekable_rec. */
    /* returns true when stream accepts seek.   */
    virtual bool is_seekable() const;

    /* should be overridden if seek is possible                       */
    /* same as fseek(), rewind(), and ftell() of LIBC.                */
    /* these functions are available when is_seekable() returns true. */
    virtual int seek( long offset, int whence );
    virtual int rewind();
    virtual long tell();

    /*
     * provided: for this->fp and this->ind_sp
     */
    virtual int flush();		/* same as fflush() in stdio.h */
    virtual int eof();			/* same as feof() in stdio.h */
    virtual int error();		/* same as ferror() in stdio.h */
    virtual cstreamio &reseterr();	/* same as clearerr() in stdio.h */

    /* should be overridden */
    /* same as fopen() and fclose() of LIBC */
    virtual int open( const char *mode, const char *path ) = 0;
    virtual int open( const char *mode );
    virtual int open( const char *mode, int fd );
    virtual int close();
    /* should be overridden; optional */
    virtual int open( const char *mode, cstreamio &sref );

  protected:

    /*
     * raw r/w member functions for classes without using this->fp.
     * Inherited classes should override these member functions or should 
     * override getchr(), getstr(), putchr(), putstr(), read() and write(). 
     */
    virtual ssize_t raw_read( void *buf, size_t size );
    virtual ssize_t raw_write( const void *buf, size_t size );

  private:
    int realloc_line_buffer( size_t len_bytes );
    int realloc_printf_buffer( size_t len_bytes );
    int realloc_read_buffer( size_t len_bytes );
    void free_line_buffer();
    void free_printf_buffer();
    void free_read_buffer();

  protected:
    void *fp;
    bool fp_seekable_rec;
    /* indirect r/w �ǻ��� */
    cstreamio *ind_sp;
    /* eof() �ǻ��� */
    int eofflg_rec;
    /* error() �ǻ��� */
    int errflg_rec;

  private:
    /* dummy... Do not call this!!  */
    cstreamio(const cstreamio &obj);
    /* dummy... Do not call this!!  */
    cstreamio &operator=(const cstreamio &obj);
    /* for this->getline() */
    size_t size_line_buffer;
    char *line_buffer;
    /* for this->printf() */
    size_t size_printf_buffer;
    char *printf_buffer;
    /* for getchr(), getstr() and read() */
    char *read_buffer;
    char *read_buffer_next_pointer;	/* �����ɤ�٤�����(���ɤʤ�NULL) */
    size_t read_buffer_read_length;	/* ��Ǽ����Ƥ���ʸ����Ĺ */
    size_t read_buffer_size;		/* �Хåե������� */

  };

}

/*
  - getstr() ��ư��ξܺ٤ʵ��ꡥ

    getstr(NULL,?) �ξ��:
    getstr(?,0) �ξ��:
      NULL ���֤롥

    getstr(buf,1) �ξ��:
      buf[0] �� '\0' ���������졤NULL ���֤롥    
      (fgets() �Ǥ� '\0' ����������ʤ�)

    ̤�����ץ�ʾ��:
      ��ǽ�ʤ�� buf[0] �� '\0' ���������졤NULL ���֤롥


  - read() ��ư��ξܺ٤ʵ��ꡥ

    read(NULL,0) �ξ��:
    read(buf,0) �ξ��:
      0 ���֤롥

    read(NULL,1) �ξ��:
      -1 (���顼) ���֤롥

    ̤�����ץ�ʾ��:
      ��� -1 (���顼) ���֤롥


  - putstr() ��ư��ξܺ٤ʵ��ꡥ

    putstr(NULL) �ξ��:
      EOF ���֤롥

    putstr("") �ξ��:
      1 (����) ���֤롥

    putstr("...") �ξ��:
      1 (����) ���֤롥

    ̤�����ץ�ʾ��:
      ��� EOF ���֤롥


  - write() ��ư��ξܺ٤ʵ��ꡥ

    write(NULL,0) �ξ��:
    write(buf,0) �ξ��:
      0 ���֤롥

    write(NULL,1) �ξ��:
      -1 (���顼) ���֤롥

    ̤�����ץ�ʾ��:
      ��� -1 (���顼) ���֤롥

*/

#endif	/* _SLI__CSTREAMIO_H */
