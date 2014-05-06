/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-12 00:00:46 cyamauch> */

/**
 * @file   tregex.cc
 * @brief  POSIX��ĥ����ɽ���򰷤�����δ���Ū�ʥ��饹 tregex �Υ�����
 */

#define CLASS_NAME "tregex"

#include "config.h"

#include "tregex.h"

#include <stdlib.h>

#include "private/err_report.h"

#include "private/c_strcmp.h"
#include "private/c_strdup.h"
#include "private/c_regex.h"
#include "private/c_regfatal.h"
#include "private/c_regsearchx.h"
#include "private/memswap.h"

namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 *
 */
tregex::tregex()
{
    this->regstr_rec = NULL;
    this->preg_rec = NULL;
    this->errstr_rec[0] = '\0';

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���ꤵ�줿����ɽ���򥳥�ѥ��뤷�����η�̤򼫿Ȥ˳�Ǽ���ޤ���
 * 
 * @param  regstr ����ѥ��뤵�������ɽ��ʸ����
 * @throw  ���꤬��­���Ƥ����硥�����Хåե��γ��ݤ˼��Ԥ�����硥
 * @note   ����ѥ��륨�顼�򸡽Ф��������ϡ�.compile() ��Ȥ��������Ǥ���
 */
tregex::tregex(const char *regstr)
{
    this->regstr_rec = NULL;
    this->preg_rec = NULL;
    this->errstr_rec[0] = '\0';

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    if ( this->compile(regstr) < 0 ) {
	//err_report(__FUNCTION__,"WARNING","this->compile() reports error");
    }

    return;
}

/**
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  obj�����ƤǼ��Ȥ��������ޤ���
 *
 * @param  obj �����Ȥʤ륪�֥�������
 */
tregex::tregex(const tregex &obj)
{
    this->regstr_rec = NULL;
    this->preg_rec = NULL;
    this->errstr_rec[0] = '\0';

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    this->init(obj);

    return;
}

/**
 * @brief  �ǥ��ȥ饯��
 * 
 */
tregex::~tregex()
{
    if ( this->regstr_rec != NULL ) {
	free(this->regstr_rec);
    }
    if ( this->preg_rec != NULL ) {
	c_regfree(this->preg_rec);
    }
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿 tregex ���֥������Ȥ򼫿Ȥ˥��ԡ����ޤ���
 *
 * @param   obj �����Ȥʤ륪�֥�������
 * @return  ���Ȥλ���
 */
tregex &tregex::operator=(const tregex &obj)
{
    this->init(obj);
    return *this;
}


/**
 * @brief  ���ꤵ�줿����ɽ���򥳥�ѥ��뤷���Ȥ˳�Ǽ
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����ɽ���򥳥�ѥ��뤷�����η�̤򼫿Ȥ�
 *  ��Ǽ���ޤ���
 *
 * @param  regstr ����ѥ��뤵�������ɽ��ʸ����
 * @throw  ���꤬��­���Ƥ����硥�����Хåե��γ��ݤ˼��Ԥ�����硥
 * @note   ����ѥ��륨�顼�򸡽Ф��������ϡ�.compile() ��Ȥ��������Ǥ���
 */
tregex &tregex::operator=(const char *regstr)
{
    if ( this->compile(regstr) < 0 ) {
	//err_report(__FUNCTION__,"WARNING","this->compile() reports error");
    }
    return *this;
}

/**
 * @brief   ���֥������Ȥν����
 * 
 * @return  ���Ȥλ���
 */
tregex &tregex::init()
{
    if ( this->regstr_rec != NULL ) {
	free(this->regstr_rec);
	this->regstr_rec = NULL;
    }
    if ( this->preg_rec != NULL ) {
	c_regfree(this->preg_rec);
	this->preg_rec = NULL;
    }
    this->errstr_rec[0] = '\0';
    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  obj �����Ƥ򼫿Ȥ˥��ԡ����ޤ���
 *
 * @param   obj tregex���֥�������
 * @return  ���Ȥλ���
 * @throw   ���꤬��­���Ƥ����硥�����Хåե��γ��ݤ˼��Ԥ�����硥
 */
tregex &tregex::init(const tregex &obj)
{
    if ( &obj == this ) return *this;
    this->tregex::init();
    if ( obj.cstr() != NULL ) this->tregex::compile(obj.cstr());
    return *this;
}

/* ���顼�����å��� cregex() �Ǥ��ǽ��cregex() ���֤��ͤ� NULL �ʤ� */
/* ����ѥ���˼��Ԥ��Ƥ��롥                                        */

/**
 * @brief  ���ꤵ�줿����ɽ���򥳥�ѥ��뤷 ��̤򼫿Ȥ˳�Ǽ (�֤��ͤ�status)
 *
 *  ����ɽ�� regstr �򥳥�ѥ��뤷������ɽ���ȥ���ѥ����̤Ȥ򥪥֥������Ȥ�
 *  ��Ǽ���ޤ���
 * 
 * @param   regstr ����ѥ��뤵�������ɽ��ʸ����
 * @return  0: ���ｪλ <br>
 *          �����: ����ɽ��ʸ���������ʾ��
 * @throw   ���꤬��­���Ƥ����硥�����Хåե��γ��ݤ˼��Ԥ�����硥
 */
int tregex::compile(const char *regstr)
{
    int status;
    void *tmp_preg;

    if ( this->regstr_rec != NULL && regstr != NULL &&
	 c_strcmp(this->regstr_rec,regstr) == 0 ) return 0;	/* OK */

    if ( this->regstr_rec != NULL ) {
	free(this->regstr_rec);
	this->regstr_rec = NULL;
    }
    if ( this->preg_rec != NULL ) {
	c_regfree(this->preg_rec);
	this->preg_rec = NULL;
    }
    this->errstr_rec[0] = '\0';

    if ( regstr == NULL ) return -1;				/* Invalid */

    status = c_regcomp(&tmp_preg, regstr);
    if ( status != 0 ) {
	if ( c_regfatal(status) ) {
	    c_regfree(tmp_preg);
	    err_throw(__FUNCTION__,"FATAL","Out of memory in c_regcomp()");
	}
	c_regerror(status,tmp_preg,this->errstr_rec,192);
	c_regfree(tmp_preg);
	return -1;						/* Error */
    }

    /* register */
    this->preg_rec = tmp_preg;
    this->regstr_rec = c_strdup(regstr);
    if ( this->regstr_rec == NULL ) {
	c_regfree(this->preg_rec);
	this->preg_rec = NULL;
	err_throw(__FUNCTION__,"FATAL","c_strdup() failed");
    }

    return 0;
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���Ȥ����Ƥ� sobj �����ƤȤ������ؤ��ޤ���
 *
 * @param   sobj tregex ���֥�������
 * @return  ���Ȥλ���
 */
tregex &tregex::swap(tregex &sobj)
{
    if ( &sobj == this ) return *this;

    char *tmp__regstr_rec;
    void *tmp__preg_rec;

    tmp__regstr_rec = sobj.regstr_rec;
    tmp__preg_rec  = sobj.preg_rec;
    sobj.regstr_rec = this->regstr_rec;
    sobj.preg_rec  = this->preg_rec;
    this->regstr_rec = tmp__regstr_rec;
    this->preg_rec  = tmp__preg_rec;

    memswap(this->errstr_rec, sobj.errstr_rec, 192);

    return *this;
}

/**
 * @brief  ���Ȥ˳�Ǽ����Ƥ�������ɽ��ʸ������֤�
 *
 * @return  ��Ǽ����Ƥ�������ɽ��ʸ����
 */
const char *tregex::cstr() const
{
    return this->regstr_rec;
}

/**
 * @brief  ���Ȥ˳�Ǽ����Ƥ��� regex_t ���֥������ȤΥ��ɥ쥹���֤�
 * 
 *  ���Ȥ˳�Ǽ����Ƥ��� regex_t ���֥������ȤΥ��ɥ쥹���֤��ޤ���<br>
 *  ���Υ��дؿ��� NULL ���֤���硤compile() �˼��Ԥ��Ƥ�����򼨤��ޤ���
 *
 * @return  ���Ȥ˳�Ǽ����Ƥ��� regex_t ���֥������ȤΥ��ɥ쥹
 */
const void *tregex::cregex() const
{
    return this->preg_rec;
}

/**
 * @brief  ���ꤵ�줿ʸ����ˤĤ��ơ�����ɽ���ˤ��ޥå��󥰤���
 *
 *  ʸ���� str �ˤĤ��ơ����Ȥ˳�Ǽ���줿����ɽ���ˤ��ޥå��󥰤�Ԥ��ޤ���
 * 
 * @param  str �����о�ʸ����
 * @param  notbol true �ξ�硤��Ƭ�ޥå��Υ��ڥ졼����ɬ�����Ԥ�����
 * @param  noteol true �ξ�硤�����ޥå��Υ��ڥ졼����ɬ�����Ԥ�����
 * @param  max_nelem ������¤θĿ�
 * @param  pos_r[] �ޥå��������֤���Ǽ�����
 * @param  len_r[] �ޥå�����ʸ��������ǿ�����Ǽ�����
 * @param  nelem_r �ޥå������Ŀ�����Ǽ�����
 * @return  0: �ޥå������� <br>
 *          0�ʳ�: �ޥå��󥰼���
 */
int tregex::exec( const char *str, bool notbol, bool noteol,
		  size_t max_nelem, size_t pos_r[], size_t len_r[],
		  size_t *nelem_r ) const
{
    return c_regsearchx(this->cregex(), str, notbol, noteol,
			max_nelem, pos_r, len_r, nelem_r);
}

/**
 * @brief  ����ѥ�����Υ��顼��å����������
 *
 *  ���Ȥ˳�Ǽ����Ƥ��� ����ѥ�����Υ��顼��å��������֤��ޤ���<br>
 *  ���줬 NULL ���֤���硤compile() ���������Ƥ�����򼨤��ޤ���
 *
 * @return  ��Ǽ����Ƥ��륨�顼��å�����: ����ѥ���˼��Ԥ����� <br>
 *          NULL: ����ѥ��������������
 */
const char *tregex::cerrstr() const
{
    if ( this->preg_rec != NULL ) return NULL;
    else return this->errstr_rec;
}

/**
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ (̤����)
 * @deprecated  ̤����
 *
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void tregex::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}

/**
 * @brief  shallow copy ����ǽ�����֤� (̤����)
 * 
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note    ���Υ��дؿ��� private �Ǥ���
 */
bool tregex::request_shallow_copy( tregex *from_obj ) const
{
    return false;
}


}

#include "private/c_strcmp.cc"
#include "private/c_strdup.c"
#include "private/c_regex.c"
#include "private/c_regfatal.c"
#include "private/c_regsearchx.c"
#include "private/memswap.cc"

