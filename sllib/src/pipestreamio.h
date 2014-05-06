/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:20 cyamauch> */

#ifndef _SLI__PIPESTREAMIO_H
#define _SLI__PIPESTREAMIO_H 1

/**
 * @file   pipestreamio.h
 * @brief  パイプを使ったストリーム入出力を行なう pipestreamio クラスの定義
 */

#include "cstreamio.h"

#include <sys/types.h>

namespace sli
{

/*
 * pipestreamio class -- read or write using pipe connections.
 *                       (concrete derived class of cstreamio)
 *
 * linker options : none
 *
 */

/**
 * @class  sli::pipestreamio
 * @brief  パイプを使ったストリーム入出力を行なう
 *
 *   pipestreamioクラスは，ファイルを実行し，その実行したプロセスとパイプで
 *   つなぎ，ストリーム入力または出力を行ないます．
 *
 * @attention  LIBC の stdio.h を併用する場合，pipestreamio.h よりも前に 
 *             stdio.h を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class pipestreamio : public cstreamio
  {

  public:
    /* constructor & destructor */
    pipestreamio();
    ~pipestreamio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* execute a command given by `path', and r/w data via pipe */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );
    int close();

    /*
     * added member functions
     */
    /* execute a command given by `argv', and r/w data via pipe */
    virtual int open( const char *mode, const char *const argv[] );

  private:
    int pfds[2];			/* 出力専用 */
    pid_t pid;

  };

/**
 * @example  examples_sllib/gnuplot_animation.cc
 *           gnuplot でパイプ接続し，アニメーションを表示するコード
 */

}

#endif	/* _SLI__PIPESTREAMIO_H */
