/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:31:18 cyamauch> */

/**
 * @file   ftpstreamio.cc
 * @brief  FTPサーバに接続しストリーム入出力を行なう ftpstreamio クラスのコード
 */

#define CLASS_NAME "ftpstreamio"

#include "config.h"

#include "ftpstreamio.h"
#include "inetstreamio.h"
#ifdef HAVE_LIBZ
#include "gzstreamio.h"
#endif
#ifdef HAVE_LIBBZ2
#include "bzstreamio.h"
#endif
#include "tstring.h"

#include <assert.h>

#include "private/err_report.h"


static void get_user_at_host( sli::tstring *ret );

#include "private/c_gethostname.h"

#include "private/c_fdopen.h"
#include "private/c_fclose.h"

#include "private/c_strcpy.h"
#include "private/c_strncmp.h"

#include "private/c_geteuid.h"
#include "private/c_dup.h"
#include "private/c_close.h"

#include "private/suffix2filetype.h"


namespace sli
{

/**
 * @brief  コンストラクタ
 */
ftpstreamio::ftpstreamio()
{
    this->inet_sp = NULL;
    this->rw_sp = NULL;
    this->filter_sp = NULL;
    this->content_length_rec = -1;
    return;
}

/**
 * @brief  デストラクタ
 */
ftpstreamio::~ftpstreamio()
{
    this->close();
    return;
}

/**
 * @brief  ftp:// から始まる URL のオープン
 *
 *  パスを指定して，URLをオープンします．<br>
 *  mode に "r%" または "w%" を指定すると，ファイル名のサフィックスにより，
 *  自動的に伸長・圧縮を行ないます．
 *
 * @param      mode URLを開くモード ("r", "r%", "w" or "w%")
 * @param      path URLのパス
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      メモリの割り当てに失敗した場合 
 */
int ftpstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;

    /* "r%" を指定する事で，圧縮ストリームの展開に対応 */
    if ( c_strncmp(mode,"r%",2) == 0 || c_strncmp(mode,"rb%",3) == 0 ) {
        return_status = this->open_rw(mode,path,true);
    } else if ( c_strncmp(mode,"w%",2) == 0 || c_strncmp(mode,"wb%",3) == 0 ) {
        return_status = this->open_rw(mode,path,true);
    } else if ( c_strncmp(mode,"r",1) == 0 || c_strncmp(mode,"rb",2) == 0 ) {
        return_status = this->open_rw(mode,path,false);
    } else if ( c_strncmp(mode,"w",1) == 0 || c_strncmp(mode,"wb",2) == 0 ) {
        return_status = this->open_rw(mode,path,false);
    } else {
        return return_status = -1;
    }

    return return_status;
}

/**
 * @brief  FTP応答コードを返す
 *
 * @param      isp inetstreamioクラスのインスタンス
 * @param      rline tstringクラスのインスタンス
 * @return     成功した場合は400未満の3桁の整数<br>
 *             失敗した場合は400以上の整数
 * @note       このメンバ関数は private です．
 */
int ftpstreamio::read_response( inetstreamio *isp, tstring &rline )
{
    assert( isp != NULL );

    while ( 1 ) {
	const char *l;
	if ( (l=isp->getline()) == NULL ) {
	    rline.assign("999 Connection broken");
	    return 999;
	}
	rline.assign(l);
	if ( rline.strspn("[0-9]") == 3 ) {
	    if ( rline.cchr(3) == ' ' ) {
		//err_report1("DEBUG","DEBUG","[%s]\n",rline.cstr());
		break;
	    }
	}
    }
    return rline.atoi(0,3);
}

/**
 * @brief  URLのオープン (低レベル)
 *
 * @param      mode URLを開くモード ("r", "r%", "w" or "w%")
 * @param      path URLのパス
 * @param      use_filter_by_suffix 圧縮ストリーム展開指定の有無
 * @return     成功した場合は0<br>
 *             エラーの場合は負値
 * @throw      メモリの割り当てに失敗した場合 
 * @note       このメンバ関数は protected です．
 */
int ftpstreamio::open_rw( const char *mode, const char *path,
			  bool use_filter_by_suffix )
{
    int return_status = -1;
    inetstreamio *isp = NULL;	/* inetstreamio (message) */
    inetstreamio *rwsp = NULL;	/* inetstreamio (stream)  */
    cstreamio *dsp = NULL;	/* gzstreamio or bzstreamio */
    char rw_mode[2] = { 'r', '\0' };

    if ( mode == NULL ) goto quit;
    if ( path == NULL ) goto quit;
    if ( c_strncmp(path,"ftp://",6) != 0 ) goto quit;
    if ( this->inet_sp != NULL ) goto quit;
    if ( this->rw_sp != NULL ) goto quit;

    if ( mode[0] == 'w' ) rw_mode[0] = 'w';

    try {
	bool path_is_gzip = false, path_is_bzip2 = false;
	int ip0,ip1,ip2,ip3,port;
	tstring t_path;
	tstring user_pass;
	ssize_t pos;
	size_t len;
	tstring user;
	tstring pass;
	tstring ans;

	isp = new inetstreamio;
	rwsp = new inetstreamio;

	t_path.assign(path);

	/* ftp://username:password@ftp.com/... でいけるようにする */
	user_pass.assign(path);
	user_pass.erase(0,6);
	pos = user_pass.regmatch("^[^:][^:]*:[^@]*@",&len);
	if ( pos == 0 ) {
	    t_path.erase(6,len);
	    user_pass.crop(pos,len-1);
	}

	/* FTP ユーザ名 */
	if ( pos == 0 ) {
	    user.assign(user_pass,0,user_pass.strchr(':'));
	}
	else if ( 0 < this->username_rec.length() &&
		  this->username_rec.strpbrk("[[:cntrl:]]") < 0 ) {
	    user.assign(this->username_rec.cstr());
	}
	else {
	    user.assign("anonymous");
	}

	/* FTP パスワード */
	if ( pos == 0 ) {
	    pass.assign(user_pass,user_pass.strchr(':')+1);
	}
	else if ( 0 < this->password_rec.length() &&
		  this->password_rec.strpbrk("[[:cntrl:]]") < 0 ) {
	    pass.assign(this->password_rec.cstr());
	}
	else {
	    get_user_at_host(&pass);
	}

	//err_report1(__FUNCTION__,"DEBUG","user : %s",user.cstr());
	//err_report1(__FUNCTION__,"DEBUG","pass : %s",pass.cstr());
	//err_report1(__FUNCTION__,"DEBUG","url  : %s",t_path.cstr());

	suffix2filetype(t_path.cstr(),&path_is_gzip,&path_is_bzip2);

#ifndef HAVE_LIBZ
	path_is_gzip = false;
#endif
#ifndef HAVE_LIBBZ2
	path_is_bzip2 = false;
#endif

	if ( isp->open("r+",t_path.cstr()) < 0 ) {
	    err_report1(__FUNCTION__,"ERROR","cannot open url: %s",t_path.cstr());
	    goto quit;
	}
	
	/* はじめのメッセージを受信 */
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}
	
	/* ユーザ */
	isp->printf("USER %s\n",user.cstr());
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	/* パスワード */
	isp->printf("PASS %s\n",pass.cstr());
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	/* バイナリ */
	isp->printf("TYPE I\n");
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	/* ファイルサイズ */
	if ( rw_mode[0] == 'r' ) {
	    long long file_size;
	    isp->printf("SIZE %s\n",isp->path());
	    isp->flush();
	    if ( 400 <= this->read_response(isp,ans) ) {
		err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",
			    ans.strtrim().cstr());
		goto quit;
	    }
	    file_size = ans.atoll(4);
	    if ( 0 <= file_size ) this->content_length_rec = file_size;
	}
	
	/* パッシブモード */
	isp->printf("PASV\n");
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}
	pos = ans.regmatch("[0-9][0-9]*,[0-9][0-9]*,"
			   "[0-9][0-9]*,[0-9][0-9]*,"
			   "[0-9][0-9]*,[0-9][0-9]*", NULL);
	if ( pos < 0 ) {
	    err_report1(__FUNCTION__,"ERROR","Invalid server response : [%s]",
			ans.strtrim("\r\n").cstr());
	    goto quit;
	}
	ip0 = ans.atoi(pos);
	pos += ans.strspn(pos,"[0-9]") + 1;
	ip1 = ans.atoi(pos);
	pos += ans.strspn(pos,"[0-9]") + 1;
	ip2 = ans.atoi(pos);
	pos += ans.strspn(pos,"[0-9]") + 1;
	ip3 = ans.atoi(pos);
	pos += ans.strspn(pos,"[0-9]") + 1;
	port = ans.atoi(pos) * 256;
	pos += ans.strspn(pos,"[0-9]") + 1;
	port += ans.atoi(pos);

	if ( rwsp->openf(rw_mode,"ftp://%d.%d.%d.%d:%d/",
			 ip0,ip1,ip2,ip3,port) < 0 ) {
	    err_report1(__FUNCTION__,"ERROR","cannot open port: %d",port);
	    goto quit;
	}

	/* 受信 or 送信コマンド */
	if ( rw_mode[0] == 'r' ) {
	    isp->printf("RETR %s\n",isp->path());
	}
	else {
	    isp->printf("STOR %s\n",isp->path());
	}
	isp->flush();

	/* xxx Opening ... */
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	if ( use_filter_by_suffix != false &&
	     ( path_is_gzip != false || path_is_bzip2 != false ) ) {
	    if ( path_is_gzip != false ) {
#ifdef HAVE_LIBZ
		dsp = new gzstreamio;
#endif
	    }
	    else if ( path_is_bzip2 != false ) {
#ifdef HAVE_LIBBZ2
		dsp = new bzstreamio;
#endif
	    }
	    if ( dsp != NULL ) {
	      if ( dsp->open(rw_mode,*rwsp) < 0 ) {
		err_report(__FUNCTION__,"ERROR","dsp->open() failed");
		goto quit;
	      }
	    }
	    else {
	      /* never */
	    }
	}

    }
    catch ( ... ) {
	if ( dsp != NULL ) delete dsp;
	if ( rwsp != NULL ) delete rwsp;
	if ( isp != NULL ) delete isp;
	err_throw(__FUNCTION__,"FATAL","Memory allocation failed");
    }

    return_status = 0;
 quit:
    if ( return_status < 0 ) {
	if ( dsp != NULL ) {
	    dsp->close();
	    delete dsp;
	}
	if ( rwsp != NULL ) {
	    rwsp->close();
	    delete rwsp;
	}
	if ( isp != NULL ) {
	    isp->close();
	    delete isp;
	}
	this->content_length_rec = -1;
    }
    else {
	if ( isp != NULL ) this->inet_sp = isp;
	if ( rwsp != NULL ) this->rw_sp = rwsp;
	if ( dsp != NULL ) this->filter_sp = dsp;
    }
    return return_status;
}

/**
 * @brief  ストリームのバイト長を取得
 *
 *  FTP サーバかか SIZE コマンドで取得した値をそのまま返します．
 *
 * @return     非負値: SIZE コマンドで取得した値<br>
 *             負値: SIZE コマンドに失敗した場合
 */
long long ftpstreamio::content_length() const
{
    return this->content_length_rec;
}

/**
 * @brief  ユーザー名設定用の文字列オブジェクトを返す
 *
 * @return  文字列オブジェクト(tstring)の参照
 */
tstring &ftpstreamio::username()
{
    return this->username_rec;
}

/**
 * @brief  パスワード設定用の文字列オブジェクトを返す
 *
 * @return  文字列オブジェクト(tstring)の参照
 */
tstring &ftpstreamio::password()
{
    return this->password_rec;
}

/**
 * @brief  標準入出力のオープン
 *
 *  標準入出力をオープンします．<br>
 *  mode に "r" が指定された場合は標準入力を，"w" が指定された場合は標準出力を
 *  オープンします.
 *
 * @deprecated  使用する意味はありません．<br>
 *              このメンバ関数では，パスを指定して，URLをオープンする事は
 *              できません．
 * @param     mode ストリームを開くモード ("r" or "w")
 * @return    成功した場合は0<br>
 *            エラーの場合は負値
 * @throw     err_rec 標準入出力のファイル・ディスクリプタの複製に失敗した場合
 * @note      実装上の理由で作成されたメンバ関数です．
 */
int ftpstreamio::open( const char *mode )
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
 *              このメンバ関数では，パスを指定して，URLをオープンする事は
 *              できません．
 * @param       mode ストリームを開くモード ("r" or "w")
 * @param       fd   ファイルディスクリプタ
 * @return      成功した場合は0<br>
 *              エラーの場合は負値
 * @note        実装上の理由で作成されたメンバ関数です．
 */
int ftpstreamio::open( const char *mode, int fd )
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
 * @return     成功した場合は0<br>
 *             エラーの場合は0以外の値
 */
int ftpstreamio::close()
{
    int return_status = 0;
    if ( this->filter_sp != NULL ) {
	return_status = this->filter_sp->close();
	delete this->filter_sp;
	this->filter_sp = NULL;
    }
    if ( this->rw_sp != NULL ) {
	int status;
	status = this->rw_sp->close();
	delete this->rw_sp;
	this->rw_sp = NULL;
	if ( status != 0 ) return_status = status;
    }
    if ( this->inet_sp != NULL ) {
	tstring ans;
	/* xxx Transfer complete. */
	if ( 400 <= this->read_response(this->inet_sp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    return_status = EOF;
	}
	this->inet_sp->printf("QUIT\n");
	this->inet_sp->flush();
	if ( 400 <= this->read_response(this->inet_sp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    return_status = EOF;
	}
    }
    if ( this->inet_sp != NULL ) {
	int status;
	status = this->inet_sp->close();
	delete this->inet_sp;
	this->inet_sp = NULL;
	if ( status != 0 ) return_status = status;
    }
    this->content_length_rec = -1;

    if ( this->fp != NULL ) {
	int status;
        status = c_fclose(this->fp);
        this->fp = NULL;
	if ( status != 0 ) return_status = status;
    }

    this->cstreamio::close();

    return return_status;
}

/**
 * @brief  書き込み用ストリームのフラッシュ
 *
 *  ストリームに，バッファに格納されているすべてのデータを書き込みます．
 *
 */
int ftpstreamio::flush()
{
    if ( this->filter_sp != NULL ) {
        return this->filter_sp->flush();
    }
    else if (this->rw_sp != NULL) {
        return this->rw_sp->flush();
    }
    else return this->cstreamio::flush();
}

/**
 * @brief  EOF指示子の取得
 */
int ftpstreamio::eof()
{
    if ( this->filter_sp != NULL ) {
        return this->filter_sp->eof();
    }
    else if (this->rw_sp != NULL) {
        return this->rw_sp->eof();
    }
    else return this->cstreamio::eof();
}

/**
 * @brief  エラー指示子の取得
 */
int ftpstreamio::error()
{
    if ( this->filter_sp != NULL ) {
        return this->filter_sp->error();
    }
    else if (this->rw_sp != NULL) {
        return this->rw_sp->error();
    }
    else return this->cstreamio::error();
}

/**
 * @brief  EOF指示子とエラー指示子のリセット
 */
ftpstreamio &ftpstreamio::reseterr()
{
    if ( this->filter_sp != NULL ) {
        this->filter_sp->reseterr();
    }
    else if (this->rw_sp != NULL) {
        this->rw_sp->reseterr();
    }
    else this->cstreamio::reseterr();
    return *this;
}


/**
 * @brief  読み込み用原始関数のスロット (このクラスにてオーバライド)
 *
 * @param    buf データの格納場所
 * @param    size データの個数
 * @return   成功した場合はバイト数<br>
 *           ストリームが終りの場合，0バイトの読み込みが指定された場合は0<br>
 *           エラーの場合は負値
 * @note     このメンバ関数は protected です．
 */
ssize_t ftpstreamio::raw_read( void *buf, size_t size )
{
    if ( this->filter_sp != NULL ) {
	return this->filter_sp->read(buf,size);
    }
    else if ( this->rw_sp != NULL ) {
	return this->rw_sp->read(buf,size);
    }
    else return -1;
}

/**
 * @brief  書き込み用原始関数のスロット (このクラスにてオーバライド)
 *
 * @param      buf データの格納場所
 * @param      size データの個数
 * @return     成功した場合はバイト数<br>
 *             エラーの場合は負値
 * @note       このメンバ関数は protected です．
 */
ssize_t ftpstreamio::raw_write( const void *buf, size_t size )
{
    if ( this->filter_sp != NULL ) {
	return this->filter_sp->write(buf,size);
    }
    else if ( this->rw_sp != NULL ) {
	return this->rw_sp->write(buf,size);
    }
    else return -1;
}

}	/* namespace sli */


#include <pwd.h>

/**
 * @brief  現在のマシンからユーザ名を得る (内部用)
 *
 * @note   private な関数です．
 */
static void get_user_at_host( sli::tstring *ret )
{
    char host_name[256];
    struct passwd *user_info;
    const char *user_name = NULL;
    /* ホスト名 */
    if ( c_gethostname(host_name,256) != 0 ) {
	c_strcpy(host_name,"VeryLongHostname");
    }
    host_name[255] = '\0';
    /* ユーザ名 */
    user_info = getpwuid(c_geteuid());
    if ( user_info != NULL ) {
	user_name = user_info->pw_name;
    }
    if ( user_name == NULL ) user_name = "unknown";
    /* */
    ret->assign(user_name).append("@").append(host_name);

    return;
}

#include "private/c_gethostname.c"

#include "private/c_fdopen.c"
#include "private/c_fclose.c"

#include "private/c_strcpy.c"
#include "private/c_strncmp.cc"

#include "private/c_geteuid.c"
#include "private/c_dup.c"
#include "private/c_close.c"

#include "private/suffix2filetype.c"
