/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2015-11-20 15:03:28 cyamauch> */

/* SLLIB version 1.4.5 [STABLE] */

#ifndef _SLI__CSTREAMIO_H
#define _SLI__CSTREAMIO_H 1

/**
 * @file   cstreamio.h
 * @brief  ストリーム入出力のための抽象基底クラス cstreamio の定義
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
 * @brief  ストリーム入出力のための抽象基底クラス
 *
 *   cstreamio クラスとその継承クラスは，様々な種類のストリームに対する入出力の
 *   ための LIBC 風の API を提供します．LIBC とそっくりな基本的な API に加え，
 *   テキストファイルを1行ずつ読む時に便利な getline() や，シークが可能な時に
 *   高速な読み飛ばしが可能な rskip() のように，あると便利な API が追加されて
 *   います．<br>
 *   プログラマが使える継承クラスは下記のとおりです．<br>
 *    - sli::stdstreamio    ... 標準入出力とローカルファイル等のストリーム <br>
 *    - sli::gzstreamio     ... gzip圧縮ストリームの入出力 <br>
 *    - sli::bzstreamio     ... bzip2圧縮ストリームの入出力 <br>
 *    - sli::httpstreamio   ... HTTPサーバへのアクセス <br>
 *    - sli::ftpstreamio    ... FTPサーバへのアクセス <br>
 *    - sli::pipestreamio   ... パイプを使ったストリーム入出力 <br>
 *    - sli::digeststreamio ... 上記すべてのストリームを含む万能型のクラス <br>
 *    - sli::termlineio     ... GNU readlineに対するwrapper <br>
 *    - sli::termscreenio   ... 出力時はpagerを，入力時はeditorを起動 <br>
 *    - sli::inetstreamio   ... 低レベルなインターネットアクセス(telnet) <br>
 *   cstreamio クラスの public な API は，基本的に上記すべてのクラスで使用可能
 *   です．したがって，SLLIB のストリームに関する基本仕様を知るには，cstreamio
 *   クラスの API を理解すれば十分です．
 *
 * @attention  LIBC の stdio.h を併用する場合，cstreamio.h よりも前に stdio.h
 *             を include してください．
 * @attention  抽象クラスのため，プログラマが直接利用する事はできません．
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
     * このブロックのメンバ関数は，stdstreamio にてオーバーライドする必要
     * がある．this->fp が NULL ではない場合，LIBC の関数を呼ぶが，stdstreamio
     * では，stdin, stdout については，その都度 this->fp を切り替えて使うためで
     * ある．
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
    /* indirect r/w で使用 */
    cstreamio *ind_sp;
    /* eof() で使用 */
    int eofflg_rec;
    /* error() で使用 */
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
    char *read_buffer_next_pointer;	/* 次回読むべき位置(既読ならNULL) */
    size_t read_buffer_read_length;	/* 格納されている文字列長 */
    size_t read_buffer_size;		/* バッファの容量 */

  };

}

/*
  - getstr() の動作の詳細な規定．

    getstr(NULL,?) の場合:
    getstr(?,0) の場合:
      NULL が返る．

    getstr(buf,1) の場合:
      buf[0] に '\0' が代入され，NULL が返る．    
      (fgets() では '\0' が代入されない)

    未オープンな場合:
      可能ならば buf[0] に '\0' が代入され，NULL が返る．


  - read() の動作の詳細な規定．

    read(NULL,0) の場合:
    read(buf,0) の場合:
      0 が返る．

    read(NULL,1) の場合:
      -1 (エラー) が返る．

    未オープンな場合:
      常に -1 (エラー) が返る．


  - putstr() の動作の詳細な規定．

    putstr(NULL) の場合:
      EOF が返る．

    putstr("") の場合:
      1 (成功) が返る．

    putstr("...") の場合:
      1 (成功) が返る．

    未オープンな場合:
      常に EOF が返る．


  - write() の動作の詳細な規定．

    write(NULL,0) の場合:
    write(buf,0) の場合:
      0 が返る．

    write(NULL,1) の場合:
      -1 (エラー) が返る．

    未オープンな場合:
      常に -1 (エラー) が返る．

*/

#endif	/* _SLI__CSTREAMIO_H */
