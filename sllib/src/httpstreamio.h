/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:57:20 cyamauch> */

#ifndef _SLI__HTTPSTREAMIO_H
#define _SLI__HTTPSTREAMIO_H 1

/**
 * @file   httpstreamio.h
 * @brief  HTTPサーバに対してストリーム入力を行なう httpstreamio クラスの定義
 */

#include "cstreamio.h"
#include "tstring.h"

namespace sli
{

/*
 * httpstreamio class -- handling GET method of HTTP client.
 *                       (concrete derived class of cstreamio)
 *
 * linker options : -lz -lbz2
 *
 */

/**
 * @class  sli::httpstreamio
 * @brief  HTTPサーバに対して GET メソッドでストリーム入力を行なう
 *
 *   httpstreamioクラスは，HTTPサーバに対して GET メソッド(HTTP 1.0)を使い，
 *   ストリーム入力を行ないます．gzip または bzip2 の伸長をサポートしています．
 *
 * @attention  LIBC の stdio.h を併用する場合，httpstreamio.h よりも前に 
 *             stdio.h を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class httpstreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    httpstreamio();
    ~httpstreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* decompression of gzip or bzip2 is enabled when "r%" is set for mode. */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );
    int close();
    int eof();
    int error();
    httpstreamio &reseterr();

    /*
     * added member functions
     */
    /* this returns value of `Content-Length:' header.                   */
    /* negative number is returned when Content-Length is not available. */
    virtual long long content_length() const;

    /* set a string when sending a value for `User-Agent:' header. */
    virtual tstring &user_agent();

  protected:
    ssize_t raw_read( void *buf, size_t size );
    /* added member functions */
    virtual int open_r( const char *mode, const char *path, 
			bool decompress_by_suffix );
  private:
    cstreamio *inet_sp;
    cstreamio *filter_sp;
    long long content_length_rec;
    tstring user_agent_rec;

  };
}

#endif  /* _SLI__HTTPSTREAMIO_H */
