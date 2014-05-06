/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:40:00 cyamauch> */

/**
 * @file   termlineio.cc
 * @brief  GNU readlineに対するwrapper  termlineio クラスのコード
 */

#define CLASS_NAME "termlineio"

#define _SLI__TERMLINEIO_CC 1

#include "config.h"

#ifdef HAVE_LIBREADLINE

#include "termlineio.h"
#include "tarray_tstring.h"

#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_pipe.h"
#include "private/c_fork.h"
#include "private/c_dup2.h"
#include "private/c_execvp.h"
#include "private/c__exit.h"
#include "private/c_dup.h"
#include "private/c_getppid.h"
#include "private/c_isatty.h"

#include "private/c_sprintf.h"
#include "private/c_snprintf.h"
#include "private/c_fdopen.h"
#include "private/c_fclose.h"

#include "private/c_strcpy.h"
#include "private/c_strlen.h"
#include "private/c_strcmp.h"
#include "private/c_strncmp.h"
#include "private/c_strrchr.h"
#include "private/c_strdup.h"
#include "private/c_memcpy.h"

#include "private/c_vasprintf.h"

#include "private/get_argv_from_env.h"

static char *c_readline( const char * );
static void c_add_history( const char * );
static void c_clear_history( void );
static void c_stifle_history( int  );
static int c_unstifle_history( void );
static int c_read_history( const char * );
static int c_write_history( const char * );

#define DEFAULT_PAGER "more"


namespace sli
{

/**
 * @brief  コンストラクタ
 */
termlineio::termlineio()
{
    c_strcpy(this->mode,"");
    this->pfds[0] = -1;
    this->pfds[1] = -1;
    this->fp = NULL;
    this->pid = -1;
    this->auto_history = true;
    this->rl_eof = true;
    this->rl_read_buffer_next_pointer = NULL;
    this->rl_read_buffer_read_length = 0;
    this->rl_read_buffer_size = 0;
    return;
}

/**
 * @brief  デストラクタ
 */
termlineio::~termlineio()
{
    this->close();
    return;
}

/* private member function */

/**
 * @brief  ターミナルから1行を読み込み，それをヒストリ・バッファに追加
 *
 * @return     成功した場合は格納バッファのアドレス<br>
 *             ストリームの終りまたはエラーの場合はNULL
 * @throw      データ読み込み用の領域の確保に失敗した場合
 * @note       このメンバ関数は private です．
 */
char *termlineio::fill_read_buffer()
{
    char *return_ptr = NULL;
    heap_mem<char> rlbuf;
    size_t len_rlbuf;

    if ( this->rl_eof == true ) goto quit;

    rlbuf = c_readline(this->prompt.ptr());
    if ( rlbuf.ptr() == NULL ) {
	this->eofflg_rec = 1;
	this->rl_eof = true;
	goto quit;
    }
    len_rlbuf = c_strlen(rlbuf.ptr());

    try {
	if ( 0 < len_rlbuf && this->auto_history == true ) {
	    this->add_history(rlbuf.ptr());
	}

	if ( this->rl_read_buffer_size < len_rlbuf+1+1 ) {	/* '\n'+'\0' */
	    if ( this->rl_read_buffer.reallocate(len_rlbuf+1+1) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->rl_read_buffer_size = len_rlbuf+1+1;
	}
	c_sprintf(this->rl_read_buffer.ptr(),"%s\n",rlbuf.ptr());
	this->rl_read_buffer_next_pointer = this->rl_read_buffer.ptr();
	this->rl_read_buffer_read_length = len_rlbuf+1;
    }
    catch (...) {
	this->errflg_rec = 1;
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }

    return_ptr = this->rl_read_buffer.ptr();
 quit:
    return return_ptr;
}

/* public member functions : read */

/**
 * @brief  1文字の読み込み
 *
 *  open()で開いたストリームから次の文字を読み込み，int型で返します．
 *
 * @return     成功した場合は読み込んだ文字をint型にキャストした値<br>
 *             ストリームの終りまたはエラーの場合はEOF
 * @throw      データ読み込み用の領域の確保に失敗した場合
 */
int termlineio::getchr()
{
    int c = EOF;

    if ( this->fp != NULL ) {
	c = this->cstreamio::getchr();
	goto quit;
    }

    if ( c_strncmp(this->mode,"r",1) != 0 ) goto quit;

    /* fill the buffer, if NULL */
    if ( this->rl_read_buffer_next_pointer == NULL ) {
	if ( this->fill_read_buffer() == NULL ) goto quit;
    }
    /* get a ch from buffer */
    c = ((unsigned char *)(this->rl_read_buffer_next_pointer))[0];
    this->rl_read_buffer_next_pointer++;
    if ( this->rl_read_buffer.ptr() + this->rl_read_buffer_read_length <=
	 this->rl_read_buffer_next_pointer ) {
	this->rl_read_buffer_next_pointer = NULL;
    }
 quit:
    return c;
}

/**
 * @brief  文字列の読み込み
 *
 *  open() で開いたストリームから文字を読み込み，指定されたバッファに格納
 *  します．
 *  読み込みは EOF または改行文字を読み込んだ後で停止します．
 *  読み込まれた改行文字も指定されたバッファに格納されます．
 *
 * @param      s    読み込んだ文字の格納場所
 * @param      size 読み込む文字数
 * @return     成功した場合は格納バッファのアドレス<br>
 *             ストリームの終りまたはエラーの場合はNULL
 * @throw      データ読み込み用の領域の確保に失敗した場合
 */
char *termlineio::getstr( char *s, size_t size )
{
    char *return_pointer = NULL;
    char *new_s_ptr = s;
    size_t new_size = size;

    if ( size == 0 || s == NULL ) goto quit;

    if ( this->fp != NULL ) {
	return_pointer = this->cstreamio::getstr(s,size);
	goto quit;
    }

    if ( size == 1 ) {
	s[0] = '\0';
	goto quit;
    }

    if ( c_strncmp(this->mode,"r",1) != 0 ) {
	s[0] = '\0';	/* fgets() behavior */
	goto quit;
    }

    /* fill the buffer, if NULL */
    if ( this->rl_read_buffer_next_pointer == NULL ) {
	if ( this->fill_read_buffer() == NULL ) goto quit;
    }
    /* */
    {
	size_t done = this->rl_read_buffer_next_pointer - this->rl_read_buffer.ptr();
	size_t left = this->rl_read_buffer_read_length - done;
	size_t i;
	for ( i=0 ; i < new_size-1 && i < left ; i++ ) {
	    new_s_ptr[i] = this->rl_read_buffer_next_pointer[0];
	    this->rl_read_buffer_next_pointer++;
	}
	if ( left <= i ) {
	    this->rl_read_buffer_next_pointer = NULL;
	}
	new_s_ptr[i] = '\0';
	return_pointer = s;
    }
 quit:
    return return_pointer;
}

/**
 * @brief  データの読み込み
 *
 *  open()で開いたストリームからデータを読み込み，与えられた場所に格納します．
 *
 * @param    buf  データの格納場所
 * @param    size データの個数
 * @return   成功した場合はバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 * @throw    データ読み込み用の領域の確保に失敗した場合
 */
ssize_t termlineio::read( void *buf, size_t size )
{
    ssize_t return_len = -1;
    size_t new_size = size;
    char *new_buf_ptr = (char *)buf;

    if ( buf == NULL && 0 < size ) goto quit;

    if ( this->fp != NULL ) {
	return_len = this->cstreamio::read(buf,size);
	goto quit;
    }

    if ( c_strncmp(this->mode,"r",1) != 0 ) goto quit;

    if ( size == 0 ) {
	return_len = 0;
	goto quit;
    }

    /* fill the buffer, if NULL */
    if ( this->rl_read_buffer_next_pointer == NULL ) {
	if ( this->fill_read_buffer() == NULL ) goto quit;
    }
    /* */
    while (1) {
	size_t done = this->rl_read_buffer_next_pointer - this->rl_read_buffer.ptr();
	size_t left = this->rl_read_buffer_read_length - done;
	if ( new_size < left ) {
	    c_memcpy(new_buf_ptr,this->rl_read_buffer_next_pointer,new_size);
	    this->rl_read_buffer_next_pointer += new_size;
	    return_len = size;
	    break;
	}
	else {
	    c_memcpy(new_buf_ptr,this->rl_read_buffer_next_pointer,left);
	    this->rl_read_buffer_next_pointer = NULL;
	    new_size -= left;
	    new_buf_ptr += left;
	    if ( new_size == 0 ) {
		return_len = size;
		break;
	    }
	    if ( this->fill_read_buffer() == NULL ) {
		return_len = size - new_size;
		break;
	    }
	}
    }
 quit:
    return return_len;
}

/* public member functions : etc */
/**
 * @brief  ストリームのオープン
 *
 *  mode に "r"，"r+" が指定された場合はコマンド履歴ファイルを，"w" が指定され
 *  た場合はデフォルトページャを起動し，パイプで接続し，書き込み用ストリームを
 *  オープンします．
 *
 * @param      mode   ファイルまたはページャを開くモード ("r", "r+" or "w")
 * @param      argv[] ファイルまたはページャの全要素データ
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      読み込み用および書き込み用のファイル・ディスクリプタの生成に
 *             失敗した場合
 * @throw      プロセスの生成に失敗した場合
 */
int termlineio::open( const char *mode, const char *const argv[] )
{
    int return_status = -1;
    bool argv_is_argv;

    if ( argv != NULL && argv[0] != NULL && 0 < c_strlen(argv[0]) ) {
        argv_is_argv = true;
    }
    else {
        argv_is_argv = false;
    }

    if ( this->mode[0] != '\0' ) goto quit;
    if ( this->fp != NULL ) goto quit;

    /* setup pager */
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
	    if ( argv_is_argv == true ) c_execvp( *argv, (char *const *)argv );
	    else {
		char **pager_argv = 
		    get_argv_from_env("PAGER",DEFAULT_PAGER,NULL);
		if ( pager_argv != NULL ) {
		    c_execvp( *pager_argv, pager_argv );
		    free(pager_argv[0]);
		    free(pager_argv);
		}
	    }
            err_report(__FUNCTION__,"ERROR","Cannot execute pager");
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

	c_snprintf(this->mode,TERMLINEIO__SIZE_MODESTRING,"%s",mode);
	return_status = 0;
    }
    else if ( c_strcmp(mode,"r") == 0 || c_strcmp(mode,"r+") == 0 ) {
#ifndef SLI_TEST
	if ( c_isatty(0) != 0 ) {
#endif /* !SLI_TEST */
	    /* setup readline */
	    if ( argv_is_argv == true ) {
	        this->path = c_strdup(argv[0]);
		if ( this->path.ptr() == NULL ) {
		    err_throw(__FUNCTION__,"FATAL","strdup() failed");
		}
		this->read_history( this->path.ptr() );
	    }
	    this->rl_eof = false;
#ifndef SLI_TEST
	}
	else {
	    int fd;
	    fd = c_dup(0);
	    if ( fd == -1 ) {
		err_throw(__FUNCTION__,"FATAL","dup() failed");
	    }
	    this->fp = c_fdopen(fd,"r");
	    if ( this->fp == NULL ) {
		goto quit;
	    }
	}
#endif /* !SLI_TEST */
	c_snprintf(this->mode,TERMLINEIO__SIZE_MODESTRING,"%s",mode);
	return_status = 0;
    }

    this->eofflg_rec = 0;
    this->errflg_rec = 0;

 quit:
    return return_status;
}

/**
 * @brief  ストリームのオープン
 *
 *  mode に "r"，"r+" が指定された場合はコマンド履歴ファイルを，"w" が指定され
 *  た場合はデフォルトページャを起動し，パイプで接続し，書き込み用ストリームを
 *  オープンします．
 *
 * @param      mode  ファイルまたはページャを開くモード ("r", "r+" or "w")
 * @param      path  ファイルまたはページャ
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      読み出し用および書き込み用のファイル・ディスクリプタの生成に
 *             失敗した場合
 * @throw      プロセスの生成に失敗した場合
 * @throw      データ読み込み用の領域の確保に失敗した場合
 */
int termlineio::open( const char *mode, const char *path )
{
    if ( mode == NULL ) return -1;
    if ( mode[0] == 'w' ) {
	const char *argv[4] = {"/bin/sh","-c",path,NULL};
	if ( path == NULL ) argv[0] = NULL;
	return this->open( mode, argv );
    }
    else {
	tarray_tstring sobj;
	sobj.split(path," ",false,NULL,'\\',true);
	return this->open( mode, sobj.cstrarray() );
    }
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
int termlineio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->mode[0] != '\0' ) goto quit;

    if ( this->fp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    }
    fd_new = c_dup( fd );
    this->fp = c_fdopen(fd_new,mode);
    if ( this->fp == NULL ) goto quit;

    c_snprintf(this->mode,TERMLINEIO__SIZE_MODESTRING,"%s",mode);
    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  ストリームのオープン
 *
 *  mode に "r" が指定された場合はコマンド履歴ファイルを，"w" が指定された場合
 *  はデフォルトページャを起動し，パイプで接続し，書き込み用ストリームをオープ
 *  ンします．
 *
 * @param      mode ストリームを開くモード ("r", "r+" or "w")
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      読み出し用および書き込み用のファイル・ディスクリプタの生成に
 *             失敗した場合
 * @throw      プロセスの生成に失敗した場合
 * @throw      データ読み込み用の領域の確保に失敗した場合
 */
int termlineio::open( const char *mode )
{
    assert( mode != NULL );
    int return_status = -1;
    if ( ( c_strncmp(mode,"r",1) == 0 ) || ( c_strncmp(mode,"w",1) == 0 ) ) {
      return_status = this->open( mode, (char *)NULL );
    }
    return return_status;
}

/**
 * @brief  ストリームのクローズ
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int termlineio::close()
{
    int return_status = 0;

    if ( this->fp != NULL ) {
	return_status = c_fclose(this->fp);
	this->fp = NULL;
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

    this->rl_read_buffer = NULL;
    this->rl_read_buffer_next_pointer = NULL;
    this->rl_read_buffer_read_length = 0;
    this->rl_read_buffer_size = 0;

    if ( c_strcmp(this->mode,"r+") == 0 ) {
	if ( this->path.ptr() != NULL ) {
	    this->write_history(this->path.ptr());
	}
    }

    this->rl_eof = true;
    this->prompt = NULL;
    this->path = NULL;
    c_strcpy(this->mode,"");

    this->cstreamio::close();

    if ( return_status == 0 ) {
	this->eofflg_rec = 0;
	this->errflg_rec = 0;
    }

    return return_status;
}

/* public member functions : termlineio-specific */

/* 
 *  private な tstring への参照でも良いが，返り値が termlineio &
 *  だと，p = foo.set_prompt("prompt> ").getline(); とかできるので
 *  便利．
 */

/**
 * @brief  プロンプトの設定
 *
 * @param      prompt プロンプト
 * @return     自身の参照
 * @throw      プロンプト設定用の領域の確保に失敗した場合
 */
termlineio &termlineio::set_prompt( const char *prompt )
{
    assert( prompt != NULL );

    if ( prompt != NULL ) {
	if ( this->prompt.reallocate( c_strlen(prompt) + 1 ) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","realloc() failed");
	}
	c_strcpy(this->prompt.ptr(), prompt);
    }
    return *this;
}

/**
 * @brief  プロンプトの設定 (printf()の記法)
 *
 * @param      format プロンプトのフォーマット指定
 * @param      ...    プロンプトの各要素データ
 * @return     自身の参照
 * @throw      プロンプト設定用の領域の確保に失敗した場合
 * @throw      プロンプトの各要素がプロンプトのフォーマット指定に合わない場合
 */
termlineio &termlineio::setf_prompt( const char *format, ... )
{
    assert( format != NULL );

    va_list ap;
    va_start(ap,format);
    try {
        this->vsetf_prompt(format,ap);
    }
    catch (...) {
        va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vsetf_prompt() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  プロンプトの設定 (printf()の記法)
 *
 * @param      fmt プロンプトのフォーマット指定
 * @param      ap  プロンプトの全要素データ
 * @return     自身の参照
 * @throw      プロンプト設定用の領域の確保に失敗した場合
 * @throw      プロンプトの各要素がプロンプトのフォーマット指定に合わない場合
 */
termlineio &termlineio::vsetf_prompt( const char *fmt, va_list ap )
{
    heap_mem<char> buf;

    if ( fmt == NULL ) return this->set_prompt(fmt);

    if ( c_vasprintf(buf.ptr_address(), fmt, ap) < 0 ) {
        err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    try {
        this->set_prompt(buf.ptr());
    }
    catch (...) {
        err_throw(__FUNCTION__,"FATAL","this->set_prompt() failed");
    }

    return *this;
}

/**
 * @brief  ヒストリを自動保存するかどうかの設定を行う
 *
 * @param      tf ヒストリ自動保存フラグ(true/false)
 * @return     自身の参照
 */
termlineio &termlineio::automate_history( bool tf )
{
    this->auto_history = tf;
    return *this;
}

/**
 * @brief  コマンドをヒストリ・バッファに追加
 *
 * @param      line コマンド名
 * @return     自身の参照
 * @throw      コマンド複写のための領域確保に失敗した場合
 * @throw      コマンドをヒストリ・バッファへの格納に失敗した場合
 */
termlineio &termlineio::add_history( const char *line )
{
    if ( line != NULL ) {
	heap_mem<char> tmp_line;
	tmp_line = c_strdup(line);
	if ( tmp_line.ptr() == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","strdup() failed");
	}
	try {
	    char *p;
	    p = c_strrchr(tmp_line.ptr(), '\n');
	    if ( p != NULL ) {
		*p = '\0';
	    }
	    c_add_history(tmp_line.ptr());
	}
	catch (...) {
	    err_throw(__FUNCTION__,"FATAL","unexpected exception");
	}
    }
    return *this;
}

/**
 * @brief  ヒストリ・バッファの内容を全て削除
 *
 * @return     自身の参照
 */
termlineio &termlineio::clear_history()
{
    c_clear_history();
    return *this;
}

/**
 * @brief  ヒストリ・バッファの最大件数の設定
 *
 *  ヒストリ・バッファの件数を最大でnum_linesに制限します．
 *
 * @param      num_lines ヒストリ・バッファの件数
 * @return     自身の参照
 */
termlineio &termlineio::stifle_history( int num_lines )
{
    c_stifle_history(num_lines);
    return *this;
}

/**
 * @brief  ヒストリ・バッファの件数制限を解除
 *
 * stifle_history()で制限したヒストリ・バッファの件数制限を解除します．
 *
 * @return     自身の参照
 */
termlineio &termlineio::unstifle_history()
{
    c_unstifle_history();
    return *this;
}

/**
 * @brief  履歴ファイルからの読み込み
 *
 *  指定されたファイルに記載された内容を読み込み，ヒストリ・バッファに
 *  追加します．
 *
 * @param      path ファイル名
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 */
int termlineio::read_history( const char *path )
{
    assert( path != NULL );

    return c_read_history(path);
}

/**
 * @brief  履歴ファイルからの読み込み (printf()の記法)
 *
 *  指定されたファイルに記載された内容を読み込み，ヒストリ・バッファに
 *  追加します．
 *
 * @param      format ファイル名のフォーマット指定
 * @param      ...    ファイル名の各要素データ
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 * @throw      ファイルパスの各要素データがフォーマット指定に合わない場合
 */
int termlineio::readf_history( const char *format, ... )
{
    assert( format != NULL );

    int ret;
    va_list ap;
    va_start(ap,format);
    try {
        ret = this->vreadf_history(format,ap);
    }
    catch (...) {
        va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vreadf_history() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  履歴ファイルからの読み込み (printf()の記法)
 *
 *  指定されたファイルに記載された内容を読み込み，ヒストリ・バッファに
 *  追加します．
 *
 * @param      fmt ファイル名のフォーマット指定
 * @param      ap  ファイル名の全要素データ
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 * @throw      ファイルパスの各要素データがフォーマット指定に合わない場合
 */
int termlineio::vreadf_history( const char *fmt, va_list ap )
{
    int ret;
    heap_mem<char> buf;

    if ( fmt == NULL ) return this->read_history(fmt);

    if ( c_vasprintf(buf.ptr_address(), fmt, ap) < 0 ) {
        err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    try {
        ret = this->read_history(buf.ptr());
    }
    catch (...) {
        err_throw(__FUNCTION__,"FATAL","this->read_history() failed");
    }

    return ret;
}

/**
 * @brief  履歴ファイルへの書き出し
 *
 *  指定されたファイルに，ヒストリ・バッファの内容を書き出します．
 *
 * @param      path ファイル名
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 */
int termlineio::write_history( const char *path )
{
    return c_write_history(path);
}

/**
 * @brief  履歴ファイルへの書き出し (printf()の記法)
 *
 *  指定されたファイルに，ヒストリ・バッファの内容を書き出します．
 *
 * @param      format ファイル名のフォーマット指定
 * @param      ...    ファイル名の各要素データ
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 * @throw      ファイルパスの各要素データがフォーマット指定に合わない場合
 */
int termlineio::writef_history( const char *format, ... )
{
    int ret;
    va_list ap;
    va_start(ap,format);
    try {
        ret = this->vwritef_history(format,ap);
    }
    catch (...) {
        va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vwritef_history() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  履歴ファイルへの書き出し (printf()の記法)
 *
 *  指定されたファイルに，ヒストリ・バッファの内容を書き出します．
 *
 * @param      fmt ファイル名のフォーマット指定
 * @param      ap  ファイル名の全要素データ
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 * @throw      ファイルパスの各要素データがフォーマット指定に合わない場合
 */
int termlineio::vwritef_history( const char *fmt, va_list ap )
{
    int ret;
    heap_mem<char> buf;

    if ( fmt == NULL ) return this->write_history(fmt);

    if ( c_vasprintf(buf.ptr_address(), fmt, ap) < 0 ) {
        err_throw(__FUNCTION__,"FATAL","c_vasprintf() failed");
    }
    try {
        ret = this->write_history(buf.ptr());
    }
    catch (...) {
        err_throw(__FUNCTION__,"FATAL","this->write_history() failed");
    }

    return ret;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_pipe.c"
#include "private/c_fork.c"
#include "private/c_dup2.c"
#include "private/c_execvp.c"
#include "private/c__exit.c"
#include "private/c_dup.c"
#include "private/c_getppid.c"
#include "private/c_isatty.c"

#include "private/c_sprintf.c"
#include "private/c_snprintf.c"
#include "private/c_fdopen.c"
#include "private/c_fclose.c"

#include "private/c_strcpy.c"
#include "private/c_strlen.cc"
#include "private/c_strcmp.cc"
#include "private/c_strncmp.cc"
#include "private/c_strrchr.cc"
#include "private/c_strdup.c"
#include "private/c_memcpy.cc"

#include "private/c_vasprintf.c"

#include "private/get_argv_from_env.c"

/* */

#include <readline/readline.h>
#include <readline/history.h>

/**
 * @brief  ターミナルから1行を読み込み，それを返す (内部用)
 *
 * @param      s プロンプト
 * @return     成功した場合は格納バッファのアドレス<br>
 *             入力の終りまたはエラーの場合はNULL
 * @note       private な関数です．
 */
static char *c_readline(const char *s)
{
    return readline(s);
}

/**
 * @brief  コマンドをヒストリ・バッファに追加 (内部用)
 *
 * @param      line コマンド名
 * @note       private な関数です．
 */
static void c_add_history( const char *line )
{
    add_history(line);
}

/**
 * @brief  ヒストリ・バッファの内容を全て削除 (内部用)
 *
 * @note       private な関数です．
 */
static void c_clear_history( void )
{
    clear_history();
}

/**
 * @brief  ヒストリ・バッファの件数制限の設定 (内部用)
 *
 *  ヒストリ・バッファの件数を最大でnum_linesに制限します．
 *
 * @param      num_lines ヒストリ・バッファの件数
 * @note       private な関数です．
 */
static void c_stifle_history( int num_lines )
{
    stifle_history(num_lines);
}

/**
 * @brief  ヒストリ・バッファの件数制限の解除 (内部用)
 *
 *  stifle_history()で制限したヒストリ・バッファの件数制限を解除します．
 *
 * @return     ヒストリ制限されていた場合は正の値
 *             (実行以前にセットされていた最大件数)<br>
 *             ヒストリ制限されていなかった場合は負の値<br>
 * @note       private な関数です．
 */
static int c_unstifle_history( void )
{
    return unstifle_history();
}

/**
 * @brief  履歴ファイルの読み込み (内部用)
 *
 *  指定されたファイルに記載された内容を読み込み，ヒストリ・バッファに
 *  追加します．
 *
 * @param      path  ファイル
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 * @note       private な関数です．
 */
static int c_read_history( const char *path )
{
    return read_history(path);
}

/**
 * @brief  履歴ファイルへの書き出し (内部用)
 *
 *  指定されたファイルに，ヒストリ・バッファの内容を書き出します．
 *
 * @param      path  ファイル
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外
 * @note       private な関数です．
 */
static int c_write_history( const char *path )
{
    return write_history(path);
}

#endif	/* #ifdef HAVE_LIBREADLINE */
