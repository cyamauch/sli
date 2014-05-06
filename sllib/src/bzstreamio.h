/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:42:52 cyamauch> */

#ifndef _SLI__BZSTREAMIO_H
#define _SLI__BZSTREAMIO_H 1

/**
 * @file   bzstreamio.h
 * @brief  bzip2���̥��ȥ꡼��򰷤� bzstreamio ���饹�����
 */

#include "cstreamio.h"
#include "stdstreamio.h"
#include "heap_mem.h"

namespace sli
{

/*
 * bzstreamio class -- handling bzip2 compression and expansion.
 *                     (concrete derived class of cstreamio)
 *
 * linker options : -lbz2
 *
 */

/**
 * @class  sli::bzstreamio
 * @brief  bzip2���̥��ȥ꡼��򰷤�
 *
 *   bzstreamio ���饹�ϡ�ɸ�������Ϥ��̾�Υե����������Ϥ˲ä���cstreamio 
 *   ���饹�ηѾ����饹�ǰ��������Ǥ��륹�ȥ꡼��ˤĤ��� bzip2���̡���Ĺ��
 *   ���ݡ��Ȥ��륯�饹�Ǥ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤bzstreamio.h �������� stdio.h
 *             �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @note  config.h �� HAVE_LIBBZ2 ���������Ƥ��ޤ���
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class bzstreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    bzstreamio();
    ~bzstreamio();

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
    bzstreamio &reseterr();

  protected:
    ssize_t raw_read( void *buf, size_t size );
    ssize_t raw_write( const void *buf, size_t size );

  private:
    ssize_t _raw_read( void *buf, size_t size );
    ssize_t _raw_write( const void *buf, size_t size );
    ssize_t indirect_read( void *buf, size_t size );
    ssize_t indirect_write( const void *buf, size_t size );

  private:
    void *bzfp;
    stdstreamio *fp_for_indrw;	/* indirect r/w �Τ����ɸ��ե�����ݥ��� */
    heap_mem<char> indr_bzptr;
    heap_mem<char> indr_buffer;
    heap_mem<char> indw_bzptr;
    heap_mem<char> indw_buffer;
    size_t indw_buffer_size;

  };
}

#endif	/* _SLI__BZSTREAMIO_H */
