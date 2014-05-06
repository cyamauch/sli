/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 20:18:52 cyamauch> */

#ifndef _SLI__TERMSCREENIO_H
#define _SLI__TERMSCREENIO_H 1

/**
 * @file   termscreenio.h
 * @brief  エディタ・ページャ経由でストリームを扱う termscreenio クラスの定義
 */

#include "cstreamio.h"

#include <sys/types.h>

#define TERMSCREENIO__SIZE_TMPFILENAME 128

namespace sli
{

/*
 * termscreenio class -- input/output on a tty screen.
 *                       execute pager for write, execute editor for read.
 *                       (concrete derived class of cstreamio)
 *
 * linker options : none
 *
 */

/**
 * @class  sli::termscreenio
 * @brief  出力時はページャを，入力時はエディタを利用したストリーム入出力
 *
 *   termscreenioクラスは，ターミナル上でのエディタ経由での入力，ページャへの
 *   出力を行ないます．w3m のようなツールを作る時に便利です．
 *
 * @attention  LIBC の stdio.h を併用する場合，termstreamio.h よりも前に
 *             stdio.h を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class termscreenio : public cstreamio
  {

  public:
    /* constructor & destructor */
    termscreenio();
    ~termscreenio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    /* set editor command to path for "r" mode, pager command for "w" mode */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );
    int close();

    /*
     * added member functions
     */
    /* set editor command to argv for "r" mode, pager command for "w" mode */
    virtual int open( const char *mode, const char *const argv[] );

  private:
    int pfds[2];			/* 出力専用 */
    pid_t pid;
    char tmpfile[TERMSCREENIO__SIZE_TMPFILENAME];

  };
}

#ifndef _SLI__TERMSCREENIO_CC
#undef TERMSCREENIO__SIZE_TMPFILENAME
#endif

#endif	/* _SLI__TERMSCREENIO_H */
