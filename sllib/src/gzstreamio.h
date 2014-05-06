/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:53:28 cyamauch> */

#ifndef _SLI__GZSTREAMIO_H
#define _SLI__GZSTREAMIO_H 1

/**
 * @file   gzstreamio.h
 * @brief  gzip圧縮ストリームを扱う gzstreamio クラスの定義
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
 * @brief  gzip圧縮ストリームを扱う
 *
 *   gzstreamio クラスは，標準入出力と通常のファイル入出力に加え，cstreamio 
 *   クラスの継承クラスで扱う事ができるストリームについて gzip圧縮・伸長を
 *   サポートするクラスです．
 *
 * @attention  LIBC の stdio.h を併用する場合，gzstreamio.h よりも前に stdio.h
 *             を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @note  config.h で HAVE_LIBZ が定義されています．
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
