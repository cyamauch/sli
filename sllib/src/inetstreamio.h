/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:30:18 cyamauch> */

#ifndef _SLI__INETSTREAMIO_H
#define _SLI__INETSTREAMIO_H 1

/**
 * @file   inetstreamio.h
 * @brief  ���٥��Internet��������(telnet)��Ԥʤ� inetstreamio ���饹�����
 */

#include "cstreamio.h"
#include "heap_mem.h"

namespace sli
{

/*
 * inetstreamio class -- a clinet on the ARPA Internet protocols with 
 *                       sequenced, reliable, two-way connection based
 *                       byte streams.
 *                       (concrete derived class of cstreamio)
 *
 * linker options : none (Linux)
 *                  -lsocket (Solaris)
 *
 */

/**
 * @class  sli::inetstreamio
 * @brief  ���٥�ʥ��󥿡��ͥåȥ�������(telnet)
 *
 *   inetstreamio���饹�ϡ������å��̿����Ѥ��ƥ��󥿡��ͥå���³��Ԥ���
 *   ���ȥ꡼�������Ϥ�Ԥʤ��ޤ���<br>
 *   httpstreamio���饹��ftpstreamio���饹�ϡ�inetstreamio ���饹��Ȥä�
 *   ����Ƥ��ޤ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤inetstreamio.h ��������
 *             stdio.h �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 *
 * @author Chisato YAMAUCHI
 * @date   2013-04-01 00:00:00
 */

  class inetstreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    inetstreamio();
    ~inetstreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    int putchr( int c );
    int putstr( const char *s );
    ssize_t write( const void *buf, size_t size );
    int flush();

    /* mode = "r", "w"     : read-only, write-only one-way connection */
    /* mode = "r+" or "w+" : two-way connections that allow both r/w  */
    int open( const char *mode );
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int close();

    /*
     * added member functions
     */
    /* returns path (string beginning with '/' in URL) */
    virtual const char *path() const;

    /* returns hostname */
    virtual const char *host() const;

  protected:
    heap_mem<char> path_rec;
    heap_mem<char> host_rec;
    void *fpw;

  };

/**
 * @example  examples_sllib/http_client.cc
 *           HTTP-1.0 ���饤����Ȥδ�ñ����
 */

}

#endif	/* _SLI__INETSTREAMIO_H */
