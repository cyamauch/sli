/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:53:28 cyamauch> */

#ifndef _SLI__GZSTREAMIO_H
#define _SLI__GZSTREAMIO_H 1

/**
 * @file   gzstreamio.h
 * @brief  gzip���̥��ȥ꡼��򰷤� gzstreamio ���饹�����
 */

#include "cstreamio.h"
#include "heap_mem.h"

namespace sli
{

/*
 * gzstreamio class -- handling gzip compression and expansion.
 *                     (concrete derived class of cstreamio)
 *
 * linker options : -lz
 *
 */

/**
 * @class  sli::gzstreamio
 * @brief  gzip���̥��ȥ꡼��򰷤�
 *
 *   gzstreamio ���饹�ϡ�ɸ�������Ϥ��̾�Υե����������Ϥ˲ä���cstreamio 
 *   ���饹�ηѾ����饹�ǰ��������Ǥ��륹�ȥ꡼��ˤĤ��� gzip���̡���Ĺ��
 *   ���ݡ��Ȥ��륯�饹�Ǥ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤gzstreamio.h �������� stdio.h
 *             �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @note  config.h �� HAVE_LIBZ ���������Ƥ��ޤ���
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class gzstreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    gzstreamio();
    ~gzstreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    int flush();
    int open( const char *mode );
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int close();
    int open( const char *mode, cstreamio &sref );
    int eof();
    int error();
    gzstreamio &reseterr();

    /*
     * added member functions
     */
    /* this calls gzflush(..., Z_SYNC_FLUSH) for local files. */
    /* see zlib.h for details.                                */
    virtual int sync();

  protected:
    ssize_t raw_read( void *buf, size_t size );
    ssize_t raw_write( const void *buf, size_t size );

  private:
    ssize_t _raw_read( void *buf, size_t size );
    ssize_t _raw_write( const void *buf, size_t size );
    ssize_t indirect_read( void *buf, size_t size );
    ssize_t indirect_write( const void *buf, size_t size );

  private:
    void *gzfp;
    heap_mem<char> indr_zptr;
    heap_mem<char> indr_buffer;
    heap_mem<char> indw_zptr;
    heap_mem<char> indw_buffer;
    size_t indw_buffer_size;
    unsigned long indw_crc;
    unsigned long indw_in;

  };
}

#endif	/* _SLI__GZSTREAMIO_H */
