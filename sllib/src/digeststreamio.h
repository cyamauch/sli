/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:24:31 cyamauch> */

#ifndef _SLI__DIGESTSTREAMIO_H
#define _SLI__DIGESTSTREAMIO_H 1

/**
 * @file   digeststreamio.h
 * @brief  �̾���̥��ȥ꡼�������б�������ǽ�ʥ��饹 digeststreamio �����
 */

#include "cstreamio.h"
#include "tstring.h"

namespace sli
{

  /*
   * switch of multi-thread compressor support
   */
  /*
   * �ޥ������å��б��� compressor/decompressor �򤳤�������Ǥ��롥
   * **_auto() �ξ�硤����ˡ��Ķ��ѿ��� on/off ������Ǥ��롥���ξ�硤
   * SLI_PIGZ��SLI_LBZIP2 �� 0 �����ꤹ��ȡ����Ѥ��ʤ�����ˤʤ롥
   * pigz��lbzip2 �ؤΰ����ϡ����δĶ��ѿ����ɲä�������Ǥ��롥
   *   SLI_PIGZ_R_ARGS   ... pigz �ɤ߹�����
   *   SLI_PIGZ_W_ARGS   ... pigz �񤭹�����
   *   SLI_LBZIP2_R_ARGS ... lbzip2 �ɤ߹�����
   *   SLI_LBZIP2_W_ARGS ... lbzip2 �񤭹�����
   * �㤨�С�pigz ��Ȥä�Ÿ��������ϡ����Τ褦�ʥ��ޥ�ɤ��¹Ԥ���롥
   *   pigz -dc $SLI_PIGZ_R_ARGS filename.gz
   *
   * ���: ���餫�θ���(�㤨�� mode �� 755 �ˤʤäƤ��ʤ���)�ǥץ���ब
   * ��ư�Ǥ��ʤ��ä����� open() �Ǥϥ��顼��Ƚ��Ǥ��ʤ��ʤɡ�pigz, lbzip2
   * ���Ѥξ��Ͼܺ٤ʥ��顼�����å����Ǥ��ʤ��Ȥ������¤����롥pigz, lbzip2
   * ���֤��ͤ䡤��������֤��ͤϡ�close() ���дؿ����֤��ͤȤ����֤�����
   * �ǡ������������å����뤳�ȡ�
   */
  extern void pigz_auto();		/* use pigz for local files */
  extern void pigz_disabled();		/* do not use pigz */
  extern void lbzip2_auto();		/* use lbzip2 for local files */
  extern void lbzip2_disabled();	/* do not use lbzip2 */

/*
 * digeststreamio class -- a transparent stream I/O of local files and files
 *                         on HTTP and FTP servers with gzip and bzip2 
 *                         compression / decompression support.
 *                         (concrete derived class of cstreamio)
 *
 * linker options : -lz -lbz2 (Linux)
 *                  -lz -lbz2 -lsocket (Solaris)
 *
 */

/**
 * @class  sli::digeststreamio
 * @brief  �̾�ե����롤HTTP�����Ф���ΰ��̥��ȥ꡼�������б�������ǽ�ʥ��饹
 *
 *   digeststreamio���饹�ϡ�open()���дؿ��� path �����Ǽ����줿 URL �ޤ���
 *   �ե������ɬ�פ˱����� gzip �ޤ��� bzip2 ���̡���Ĺ��Ԥʤ��ʤ��饹��
 *   �꡼�������Ϥ�Ԥʤ��ޤ���openp()���дؿ���Ȥ��С��ѥ��������Ϥ��ǽ
 *   �Ǥ���<br>
 *   ������ե������ pigz��lbzip2 �Ǥι�®�ʰ��̡���Ĺ�������б����Ƥ��ޤ���
 *
 * @attention  LIBC �� stdio.h ��ʻ�Ѥ����硤digeststreamio.h ��������
 *             stdio.h �� include ���Ƥ���������
 * @attention  ��ݴ��쥯�饹�Ǥ��� cstreamio ��Ѿ����Ƥ��ޤ���<br>
 *             ����Ū�� API �ˤĤ��Ƥ� cstreamio ���饹�� public ���дؿ��Ȥ�
 *             ���������Ƥ��ޤ��Τǡ��������������������
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class digeststreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    digeststreamio();
    ~digeststreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* open() and openp() member functions automatically setup internal     */
    /* operation mode using prefix/suffix of path such as "http://", ".gz", */
    /* etc.                                                                 */
    int open( const char *mode );
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int close();
    int flush();
    int eof();
    int error();
    digeststreamio &reseterr();

    /*
     * added member functions
     */
    /* these APIs accept Perl-like arguments                 */
    /* e.g., openp("./my_program |"), openp("> outfile.txt") */
    virtual int openp( const char *path );
    virtual int openpf( const char *path_fmt, ... );
    virtual int vopenpf( const char *path_fmt, va_list ap );

    /* this returns true for stream opened as write mode */
    virtual bool is_write_mode() const;

    /* returns length of stream when accessing FTP or HTTP server */
    virtual long long content_length() const;

    /* set a string when sending a value for `User-Agent:' header (for HTTP) */
    virtual tstring &user_agent();

    /* set username and password for non-anonymous login (for FTP) */
    virtual tstring &username();
    virtual tstring &password();

  protected:
    ssize_t raw_read( void *buf, size_t size );
    ssize_t raw_write( const void *buf, size_t size );
    /* added member functions */
    virtual int open_rw( const char *mode, const char *path );

  private:
    cstreamio *isp;	/* httpstreamio, gzstreamio or bzstreamio */
    bool is_httpstream;
    bool is_ftpstream;
    bool is_write_mode_rec;
    /* */
    tstring user_agent_rec;	/* for httpstreamio */
    tstring username_rec;	/* for ftpstreamio */
    tstring password_rec;	/* for ftpstreamio */

  };

/**
 * @example  examples_sllib/read_text_from_net.cc
 *           http�����Ф� ftp�����о�ˤ���ƥ����ȥե������1�Ԥ����ɤߡ�
 *           ���Ϥ��륳����
 */

}

#endif	/* _SLI__DIGESTSTREAMIO_H */
