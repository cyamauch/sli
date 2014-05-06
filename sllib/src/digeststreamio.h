/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:24:31 cyamauch> */

#ifndef _SLI__DIGESTSTREAMIO_H
#define _SLI__DIGESTSTREAMIO_H 1

/**
 * @file   digeststreamio.h
 * @brief  通常・圧縮ストリーム等に対応する万能なクラス digeststreamio の定義
 */

#include "cstreamio.h"
#include "tstring.h"

namespace sli
{

  /*
   * switch of multi-thread compressor support
   */
  /*
   * マルチスレッド対応の compressor/decompressor をここで制御できる．
   * **_auto() の場合，さらに，環境変数で on/off が制御できる．その場合，
   * SLI_PIGZ，SLI_LBZIP2 を 0 に設定すると，使用しない設定になる．
   * pigz，lbzip2 への引数は，次の環境変数で追加する事ができる．
   *   SLI_PIGZ_R_ARGS   ... pigz 読み込み用
   *   SLI_PIGZ_W_ARGS   ... pigz 書き込み用
   *   SLI_LBZIP2_R_ARGS ... lbzip2 読み込み用
   *   SLI_LBZIP2_W_ARGS ... lbzip2 書き込み用
   * 例えば，pigz を使って展開する場合は，次のようなコマンドが実行される．
   *   pigz -dc $SLI_PIGZ_R_ARGS filename.gz
   *
   * 注意: 何らかの原因(例えば mode が 755 になっていない等)でプログラムが
   * 起動できなかった場合に open() ではエラーが判定できないなど，pigz, lbzip2
   * 使用の場合は詳細なエラーチェックができないという制限がある．pigz, lbzip2
   * の返り値や，シェルの返り値は，close() メンバ関数の返り値として返されるの
   * で，そちらもチェックすること．
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
 * @brief  通常ファイル，HTTPサーバからの圧縮ストリーム等に対応する万能なクラス
 *
 *   digeststreamioクラスは，open()メンバ関数の path 引数で示された URL または
 *   ファイルを，必要に応じて gzip または bzip2 圧縮・伸長を行ないながらスト
 *   リーム入出力を行ないます．openp()メンバ関数を使えば，パイプ入出力も可能
 *   です．<br>
 *   ローカルファイルは pigz，lbzip2 での高速な圧縮・伸長処理に対応しています．
 *
 * @attention  LIBC の stdio.h を併用する場合，digeststreamio.h よりも前に
 *             stdio.h を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
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
 *           httpサーバや ftpサーバ上にあるテキストファイルを1行ずつ読み，
 *           出力するコード
 */

}

#endif	/* _SLI__DIGESTSTREAMIO_H */
