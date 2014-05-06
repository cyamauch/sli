/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:41:42 cyamauch> */

/**
 * @file   digeststreamio.cc
 * @brief  �̾���̥��ȥ꡼�������б�������ǽ�ʥ��饹 digeststreamio �Υ�����
 */

#define CLASS_NAME "digeststreamio"

#include "config.h"

#include "digeststreamio.h"
#ifdef HAVE_LIBZ
#include "gzstreamio.h"
#endif
#ifdef HAVE_LIBBZ2
#include "bzstreamio.h"
#endif
#include "httpstreamio.h"
#include "ftpstreamio.h"
#include "pipestreamio.h"

#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include "tstring.h"
#include "tarray_tstring.h"

#include "private/err_report.h"

#include "private/c_fdopen.h"
#include "private/c_fopen.h"
#include "private/c_fclose.h"

#include "private/c_strcmp.h"
#include "private/c_strncmp.h"
#include "private/c_dup.h"
#include "private/c_close.h"

#include "private/suffix2filetype.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

namespace sli
{

/*
 * �ޥ������å� compressor �ط�
 */

static bool Pigz_flag = true;
static char Pigz_cmd[PATH_MAX] = {'\0','\0'};
static bool Lbzip2_flag = true;
static char Lbzip2_cmd[PATH_MAX] = {'\0','\0'};

/**
 * @brief  pigz �μ�ưŪ�ʻ��Ѥ�ͭ����
 *
 */
void pigz_auto()
{
    Pigz_flag = true;
    return;
}

/**
 * @brief  pigz �λ��Ѥ�̵����
 *
 */
void pigz_disabled()
{
    Pigz_flag = false;
    return;
}

/**
 * @brief  lbzip2 �μ�ưŪ�ʻ��Ѥ�ͭ����
 *
 */
void lbzip2_auto()
{
    Lbzip2_flag = true;
    return;
}

/**
 * @brief  lbzip2 �λ��Ѥ�̵����
 *
 */
void lbzip2_disabled()
{
    Lbzip2_flag = false;
    return;
}


/*
 * gzstreamio �Υ��дؿ�
 */

/**
 * @brief  ���󥹥ȥ饯��
 */
digeststreamio::digeststreamio()
{
    this->isp = NULL;
    this->is_httpstream = false;
    this->is_ftpstream = false;
    this->is_write_mode_rec = false;
    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 */
digeststreamio::~digeststreamio()
{
    this->close();
    return;
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ�.<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ�URL�ޤ��ϥե������ɬ�פ˱�����
 *              gzip�ޤ���bzip2���̡���Ĺ��Ԥʤ��ʤ��饹�ȥ꡼���
 *              �����ץ󤹤���ϤǤ��ޤ���
 * @param     mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return    ������������0<br>
 *            ���顼�ξ�������
 * @throw     err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 * @note      ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int digeststreamio::open( const char *mode )
{
    int ret;
    assert( mode != NULL );

    if ( this->fp != NULL || this->isp != NULL ) return -1;

    ret = this->cstreamio::open(mode);

    if ( c_strncmp(mode,"w",1) == 0 ) this->is_write_mode_rec = true;
    else this->is_write_mode_rec = false;

    return ret;
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ�URL�ޤ��ϥե������ɬ�פ˱�����
 *              gzip�ޤ���bzip2���̡���Ĺ��Ԥʤ��ʤ��饹�ȥ꡼���
 *              �����ץ󤹤���ϤǤ��ޤ���
 * @param       mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 * @note        ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int digeststreamio::open( const char *mode, int fd )
{
    assert( mode != NULL );

    int return_status = -1;
    int fd_new = -1;
    struct stat st;

    if ( this->fp != NULL || this->isp != NULL ) goto quit;

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

    if ( fstat(fd_new, &st) == 0 ) {
	if ( S_ISREG(st.st_mode) ) this->fp_seekable_rec = true;
    }

    if ( c_strncmp(mode,"w",1) == 0 ) this->is_write_mode_rec = true;
    else this->is_write_mode_rec = false;

    return_status = 0;
 quit:
    if ( return_status != 0 && 0 <= fd_new ) c_close(fd_new);
    return return_status;
}

/**
 * @brief  �ե�����ޤ���URL�Υ����ץ�
 *
 *  path�Ǽ����줿�ե�����ޤ���URL�򥪡��ץ󤷤ޤ���
 *
 * @param   mode �ե�����ޤ���URL�򳫤��⡼�� ("r" or "w")
 * @param   path �ե�����ޤ���URL��
 *          "" �ޤ��� "-" �ξ�硤ɸ�����Ϥޤ���ɸ����Ϥ���³
 * @return  ������������0<br>
 *          ���顼�ξ�������
 * @throw   ���󥹥��󥹤������˼��Ԥ������
 */
int digeststreamio::open( const char *mode, const char *path )
{
    if ( mode == NULL ) return -1;
    if ( mode[0] != 'r' && mode[0] != 'w' ) return -1;
    if ( mode[1] != '\0' ) return -1;
    if ( path == NULL ) return this->open(mode);
    if ( path[0] == '\0' ) return this->open(mode);
    if ( path[0] == '-' && path[1] == '\0' ) return this->open(mode);
    return this->open_rw(mode,path);
}

/**
 * @brief  �ե����롤URL���ѥ��פΥ����ץ�
 *
 *  path���ե�����ޤ���URL�򼨤��Ƥ�����ϡ������򥪡��ץ󤷤ޤ���<br>
 *  path�����ޥ�ɤ򼨤��Ƥ�����ϡ������¹Ԥ����ѥ��פ���³���ޤ���
 *
 * @param      path  �ե����롤URL�ޤ��ϥ��ޥ��
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ���󥹥��󥹤������˼��Ԥ������
 */
int digeststreamio::openp( const char *path )
{
    if ( path == NULL ) return this->open("r");
    return this->open_rw(NULL,path);
}

/**
 * @brief  �ե����롤URL���ѥ��פΥ����ץ�
 *
 *  path_fmt���ե�����ޤ���URL�򼨤��Ƥ�����ϡ������򥪡��ץ󤷤ޤ���<br>
 *  path_fmt�����ޥ�ɤ򼨤��Ƥ�����ϡ������¹Ԥ����ѥ��פ���³���ޤ���
 *
 * @param      path_fmt �ե����롤URL�ޤ��ϥ��ޥ�ɤΥե����ޥåȻ���
 * @param      ... �ե����롤URL�ޤ��ϥ��ޥ�ɤγ����ǥǡ���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ���󥹥��󥹤������˼��Ԥ������
 */
int digeststreamio::openpf( const char *path_fmt, ... )
{
    assert( path_fmt != NULL );

    int ret;
    va_list ap;
    va_start(ap,path_fmt);
    try {
        ret = this->vopenpf(path_fmt,ap);
    }
    catch (...) {
        va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vopenpf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  �ե����롤URL���ѥ��פΥ����ץ�
 *
 *  path_fmt���ե�����ޤ���URL�򼨤��Ƥ�����ϡ������򥪡��ץ󤷤ޤ���<br>
 *  path_fmt�����ޥ�ɤ򼨤��Ƥ�����ϡ������¹Ԥ����ѥ��פ���³���ޤ���
 *
 * @param      path_fmt �ե����롤URL�ޤ��ϥ��ޥ�ɤΥե����ޥåȻ���
 *             ap �ե����롤URL�ޤ��ϥ��ޥ�ɤ������ǥǡ���
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ���󥹥��󥹤������˼��Ԥ������
 */
int digeststreamio::vopenpf( const char *path_fmt, va_list ap )
{
    assert( path_fmt != NULL );

    tstring buf;
    if ( path_fmt == NULL ) return this->openp(path_fmt);
    buf.vprintf(path_fmt,ap);
    return this->openp(buf.cstr());
}

/**
 * @brief  �⡼��(�񤭹��ߤ��ɤ߹��ߤ�)�μ���
 *
 *  �����ץ󤵤줿���ȥ꡼�ब�񤭹��ߥ⡼�ɤ��ɤ������֤��ޤ���
 *
 * @return     �񤭹��ߥ⡼�ɤξ��� true<br>
 *             �ɤ߹��ߥ⡼�ɤξ��� false
 */
bool digeststreamio::is_write_mode() const
{
    return this->is_write_mode_rec;
}

/**
 * @brief  ���ȥ꡼��ΥХ���Ĺ�μ���
 *
 *  ���ȥ꡼��ΥХ���Ĺ���֤��ޤ���
 *
 * @return     ������������������<br>
 *             ���Ԥ�����������
 */
long long digeststreamio::content_length() const
{
    if ( this->isp != NULL ) {
	if ( this->is_httpstream == true ) {
	    httpstreamio *sp = (httpstreamio *)(this->isp);
	    return sp->content_length();
	}
	else if ( this->is_ftpstream == true ) {
	    ftpstreamio *sp = (ftpstreamio *)(this->isp);
	    return sp->content_length();
	}
    }
    return -1;
}

/**
 * @brief  �桼��������������������Ѥ�ʸ���󥪥֥�������
 *
 *  �桼��������������������Ѥ�ʸ���󥪥֥������Ȥ��֤��ޤ���
 *
 * @return     ʸ���󥪥֥������Ȥλ���
 */
tstring &digeststreamio::user_agent()
{
    return this->user_agent_rec;
}

/**
 * @brief  �桼����̾�����Ѥ�ʸ���󥪥֥�������
 *
 *  �桼����̾�����Ѥ�ʸ���󥪥֥������Ȥ��֤��ޤ���
 *
 * @return     ʸ���󥪥֥������Ȥλ���
 */
tstring &digeststreamio::username()
{
    return this->username_rec;
}

/**
 * @brief  �ѥ���������Ѥ�ʸ���󥪥֥�������
 *
 *  �ѥ���������Ѥ�ʸ���󥪥֥������Ȥ��֤��ޤ���
 *
 * @return     ʸ���󥪥֥������Ȥλ���
 */
tstring &digeststreamio::password()
{
    return this->password_rec;
}

/**
 * @brief  ���ȥ꡼��Υ����ץ�(���٥�)
 *
 * path���ե�����ޤ���URL�򼨤��Ƥ�����ϡ������򥪡��ץ󤷤ޤ���<br>
 * mode��NULL�ǡ�path�����ޥ�ɤ򼨤��Ƥ�����ϡ������¹Ԥ���<br>
 * �ѥ��פ���³���ޤ���
 *
 * @param      mode ���ȥ꡼��򳫤��⡼��
 * @param      path �ե����롤URL�ޤ��ϥ��ޥ��
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ���󥹥��󥹤������˼��Ԥ������
 * @note       ���Υ��дؿ���protected�Ǥ���
 *             �桼����ľ�����Ѥ�����ϤǤ��ޤ���
 */
int digeststreamio::open_rw( const char *mode, const char *path )
{
    tstring path_str;
    int return_status = -1;
    char mode_str[3] = {'r','\0','\0'};

    if ( this->fp != NULL || this->isp != NULL ) goto quit;
    if ( path == NULL ) return this->open(mode);

    path_str.assign(path).strtrim();

    /* mode �� NULL �ˤ���ȡ�perl �Τ褦�ʵ�ư�ˤʤ� */
    if ( mode == NULL ) {
	if ( path_str.cchr(0) == '>' ) {
	    mode_str[0] = 'w';
	    path_str.erase(0,1).strtrim();
	}
	else if ( path_str.cchr(0) == '<' ) {
	    mode_str[0] = 'r';
	    path_str.erase(0,1).strtrim();
	}
	else if ( path_str.cchr(0) == '|' ) {
	    mode_str[0] = 'w';
	}
	else if ( path_str.cchr(path_str.length()-1) == '|' &&
		  path_str.cchr(path_str.length()-2) != '\\' ) { 
	    mode_str[0] = 'r';
	}
    }
    else {
	mode_str[0] = mode[0];
    }

    if ( path_str.strncmp("http://",7) == 0 ) {
	/* "r%" �ǰ��̥��ȥ꡼���Ÿ�����б� */ 
	httpstreamio *sp;
	mode_str[1] = '%';
	try {
	    sp = new httpstreamio;
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","`new' failed");
	}
	this->isp = sp;
	if ( this->user_agent_rec.cstr() != NULL ) {
	    sp->user_agent().assign(this->user_agent_rec);
	}
	if ( sp->open(mode_str,path_str.cstr()) < 0 ) {
	    delete sp;
	    this->isp = NULL;
	    err_report(__FUNCTION__,"ERROR","sp->open() failed");
	    goto quit;
	}
	this->is_httpstream = true;
    }
    else if ( path_str.strncmp("ftp://",6) == 0 ) {
	/* "r%" �ǰ��̥��ȥ꡼���Ÿ�����б� */
	ftpstreamio *sp;
	mode_str[1] = '%';
	try {
	    sp = new ftpstreamio;
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","`new' failed");
	}
	this->isp = sp;
	if ( this->username_rec.cstr() != NULL ) {
	    sp->username().assign(this->username_rec);
	}
	if ( this->password_rec.cstr() != NULL ) {
	    sp->password().assign(this->password_rec);
	}
	if ( sp->open(mode_str,path_str.cstr()) < 0 ) {
	    delete sp;
	    this->isp = NULL;
	    err_report(__FUNCTION__,"ERROR","sp->open() failed");
	    goto quit;
	}
	this->is_ftpstream = true;
    }
    else if ( mode == NULL && mode_str[0] == 'w' &&
	      path_str.cchr(0) == '|' ) {
	pipestreamio *sp;
	path_str.erase(0,1).strtrim();
	try {
	    sp = new pipestreamio;
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","`new' failed");
	}
	this->isp = sp;
	if ( sp->open(mode_str,path_str.cstr()) < 0 ) {
	    delete sp;
	    this->isp = NULL;
	    err_report(__FUNCTION__,"ERROR","sp->open() failed");
	    goto quit;
	}
    }
    else if ( mode == NULL && mode_str[0] == 'r' &&
	      path_str.cchr(path_str.length()-1) == '|' &&
	      path_str.cchr(path_str.length()-2) != '\\' ) {
	pipestreamio *sp;
	path_str.erase(path_str.length()-1,1).strtrim();
	try {
	    sp = new pipestreamio;
	}
	catch ( ... ) {
	    err_throw(__FUNCTION__,"FATAL","`new' failed");
	}
	this->isp = sp;
	if ( sp->open(mode_str,path_str.cstr()) < 0 ) {
	    delete sp;
	    this->isp = NULL;
	    err_report(__FUNCTION__,"ERROR","sp->open() failed");
	    goto quit;
	}
    }
    else {
	bool path_is_gzip = false, path_is_bzip2 = false;
	if ( path_str.strncmp("file://",7) == 0 ) path_str.erase(0,7);
	else if ( path_str.strncmp("file:",5) == 0 ) path_str.erase(0,5);
	suffix2filetype(path_str.cstr(),&path_is_gzip,&path_is_bzip2);
#ifndef HAVE_LIBZ
	path_is_gzip = false;
#endif
#ifndef HAVE_LIBBZ2
	path_is_bzip2 = false;
#endif
	if ( path_is_gzip == false && path_is_bzip2 == false ) {
	    struct stat st;
	    /*
	     * local normal file support
	     */
	    this->fp = c_fopen(path_str.cstr(),mode_str);
	    if ( this->fp == NULL ) {
		err_report(__FUNCTION__,"ERROR","fopen() failed");
		goto quit;
	    }
	    /* for rskip() and wskip() */
	    if ( stat(path_str.cstr(),&st) == 0 ) {
		if ( S_ISREG(st.st_mode) ) this->fp_seekable_rec = true;
	    }
	}
	else {
	    cstreamio *sp0 = NULL;
	    cstreamio *sp = NULL;
	    tstring path_str0 = path_str;
	    /*
	     * local GZ file support
	     */
	    if ( path_is_gzip != false ) {
		bool file_rw_ok = true;
		/* confirm file */
		if ( c_strcmp(mode_str,"w") == 0 || 
		     c_strcmp(mode_str,"wb") == 0 ) {
		    void *c_fp;
		    c_fp = c_fopen(path_str.cstr(), "w");
		    if ( c_fp == NULL ) file_rw_ok = false;
		    else c_fclose(c_fp);
		}
		else if ( c_strcmp(mode_str,"r") == 0 || 
			  c_strcmp(mode_str,"rb") == 0 ) {
		    void *c_fp;
		    c_fp = c_fopen(path_str.cstr(), "r");
		    if ( c_fp == NULL ) file_rw_ok = false;
		    else c_fclose(c_fp);
		}
		else file_rw_ok = false;
		/* support of "pigz" multi-thread compressor */
		if ( file_rw_ok == true && Pigz_flag == true && 
		     Pigz_cmd[0] == '\0' && Pigz_cmd[1] == '\0' ) {
		    tstring env_use_pigz;
		    /* check SLI_PIGZ env variable */
		    env_use_pigz = getenv("SLI_PIGZ");
		    if ( env_use_pigz.strcmp("0") != 0 ) {
			tstring env_path;
			tarray_tstring paths;
			/* find pigz */
			env_path = getenv("PATH");
			if ( 0 < env_path.length() ) {
			    struct stat st;
			    size_t i;
			    paths.split(env_path,":",true);
			    for ( i=0 ; i < paths.length() ; i++ ) {
				if ( 0 < paths[i].length() ) {
				    paths[i].append("/pigz");
				    if ( stat(paths[i].cstr(),&st) == 0 &&
					 (st.st_mode & S_IROTH) != 0 &&
					 (st.st_mode & S_IXOTH) != 0 ) {
					paths[i].getstr(Pigz_cmd,PATH_MAX);
					break;
				    }
				}
			    }
			    /* flag */
			    if ( i == paths.length() ) Pigz_cmd[1] = 'F';
			}
		    }
		}
		if ( Pigz_cmd[0] != '\0' ) {
		    tstring env_pigz_r_args, env_pigz_w_args;
		    /* check env variables */
		    env_pigz_r_args = getenv("SLI_PIGZ_R_ARGS");
		    env_pigz_w_args = getenv("SLI_PIGZ_W_ARGS");
		    /* determine pigz command */
		    if ( mode_str[0] == 'w' ) {			/* write */
			if ( 0 < env_pigz_w_args.length() ) {
			    path_str0.printf("%s -c %s > %s",
				      Pigz_cmd, 
				      env_pigz_w_args.cstr(), path_str.cstr());
			}
			else {
			    path_str0.printf("%s -c > %s", 
					      Pigz_cmd, path_str.cstr());
			}
		    }
		    else {					/* read */
			if ( 0 < env_pigz_r_args.length() ) {
			    path_str0.printf("%s -dc %s %s",
				      Pigz_cmd,
				      env_pigz_r_args.cstr(), path_str.cstr());
			}
			else {
			    path_str0.printf("%s -dc %s", 
					      Pigz_cmd, path_str.cstr());
			}
		    }
		    try {
			sp0 = new pipestreamio;
		    }
		    catch ( ... ) {
			err_throw(__FUNCTION__,"FATAL","`new' failed");
		    }
		}
		/* use zlib */
#ifdef HAVE_LIBZ
		try {
		    sp = new gzstreamio;
		}
		catch ( ... ) {
		    if ( sp0 != NULL ) delete sp0;
		    err_throw(__FUNCTION__,"FATAL","`new' failed");
		}
#endif
	    }
	    /*
	     * local BZIP2 file support
	     */
	    else if ( path_is_bzip2 != false ) {
		bool file_rw_ok = true;
		/* confirm file */
		if ( c_strcmp(mode_str,"w") == 0 || 
		     c_strcmp(mode_str,"wb") == 0 ) {
		    void *c_fp;
		    c_fp = c_fopen(path_str.cstr(), "w");
		    if ( c_fp == NULL ) file_rw_ok = false;
		    else c_fclose(c_fp);
		}
		else if ( c_strcmp(mode_str,"r") == 0 || 
			  c_strcmp(mode_str,"rb") == 0 ) {
		    void *c_fp;
		    c_fp = c_fopen(path_str.cstr(), "r");
		    if ( c_fp == NULL ) file_rw_ok = false;
		    else c_fclose(c_fp);
		}
		else file_rw_ok = false;
		/* support of "lbzip2" multi-thread compressor */
		if ( file_rw_ok == true && Lbzip2_flag == true && 
		     Lbzip2_cmd[0] == '\0' && Lbzip2_cmd[1] == '\0' ) {
		    tstring env_use_lbzip2;
		    /* check SLI_LBZIP2 env variable */
		    env_use_lbzip2 = getenv("SLI_LBZIP2");
		    if ( env_use_lbzip2.strcmp("0") != 0 ) {
			tstring env_path;
			tarray_tstring paths;
			/* find lbzip2 */
			env_path = getenv("PATH");
			if ( 0 < env_path.length() ) {
			    struct stat st;
			    size_t i;
			    paths.split(env_path,":",true);
			    for ( i=0 ; i < paths.length() ; i++ ) {
				if ( 0 < paths[i].length() ) {
				    paths[i].append("/lbzip2");
				    if ( stat(paths[i].cstr(),&st) == 0 &&
					 (st.st_mode & S_IROTH) != 0 &&
					 (st.st_mode & S_IXOTH) != 0 ) {
					paths[i].getstr(Lbzip2_cmd,PATH_MAX);
					break;
				    }
				}
			    }
			    /* flag */
			    if ( i == paths.length() ) Lbzip2_cmd[1] = 'F';
			}
		    }
		}
		if ( Lbzip2_cmd[0] != '\0' ) {
		    tstring env_lbzip2_r_args, env_lbzip2_w_args;
		    /* check env variables */
		    env_lbzip2_r_args = getenv("SLI_LBZIP2_R_ARGS");
		    env_lbzip2_w_args = getenv("SLI_LBZIP2_W_ARGS");
		    /* determine lbzip2 command */
		    if ( mode_str[0] == 'w' ) {			/* write */
			if ( 0 < env_lbzip2_w_args.length() ) {
			    path_str0.printf("%s -c %s > %s",
				    Lbzip2_cmd, 
				    env_lbzip2_w_args.cstr(), path_str.cstr());
			}
			else {
			    path_str0.printf("%s -c > %s", 
					      Lbzip2_cmd, path_str.cstr());
			}
		    }
		    else {					/* read */
			if ( 0 < env_lbzip2_r_args.length() ) {
			    path_str0.printf("%s -dc %s %s",
				    Lbzip2_cmd,
				    env_lbzip2_r_args.cstr(), path_str.cstr());
			}
			else {
			    path_str0.printf("%s -dc %s", 
					      Lbzip2_cmd, path_str.cstr());
			}
		    }
		    try {
			sp0 = new pipestreamio;
		    }
		    catch ( ... ) {
			err_throw(__FUNCTION__,"FATAL","`new' failed");
		    }
		}
		/* use libbz2 */
#ifdef HAVE_LIBBZ2
		try {
		    sp = new bzstreamio;
		}
		catch ( ... ) {
		    if ( sp0 != NULL ) delete sp0;
		    err_throw(__FUNCTION__,"FATAL","`new' failed");
		}
#endif
	    }
	    if ( sp0 != NULL ) {
		if ( sp0->open(mode_str,path_str0.cstr()) < 0 ) {
		    delete sp0;
		    sp0 = NULL;
		}
		else {
		    this->isp = sp0;
		    if ( sp != NULL ) {
			delete sp;
			sp = NULL;
		    }
		}
	    }
	    if ( sp != NULL ) {
		if ( sp->open(mode_str,path_str.cstr()) < 0 ) {
		    delete sp;
		    sp = NULL;
		    this->isp = NULL;
		    err_report(__FUNCTION__,"ERROR","sp->open() failed");
		    goto quit;
		}
		else {
		    this->isp = sp;
		}
	    }
	}
    }

    if ( c_strncmp(mode_str,"w",1) == 0 ) this->is_write_mode_rec = true;
    else this->is_write_mode_rec = false;

    return_status = 0;
 quit:
    return return_status;
}

/**
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open()�ǳ��������ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
 */
int digeststreamio::close()
{
    int return_status = 0;

    if ( this->isp != NULL ) {
	return_status = this->isp->close();
	delete this->isp;
	this->isp = NULL;
    }
    if ( this->fp != NULL ) {
	int status;
        status = c_fclose(this->fp);
        this->fp = NULL;
	if ( status != 0 ) return_status = status;
    }

    this->cstreamio::close();

    this->is_httpstream = false;
    this->is_ftpstream = false;
    this->is_write_mode_rec = false;

    return return_status;
}

/**
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å���
 *
 *  �Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ����򥹥ȥ꡼��˽񤭹��ߤޤ���
 */
int digeststreamio::flush()
{
    if ( this->isp != NULL )
        return this->isp->flush();
    else return this->cstreamio::flush();
}

/**
 * @brief  EOF�ؼ��Ҥμ���
 */
int digeststreamio::eof()
{
    if ( this->isp != NULL ) {
	return this->isp->eof();
    }
    else return this->cstreamio::eof();
}

/**
 * @brief  ���顼�ؼ��Ҥμ���
 */
int digeststreamio::error()
{
    if ( this->isp != NULL ) {
	return this->isp->error();
    }
    else return this->cstreamio::error();
}

/**
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
 */
digeststreamio &digeststreamio::reseterr()
{
    if ( this->isp != NULL ) {
	this->isp->reseterr();
    }
    else this->cstreamio::reseterr();
    return *this;
}

/**
 * @brief  �ܥ��饹�Ǥ��ɤ߹����Ѹ��ϴؿ������
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �ǡ����θĿ�
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ���Υ��дؿ���protected�Ǥ���
 *           �桼����ľ�����Ѥ�����ϤǤ��ޤ���
 */
ssize_t digeststreamio::raw_read( void *buf, size_t size )
{
    if ( this->isp != NULL ) {
	return this->isp->read(buf,size);
    }
    else return -1;
}

/**
 * @brief  �ܥ��饹�Ǥν񤭹����Ѹ��ϴؿ������
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����θĿ�
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ���Υ��дؿ���protected�Ǥ���
 *             �桼����ľ�����Ѥ�����ϤǤ��ޤ���
 */
ssize_t digeststreamio::raw_write( const void *buf, size_t size )
{
    if ( this->isp != NULL ) {
	return this->isp->write(buf,size);
    }
    else return -1;
}

}	/* namespace sli */


#include "private/c_fdopen.c"
#include "private/c_fopen.c"
#include "private/c_fclose.c"

#include "private/c_strcmp.cc"
#include "private/c_strncmp.cc"
#include "private/c_dup.c"
#include "private/c_close.c"

#include "private/suffix2filetype.c"
