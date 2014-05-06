/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:30 cyamauch> */

#ifndef _SLI__STDSTREAMIO_H
#define _SLI__STDSTREAMIO_H 1

/**
 * @file   stdstreamio.h
 * @brief  標準入出力とローカルファイル等のストリームを扱う stdstreamio の定義
 */

#include "cstreamio.h"

namespace sli
{

/*
 * stdstreamio class -- for stdin, stdout, stderr and I/O of local files.
 *                      (concrete derived class of cstreamio)
 *
 * linker options : none
 *
 */

/**
 * @class  sli::stdstreamio
 * @brief  標準入出力とローカルファイル等のストリームを扱う
 *
 *   stdstreamio クラスは，LIBC の stdio.h と同様，標準入出力・標準エラー出力
 *   と通常のファイル入出力を扱うためのクラスです．
 *
 * @attention  LIBC の stdio.h を併用する場合，stdstreamio.h よりも前に stdio.h
 *             を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class stdstreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    stdstreamio();
    stdstreamio(bool to_stderr);
    ~stdstreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    int open( const char *mode );
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int close();

    int getchr();
    char *getstr( char *s, size_t size );
    ssize_t read( void *buf, size_t size );

    int putchr( int c );
    int putstr( const char *s );
    ssize_t write( const void *buf, size_t size );

    ssize_t rskip( size_t n );
    ssize_t wskip( size_t n, int ch = 0 );

    /* same as fseek(), rewind(), and ftell() of LIBC */
    int seek( long offset, int whence );
    int rewind();
    long tell();

    int flush();
    int eof();
    int error();
    stdstreamio &reseterr();

    /* printf() for stderr */
    virtual int eprintf( const char *format, ... );
    virtual int veprintf( const char *format, va_list ap );

    /* flush() for stderr */
    virtual int eflush();

    /* this returns file size.                                             */
    /* negative number is returned when size information is not available. */
    virtual long long content_length() const;

  private:
    void *p_stdout;

  };

/**
 * @example  examples_sllib/stdout_stderr.cc
 *           標準出力，標準エラー出力へ出力するためのコード
 */

/**
 * @example  examples_sllib/read_local_text_file.cc
 *           ローカルにあるテキストファイルを1行ずつ読み，出力するコード
 */

}

#endif	/* _SLI__STDSTREAMIO_H */
