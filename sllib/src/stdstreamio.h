/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:30 cyamauch> */

#ifndef _SLI__STDSTREAMIO_H
#define _SLI__STDSTREAMIO_H 1

/**
 * @file   stdstreamio.h
 * @brief  ɸ�������Ϥȥ�����ե��������Υ��ȥ꡼��򰷤� stdstreamio �����
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
 * @brief  ɸ�������Ϥȥ�����ե��������Υ��ȥ꡼��򰷤�
 *
 *   stdstreamio ���饹�ϡ�LIBC �� stdio.h ��Ʊ�͡�ɸ�������ϡ�ɸ�२�顼����
 *   ���̾�Υե����������Ϥ򰷤�����Υ��饹�Ǥ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤stdstreamio.h �������� stdio.h
 *             �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
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
 *           ɸ����ϡ�ɸ�२�顼���Ϥؽ��Ϥ��뤿��Υ�����
 */

/**
 * @example  examples_sllib/read_local_text_file.cc
 *           ������ˤ���ƥ����ȥե������1�Ԥ����ɤߡ����Ϥ��륳����
 */

}

#endif	/* _SLI__STDSTREAMIO_H */
