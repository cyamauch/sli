/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-06-05 11:25:55 cyamauch> */

#ifndef _SLI__TERMLINEIO_H
#define _SLI__TERMLINEIO_H 1

/**
 * @file   termlineio.h
 * @brief  GNU readlineに対するwrapper  termlineio クラスの定義
 */

#include "cstreamio.h"
#include "heap_mem.h"

#include <sys/types.h>

#define TERMLINEIO__SIZE_MODESTRING 8

namespace sli
{

/*
 * termlineio class -- line-based tty input/output.
 *                     wrapper for GNU readline.
 *                     (concrete derived class of cstreamio)
 *
 * linker options : -lreadline -lncurses
 *
 */

/**
 * @class  sli::termlineio
 * @brief  GNU readlineに対するwrapper
 *
 *   termlineoクラスは，読み込みモードにおいて，GNU readlineライブラリを利用し
 *   たコマンド入力に関する支援を行ないます．<br>
 *   読み込みモードでは open() メンバ関数の path 引数にコマンドの履歴ファイルを
 *   与えます．書き込みモードでは，termscreenioクラスの場合と同様の動作です
 *   (ページャを起動します)．
 *
 * @attention  LIBC の stdio.h を併用する場合，termlineio.h よりも前に stdio.h
 *             を include してください．
 * @attention  抽象基底クラスである cstreamio を継承しています．<br>
 *             基本的な API については cstreamio クラスの public メンバ関数とし
 *             て定義されていますので，そちらをご覧ください．
 * @author  Chisato YAMAUCHI
 * @date  2013-04-01 00:00:00
 */

  class termlineio : public cstreamio
  {

  public:
    /* constructor & destructor */
    termlineio();
    ~termlineio();

    /*
     * overridden member functions:  See cstreamio.h for details.
     */
    int getchr();
    char *getstr( char *s, size_t size );
    ssize_t read( void *buf, size_t size );

    /* set history file to `path' arg for read mode, and set pager command */
    /* for write mode.                                                     */
    int open( const char *mode, const char *path );
    int open( const char *mode, int fd );
    int open( const char *mode );

    int close();

    /*
     * added member functions
     */
    /* set history file to `path' arg for read mode, and set pager command */
    /* for write mode.                                                     */
    virtual int open( const char *mode, const char *const argv[] );

    /* set prompt for read mode. default is zero-length prompt */
    virtual termlineio &set_prompt( const char *prompt );
    virtual termlineio &setf_prompt( const char *format, ... );
    virtual termlineio &vsetf_prompt( const char *fmt, va_list ap );

    /* set true to `tf' arg to enable auto-save of history */
    virtual termlineio &automate_history( bool tf );

    /* add a command line to history buffer */
    virtual termlineio &add_history( const char *line );

    /* erase all entries in history buffer */
    virtual termlineio &clear_history();

    /* set the limit of entries of history buffer */
    virtual termlineio &stifle_history( int num_lines );

    /* disable the limit set by stifle_history() */
    virtual termlineio &unstifle_history();

    /* read and write a history file */
    virtual int read_history( const char *path );
    virtual int readf_history( const char *format, ... );
    virtual int vreadf_history( const char *fmt, va_list ap );
    virtual int write_history( const char *path );
    virtual int writef_history( const char *format, ... );
    virtual int vwritef_history( const char *fmt, va_list ap );

  private:
    char *fill_read_buffer();

  private:
    char mode[TERMLINEIO__SIZE_MODESTRING];
    int pfds[2];			/* 出力専用 */
    pid_t pid;
    /* for readline */
    heap_mem<char> path;
    heap_mem<char> prompt;
    bool auto_history;			/* 自動でヒストリに追加するか */
    bool rl_eof;			/* readline は EOF を検出したか */
    /* read(),getxxx() で使用 */
    heap_mem<char> rl_read_buffer;
    char *rl_read_buffer_next_pointer;	/* 次回読むべき位置(既読ならNULL) */
    size_t rl_read_buffer_read_length;	/* 格納されている文字列長 */
    size_t rl_read_buffer_size;		/* バッファの容量 */

  };


/**
 * @example  examples_sllib/readline.cc
 *           GNU readline を使ってターミナルからコマンド入力を受け付ける例
 */

}

#ifndef _SLI__TERMLINEIO_CC
#undef TERMLINEIO__SIZE_MODESTRING
#endif

#endif	/* _SLI__TERMLINEIO_H */
