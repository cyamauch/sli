/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:20:29 cyamauch> */

/**
 * @file  stdstreamio.cc
 * @brief 標準入出力とローカルファイル等のストリームを扱う stdstreamio のコード
 */

#define CLASS_NAME "stdstreamio"

#include "config.h"

#include "stdstreamio.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_stdout.h"
#include "private/c_stdin.h"
#include "private/c_stderr.h"
#include "private/c_fopen.h"
#include "private/c_fclose.h"
#include "private/c_vfprintf.h"
#include "private/c_fflush.h"
#include "private/c_fileno.h"
#include "private/get_file_size.h"


namespace sli
{

/**
 * @brief  コンストラクタ
 */
stdstreamio::stdstreamio()
{
    this->p_stdout = c_stdout();
    this->fp = this->p_stdout;
    return;
}

/**
 * @brief  コンストラクタ
 *
 *  標準出力のかわりに標準エラー出力をデフォルトの出力先にする場合に使います．
 */
stdstreamio::stdstreamio(bool to_stderr)
{
    if ( to_stderr == true ) this->p_stdout = c_stderr();
    else this->p_stdout = c_stdout();
    this->fp = this->p_stdout;
    return;
}

/**
 * @brief  デストラクタ
 */
stdstreamio::~stdstreamio()
{
    this->close();
    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    return;
}

/* overridden member functions */

/**
 * @brief  標準入出力のオープン
 *
 *  標準入出力をオープンします.<br>
 *  modeに "r" が指定された場合は標準入力を，"w" が指定された場合は標準出力を
 *  オープンします.
 *
 * @param      mode ストリームを開くモード ("r" or "w")
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 */
int stdstreamio::open( const char *mode )
{
    assert( mode != NULL );

    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    return this->cstreamio::open(mode);
}

/**
 * @brief  ファイルのオープン
 *
 *  ファイル名を指定して，ストリームをオープンします．
 *
 * @param      mode ストリームを開くモード ("r" or "w")
 * @param      path   ファイル名
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 */
int stdstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;
    struct stat st;

    if ( path == NULL ) return this->open(mode);

    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    if ( this->fp != NULL ) goto quit;
    this->fp = c_fopen(path,mode);
    if ( this->fp == NULL ) goto quit;

    if ( stat(path,&st) == 0 ) {
	if ( S_ISREG(st.st_mode) ) this->fp_seekable_rec = true;
    }

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  ディスクリプタ指定によるストリームのオープン
 *
 *  ディスクリプタを指定して，ストリームをオープンします．
 *
 * @param       mode ストリームを開くモード ("r" or "w")
 * @param       fd   ファイルディスクリプタ
 * @return      成功した場合は0<br>
 *              エラーの場合は負値
 */
int stdstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    if ( this->fp == this->p_stdout || this->fp == c_stdin() ) this->fp = NULL;
    return this->cstreamio::open(mode,fd);
}

/**
 * @brief  ストリームのクローズ
 *
 *  open()で開いたストリームをクローズします．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int stdstreamio::close()
{
    int status = 0;
    if ( this->fp != NULL ) {
	if ( this->fp == this->p_stdout || this->fp == c_stdin() ) {
	    void *p_fp;
	    p_fp = this->fp;
	    this->fp = NULL;
	    this->cstreamio::close();
	    this->fp = p_fp;
	}
	else {
	    status = c_fclose(this->fp);
	    this->fp = NULL;
	    this->cstreamio::close();
	}
    }
    else this->cstreamio::close();

    return status;
}

/**
 * @brief  1文字の読み込み
 *
 *  open()で開いたストリームから次の文字を読み込み，int型で返します．
 *
 * @return     成功した場合は読み込んだ文字をint型にキャストした値<br>
 *             ストリームの終りまたはエラーの場合はEOF
 */
int stdstreamio::getchr()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::getchr();
}

/**
 * @brief  文字列の読み込み
 *
 *  open() で開いたストリームから文字を読み込み，指定されたバッファに格納しま
 *  す．<br>
 *  読み込みは EOF または改行文字を読み込んだ後で停止します．
 *  読み込まれた改行文字も指定されたバッファに格納されます．
 *
 * @param      s 読み込んだ文字の格納場所
 * @param      size 読み込む文字数
 * @return     成功した場合は格納バッファのアドレス<br>
 *             ストリームの終りまたはエラーの場合はNULL
 * @throw      データ読み込み用の領域の確保に失敗した場合
 */
char *stdstreamio::getstr( char *s, size_t size )
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::getstr(s,size);
}

/**
 * @brief  データの読み込み
 *
 *  open()で開いたストリームからデータを読み込み，与えられた場所に格納します．
 *
 * @param    buf データの格納場所
 * @param    size データの個数
 * @return   成功した場合はバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 */
ssize_t stdstreamio::read( void *buf, size_t size )
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::read(buf,size);
}

/**
 * @brief  1文字の書き込み
 *
 *  指定された文字をopen()で開いたストリームに書き込みます．
 *
 * @param      c 書き込む文字
 * @return     成功した場合は書き込まれた文字をint型にキャストした値<br>
 *             エラーの場合はEOF
 */
int stdstreamio::putchr( int c )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::putchr(c);
}

/**
 * @brief  文字列の書き込み
 *
 *  指定された文字列をopen()で開いたストリームに書き込みます．
 *
 * @param      s 書き込む文字列
 * @return     成功した場合は負ではない値<br>
 *             エラーの場合はEOF
 */
int stdstreamio::putstr( const char *s )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::putstr(s);
}

/**
 * @brief  データの書き込み
 *
 *  指定されたデータを，open()で開いたストリームに書き込みます．
 *
 * @param      buf データの格納場所
 * @param      size データの個数
 * @return     成功した場合はバイト数<br>
 *             エラーの場合は負値
 */
ssize_t stdstreamio::write( const void *buf, size_t size )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::write(buf,size);
}

/**
 * @brief  読み飛ばし(可能ならシークを利用)
 *
 *  読み込み用ストリームに対して 可能であれば n バイトのシークを行ないます．
 *  シークができない場合は，n バイトの読み飛ばしを行ないます．
 *
 * @param   n バイト数
 * @return  成功した場合はバイト数<br>
 *          ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *          エラーの場合は負値
 * @throw   一時領域の確保に失敗した場合
 */
ssize_t stdstreamio::rskip( size_t n )
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::rskip(n);
}

/**
 * @brief  書き飛ばし(可能ならシークを利用)
 *
 *  書き込み用ストリームに対して 可能であれば n バイトのシークを行ないます．
 *  シークができない場合は，n バイト分キャラクタ ch を書き込みます．
 *
 * @param      n バイト数
 * @param      ch シークできない場合に書き込むキャラクタ
 * @return     成功した場合はバイト数<br>
 *             エラーの場合は負値
 * @throw      一時領域の確保に失敗した場合
 */
ssize_t stdstreamio::wskip( size_t n, int ch )
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::wskip(n,ch);
}

/**
 * @brief  ストリームの位置の変更
 *
 *  ストリームの位置を変更します
 *
 * @param      offset ストリーム位置表示子のオフセット
 * @param      whence ストリーム位置表示子の基準位置
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 */
int stdstreamio::seek( long offset, int whence )
{
    return this->cstreamio::seek(offset, whence);
}

/**
 * @brief  ストリームの位置を先頭に変更
 *
 *  ストリームの位置を先頭に変更します
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 */
int stdstreamio::rewind()
{
    return this->cstreamio::rewind();
}

/**
 * @brief  ストリーム位置の取得
 *
 *  ストリーム位置表示子の値を返します
 *
 * @return     成功した場合は0以上の値<br>
 *             エラーの場合は負値
 */
long stdstreamio::tell()
{
    return this->cstreamio::tell();
}

/**
 * @brief  書き込み用ストリームのフラッシュ
 *
 *  標準出力ストリームにバッファに格納されているすべてのデータを書き込みます．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int stdstreamio::flush()
{
    if ( this->fp == c_stdin() ) this->fp = this->p_stdout;
    return this->cstreamio::flush();
}

/**
 * @brief  EOF指示子の取得
 */
int stdstreamio::eof()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::eof();
}

/**
 * @brief  エラー指示子の取得
 */
int stdstreamio::error()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    return this->cstreamio::error();
}

/**
 * @brief  EOF指示子とエラー指示子のリセット
 */
stdstreamio &stdstreamio::reseterr()
{
    if ( this->fp == this->p_stdout ) this->fp = c_stdin();
    this->cstreamio::reseterr();
    return *this;
}

/**
 * @brief  標準エラー出力へのフォーマット書き込み
 *
 *  標準エラー出力ストリームに，フォーマットに従って書き込みます．
 *
 * @param      format 書き込みフォーマット指定
 * @param      ... 書き込む各要素データ
 * @return     成功した場合は書き込まれた文字数<br>
 *             エラーの場合は負値
 */
int stdstreamio::eprintf(const char *format, ... )
{
    assert( format != NULL );

    va_list ap;
    int ret;
    va_start(ap,format);
    try {
	ret = c_vfprintf(c_stderr(),format,ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","unexpected exception");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  標準エラー出力へのフォーマット書き込み
 *
 *  標準エラー出力ストリームに，フォーマットに従って書き込みます．
 *
 * @param      format 書き込みフォーマット指定
 * @param      ap 書き込む全要素データ
 * @return     成功した場合は書き込まれた文字数<br>
 *             エラーの場合は負値
 */
int stdstreamio::veprintf( const char *format, va_list ap )
{
    assert( format != NULL );

    return c_vfprintf(c_stderr(),format,ap);
}

/**
 * @brief  標準エラー出力ストリームのフラッシュ
 *
 *  標準エラー出力ストリーム内のバッファに格納されているすべてのデータを書き
 *  込みます．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int stdstreamio::eflush()
{
    return c_fflush(c_stderr());
}

/**
 * @brief  ファイルサイズの取得
 *
 *  ファイルのサイズを返します．
 *
 * @return     成功した場合はファイルサイズ<br>
 *             エラーの場合は負値
 * @note       標準入力に対しては 0 を返します．
 */
long long stdstreamio::content_length() const
{
    void *fp_r = this->fp;
    if ( fp_r == this->p_stdout ) fp_r = c_stdin();
    if ( fp_r == NULL ) return -1;
    else return get_file_size(c_fileno(fp_r));
}

}	/* namespace sli */


#include "private/c_stdout.c"
#include "private/c_stdin.c"
#include "private/c_stderr.c"
#include "private/c_fopen.c"
#include "private/c_fclose.c"
#include "private/c_vfprintf.c"
#include "private/c_fflush.c"
#include "private/c_fileno.c"
#include "private/get_file_size.c"
