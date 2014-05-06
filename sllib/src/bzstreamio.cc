/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:24:08 cyamauch> */

/**
 * @file   bzstreamio.cc
 * @brief  bzip2圧縮ストリームを扱う bzstreamio クラスのコード
 */

#define CLASS_NAME "bzstreamio"

#include "config.h"

#ifdef HAVE_LIBBZ2

/* BZ2_bzopen() 等を使うと，eof() などの情報がとれないので */
/* 低レベル API のみを使う */
#define SLI__BZ2_ONLY_USE_LOW_API 1

#include "bzstreamio.h"

#include <stdlib.h>
#include <bzlib.h>
#include <assert.h>

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_strncmp.h"

#ifndef SLI__BZ2_ONLY_USE_LOW_API
#include "private/c_dup.h"
#endif

/* raw_read(), raw_write() での最大メモリ使用量  */
static const size_t RW_BUFFER_SIZE_UNIT = 65536;

namespace sli
{

/**
 * @brief  コンストラクタ
 */
bzstreamio::bzstreamio()
{
    this->bzfp = NULL;
    this->fp_for_indrw = NULL;
    this->indw_buffer_size = 0;
    return;
}

/**
 * @brief  デストラクタ
 */
bzstreamio::~bzstreamio()
{
    this->close();
    return;
}

/* overridden member functions */
/**
 * @brief  書き込み用ストリームのフラッシュ
 *
 *  open()で開いたストリームに，バッファに格納されているすべてのデータを
 *  bzip2フォーマットで書き込みます．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int bzstreamio::flush()
{
    if ( this->ind_sp != NULL ) {
	return this->ind_sp->flush();
    }
    else if ( this->bzfp != NULL ) {
	return BZ2_bzflush(this->bzfp);
    }
    else return -1;
}

/**
 * @brief  標準入出力のオープン
 *
 *  標準入出力をbzip2フォーマットのストリームとしてオープンします．
 *  modeに "r" が指定された場合は標準入力を，"w" が指定された場合は標準出力を
 *  オープンします．
 *
 * @param      mode ストリームを開くモード(r or w)
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 */
int bzstreamio::open( const char *mode )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd = -1;

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;

#ifdef SLI__BZ2_ONLY_USE_LOW_API
    try {
	if ( this->fp_for_indrw != NULL ) delete this->fp_for_indrw;
	this->fp_for_indrw = new stdstreamio;
    }
    catch (...) {
	this->fp_for_indrw = NULL;
	err_throw(__FUNCTION__,"FATAL","new failed");
    }
    if ( this->fp_for_indrw->open(mode) < 0 ) {
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
    if ( this->open(mode, *(this->fp_for_indrw)) < 0 ) {
	this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
#else
    if ( c_strncmp(mode,"r",1) == 0 ) {
	fd = c_dup(0);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }
    else if ( c_strncmp(mode,"w",1) == 0 ) {
	fd = c_dup(1);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }

    this->bzfp = BZ2_bzdopen(fd,mode);
    if ( this->bzfp == NULL ) goto quit;
#endif

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd ) c_close(fd);
    return return_status;
}

/**
 * @brief  ファイルのオープン
 *
 *  ファイル名を指定して，bzip2フォーマットのストリームをオープンします．
 *
 * @param      mode ストリームを開くモード(r or w)
 * @param      path ファイル名
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 */
int bzstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;

    if ( path == NULL ) return this->open(mode);

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;

    if ( (c_strncmp(mode,"r",1) != 0) && (c_strncmp(mode,"w",1) != 0) ) goto quit;

#ifdef SLI__BZ2_ONLY_USE_LOW_API
    try {
	if ( this->fp_for_indrw != NULL ) delete this->fp_for_indrw;
	this->fp_for_indrw = new stdstreamio;
    }
    catch (...) {
	this->fp_for_indrw = NULL;
	err_throw(__FUNCTION__,"FATAL","new failed");
    }
    if ( this->fp_for_indrw->open(mode,path) < 0 ) {
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
    if ( this->open(mode, *(this->fp_for_indrw)) < 0 ) {
	this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
#else
    this->bzfp = BZ2_bzopen(path,mode);
    if ( this->bzfp == NULL ) goto quit;
#endif

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  ディスクリプタ指定によるストリームのオープン
 *
 *  ディスクリプタを指定して，bzip2フォーマットのストリームをオープンします．
 *
 * @param   mode ストリームを開くモード(r or w)
 * @param   fd   ファイルディスクリプタ
 * @return  成功した場合は0<br>
 *          エラーの場合は負値
 */
int bzstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    }

#ifdef SLI__BZ2_ONLY_USE_LOW_API
    try {
	if ( this->fp_for_indrw != NULL ) delete this->fp_for_indrw;
	this->fp_for_indrw = new stdstreamio;
    }
    catch (...) {
	this->fp_for_indrw = NULL;
	err_throw(__FUNCTION__,"FATAL","new failed");
    }
    if ( this->fp_for_indrw->open(mode,fd) < 0 ) {
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
    if ( this->open(mode, *(this->fp_for_indrw)) < 0 ) {
	this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	goto quit;
    }
#else
    fd_new = c_dup(fd);
    this->bzfp = BZ2_bzdopen(fd_new,mode);
    if ( this->bzfp == NULL ) goto quit;
#endif

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  ストリームのクローズ
 *
 *  open() で開いた bzip2 フォーマットのストリームをクローズします．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int bzstreamio::close()
{
    int status = 0;

    if ( this->bzfp != NULL ) {
	BZ2_bzclose(this->bzfp);
	this->bzfp = NULL;
    }

    if ( this->indr_bzptr.ptr() != NULL ) {
	bz_stream *bzsp = (bz_stream *)(this->indr_bzptr.ptr());
	BZ2_bzDecompressEnd(bzsp);
	this->indr_bzptr = NULL;
    }
    this->indr_buffer = NULL;

    /* */
    if ( this->indw_bzptr.ptr() != NULL ) {
	bz_stream *bzsp = (bz_stream *)(this->indw_bzptr.ptr());

	if ( this->indw_buffer.ptr() != NULL ) {
	    ssize_t sz;
	    bool done = false;
	    do {
		int zstatus;
		if ( bzsp->avail_out < this->indw_buffer_size ) {
		    sz = this->ind_sp->write(this->indw_buffer.ptr(),
				     this->indw_buffer_size - bzsp->avail_out);
		    if ( sz < 0 ) {
			status = EOF;
			this->errflg_rec = 1;
			err_report(__FUNCTION__,"ERROR","ind_sp->write() failed");
			goto quit0;
		    }
		}
		if ( done != false ) break;
		/* */
		bzsp->next_out = this->indw_buffer.ptr();
		bzsp->avail_out = this->indw_buffer_size;
		/* */
		zstatus = BZ2_bzCompress(bzsp,BZ_FINISH);
		if ( zstatus == BZ_STREAM_END ) {
		    this->eofflg_rec = 1;
		    done = true;
		}
		else if ( bzsp->avail_out != 0 ) {
		    done = true;
		}
		else if ( zstatus != BZ_FINISH_OK ) {
		    status = EOF;
		    this->errflg_rec = zstatus;
		    err_report(__FUNCTION__,"ERROR","BZ2_bzCompress() failed");
		    goto quit0;
		}
	    } while ( 1 );

	    this->ind_sp->flush();
	}
    quit0:
	BZ2_bzCompressEnd(bzsp);
	this->indw_bzptr = NULL;
    }
    this->indw_buffer = NULL;
    this->indw_buffer_size = 0;

    this->ind_sp = NULL;

    if ( this->fp_for_indrw != NULL ) {
	int fp_status;
	fp_status = this->fp_for_indrw->close();
	delete this->fp_for_indrw;
	this->fp_for_indrw = NULL;
	if ( status == 0 && fp_status != 0 ) status = fp_status;
    }

    if ( status == 0 ) {
	this->eofflg_rec = 0;
	this->errflg_rec = 0;
    }

    this->cstreamio::close();

    return status;
}

/*
 * 参考:
 *       bzlib の bzlib.c
 */
/**
 * @brief  cstreamioの継承クラスのストリームに対するオープン
 *
 *  何からのストリームをオープンしているcstreamioクラスの継承クラスのオブジェク
 *  トを指定して，bzip2フォーマットのストリームを開きます．
 *
 * @param      mode ストリームを開くモード(r or w)
 * @param      sref cstreamioクラスの継承クラスのインスタンス
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 */
int bzstreamio::open( const char *mode, cstreamio &sref )
{
    assert( mode != NULL );

    int return_status = -1;
    bz_stream *bzsp;

    /* err_report(__FUNCTION__,"DEBUG","indirect mode"); */

    if ( this->ind_sp != NULL || this->bzfp != NULL ) goto quit;
    if ( this->indr_bzptr.ptr() != NULL ) goto quit;
    if ( this->indw_bzptr.ptr() != NULL ) goto quit;

    if ( c_strncmp(mode,"w",1) == 0 ) {

	if ( this->indw_bzptr.allocate(sizeof(bz_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indw_bzptr.fill(0, sizeof(bz_stream));
	bzsp = (bz_stream *)(this->indw_bzptr.ptr());

	/* bzlib でメモリを管理する */
	bzsp->opaque = NULL;
	bzsp->bzalloc = NULL;
	bzsp->bzfree = NULL;
	/* 初期化する */
	bzsp->avail_in = 0;
	bzsp->next_in = NULL;
	bzsp->avail_out = 0;
	bzsp->next_out = NULL;

	if ( BZ2_bzCompressInit ( bzsp, 9, 0, 30 ) != BZ_OK ) {
	    err_throw(__FUNCTION__,"FATAL","BZ2_bzCompressInit() failed");
	}

	/* 出力バッファに適当なサイズでわりあてておく */
	if ( this->indw_buffer.ptr() == NULL ) {
	    if ( this->indw_buffer.allocate(4096) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    this->indw_buffer_size = 4096;
	    bzsp->next_out = this->indw_buffer.ptr();
	    bzsp->avail_out = this->indw_buffer_size;
	}
    }
    else if ( c_strncmp(mode,"r",1) == 0 ) {

	if ( this->indr_bzptr.allocate(sizeof(bz_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indr_bzptr.fill(0, sizeof(bz_stream));
	bzsp = (bz_stream *)(this->indr_bzptr.ptr());
	
	/* bzlib でメモリを管理する */
	bzsp->opaque = NULL;
	bzsp->bzalloc = NULL;
	bzsp->bzfree = NULL;
	/* 初期化する */
	bzsp->avail_in = 0;
	bzsp->next_in = NULL;
	bzsp->avail_out = 0;
	bzsp->next_out = NULL;

	if ( BZ2_bzDecompressInit ( bzsp, 0, 0 ) != BZ_OK ) {
	    err_throw(__FUNCTION__,"FATAL","BZ2_bzDecompressInit() failed");
	}
    }
    else {
        err_report1(__FUNCTION__,"ERROR","unsupported mode: `%s'",mode);
        goto quit;
    }

    this->ind_sp = &sref;

    this->eofflg_rec = 0;
    this->errflg_rec = 0;

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  EOF指示子の取得
 */
int bzstreamio::eof()
{
    return this->cstreamio::eof();
}

/**
 * @brief  エラー指示子の取得
 */
int bzstreamio::error()
{
    return this->cstreamio::error();
}

/**
 * @brief  EOF指示子とエラー指示子のリセット
 */
bzstreamio &bzstreamio::reseterr()
{
    this->cstreamio::reseterr();
    return *this;
}

/**
 * @brief  読み込み用原始関数のスロット (このクラスにてオーバライド)
 *
 * @param    buf データの格納場所
 * @param    size 解凍後のデータのバイト数
 * @return   成功した場合は読み込んだデータの解凍後のバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 * @note     メモリを節約するため，size が大きい場合は _raw_read() 関数を
 *           数回に分けて呼び出します．<br>
 *           このメンバ関数は protected です．
 */
ssize_t bzstreamio::raw_read( void *buf, size_t size )
{
    char *buf_ptr = (char *)buf;
    size_t cnt = size / RW_BUFFER_SIZE_UNIT;
    size_t left = size - RW_BUFFER_SIZE_UNIT * cnt;
    size_t i;
    ssize_t ret = 0;

    if ( buf == NULL && 0 < size ) return -1;

    /* save the memory cost */
    for ( i=0 ; i < cnt ; i++ ) {
	ssize_t s = this->_raw_read(buf_ptr, RW_BUFFER_SIZE_UNIT);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	    break;
	}
	else {
	    ret += s;
	    if ( s < (ssize_t)RW_BUFFER_SIZE_UNIT ) break;
	    buf_ptr += RW_BUFFER_SIZE_UNIT;
	}
    }
    if ( i == cnt ) {
	ssize_t s = this->_raw_read(buf_ptr, left);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	}
	else {
	    ret += s;
	}
    }
    return ret;
}

/**
 * @brief  低レベルな読み込み用原始関数
 *
 *  indirect_read() メンバ関数または BZ2_bzread() 関数を呼び出し，bzip2フォー
 *  マットのストリームから最大 size バイトのデータを読み，バッファに書き込み
 *  ます．
 *
 * @param    buf データの格納場所
 * @param    size 解凍後のデータのバイト数
 * @return   成功した場合は読み込んだデータの解凍後のバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 * @note     このメンバ関数は private です．
 */
ssize_t bzstreamio::_raw_read( void *buf, size_t size )
{
    if ( this->ind_sp != NULL ) {
	return this->indirect_read(buf,size);
    }
    if ( this->bzfp == NULL ) return -1;
    if ( buf == NULL && size == 0 ) return 0;
    return BZ2_bzread( this->bzfp, buf, size );
}

/**
 * @brief  書き込み用原始関数のスロット (このクラスにてオーバライド)
 *
 * @param      buf データの格納場所
 * @param      size データの圧縮前のバイト数
 * @return     成功した場合は圧縮前のバイト数<br>
 *             エラーの場合は負値
 * @note       メモリを節約するため，size が大きい場合は _raw_write() 関数を
 *             数回に分けて呼び出します．<br>
 *             このメンバ関数は protected です．
 */
ssize_t bzstreamio::raw_write( const void *buf, size_t size )
{
    const char *buf_ptr = (const char *)buf;
    size_t cnt = size / RW_BUFFER_SIZE_UNIT;
    size_t left = size - RW_BUFFER_SIZE_UNIT * cnt;
    size_t i;
    ssize_t ret = 0;

    if ( buf == NULL && 0 < size ) return -1;

    /* save the memory cost */
    for ( i=0 ; i < cnt ; i++ ) {
	ssize_t s = this->_raw_write(buf_ptr, RW_BUFFER_SIZE_UNIT);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	    break;
	}
	else {
	    ret += s;
	    if ( s < (ssize_t)RW_BUFFER_SIZE_UNIT ) break;
	    buf_ptr += RW_BUFFER_SIZE_UNIT;
	}
    }
    if ( i == cnt ) {
	ssize_t s = this->_raw_write(buf_ptr, left);
	if ( s < 0 ) {
	    if ( ret == 0 ) ret = s;
	}
	else {
	    ret += s;
	}
    }
    return ret;
}

/**
 * @brief  低レベルな書き込み用原始関数
 *
 *  indirect_write() メンバ関数か BZ2_bzwrite() 関数を呼び出し，与えられたバッ
 *  ファから最大 size バイトのデータを読み込み，現在開かれているストリームに書
 *  き込みます．
 *
 * @param      buf データの格納場所
 * @param      size データの圧縮前のバイト数
 * @return     成功した場合は圧縮前のバイト数<br>
 *             エラーの場合は負値
 * @note       このメンバ関数は private です．
 */
ssize_t bzstreamio::_raw_write( const void *buf, size_t size )
{
    if ( this->ind_sp != NULL ) {
	return this->indirect_write(buf,size);
    }
    if ( this->bzfp == NULL ) return -1;
    if ( buf == NULL && size == 0 ) return 0;
    return BZ2_bzwrite( this->bzfp, (void *)buf, size );
}

/**
 * @brief  超低レベルな読み込み用原始関数 (間接read)
 *
 *  cstreamio の継承クラスでオープンされたストリームに対して，本クラスでさらに
 *  オープンした場合に使われます．
 *
 * @param    buf データの格納場所
 * @param    size 解凍後のデータのバイト数
 * @return   成功した場合は読み込んだデータの解凍後のバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 * @note     このメンバ関数は private です．
 */
ssize_t bzstreamio::indirect_read( void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = BZ_OK;
    bz_stream *bzsp = (bz_stream *)(this->indr_bzptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( bzsp == NULL ) {
	if ( this->ind_sp != NULL ) return 0;
	goto quit;
    }

    bzsp->next_out = (char *)buf;	/* 出力バッファをセット */
    bzsp->avail_out = size;

    do {
        if ( bzsp->avail_in == 0 ) {	/* 読み終わったら… */
	    ssize_t sz;
	    if ( this->indr_buffer.reallocate(2 * size) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
            bzsp->next_in = this->indr_buffer.ptr();
	    sz = this->ind_sp->read(this->indr_buffer.ptr(), 2 * size);
	    if ( sz < 0 ) {
		this->errflg_rec = 1;
		err_report(__FUNCTION__,"ERROR","ind_sp->read() failed");
		goto quit;
	    }
	    else if ( sz == 0 ) {
		this->errflg_rec = 1;
		err_report(__FUNCTION__,"ERROR","unexpected EOF");
		goto quit;
	    }
            bzsp->avail_in = sz;
        }
        zstatus = BZ2_bzDecompress(bzsp);
        if ( zstatus == BZ_STREAM_END ) {
	    this->eofflg_rec = 1;
	    break;
	}
        if ( zstatus != BZ_OK ) {
	    this->errflg_rec = zstatus;
	    err_report(__FUNCTION__,"ERROR","BZ2_bzDecompress() failed");
	    goto quit;
        }
    } while ( 0 < bzsp->avail_out );

    return_len = (ssize_t)size - bzsp->avail_out;

    if ( zstatus == BZ_STREAM_END ) {
	/* 後始末 */
	BZ2_bzDecompressEnd(bzsp);
	this->indr_buffer = NULL;
	this->indr_bzptr = NULL;
    }

 quit:
    return return_len;
}

/**
 * @brief  超低レベルな書き込み用原始関数 (間接write)
 *
 *  cstreamio の継承クラスでオープンされたストリームに対して，本クラスでさらに
 *  オープンした場合に使われます．
 *
 * @param      buf データの格納場所
 * @param      size データの圧縮前のバイト数
 * @return     成功した場合は圧縮前のバイト数<br>
 *             エラーの場合は負値
 * @note       このメンバ関数は private です．
 */
ssize_t bzstreamio::indirect_write( const void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = BZ_RUN_OK;
    bz_stream *bzsp = (bz_stream *)(this->indw_bzptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( bzsp == NULL ) {
	goto quit;
    }

    bzsp->next_in = (char *)buf;	/* 入力バッファをセット */
    bzsp->avail_in = size;

    while ( 0 < bzsp->avail_in ) {	/* 入力が消費されるまで */
        //this->indw_in += zsp->avail_in;
	zstatus = BZ2_bzCompress(bzsp,BZ_RUN);
        //this->indw_in -= zsp->avail_in;
        if ( zstatus != BZ_RUN_OK ) {
	    this->errflg_rec = zstatus;
	    err_report(__FUNCTION__,"ERROR","BZ2_bzCompress() failed");
	    goto quit;
        }
        if ( bzsp->avail_out == 0 ) {	/* 満ちたら… */
	    ssize_t sz;
	    /* 書き出す */
	    sz = this->ind_sp->write(this->indw_buffer.ptr(),
				     this->indw_buffer_size);
	    if ( sz < 0 ) {
		this->errflg_rec = 1;
		err_report(__FUNCTION__,"ERROR","ind_sp->write() failed");
		goto quit;
	    }
	    /* */
	    if ( this->indw_buffer.reallocate(2 * size) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    this->indw_buffer_size = 2 * size;
	    /* */
            bzsp->next_out = this->indw_buffer.ptr();
            bzsp->avail_out = this->indw_buffer_size;
        }
    }


    return_len = size;
 quit:
    return return_len;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_strncmp.cc"

#ifndef SLI__BZ2_ONLY_USE_LOW_API
#include "private/c_dup.c"
#endif

#endif	/* #ifdef HAVE_LIBBZ2 */
