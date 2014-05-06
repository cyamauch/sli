/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-13 23:31:18 cyamauch> */

/**
 * @file   ftpstreamio.cc
 * @brief  FTP�����Ф���³�����ȥ꡼�������Ϥ�Ԥʤ� ftpstreamio ���饹�Υ�����
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
 * @brief  ���󥹥ȥ饯��
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
 * @brief  �ǥ��ȥ饯��
 */
ftpstreamio::~ftpstreamio()
{
    this->close();
    return;
}

/**
 * @brief  ftp:// ����Ϥޤ� URL �Υ����ץ�
 *
 *  �ѥ�����ꤷ�ơ�URL�򥪡��ץ󤷤ޤ���<br>
 *  mode �� "r%" �ޤ��� "w%" ����ꤹ��ȡ��ե�����̾�Υ��ե��å����ˤ�ꡤ
 *  ��ưŪ�˿�Ĺ�����̤�Ԥʤ��ޤ���
 *
 * @param      mode URL�򳫤��⡼�� ("r", "r%", "w" or "w%")
 * @param      path URL�Υѥ�
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ����γ�����Ƥ˼��Ԥ������ 
 */
int ftpstreamio::open( const char *mode, const char *path )
{
    assert( mode != NULL );

    int return_status = -1;

    /* "r%" ����ꤹ����ǡ����̥��ȥ꡼���Ÿ�����б� */
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
 * @brief  FTP���������ɤ��֤�
 *
 * @param      isp inetstreamio���饹�Υ��󥹥���
 * @param      rline tstring���饹�Υ��󥹥���
 * @return     ������������400̤����3�������<br>
 *             ���Ԥ�������400�ʾ������
 * @note       ���Υ��дؿ��� private �Ǥ���
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
 * @brief  URL�Υ����ץ� (���٥�)
 *
 * @param      mode URL�򳫤��⡼�� ("r", "r%", "w" or "w%")
 * @param      path URL�Υѥ�
 * @param      use_filter_by_suffix ���̥��ȥ꡼��Ÿ�������̵ͭ
 * @return     ������������0<br>
 *             ���顼�ξ�������
 * @throw      ����γ�����Ƥ˼��Ԥ������ 
 * @note       ���Υ��дؿ��� protected �Ǥ���
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

	/* ftp://username:password@ftp.com/... �Ǥ�����褦�ˤ��� */
	user_pass.assign(path);
	user_pass.erase(0,6);
	pos = user_pass.regmatch("^[^:][^:]*:[^@]*@",&len);
	if ( pos == 0 ) {
	    t_path.erase(6,len);
	    user_pass.crop(pos,len-1);
	}

	/* FTP �桼��̾ */
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

	/* FTP �ѥ���� */
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
	
	/* �Ϥ���Υ�å���������� */
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}
	
	/* �桼�� */
	isp->printf("USER %s\n",user.cstr());
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	/* �ѥ���� */
	isp->printf("PASS %s\n",pass.cstr());
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	/* �Х��ʥ� */
	isp->printf("TYPE I\n");
	isp->flush();
	if ( 400 <= this->read_response(isp,ans) ) {
	    err_report1(__FUNCTION__,"ERROR","ftpd: [%s]",ans.strtrim().cstr());
	    goto quit;
	}

	/* �ե����륵���� */
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
	
	/* �ѥå��֥⡼�� */
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

	/* ���� or �������ޥ�� */
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
 * @brief  ���ȥ꡼��ΥХ���Ĺ�����
 *
 *  FTP �����Ф��� SIZE ���ޥ�ɤǼ��������ͤ򤽤Τޤ��֤��ޤ���
 *
 * @return     ������: SIZE ���ޥ�ɤǼ���������<br>
 *             ����: SIZE ���ޥ�ɤ˼��Ԥ������
 */
long long ftpstreamio::content_length() const
{
    return this->content_length_rec;
}

/**
 * @brief  �桼����̾�����Ѥ�ʸ���󥪥֥������Ȥ��֤�
 *
 * @return  ʸ���󥪥֥�������(tstring)�λ���
 */
tstring &ftpstreamio::username()
{
    return this->username_rec;
}

/**
 * @brief  �ѥ���������Ѥ�ʸ���󥪥֥������Ȥ��֤�
 *
 * @return  ʸ���󥪥֥�������(tstring)�λ���
 */
tstring &ftpstreamio::password()
{
    return this->password_rec;
}

/**
 * @brief  ɸ�������ϤΥ����ץ�
 *
 *  ɸ�������Ϥ򥪡��ץ󤷤ޤ���<br>
 *  mode �� "r" �����ꤵ�줿����ɸ�����Ϥ�"w" �����ꤵ�줿����ɸ����Ϥ�
 *  �����ץ󤷤ޤ�.
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ��ѥ�����ꤷ�ơ�URL�򥪡��ץ󤹤����
 *              �Ǥ��ޤ���
 * @param     mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @return    ������������0<br>
 *            ���顼�ξ�������
 * @throw     err_rec ɸ�������ϤΥե����롦�ǥ�������ץ���ʣ���˼��Ԥ������
 * @note      ���������ͳ�Ǻ������줿���дؿ��Ǥ���
 */
int ftpstreamio::open( const char *mode )
{
    assert( mode != NULL );

    return this->cstreamio::open(mode);
}

/**
 * @brief  �ǥ�������ץ�����ˤ�륹�ȥ꡼��Υ����ץ�
 *
 *  �ǥ�������ץ�����ꤷ�ơ����ȥ꡼��򥪡��ץ󤷤ޤ���
 *
 * @deprecated  ���Ѥ����̣�Ϥ���ޤ���<br>
 *              ���Υ��дؿ��Ǥϡ��ѥ�����ꤷ�ơ�URL�򥪡��ץ󤹤����
 *              �Ǥ��ޤ���
 * @param       mode ���ȥ꡼��򳫤��⡼�� ("r" or "w")
 * @param       fd   �ե�����ǥ�������ץ�
 * @return      ������������0<br>
 *              ���顼�ξ�������
 * @note        ���������ͳ�Ǻ������줿���дؿ��Ǥ���
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
 * @brief  ���ȥ꡼��Υ�����
 *
 *  open()�ǳ��������ȥ꡼��򥯥������ޤ���
 *
 * @return     ������������0<br>
 *             ���顼�ξ���0�ʳ�����
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
 * @brief  �񤭹����ѥ��ȥ꡼��Υե�å���
 *
 *  ���ȥ꡼��ˡ��Хåե��˳�Ǽ����Ƥ��뤹�٤ƤΥǡ�����񤭹��ߤޤ���
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
 * @brief  EOF�ؼ��Ҥμ���
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
 * @brief  ���顼�ؼ��Ҥμ���
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
 * @brief  EOF�ؼ��Ҥȥ��顼�ؼ��ҤΥꥻ�å�
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
 * @brief  �ɤ߹����Ѹ��ϴؿ��Υ���å� (���Υ��饹�ˤƥ����Х饤��)
 *
 * @param    buf �ǡ����γ�Ǽ���
 * @param    size �ǡ����θĿ�
 * @return   �����������ϥХ��ȿ�<br>
 *           ���ȥ꡼�ब����ξ�硤0�Х��Ȥ��ɤ߹��ߤ����ꤵ�줿����0<br>
 *           ���顼�ξ�������
 * @note     ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  �񤭹����Ѹ��ϴؿ��Υ���å� (���Υ��饹�ˤƥ����Х饤��)
 *
 * @param      buf �ǡ����γ�Ǽ���
 * @param      size �ǡ����θĿ�
 * @return     �����������ϥХ��ȿ�<br>
 *             ���顼�ξ�������
 * @note       ���Υ��дؿ��� protected �Ǥ���
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
 * @brief  ���ߤΥޥ��󤫤�桼��̾������ (������)
 *
 * @note   private �ʴؿ��Ǥ���
 */
static void get_user_at_host( sli::tstring *ret )
{
    char host_name[256];
    struct passwd *user_info;
    const char *user_name = NULL;
    /* �ۥ���̾ */
    if ( c_gethostname(host_name,256) != 0 ) {
	c_strcpy(host_name,"VeryLongHostname");
    }
    host_name[255] = '\0';
    /* �桼��̾ */
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
