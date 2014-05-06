/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:50:55 cyamauch> */

#ifndef _SLI__FTPSTREAMIO_H
#define _SLI__FTPSTREAMIO_H 1

/**
 * @file   ftpstreamio.h
 * @brief  FTPサーバに接続しストリーム入出力を行なう ftpstreamio クラスの定義
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
 * @brief  FTPサーバに接続し，パッシブモードでストリーム入出力を行なう
 *
 *   ftpstreamioクラスは，FTPサーバに匿名または特定のユーザで接続し，
 *   パッシブモードでサーバからストリーム入力，あるいはサーバへストリーム出力
 *   を行ないます．gzip または bzip2 の伸長をサポートしています．
 *
 * @attention  LIBC の stdio.h を併用する場合，ftpstreamio.h よりも前に stdio.h
 *             を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
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
