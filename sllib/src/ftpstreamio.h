/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:50:55 cyamauch> */

#ifndef _SLI__FTPSTREAMIO_H
#define _SLI__FTPSTREAMIO_H 1

/**
 * @file   ftpstreamio.h
 * @brief  FTP�����Ф���³�����ȥ꡼�������Ϥ�Ԥʤ� ftpstreamio ���饹�����
 */

#include "cstreamio.h"
#include "inetstreamio.h"
#include "tstring.h"

namespace sli
{

/*
 * ftpstreamio class -- access FTP server.
 *                      (concrete derived class of cstreamio)
 *
 * linker options : -lz -lbz2
 *
 */

/**
 * @class  sli::ftpstreamio
 * @brief  FTP�����Ф���³�����ѥå��֥⡼�ɤǥ��ȥ꡼�������Ϥ�Ԥʤ�
 *
 *   ftpstreamio���饹�ϡ�FTP�����Ф�ƿ̾�ޤ�������Υ桼������³����
 *   �ѥå��֥⡼�ɤǥ����Ф��饹�ȥ꡼�����ϡ����뤤�ϥ����Фإ��ȥ꡼�����
 *   ��Ԥʤ��ޤ���gzip �ޤ��� bzip2 �ο�Ĺ�򥵥ݡ��Ȥ��Ƥ��ޤ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤ftpstreamio.h �������� stdio.h
 *             �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class ftpstreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    ftpstreamio();
    ~ftpstreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* compression and decompression of gzip or bzip2 are enabled when     */
    /* "w%" or "r%" is set for mode.                                       */
    /* open() accepts path string like "ftp://username:password@ftp.com/". */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );
    int close();
    int flush();
    int eof();
    int error();
    ftpstreamio &reseterr();

    /*
     * added member functions
     */
    /* this returns length of a file obtained SIZE command */
    virtual long long content_length() const;

    /* set username and password for non-anonymous login */
    virtual tstring &username();
    virtual tstring &password();

  protected:
    ssize_t raw_read( void *buf, size_t size );
    ssize_t raw_write( const void *buf, size_t size );
    /* added member functions */
    virtual int open_rw( const char *mode, const char *path, 
			 bool use_filter_by_suffix );

  private:
    int read_response( inetstreamio *isp, tstring &rline );
    inetstreamio *inet_sp;
    inetstreamio *rw_sp;
    cstreamio *filter_sp;
    long long content_length_rec;
    tstring username_rec;
    tstring password_rec;

  };
}

#endif  /* _SLI__FTPSTREAMIO_H */
