/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:57:20 cyamauch> */

#ifndef _SLI__HTTPSTREAMIO_H
#define _SLI__HTTPSTREAMIO_H 1

/**
 * @file   httpstreamio.h
 * @brief  HTTP�����Ф��Ф��ƥ��ȥ꡼�����Ϥ�Ԥʤ� httpstreamio ���饹�����
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
 * @brief  HTTP�����Ф��Ф��� GET �᥽�åɤǥ��ȥ꡼�����Ϥ�Ԥʤ�
 *
 *   httpstreamio���饹�ϡ�HTTP�����Ф��Ф��� GET �᥽�å�(HTTP 1.0)��Ȥ���
 *   ���ȥ꡼�����Ϥ�Ԥʤ��ޤ���gzip �ޤ��� bzip2 �ο�Ĺ�򥵥ݡ��Ȥ��Ƥ��ޤ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤httpstreamio.h �������� 
 *             stdio.h �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
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
