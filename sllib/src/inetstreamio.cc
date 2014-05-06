/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:47:25 cyamauch> */

/**
 * @file   inetstreamio.cc
 * @brief  低レベルなInternetアクセス(telnet)を行なうinetstreamioクラスのコード
 */

#define CLASS_NAME "inetstreamio"

#include "config.h"

#include "inetstreamio.h"

#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"

#include "private/c_fflush.h"
#include "private/c_fdopen.h"
#include "private/c_fclose.h"
#include "private/c_fputc.h"
#include "private/c_fputs.h"
#include "private/c_fwrite.h"
#include "private/c_dup.h"
#include "private/c_strncmp.h"
#include "private/c_strstr.h"
#include "private/c_strchr.h"
#include "private/c_strdup.h"
#include "private/c_memset.h"
#include "private/c_memcpy.h"

namespace sli
{

/**
 * @brief  コンストラクタ
 */
inetstreamio::inetstreamio()
{
    this->fpw = NULL;
    return;
}

/**
 * @brief  デストラクタ
 */
inetstreamio::~inetstreamio()
{
    this->close();
    return;
}

/**
 * @brief  送信用ストリームのフラッシュ
 *
 *  バッファに格納されているすべてのデータを送信用ストリームに書き込みます．
 */
int inetstreamio::flush()
{
    if ( this->fpw != NULL )
	return c_fflush(this->fpw);
    else
	return EOF;
}

/**
 * @brief  URLのオープン (双方向)
 *
 *  URL を指定して, 双方向(送信用・受信用)のストリームをオープンします．
 *
 * @param      mode  ストリームを開くモード
 * @param      path  URL
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      メモリの割り当てに失敗した場合 
 * @throw      ソケット通信に失敗した場合
 * @throw      与えられたmodeでソケットがopen出来なかった場合 
 */
int inetstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;
    struct hostent *svrhost;
    struct sockaddr_in server;
    int sock=-1;
    heap_mem<char> url;

    if ( path == NULL ) return this->open(mode);

    if ( mode == NULL ) goto quit;
    if ( mode[0] != 'r' && mode[0] != 'w' ) goto quit;
    if ( mode[1] != '+' && mode[1] != '\0' ) goto quit;
    if ( mode[1] == '+' && mode[2] != '\0' ) goto quit;
    if ( this->fp != NULL ) goto quit;
    if ( this->fpw != NULL ) goto quit;

    c_memset(&server,0,sizeof(server));

    url = c_strdup(path);
    if ( url.ptr() == NULL ) {
	err_throw(__FUNCTION__,"FATAL","strdup() failed");
    }

    try {
	struct servent *service;
	char *slasla;
	char *sla;
	char *colon;

	slasla = (char *)c_strstr(url.ptr(),"://");
	if ( slasla == NULL ) {
	    err_report1(__FUNCTION__,"ERROR","invalid url : %s",path);
	    goto quit;
	}
	*slasla = '\0';

	/* service name -> port */
	service = getservbyname(url.ptr() /* protocol */, "tcp");
	if ( service != NULL ) {
	    server.sin_port = service->s_port;
	} else {
	    server.sin_port = htons(80);
	}
	
	sla = c_strchr(slasla + 3, '/');
	if ( sla != NULL ) {
	    *sla = '\0';
	}
	
	this->host_rec = c_strdup(slasla+3);
	if ( this->host_rec.ptr() == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","strdup() failed");
	}
	
	colon = c_strchr(slasla + 3, ':');
	if ( colon != NULL ) {
	    uint16_t port;
	    *colon = '\0';
	    port = atoi(colon + 1);
	    if ( 0 < port ) server.sin_port = htons(port);
	}
	
	/* hostname -> IP */
	svrhost = gethostbyname(slasla+3 /* host */);
	if ( svrhost == NULL ){
	    err_report1(__FUNCTION__,"ERROR","cannot be resolved : %s",slasla+3);
	    goto quit;
	}
	
	if ( sla != NULL ) {
	    *sla = '/';
	    this->path_rec = c_strdup(sla);
	}
	else {
	    this->path_rec = c_strdup("/");
	}
	if ( this->path_rec.ptr() == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","strdup() failed");
	}

    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","Memory allocation failed");
    }

    server.sin_family = AF_INET;
    c_memcpy(&server.sin_addr, svrhost->h_addr, svrhost->h_length);

    /* サーバへ接続 */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
	err_throw(__FUNCTION__,"FATAL","cannot create socket");
    }

    if ( connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1 ) {
	err_report(__FUNCTION__,"ERROR","cannot connect to the server");
	goto quit;
    }

    if ( mode[0] == 'w' || (mode[0] == 'r' && mode[1] == '+') ) {
	this->fpw = c_fdopen(sock,"w");
	if ( this->fpw == NULL ) {
	    c_close(sock);
	    err_throw(__FUNCTION__,"FATAL","fdopen() failed");
	}
    }

    if ( mode[0] == 'r' || (mode[0] == 'w' && mode[1] == '+') ) {
	this->fp = c_fdopen(sock,"r");
	if ( this->fp == NULL ) {
	    if ( this->fpw != NULL ) {
		c_fclose(this->fpw);
		this->fpw = NULL;
	    }
	    else {
		c_close(sock);
	    }
	    err_throw(__FUNCTION__,"FATAL","fdopen() failed");
	}
    }

    return_status = 0;
 quit:
    if ( return_status != 0 ) {
	if ( 0 <= sock ) c_close(sock);
	this->path_rec = NULL;
	this->host_rec = NULL;
    }
    return return_status;
}

/**
 * @brief  標準入出力のオープン
 *
 *  標準入出力をオープンします.<br>
 *  mode に "r" が指定された場合は標準入力を，"w" が指定された場合は標準出力を
 *  オープンします.
 *
 * @deprecated  使用しないで下さい．<br>
 *              このメンバ関数では，パスを指定して，URLをオープンする事は
 *              できません．
 * @param     mode ストリームを開くモード ("r" or "w")
 * @return    成功した場合は0<br>
 *            エラーの場合は負値
 * @throw     err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 * @note      実装上の理由で作成されたメンバ関数であり，ユーザが直接利用
 *            すべきではありせん．
 */
int inetstreamio::open( const char *mode )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode);
}

/**
 * @brief  ディスクリプタ指定によるストリームのオープン
 *
 *  ディスクリプタを指定して，ストリームをオープンします．
 *
 * @deprecated  使用する意味はありません．<br>
 *              このメンバ関数では，パスを指定して，URLをオープンする事は<br>
 *              できません．
 * @param       mode ストリームを開くモード
 * @param       fd   ファイルディスクリプタ
 * @return      成功した場合は0<br>
 *              エラーの場合は負値
 * @note        実装上の理由で作成されたメンバ関数です．
 */
int inetstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->fp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    }
    fd_new = c_dup(fd);
    this->fp = c_fdopen(fd_new,mode);
    this->fpw = this->fp;
    if ( this->fp == NULL ) goto quit;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  ストリームのクローズ
 *
 *  open()で開いたストリームをクローズします．
 *
 * @return     成功した場合は0 <br>
 *             エラーの場合は0以外の値
 */
int inetstreamio::close()
{
    int return_status = 0;
    int status;
    if ( this->fpw == this->fp ) {
	this->fpw = NULL;
    }
    if ( this->fp != NULL ) {
	status = c_fclose(this->fp);
	this->fp = NULL;
	if ( status != 0 ) return_status = status;
    }
    if ( this->fpw != NULL ) {
	status = c_fclose(this->fpw);
	this->fpw = NULL;
	/* ここの status は常に EOF が返るので，これは報告しない */
	/* 書き込み側または読み出し側の一方がクローズされると，他方はエラー */
	/* になるらしい． */
	/* if ( status != 0 ) return_status = status; */
    }
    this->path_rec = NULL;
    this->host_rec = NULL;
    return return_status;
}

/**
 * @brief  1文字の書き込み
 *
 *  指定された文字をopen()で開いた送信用ストリームに書き込みます．
 *
 * @param      c 書き込む文字
 * @return     成功した場合は書き込まれた文字をint型にキャストした値<br>
 *             エラーの場合はEOF
 */
int inetstreamio::putchr( int c )
{
    if ( this->fpw != NULL ) {
	return c_fputc(c,this->fpw);
    }
    else return EOF;
}

/**
 * @brief  文字列の書き込み
 *
 *  指定された文字列をopen()で開いた送信用ストリームに書き込みます．
 *
 * @param      s 書き込む文字列
 * @return     成功した場合は負ではない値<br>
 *             エラーの場合はEOF
 */
int inetstreamio::putstr( const char *s )
{
    assert( s != NULL );

    if ( this->fpw != NULL ) {
	return c_fputs(s,this->fpw);
    }
    else return EOF;
}

/**
 * @brief  データの書き込み
 *
 *  指定されたデータを，open()で開いた送信用ストリームに書き込みます．
 *
 * @param      buf データの格納場所
 * @param      size データの個数
 * @return     成功した場合はバイト数<br>
 *             エラーの場合は負値
 */
ssize_t inetstreamio::write( const void *buf, size_t size )
{
    assert( buf != NULL );

    if ( this->fpw != NULL ) {
	size_t n;
	n = c_fwrite( buf, 1,size, this->fpw );
	if ( 0 < n ) return n;
	else {
	    if ( n == size ) return n;
	    else return -1;
	}
    }
    else return -1;
}

/**
 * @brief  パス(URLの"/"以降の文字列)を返す
 *
 * @return     URL内のパス (例: http://foo.com/abc なら /abc を返す)
 */
const char *inetstreamio::path() const
{
    return this->path_rec.ptr();
}

/**
 * @brief  ホスト名(URLの "://" と次の "/" に挟まれた文字列)を返す
 *
 * @return     URL内のホスト名 (例: http://foo.com/abc なら foo.com を返す)
 */
const char *inetstreamio::host() const
{
    return this->host_rec.ptr();
}

}	/* namespace sli */


#include "private/c_close.c"

#include "private/c_fflush.c"
#include "private/c_fdopen.c"
#include "private/c_fclose.c"
#include "private/c_fputc.cc"
#include "private/c_fputs.c"
#include "private/c_fwrite.c"
#include "private/c_dup.c"
#include "private/c_strncmp.cc"
#include "private/c_strstr.cc"
#include "private/c_strchr.cc"
#include "private/c_strdup.c"
#include "private/c_memset.cc"
#include "private/c_memcpy.cc"
