/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:19:41 cyamauch> */

/**
 * @file   gzstreamio.cc
 * @brief  gzip圧縮ストリームを扱う gzstreamio クラスのコード
 */

#define CLASS_NAME "gzstreamio"

#include "config.h"

#ifdef HAVE_LIBZ

#include "gzstreamio.h"

#include <stdlib.h>
#include <zlib.h>
#include <zconf.h>
#include <assert.h>

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL MAX_MEM_LEVEL
#endif

#define OS_CODE  0x03 /* UNIX */

#include "private/err_report.h"

#include "private/c_close.h"
#include "private/c_dup.h"
#include "private/c_strncmp.h"

/* raw_read(), raw_write() での最大メモリ使用量  */
static const size_t RW_BUFFER_SIZE_UNIT = 65536;

namespace sli
{

/**
 * @brief  コンストラクタ
 */
gzstreamio::gzstreamio()
{
    this->gzfp = NULL;
    this->indw_buffer_size = 0;
    this->indw_crc = 0;
    this->indw_in = 0;
    return;
}

/**
 * @brief  デストラクタ
 */
gzstreamio::~gzstreamio()
{
    this->close();
    return;
}

/*
  [citation from zlib.h]
  All output is flushed as with sync(), and the compression state is
  reset so that decompression can restart from this point if previous 
  compressed data has been damaged or if random access is desired. Using 
  flush() too often can seriously degrade the compression.
 */
/**
 * @brief  書き込み用ストリームのフラッシュ
 *
 *  open()で開いたストリームに，バッファに格納されているすべてのデータを
 *  gzipフォーマットで書き込みます．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 * @attention  むやみに使うと圧縮率の悪化を招きます．
 */
int gzstreamio::flush()
{
    if ( this->ind_sp != NULL )
	return this->ind_sp->flush();
    else if ( this->gzfp != NULL )
	return gzflush((gzFile)(this->gzfp), Z_FULL_FLUSH);
    else return -1;
}

/**
 * @brief  標準入出力のオープン
 *
 *  標準入出力をgzipフォーマットのストリームとしてオープンします．<br>
 *  mode に "r" が指定された場合は標準入力を，"w" が指定された場合は標準出力を
 *  オープンします．
 *
 * @param      mode ストリームを開くモード(r or w)と圧縮モード 
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 */
int gzstreamio::open( const char *mode )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd = -1;
    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;
    if ( c_strncmp(mode,"r",1) == 0 ) {
	fd=c_dup(0);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }
    else if ( c_strncmp(mode,"w",1) == 0 ) {
	fd=c_dup(1);
	if ( fd == -1 ) {
            err_throw(__FUNCTION__,"FATAL","dup() failed");
	}
    }
    this->gzfp = gzdopen(fd,mode);
    if ( this->gzfp == NULL ) goto quit;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd ) c_close(fd);
    return return_status;
}

/**
 * @brief  ファイルのオープン
 *
 *  ファイル名を指定して，gzipフォーマットのストリームをオープンします．
 *
 * @param      mode ストリームを開くモード(r or w)と圧縮モード
 * @param      path ファイル名
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 */
int gzstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;

    if ( path == NULL ) return this->open(mode);

    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;
    this->gzfp = gzopen(path,mode);
    if ( this->gzfp == NULL ) goto quit;

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  ディスクリプタ指定によるストリームのオープン
 *
 *  ディスクリプタを指定して，gzipフォーマットのストリームをオープンします．
 *
 * @param       mode ストリームを開くモード(r or w)と圧縮モード
 * @param       fd   ファイルディスクリプタ
 * @return      成功した場合は0<br>
 *              エラーの場合は負値
 */
int gzstreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;

    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;

    if ( fd == 0 ) {
        if ( c_strncmp(mode,"r",1) != 0 ) goto quit;
    } else if (fd == 1 || fd == 2) {
        if ( c_strncmp(mode,"w",1) != 0 ) goto quit;
    } else {
        /* DO_NOTHIG */
    } 
    fd_new = c_dup(fd);
    this->gzfp = gzdopen(fd_new,mode);
    if ( this->gzfp == NULL ) goto quit;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  リトルエンディアンでの 4 バイト整数の書き込み (内部用)
 *
 *  指定されたストリームに対して 4 バイトの整数型データを，処理系に依存せず
 *  常に同じバイトオーダで書き込みます．
 *
 * @param       sp cstreamioクラスの継承クラスのインスタンス
 * @param       val 符号なしlong型データ
 */
static void put_ulong32( cstreamio *sp, unsigned long val )
{
    assert( sp != NULL );

    int i;
    for ( i = 0 ; i < 4 ; i++ ) {
        sp->putchr((int)(val & 0x00ff));
        val >>= 8;
    }
    return;
}

/**
 * @brief  ストリームのクローズ
 *
 *  open() で開いた gzip フォーマットのストリームをクローズします．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int gzstreamio::close()
{
    int status = 0;

    if ( this->gzfp != NULL ) {
	status = gzclose((gzFile)(this->gzfp));
	this->gzfp = NULL;
    }

    if ( this->indr_zptr.ptr() != NULL ) {
	int zstatus;
	z_stream *zsp = (z_stream *)(this->indr_zptr.ptr());
	if ( (zstatus = inflateEnd(zsp)) != Z_OK ) {
	    status = EOF;
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","inflateEnd() failed, msg: %s",
			zsp->msg);
	}
	this->indr_zptr = NULL;
    }
    this->indr_buffer = NULL;

    /* */
    if ( this->indw_zptr.ptr() != NULL ) {
	int zstatus;
	z_stream *zsp = (z_stream *)(this->indw_zptr.ptr());

	if ( this->indw_buffer.ptr() != NULL ) {
	    ssize_t sz;
	    bool done = false;
	    do {
		if ( zsp->avail_out < this->indw_buffer_size ) {
		    sz = this->ind_sp->write(this->indw_buffer.ptr(),
				      this->indw_buffer_size - zsp->avail_out);
		    if ( sz < 0 ) {
			status = EOF;
			this->errflg_rec = 1;
			err_report(__FUNCTION__,"ERROR","ind_sp->write() failed");
			goto quit0;
		    }
		}
		if ( done != false ) break;
		/* */
		zsp->next_out = (Bytef *)(this->indw_buffer.ptr());
		zsp->avail_out = this->indw_buffer_size;
		/* */
		zstatus = deflate(zsp,Z_FINISH);
		if ( zstatus == Z_STREAM_END ) {
		    this->eofflg_rec = 1;
		    done = true;
		}
		else if ( zsp->avail_out != 0 ) {
		    done = true;
		}
		else if ( zstatus != Z_OK ) {
		    status = EOF;
		    this->errflg_rec = zstatus;
		    err_report1(__FUNCTION__,"ERROR","deflate() failed, msg: %s",
				zsp->msg);
		    goto quit0;
		}
	    } while ( 1 );
	    /* CRC とかを出力 */
	    put_ulong32(this->ind_sp,this->indw_crc);
	    put_ulong32(this->ind_sp,
			(unsigned long)(this->indw_in & 0xffffffff));
	    this->ind_sp->flush();
	}
    quit0:
	if ( (zstatus = deflateEnd(zsp)) != Z_OK ) {
	    status = EOF;
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","deflateEnd() failed, msg: %s",
			zsp->msg);
	}
	this->indw_zptr = NULL;
    }
    this->indw_buffer = NULL;
    this->indw_buffer_size = 0;
    this->indw_crc = 0;
    this->indw_in = 0;

    this->ind_sp = NULL;

    if ( status == 0 ) {
	this->eofflg_rec = 0;
	this->errflg_rec = 0;
    }

    this->cstreamio::close();

    return status;
}

/*
 * 参考:
 *       zlib の gzio.c
 *       このへん → http://www.limo.fumi2kick.com/tips/rrda/rrda01.html
 *       このへん → http://oku.edu.mie-u.ac.jp/~okumura/compression/zlib.html
 */
/**
 * @brief  cstreamioの継承クラスのストリームに対するオープン
 *
 *  何からのストリームをオープンしているcstreamioクラスの継承クラスのオブジェク
 *  トを指定して，gzipフォーマットのストリームを開きます．
 *
 * @param      mode ストリームを開くモード(r or w)と圧縮モード
 * @param      sref cstreamioクラスの継承クラスのインスタンス
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 */
int gzstreamio::open( const char *mode, cstreamio &sref )
{
    int return_status = -1;
    z_stream *zsp;

    if ( this->ind_sp != NULL || this->gzfp != NULL ) goto quit;
    if ( this->indr_zptr.ptr() != NULL ) goto quit;
    if ( this->indw_zptr.ptr() != NULL ) goto quit;

    if ( c_strncmp(mode,"w",1) == 0 ) {

	if ( this->indw_zptr.allocate(sizeof(z_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indw_zptr.fill(0,sizeof(z_stream));
	zsp = (z_stream *)(this->indw_zptr.ptr());
	
	/* zlib でメモリを管理する */
	zsp->zalloc = (alloc_func)0;
	zsp->zfree = (free_func)0; 
	zsp->opaque = (voidpf)0;
	/* 初期化 */
	zsp->avail_in = 0;
	zsp->next_in = Z_NULL;
	zsp->avail_out = 0;
	zsp->next_out = Z_NULL;

	/* 初期化 */
        if ( deflateInit2(zsp, Z_DEFAULT_COMPRESSION,
			  Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, 
			  Z_DEFAULT_STRATEGY) != Z_OK ) {
	    err_throw(__FUNCTION__,"FATAL","deflateInit2() failed");
	}

	/* 出力バッファに適当なサイズでわりあてておく */
	if ( this->indw_buffer.ptr() == NULL ) {
	    if ( this->indw_buffer.allocate(4096) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","malloc() failed");
	    }
	    this->indw_buffer_size = 4096;
	    zsp->next_out = (Bytef *)(this->indw_buffer.ptr());
	    zsp->avail_out = this->indw_buffer_size;
	}

        sref.printf("%c%c%c%c%c%c%c%c%c%c", 0x01f, 0x08b,
		    Z_DEFLATED, 0 /* flags */, 
		    0,0,0,0 /* time */, 0 /*xflags*/, 
		    OS_CODE);

	this->indw_crc = crc32(0L, Z_NULL, 0);
	this->indw_in = 0;
    }
    else if ( c_strncmp(mode,"r",1) == 0 ) {
	int c0 = EOF,c1 = EOF;

	if ( this->indr_zptr.allocate(sizeof(z_stream)) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	this->indr_zptr.fill(0,sizeof(z_stream));
	zsp = (z_stream *)(this->indr_zptr.ptr());
	
	/* zlib でメモリを管理する */
	zsp->zalloc = (alloc_func)0;
	zsp->zfree = (free_func)0; 
	zsp->opaque = (voidpf)0;
	/* 初期化 */
	zsp->avail_in = 0;
	zsp->next_in = Z_NULL;
	zsp->avail_out = 0;
	zsp->next_out = Z_NULL;

	/* 初期化 */
	if ( inflateInit2(zsp,-MAX_WBITS) != Z_OK ) {
	    err_throw(__FUNCTION__,"FATAL","inflateInit() failed");
	}

	/* magic number をチェック */
	c0 = sref.getchr();
	if ( c0 != EOF ) {
	    c1 = sref.getchr();
	}
	if ( c0 != 0x01f || c1 != 0x08b ) {	/* .gz でない場合 */
	    if ( this->indr_buffer.reallocate(2) < 0 ) {
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
	    if ( c0 != EOF && c1 != EOF ) {
		zsp->next_in = (Bytef *)(this->indr_buffer.ptr());
		zsp->avail_in = 2;
		this->indr_buffer[0] = c0;
		this->indr_buffer[1] = c1;
	    }
	    else if ( c0 != EOF && c1 == EOF ) {
		zsp->next_in = (Bytef *)(this->indr_buffer.ptr());
		zsp->avail_in = 1;
		this->indr_buffer[0] = c0;
	    }
	}
	else {					/* .gz の場合 */
	    int method = 0, flags = 0, i, c = 0;
	    unsigned int len;
	    c = sref.getchr();
	    if ( c != EOF ) {
		method = c;
		c = sref.getchr();
		if ( c != EOF ) {
		    flags = c;
		    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
			err_report(__FUNCTION__,"ERROR","data error");
			goto quit;
		    }
		}
	    }
	    /* Discard time, xflags and OS code: */
	    for ( i=0 ; i < 6 && c != EOF ; i++ ) c = sref.getchr();
	    /* skip the extra field */
	    if ( c != EOF && (flags & EXTRA_FIELD) != 0 ) {
		c = sref.getchr();
		if ( c != EOF ) {
		    len  =  (unsigned int)c;
		    c = sref.getchr();
		    if ( c != EOF ) {
			len += ((unsigned int)c)<<8;
			/* len is garbage if EOF but the loop below will quit 
			   anyway */
			while ( len-- != 0 && (c = sref.getchr()) != EOF ) ;
		    }
		}
	    }
	    /* skip the original file name */
	    if ( c != EOF && (flags & ORIG_NAME) != 0 ) {
		while ( (c = sref.getchr()) != 0 && c != EOF ) ;
	    }
	    /* skip the .gz file comment */
	    if ( c != EOF && (flags & COMMENT) != 0 ) {
		while ( (c = sref.getchr()) != 0 && c != EOF) ;
	    }
	    /* skip the header crc */
	    if ( c != EOF && (flags & HEAD_CRC) != 0 ) {
		for ( i = 0 ; i < 2 && c != EOF ; i++ ) c = sref.getchr();
	    }
	    if ( c == EOF ) {
		err_report(__FUNCTION__,"ERROR","unexpected EOF");
		goto quit;
	    }
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
int gzstreamio::eof()
{
    if ( this->gzfp != NULL ) return gzeof((gzFile)(this->gzfp));
    else return this->cstreamio::eof();
}

/**
 * @brief  エラー指示子の取得
 */
int gzstreamio::error()
{
    if ( this->gzfp != NULL ) {
	int errnum = 0;
	const char *msg;
	msg = gzerror((gzFile)(this->gzfp), &errnum);
	// err_report1(__FUNCTION__,"DEBUG","msg: `%s'",msg);
	if ( errnum < 0 ) return errnum;
	else return 0;
    }
    else return this->cstreamio::error();
}

/**
 * @brief  EOF指示子とエラー指示子のリセット
 */
gzstreamio &gzstreamio::reseterr()
{
    if ( this->gzfp != NULL ) gzclearerr((gzFile)(this->gzfp));
    else this->cstreamio::reseterr();
    return *this;
}


/*
  [citation from zlib.h]
  All pending output is flushed to the output buffer and the output is aligned
  on a byte boundary, so that the decompressor can get all input data available
  so far.  Flushing may degrade compression for some compression algorithms and
  so it should be used only when necessary.
*/
/**
 * @brief  書き込み用ストリームのフラッシュ (バイト境界でアライン)
 *
 *  open()で開いたストリームに，バッファに格納されているすべてのデータを
 *  gzipフォーマットで書き込みます．
 *
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 * @attention  むやみに使うと圧縮率の悪化を招きます．
 */
int gzstreamio::sync()
{
    if ( this->gzfp != NULL )
	return gzflush((gzFile)(this->gzfp), Z_SYNC_FLUSH);
    else if ( this->ind_sp != NULL )
	return this->ind_sp->flush();
    else return -1;

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
/* cstreamio に実装すべきかもしれないが，cstreamio が複雑化するのと， */
/* 継承クラス側の自由度を考え，継承クラス側で実装する                 */
ssize_t gzstreamio::raw_read( void *buf, size_t size )
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
 *  indirect_read() メンバ関数または gzread() 関数を呼び出し，gzip フォーマット
 *  のストリームから最大 size バイトのデータを読み，バッファに書き込みます．
 *
 * @param    buf データの格納場所
 * @param    size 解凍後のデータのバイト数
 * @return   成功した場合は読み込んだデータの解凍後のバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 * @note     このメンバ関数は private です．
 */
ssize_t gzstreamio::_raw_read( void *buf, size_t size )
{
    ssize_t l;
    if ( this->ind_sp != NULL ) {
	return this->indirect_read(buf,size);
    }
    if ( this->gzfp == NULL ) return -1;
    if ( buf == NULL && 0 < size ) return -1;
    /* gzreadで0バイト読んでしまうと挙動不審になる 2012/10/19 */
    if ( size == 0 ) return 0;
    l = gzread( (gzFile)(this->gzfp), buf, size );
    if ( l < 0 ) return -1;
    else return l;
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
ssize_t gzstreamio::raw_write( const void *buf, size_t size )
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
 *  indirect_write() メンバ関数か gzwrite() 関数を呼び出し，与えられたバッファ
 *  から最大 size バイトのデータを読み込み，現在開かれているストリームに書き込
 *  みます．
 *
 * @param      buf データの格納場所
 * @param      size データの圧縮前のバイト数
 * @return     成功した場合は圧縮前のバイト数<br>
 *             エラーの場合は負値
 * @note       このメンバ関数は private です．
 */
ssize_t gzstreamio::_raw_write( const void *buf, size_t size )
{
    ssize_t l;
    if ( this->ind_sp != NULL ) {
	return this->indirect_write(buf,size);
    }
    if ( this->gzfp == NULL ) return -1;
    if ( buf == NULL && 0 < size ) return -1;
    //fprintf(stderr,"debug: size = %ld\n",size);
    /* 長さゼロのんを書こうとするとゴミが入るらしい…orz */
    if ( size == 0 ) return 0;
    l = gzwrite( (gzFile)(this->gzfp), buf, size );
    if ( l < 0 ) return -1;
    else return l;
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
ssize_t gzstreamio::indirect_read( void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = Z_OK;
    z_stream *zsp = (z_stream *)(this->indr_zptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( zsp == NULL ) {
	if ( this->ind_sp != NULL ) return 0;
	goto quit;
    }

    zsp->next_out = (Bytef *)buf;	/* 出力バッファをセット */
    zsp->avail_out = size;

    do {
        if ( zsp->avail_in == 0 ) {	/* 読み終わったら… */
	    ssize_t sz;
	    if ( this->indr_buffer.reallocate(2 * size) < 0 ) {
		this->errflg_rec = 1;
		err_throw(__FUNCTION__,"FATAL","realloc() failed");
	    }
            zsp->next_in = (Bytef *)(this->indr_buffer.ptr());
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
            zsp->avail_in = sz;
        }
        zstatus = inflate(zsp, Z_NO_FLUSH);
        if ( zstatus == Z_STREAM_END ) {
	    this->eofflg_rec = 1;
	    break;
	}
        if ( zstatus != Z_OK ) {
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","inflate() failed, msg: %s",
			zsp->msg);
	    goto quit;
        }
    } while ( 0 < zsp->avail_out );

    return_len = (ssize_t)size - zsp->avail_out;

    if ( zstatus == Z_STREAM_END ) {
	int zs;
	/* 後始末 */
	if ( (zs = inflateEnd(zsp)) != Z_OK ) {
	    this->errflg_rec = zs;
	    err_report1(__FUNCTION__,"ERROR","inflateEnd() failed, msg: %s",
			zsp->msg);
	}
	this->indr_buffer = NULL;
	this->indr_zptr = NULL;
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
ssize_t gzstreamio::indirect_write( const void *buf, size_t size )
{
    ssize_t return_len = -1;
    int zstatus = Z_OK;
    z_stream *zsp = (z_stream *)(this->indw_zptr.ptr());

    if ( buf == NULL && 0 < size ) goto quit;
    if ( size == 0 ) return 0;
    if ( zsp == NULL ) {
	goto quit;
    }

    zsp->next_in = (Bytef *)buf;	/* 入力バッファをセット */
    zsp->avail_in = size;

    while ( 0 < zsp->avail_in ) {	/* 入力が消費されるまで */
        this->indw_in += zsp->avail_in;
	zstatus = deflate(zsp,Z_NO_FLUSH);
        this->indw_in -= zsp->avail_in;
        if ( zstatus != Z_OK ) {
	    this->errflg_rec = zstatus;
	    err_report1(__FUNCTION__,"ERROR","deflate() failed, msg: %s",
			zsp->msg);
	    goto quit;
        }
        if ( zsp->avail_out == 0 ) {	/* 満ちたら… */
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
            zsp->next_out = (Bytef *)(this->indw_buffer.ptr());
            zsp->avail_out = this->indw_buffer_size;
        }
    }

    this->indw_crc = crc32(this->indw_crc, (const Bytef *)buf, size);

    return_len = size;
 quit:
    return return_len;
}

}	/* namespace sli */


#include "private/c_close.c"
#include "private/c_dup.c"
#include "private/c_strncmp.cc"

#endif	/* #ifdef HAVE_LIBZ */
