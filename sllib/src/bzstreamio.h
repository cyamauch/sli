/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 19:42:52 cyamauch> */

#ifndef _SLI__BZSTREAMIO_H
#define _SLI__BZSTREAMIO_H 1

/**
 * @file   bzstreamio.h
 * @brief  bzip2圧縮ストリームを扱う bzstreamio クラスの定義
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
 * @brief  bzip2圧縮ストリームを扱う
 *
 *   bzstreamio クラスは，標準入出力と通常のファイル入出力に加え，cstreamio 
 *   クラスの継承クラスで扱う事ができるストリームについて bzip2圧縮・伸長を
 *   サポートするクラスです．
 *
 * @attention  LIBC の stdio.h を併用する場合，bzstreamio.h よりも前に stdio.h
 *             を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @note  config.h で HAVE_LIBBZ2 が定義されています．
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
    stdstreamio *fp_for_indrw;	/* indirect r/w のための標準ファイルポインタ */
    heap_mem<char> indr_bzptr;
    heap_mem<char> indr_buffer;
    heap_mem<char> indw_bzptr;
    heap_mem<char> indw_buffer;
    size_t indw_buffer_size;

  };
}

#endif	/* _SLI__BZSTREAMIO_H */
