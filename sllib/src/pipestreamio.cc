/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:32:31 cyamauch> */

/**
 * @file   pipestreamio.cc
 * @brief  パイプを使ったストリーム入出力を行なう pipestreamio クラスのコード
 */

#define CLASS_NAME "pipestreamio"

#include "config.h"

#include "pipestreamio.h"
#include "tarray_tstring.h"

#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_getppid.h"
#include "private/c_dup2.h"
#include "private/c_pipe.h"
#include "private/c_fork.h"
#include "private/c_execvp.h"
#include "private/c__exit.h"

#include "private/c_fdopen.h"
#include "private/c_fclose.h"

#include "private/c_strlen.h"
#include "private/c_strcmp.h"

namespace sli
{

/**
 * @brief  コンストラクタ
 */
pipestreamio::pipestreamio()
{
    this->pfds[0] = -1;
    this->pfds[1] = -1;
    this->fp = NULL;
    this->pid = -1;
    return;
}

/**
 * @brief  デストラクタ
 */
pipestreamio::~pipestreamio()
{
    this->close();
    return;
}

/* public member functions : etc */
/**
 * @brief  コマンドを実行し，パイプで接続して，ストリームをオープン
 *
 *  コマンドは，fork() 呼び出し後に execvp() で実行されます．
 *
 * @param      mode    ストリームを開くモード
 * @param      argv[]  コマンドの全要素データ
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      読み出し用および書き込み用のファイル・ディスクリプタの生成に
 *             失敗した場合
 * @throw      プロセスの生成に失敗した場合
 */
int pipestreamio::open( const char *mode, const char *const argv[] )
{
    int return_status = -1;
    bool argv_is_argv = false;

    if ( argv == NULL ) return this->open(mode);

    if ( argv != NULL && argv[0] != NULL && 0 < c_strlen(argv[0]) ) {
        argv_is_argv = true;
    }
    else {
        goto quit;
    }

    /* setup output */
    if ( c_strcmp(mode,"w") == 0 && this->fp == NULL ) {
	if ( this->pid != -1 ) goto quit;
	/* prepare pipe */
	if ( c_pipe(this->pfds) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","pipe() failed");
	}
	if ( ( this->pid=c_fork() ) < 0 ) {
	    c_close(this->pfds[1]);
	    this->pfds[1]=-1;
	    c_close(this->pfds[0]);
	    this->pfds[0]=-1;
            err_throw(__FUNCTION__,"FATAL","fork() failed");
	}
	if ( this->pid == 0 ) {		/* child process */
	    pid_t ppid;
	    c_dup2( this->pfds[0],0 );	/* stdin */
	    c_close( this->pfds[1] );
	    c_close( this->pfds[0] );
	    c_execvp( *argv, (char *const *)argv );
            err_report1(__FUNCTION__,"ERROR","Cannot execute %s",*argv);
	    ppid = c_getppid();
	    kill(ppid,SIGTERM);
	    c__exit(-1);
	}
	/* parent process */
	c_close(this->pfds[0]);
	this->pfds[0]=-1;
	this->fp = c_fdopen(this->pfds[1],"w");
	if ( this->fp == NULL ) {
	    c_close(this->pfds[1]);
	    this->pfds[1]=-1;
	    goto quit;
	}

	return_status = 0;
    }
    /* setup input */
    else if ( c_strcmp(mode,"r") == 0 && this->fp == NULL ) {
	if ( this->pid != -1 ) goto quit;
	/* prepare pipe */
	if ( c_pipe(this->pfds) < 0 ) {
            err_throw(__FUNCTION__,"FATAL","pipe() failed");
	}
	if ( ( this->pid=c_fork() ) < 0 ) {
	    c_close(this->pfds[1]);
	    this->pfds[1]=-1;
	    c_close(this->pfds[0]);
	    this->pfds[0]=-1;
            err_throw(__FUNCTION__,"FATAL","fork() failed");
	}
	if ( this->pid == 0 ) {		/* child process */
	    pid_t ppid;
	    c_dup2( this->pfds[1],1 );	/* stdout */
	    c_close( this->pfds[1] );
	    c_close( this->pfds[0] );
	    c_execvp( *argv, (char *const *)argv );
            err_report1(__FUNCTION__,"ERROR","Cannot execute %s",*argv);
	    ppid = c_getppid();
	    kill(ppid,SIGTERM);
	    c__exit(-1);
	}
	/* parent process */
	c_close(this->pfds[1]);
	this->pfds[1]=-1;
	this->fp = c_fdopen(this->pfds[0],"r");
	if ( this->fp == NULL ) {
	    c_close(this->pfds[0]);
	    this->pfds[0]=-1;
	    goto quit;
	}

	return_status = 0;
    }

 quit:
    return return_status;
}

/**
 * @brief  コマンドを実行し，パイプで接続して，ストリームをオープン
 *
 *  コマンドは，/bin/sh -c … の形で実行されます．
 *
 * @param      mode  ストリームを開くモード
 * @param      path  コマンド
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      読み出し用および書き込み用のファイル・ディスクリプタの生成に
 *             失敗した場合
 * @throw      プロセスの生成に失敗した場合
 */
int pipestreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    const char *argv[4] = {"/bin/sh","-c",path,NULL};
    if ( path == NULL ) return this->open(mode);
    else return this->open( mode, argv );
}

/**
 * @brief  ディスクリプタ指定によるストリームのオープン
 *
 *  ディスクリプタを指定して，ストリームをオープンします．
 *
 * @deprecated  使用する意味はありません．<br>
 *              このメンバ関数では，コマンドを実行し，パイプで接続して，
 *              ストリームをオープンする事はできません．
 * @param       mode ストリームを開くモード ("r" or "w")
 * @param       fd   ファイルディスクリプタ
 * @return      成功した場合は0<br>
 *              エラーの場合は負値
 * @note        実装上の理由で作成されたメンバ関数です．
 */
int pipestreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode,fd);
}

/**
 * @brief  標準入出力のオープン
 *
 *  標準入出力をオープンします．<br>
 *  mode に "r" が指定された場合は標準入力を，"w" が指定された場合は標準出力を
 *  オープンします.
 *
 * @deprecated  使用する意味はありません．<br>
 *              このメンバ関数では，コマンドを実行し，パイプで接続して，
 *              ストリームをオープンする事はできません．
 * @param     mode ストリームを開くモード("r" or "w")
 * @return    成功した場合は0<br>
 *            エラーの場合は負値
 * @throw     err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 * @note      実装上の理由で作成されたメンバ関数です．
 */
int pipestreamio::open( const char *mode )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode);
}

/**
 * @brief  ストリームのクローズ
 *
 *  open()で開いたストリームをクローズします．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int pipestreamio::close()
{
    int return_status = 0;

    if ( this->fp != NULL ) {
	return_status = c_fclose(this->fp);
	this->fp = NULL;
	this->pfds[0]=-1;
	this->pfds[1]=-1;
    }
    if ( this->pfds[1] != -1 ) {
	int status;
	status = c_close(this->pfds[1]);
	this->pfds[1]=-1;
	if ( status != 0 ) return_status = status;
    }
    if ( this->pfds[0] != -1 ) {
	int status;
	status = c_close(this->pfds[0]);
	this->pfds[0]=-1;
	if ( status != 0 ) return_status = status;
    }
    if ( 0 <= this->pid ) {
	int status, xstatus;
        while ( waitpid(this->pid,&status,0) != this->pid );
	this->pid = -1;
	if ( WIFEXITED(status) ) {	/* 正常終了の場合 */
	    xstatus = WEXITSTATUS(status);
	}
	else {				/* 異常終了の場合 */
	    xstatus = EOF;
	}
	if ( xstatus != 0 ) return_status = xstatus;
    }

    this->cstreamio::close();

    return return_status;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_getppid.c"
#include "private/c_dup2.c"
#include "private/c_pipe.c"
#include "private/c_fork.c"
#include "private/c_execvp.c"
#include "private/c__exit.c"

#include "private/c_fdopen.c"
#include "private/c_fclose.c"

#include "private/c_strlen.cc"
#include "private/c_strcmp.cc"
