/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 00:14:25 cyamauch> */

/**
 * @file   tarray_tstring.cc
 * @brief  ʸ��������򰷤�tarray_tstring���饹�Υ�����
 */

#define CLASS_NAME "tarray_tstring"

#include "config.h"

#include "tarray_tstring.h"
#include "heap_mem.h"

#include <stdlib.h>

#include "private/err_report.h"

#include "private/c_memcpy.h"
#include "private/c_strncmp.h"
#include "private/c_isalpha.h"
#include "private/c_regsearchx.h"
#include "private/c_regfatal.h"

/* #define TARRAY_TSTRING__REMOVE_ESCAPE 1 */

/* ����ɽ���θ����λ�����������å����Ȥ� */
#define REGFUNCS_USING_CACHE 1

namespace sli
{

/* make_accepts_regexp() and is_found() */
#include "private/tstring_regexp_cset.cc"

/*
 * ���֥��������������ϡ��ҡ��פ��ΰ����ݤ��ʤ���
 * �������äơ����⤷�ʤ��� obj.elements() �� NULL 
 * ���֤롥
 *
 * const char tbl[] = {NULL};
 * hoge.init(tbl) �� hoge.append(tbl)
 * ���Ф��Ƥϡ�this->cstr_ptrs[0] = NULL ���롥
 *
 * hoge.init((char **)NULL)
 * ���Ƚ����
 * 
 * hoge.append((char **)NULL) ���Ф��Ƥϡ�̵ȿ����
 *
 * ���֥������Ȥ������֤��ᤷ����(�ҡ��פ���������)���ϡ�
 * obj = NULL;
 * �Ȥ��롥
 */

/**
 * @brief  tarray �����ǿ������֥������Ȥ�����˸ƤФ��ؿ�
 *
 * @note   private �ʴؿ��Ǥ���
 */
static tstring *tstring_creator(void *ptr)
{
    tstring *p;
    p = new tstring(true);
    return p;
}


/* constructor */

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
tarray_tstring::tarray_tstring()
{
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    return;
}

/* constructor */

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���Ȥ������ʸ�������� elements �����Ƥǽ�������ޤ���<br>
 *  elements �ν�ü��NULL�Ǥʤ���Фʤ�ޤ���
 *
 * @param  elements ʸ��������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
tarray_tstring::tarray_tstring(const char *const *elements)
{
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    if ( elements != NULL ) this->assign(elements);
    return;
}

/* constructor */

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���Ȥ�Ϳ����줿ʸ����ǽ�������ޤ���<br>
 *  �����κǸ��ɬ�� NULL ��Ϳ���ޤ���
 * 
 * @param  el0 ʸ����
 * @param  el1 ʸ����
 * @param  ... ʸ����γ�����.
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
tarray_tstring::tarray_tstring(const char *el0, const char *el1, ...)
{
    va_list ap;

    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);

    va_start(ap, el1);
    try {
	this->vassign( el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign() failed");
    }
    va_end(ap);

    return;
}

/* copy constructor */

/**
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  ���Ȥ� obj �����Ƥǽ�������ޤ���
 * 
 * @param  obj tarray_tstring�Υ��֥�������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
tarray_tstring::tarray_tstring(const tarray_tstring &obj)
{
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    this->init(obj);
    return;
}

/* destructor */

/**
 * @brief  �ǥ��ȥ饯��
 */
tarray_tstring::~tarray_tstring()
{
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿���֥������Ȥ����Ƥޤ���ʸ���������
 *  ���Ȥ��������ޤ���
 *
 * @param      obj tarray_tstring���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::operator=(const tarray_tstring &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ�ʸ��������(tarray_tstring)���ɲ�
 *
 *  ���Ȥ�����ˡ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿ʸ����������ɲä�Ԥ��ޤ���
 *
 * @param      obj tarray_tstring���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::operator+=(const tarray_tstring &obj)
{
    this->append(obj);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿ʸ��������(const char *[])������
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿���֥������Ȥ����Ƥޤ���ʸ���������
 *  ���Ȥ��������ޤ���
 *
 * @param      elements ʸ����Υݥ�������Υ��ɥ쥹(NULL ��ü)
 * @return     ����ʸ����Хåե��ؤΥݥ�������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
const char *const *tarray_tstring::operator=(const char *const *elements)
{
    if ( elements == NULL ) {	/* NULL�λ��Ϥ���äݤˤ��� */
        this->init();
    }
    else {
        this->assign(elements);
    }
    return this->cstrarray();
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ��������(const char *[])���ɲ�
 *
 *  ���Ȥ�����ˡ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿ʸ����������ɲä�Ԥ��ޤ���
 *
 * @param      elements ʸ����Υݥ�������Υ��ɥ쥹(NULL ��ü)
 * @return     ����ʸ����Хåե��ؤΥݥ�������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
const char *const *tarray_tstring::operator+=(const char *const *elements)
{
    if ( elements != NULL ) this->append(elements);
    return this->cstrarray();
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ��������(const char *)���ɲ�
 *
 *  ���Ȥ�����ˡ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param   str ʸ����Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::operator+=(const char *str)
{
    if ( str != NULL ) {
        this->append(str,1);
    }
    return *this;
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ��������(tstring)���ɲ�
 *
 *  ���Ȥ�����ˡ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿ʸ������ɲä��ޤ���
 *
 * @param   one ʸ����Υ��ɥ쥹(�ޥ˥奢��Ǥ�str)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::operator+=(const tstring &one)
{
    if ( one.cstr() != NULL ) {
        this->append(one,1);
    }
    return *this;
}

/*
 * public member functions
 *
 * �������ˤȤ��ơ�������ʸ����,�ݥ�������� object �����Ǵ�������Ƥ���
 * �ΰ褬Ϳ�����Ƥ�ư�����ͤȤ��롥
 *
 */

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�ʸ���������õ�����֥������Ȥν������Ԥ��ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::init()
{
    this->arrs_rec.init();
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    this->regex_rec.init();

    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  ���ꤵ�줿���֥������� obj �����Ƥ򼫿Ȥ˥��ԡ����ޤ���
 *
 * @param      obj tarray_tstring ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::init(const tarray_tstring &obj)
{
    if ( &obj == this ) return *this;
    this->arrs_rec.init();
    this->arrs_rec.register_creator(&tstring_creator, NULL);
    this->cstr_ptrs_rec.init(sizeof(char *), true);
    this->reg_pos_rec.init(obj.reg_pos_rec);
    this->reg_length_rec.init(obj.reg_length_rec);
    /* this->regex_rec.init(obj.regex_rec); */
    this->regex_rec.init();
    this->append(obj);

    return *this;
}

/**
 * @brief  �������Τ���ꤵ�줿ʸ����(const char *)�ǥѥǥ���
 *
 *  ���Ȥ���������������Ǥ�ʸ���� str �ǥѥǥ��󥰤��ޤ���
 *
 * @param      str ʸ���������ѥǥ��󥰤��뤿���ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::clean(const char *str)
{
    if ( str != NULL ) {
	tstring one = str;
	this->arrs_rec.clean(one);
    }
    return *this;
}

/**
 * @brief  �������Τ���ꤵ�줿ʸ����(tstring)�ǥѥǥ���
 *
 *  ���Ȥ���������������Ǥ�ʸ���� str �ǥѥǥ��󥰤��ޤ���
 *
 * @param      one ʸ���������ѥǥ��󥰤��뤿���ʸ����(�ޥ˥奢��Ǥ�str)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::clean(const tstring &one)
{
    if ( one.cstr() != NULL ) {
	this->arrs_rec.clean(one);
    }
    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
tarray_tstring &tarray_tstring::assign( size_t n )
{
    return this->replace(0, this->length(), n);
}

/**
 * @brief  ���ꤵ�줿ʸ��������(const char *[])�򼫿Ȥ�����
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ��������ޤ���
 *
 * @param      elements ���Ǥ�����ʸ����Υݥ�������(NULL �ǽ�ü)
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::assign( const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->assign(elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->assign(elements,i);
}

/**
 * @brief  ���ꤵ�줿ʸ��������(const char *[])��n���Ǥ򼫿Ȥ�����
 *
 *  elements�ǻ��ꤵ�줿ʸ��������� n �Ĥ򡤼��Ȥ��������ޤ���
 *
 * @param      elements ���Ǥ�����ʸ����Υݥ�������
 * @param      n ����elements �����ǿ�
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::assign( const char *const *elements, size_t n )
{
    return this->replace(0, this->length(), elements, n);
}

/**
 * @brief  ���ꤵ�줿ʸ��������(tarray_tstring)��(����)���Ǥ򼫿Ȥ�����
 *
 *  src�ǻ��ꤵ�줿ʸ��������������ޤ��ϰ����򡤼��Ȥ��������ޤ���
 *
 * @param    src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param    idx2 src������Ǥγ��ϰ���(src ����ʬ���������������)
 * @return   ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::assign(const tarray_tstring &src, size_t idx2)
{
    return this->assign(src, idx2, src.length());
}

/**
 * @brief  ���ꤵ�줿ʸ��������(tarray_tstring)��(����)���Ǥ򼫿Ȥ�����
 *
 *  src�ǻ��ꤵ�줿ʸ��������������ޤ��ϰ����򡤼��Ȥ��������ޤ���
 *
 * @param    src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param    idx2 src������Ǥγ��ϰ���(src ����ʬ���������������)
 * @param    n2 src ������ǤθĿ�(src ����ʬ���������������)
 * @return   ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::assign( const tarray_tstring &src, 
					size_t idx2, size_t n2 )
{
    return this->replace(0, this->length(), src, idx2, n2);
}

/**
 * @brief  ���ꤵ�줿ʸ����(const char *)��n�� ���Ȥ�����
 *
 *  ���Ȥ��������Ǥ�n�ĤȤ������٤Ƥ����Ǥ˻��ꤵ�줿ʸ������������ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @param      n ��������ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::assign( const char *str, size_t n )
{
    return this->replace(0, this->length(), str, n);
}

/**
 * @brief  ���ꤵ�줿ʸ����(tstring)��n�� ���Ȥ�����
 *
 *  ���Ȥ��������Ǥ�n�ĤȤ������٤Ƥ����Ǥ˻��ꤵ�줿ʸ������������ޤ���
 *
 * @param      one �����Ȥʤ�ʸ����(�ޥ˥奢��Ǥ�str)
 * @param      n ��������ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::assign( const tstring &one, size_t n )
{
    return this->replace(0, this->length(), one, n);
}

/**
 * @brief  ���ꤵ�줿ʸ����(printf()�ε�ˡ�ǻ���)��n�� ���Ȥ�����
 *
 *  ���Ȥ��������Ǥ�n�ĤȤ������٤Ƥ����Ǥ˻��ꤵ�줿ʸ������������ޤ���
 *
 * @param   n ��������ǿ�
 * @param   format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param   ... format���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ�
 *          �Ѵ��Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::assignf( size_t n, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vassignf(n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassignf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ����(printf()�ε�ˡ�ǻ���)��n�� ���Ȥ�����
 *
 *  ���Ȥ��������Ǥ�n�ĤȤ������٤Ƥ����Ǥ˻��ꤵ�줿ʸ������������ޤ���
 *
 * @param      n ��������ǿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ�
 *             �Ѵ��Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::vassignf(size_t n, const char *format, va_list ap)
{
    tstring one;

    if ( format == NULL ) return this->assign(format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->assign(one,n);

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ����򼫿Ȥ�����
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʣ����ʸ�����ʸ��������Ȥ��Ƽ��Ȥ��������ޤ���
 *
 * @param      el0 ���Ǥ�����ʸ����(0 ����)
 * @param      el1 ���Ǥ�����ʸ����(1 ����)
 * @param      ... ���Ǥ�����ʸ����(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::assign( const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vassign( el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ����򼫿Ȥ�����
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʣ����ʸ�����ʸ��������Ȥ��Ƽ��Ȥ��������ޤ���
 *
 * @param      el0 ���Ǥ�����ʸ����(0 ����)
 * @param      el1 ���Ǥ�����ʸ����(1 ����)
 * @param      ap ���Ǥ�����ʸ����β���Ĺ�����Υꥹ��(2 ���ܰʹ�; NULL ��ü)
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::vassign( const char *el0, const char *el1, 
					 va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->assign(tmp_ptbl.ptr());

    return *this;
}


#define LEN_RET_S_STK 512
#define LEN_RET_P_STK 128

/* len_src_str �������ʾ��� 0 �ˤ��Ƥ��ɤ� */
inline void explode_php( const char *src_str, size_t len_src_str, 
		   const char *delim, bool zero_str, tarray_tstring *retobjp )
{
    /* ����Хåե�: */
    /* ����Ū�ˤϺǽ�ϥ����å���Ȥ���­��ʤ��ʤä���ҡ��פ��ڤ��ؤ��� */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delim == NULL ) delim = "";

    /* len_src_str �� 0 �Ǥʤ���硤���餫��­��ʤ����ϥҡ��פ��ڤ��ؤ� */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * parse src_str
     */

    try {
	i = 0;
	while ( src_str[i] != '\0' ) {
	    char ch = src_str[i];
	    size_t i_step = 1;
	    size_t j;
	    for ( j=0 ; delim[j] != '\0' ; j++ ) {
		if ( src_str[i+j] != delim[j] ) break;
	    }
	    if ( 0 < j && delim[j] == '\0' ) {
		ch = '\0';
		i_step = j;
	    }

	    /* ����Хåե��˳�Ǽ */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_str��Ĺ���������ξ���Ĵ�٤Ƥ��ޤ� */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;
	    
	    /* ���ڤ�ʸ������ä����ν���: ���ɥ쥹�ơ��֥����Ͽ */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	    i += i_step;
	}
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}


/* len_src_str �������ʾ��� 0 �ˤ��Ƥ��ɤ� */
inline void explode_advanced( const char *src_str, size_t len_src_str, 
			      const char *delim, bool zero_str, 
			      const char *quot_bkt, 
			      int escape, bool rm_escape,
			      tarray_tstring *retobjp )
{
    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape = false;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* ���Ĥ���٤��ڥ��γ�� */
    size_t depth_bkt = 0;			/* ��̤ο��� */

    /* ����Хåե�: */
    /* ����Ū�ˤϺǽ�ϥ����å���Ȥ���­��ʤ��ʤä���ҡ��פ��ڤ��ؤ��� */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delim == NULL ) delim = "";

    /* len_src_str �� 0 �Ǥʤ���硤���餫��­��ʤ����ϥҡ��פ��ڤ��ؤ� */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }

    /*
     * parse src_str
     */

    try {
      i = 0;
      while ( src_str[i] != '\0' ) {
	char ch = src_str[i];
	bool cpy_ok = true;
	size_t i_step = 1;
	if ( prev_escape == true ) {		/* ����chr��escapeʸ���ʤ� */
	    prev_escape = false;
	}
	else if ( ch == escape ) {		/* ����chr��escapeʸ���ʤ� */
	    if ( rm_escape == true ) cpy_ok = false;
	    prev_escape = true;
	}
	else if ( quot_to_find != '\0' ) {	/* �ڥ��� quot �򸫤Ĥ��� */
	    if ( ch == quot_to_find ) quot_to_find = '\0';
	}
	else {
	    size_t j;
	    /* quot �γ��Ϥ򸫤Ĥ��� */
	    for ( j=0 ; quot[j] != '\0' ; j++ ) {
		if ( ch == quot[j] ) {
		    quot_to_find = ch;
		    break;
		}
	    }
	    if ( quot[j] == '\0' ) {
		/* �����γ�̤򸫤Ĥ��� */
		for ( j=0 ; bkt_l[j] != '\0' ; j++ ) {
		    if ( ch == bkt_l[j] ) {
			bkt_to_find.at(depth_bkt) = bkt_r[j];
			depth_bkt ++;
			break;
		    }
		}
	    }
	    if ( bkt_l[j] == '\0' ) {
		/* �ڥ��γ�̤򸫤Ĥ��� */
		if ( 0 < depth_bkt ) {
		    if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			depth_bkt --;
		    }
		}
		/* �褦�䤯�����Ƕ��ڤ�ʸ����Ƚ�� */
		else {
		    for ( j=0 ; delim[j] != '\0' ; j++ ) {
			if ( src_str[i+j] != delim[j] ) break;
		    }
		    if ( 0 < j && delim[j] == '\0' ) {
			ch = '\0';
			i_step = j;
		    }
		}
	    }
	}
	if ( cpy_ok == true ) {

	    /* ����Хåե��˳�Ǽ */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_str��Ĺ���������ξ���Ĵ�٤Ƥ��ޤ� */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;

	    /* ���ڤ�ʸ������ä����ν���: ���ɥ쥹�ơ��֥����Ͽ */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	}
	i += i_step;
      }
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}


/* len_src_str �������ʾ��� 0 �ˤ��Ƥ��ɤ� */
inline void split_simple( const char *src_str, size_t len_src_str, 
		   const char *delims, bool zero_str, tarray_tstring *retobjp )
{
    /* ����Хåե�: */
    /* ����Ū�ˤϺǽ�ϥ����å���Ȥ���­��ʤ��ʤä���ҡ��פ��ڤ��ؤ��� */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    bool reg_delims = false;
    tstring accepts;
    bool ac_bl;
    int ac_flg = 0;
    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delims == NULL ) delims = "";

    /* check [A-Z] style */
    if ( delims[0] == '[' ) {
	for ( i=0 ; delims[i] != '\0' ; i++ );
	if ( delims[i-1] == ']' ) {
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(delims, true, accepts, &ac_flg, &ac_bl);
	    if ( p0 != 0 ) reg_delims = true;
	}
    }

    /* len_src_str �� 0 �Ǥʤ���硤���餫��­��ʤ����ϥҡ��פ��ڤ��ؤ� */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * parse src_str
     */

    try {
	for ( i=0 ; src_str[i] != '\0' ; i++ ) {
	    char ch = src_str[i];
	    size_t j;
	    if ( reg_delims == true ) {
		if ( is_found(ch, accepts.cstr(), ac_flg) == ac_bl ) {
		    ch = '\0';
		}
	    }
	    else {
		for ( j=0 ; delims[j] != '\0' ; j++ ) {
		    if ( ch == delims[j] ) {
			ch = '\0';
			break;
		    }
		}
	    }
	    /* ����Хåե��˳�Ǽ */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_str��Ĺ���������ξ���Ĵ�٤Ƥ��ޤ� */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;
	    
	    /* ���ڤ�ʸ�����ä����ν���: ���ɥ쥹�ơ��֥����Ͽ */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	}
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}

/* len_src_str �������ʾ��� 0 �ˤ��Ƥ��ɤ� */
inline void split_advanced( const char *src_str, size_t len_src_str, 
		     const char *delims, bool zero_str, const char *quot_bkt,
		     int escape, bool rm_escape, tarray_tstring *retobjp )
{
    char quot[96];				/* list of quotations */
    char bkt_l[5];				/* list of brackets (left) */
    char bkt_r[5];				/* list of brackets (right) */

    bool prev_escape = false;
    char quot_to_find = '\0';			/* quotation to be found */
    tstring bkt_to_find;			/* ���Ĥ���٤��ڥ��γ�� */
    size_t depth_bkt = 0;			/* ��̤ο��� */

    /* ����Хåե�: */
    /* ����Ū�ˤϺǽ�ϥ����å���Ȥ���­��ʤ��ʤä���ҡ��פ��ڤ��ؤ��� */
    char ret_s_stk[LEN_RET_S_STK];		/* result for temp string */
    char *ret_s_heap = NULL;
    size_t len_ret_s_buf = LEN_RET_S_STK;
    char *ret_s = ret_s_stk;
    size_t ix_ret_s = 0;			/* current index of ret_s */
    size_t ix0_ret_s = 0;			/* first pos of current elem */

    union _ret_p {
	char *ptr;
	size_t offset;
    };
    union _ret_p ret_p_stk[LEN_RET_P_STK];	/* result for temp str ptrs */
    union _ret_p *ret_p_heap = NULL;
    size_t len_ret_p_buf = LEN_RET_P_STK;
    union _ret_p *ret_p = ret_p_stk;
    size_t ix_ret_p = 0;			/* current index of ret_p */

    bool reg_delims = false;
    tstring accepts;
    bool ac_bl;
    int ac_flg = 0;
    size_t i;

    if ( src_str == NULL ) {
	retobjp->assign(src_str, 1);
	goto quit;
    }
    if ( delims == NULL ) delims = "";

    /* check [A-Z] style */
    if ( delims[0] == '[' ) {
	for ( i=0 ; delims[i] != '\0' ; i++ );
	if ( delims[i-1] == ']' ) {
	    size_t p0 = 0;
	    p0 += make_accepts_regexp(delims, true, accepts, &ac_flg, &ac_bl);
	    if ( p0 != 0 ) reg_delims = true;
	}
    }

    /* len_src_str �� 0 �Ǥʤ���硤���餫��­��ʤ����ϥҡ��פ��ڤ��ؤ� */
    if ( LEN_RET_S_STK < len_src_str + 1 ) {
	ret_s_heap = (char *)malloc(len_src_str + 1);
	if ( ret_s_heap == NULL ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	len_ret_s_buf = len_src_str + 1;
	ret_s = ret_s_heap;
    }

    /*
     * setup quotations and bracket info: quot[] bkt_l[] bkt_r[]
     */
    if ( quot_bkt != NULL ) {
	bool b0=false, b1=false, b2=false, b3=false;
	size_t j=0, k=0;
	for ( i=0 ; quot_bkt[i] != '\0' ; i++ ) {
	    const char ch = quot_bkt[i];
	    if ( ch == '[' || ch == ']' ) {
		if ( b0 == false ) {
		    bkt_l[k] = '[';  bkt_r[k] = ']';
		    k++;  b0 = true;
		}
	    }
	    else if ( ch == '{' || ch == '}' ) {
		if ( b1 == false ) {
		    bkt_l[k] = '{';  bkt_r[k] = '}';
		    k++;  b1 = true;
		}
	    }
	    else if ( ch == '(' || ch == ')' ) {
		if ( b2 == false ) {
		    bkt_l[k] = '(';  bkt_r[k] = ')';
		    k++;  b2 = true;
		}
	    }
	    else if ( ch == '<' || ch == '>' ) {
		if ( b3 == false ) {
		    bkt_l[k] = '<';  bkt_r[k] = '>';
		    k++;  b3 = true;
		}
	    }
	    else {
		quot[j] = ch;
		if ( j + 1 < 96 ) j++;
	    }
	}
	quot[j] = '\0';
	bkt_l[k] = '\0';
	bkt_r[k] = '\0';
    }
    else {
	quot[0] = '\0';
	bkt_l[0] = '\0';
	bkt_r[0] = '\0';
    }

    /*
     * parse src_str
     */

    try {
      for ( i=0 ; src_str[i] != '\0' ; i++ ) {
	char ch = src_str[i];
	bool cpy_ok = true;
	if ( prev_escape == true ) {		/* ����chr��escapeʸ���ʤ� */
	    prev_escape = false;
	}
	else if ( ch == escape ) {		/* ����chr��escapeʸ���ʤ� */
	    if ( rm_escape == true ) cpy_ok = false;
	    prev_escape = true;
	}
	else if ( quot_to_find != '\0' ) {	/* �ڥ��� quot �򸫤Ĥ��� */
	    if ( ch == quot_to_find ) quot_to_find = '\0';
	}
	else {
	    size_t j;
	    /* quot �γ��Ϥ򸫤Ĥ��� */
	    for ( j=0 ; quot[j] != '\0' ; j++ ) {
		if ( ch == quot[j] ) {
		    quot_to_find = ch;
		    break;
		}
	    }
	    if ( quot[j] == '\0' ) {
		/* �����γ�̤򸫤Ĥ��� */
		for ( j=0 ; bkt_l[j] != '\0' ; j++ ) {
		    if ( ch == bkt_l[j] ) {
			bkt_to_find.at(depth_bkt) = bkt_r[j];
			depth_bkt ++;
			break;
		    }
		}
	    }
	    if ( bkt_l[j] == '\0' ) {
		/* �ڥ��γ�̤򸫤Ĥ��� */
		if ( 0 < depth_bkt ) {
		    if ( ch == bkt_to_find.at_cs(depth_bkt-1) ) {
			depth_bkt --;
		    }
		}
		/* �褦�䤯�����Ƕ��ڤ�ʸ����Ƚ�� */
		else {
		    if ( reg_delims == true ) {
			if ( is_found(ch, accepts.cstr(), ac_flg) == ac_bl ) {
			    ch = '\0';
			}
		    }
		    else {
			for ( j=0 ; delims[j] != '\0' ; j++ ) {
			    if ( ch == delims[j] ) {
				ch = '\0';
				break;
			    }
			}
		    }
		}
	    }
	}
	if ( cpy_ok == true ) {

	    /* ����Хåե��˳�Ǽ */
	    if ( len_ret_s_buf <= ix_ret_s + 1 ) {
		size_t ii, len_realloc;
		if ( len_src_str == 0 ) {
		    /* src_str��Ĺ���������ξ���Ĵ�٤Ƥ��ޤ� */
		    for ( ii=0 ; src_str[i + ii] != '\0' ; ii++ );
		    len_realloc = i + ii + 1;
		} else len_realloc = len_src_str + 1;

		if ( ret_s_heap == NULL ) {
		    ret_s_heap = (char *)malloc(len_realloc);
		    if ( ret_s_heap == NULL ) {
			err_throw(__FUNCTION__,"FATAL","malloc() failed");
		    }
		    c_memcpy(ret_s_heap, ret_s, len_ret_s_buf);
		}
		else {
		    void *tmp_ptr = realloc(ret_s_heap, len_realloc);
		    if ( tmp_ptr == NULL ) {
			err_throw(__FUNCTION__,"FATAL","realloc() failed");
		    }
		    ret_s_heap = (char *)tmp_ptr;
		}
		ret_s = ret_s_heap;
		len_ret_s_buf = len_realloc;
	    }
	    ret_s[ix_ret_s] = ch;
	    ix_ret_s ++;

	    /* ���ڤ�ʸ�����ä����ν���: ���ɥ쥹�ơ��֥����Ͽ */
	    if ( ch == '\0' ) {
		if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
		    if ( len_ret_p_buf <= ix_ret_p + 2 ) {
			size_t len_realloc = len_ret_p_buf * 2;
			if ( ret_p_heap == NULL ) {
			    ret_p_heap = (union _ret_p *)malloc(sizeof(*ret_p_heap)*len_realloc);
			    if ( ret_p_heap == NULL ) {
				err_throw(__FUNCTION__,"FATAL","malloc() failed");
			    }
			    c_memcpy(ret_p_heap, ret_p, sizeof(*ret_p_heap)*len_ret_p_buf);
			}
			else {
			    void *tmp_ptr = realloc(ret_p_heap, sizeof(*ret_p_heap)*len_realloc);
			    if ( tmp_ptr == NULL ) {
				err_throw(__FUNCTION__,"FATAL","realloc() failed");
			    }
			    ret_p_heap = (union _ret_p *)tmp_ptr;
			}
			ret_p = ret_p_heap;
			len_ret_p_buf = len_realloc;
		    }
		    ret_p[ix_ret_p].offset = ix0_ret_s;
		    ix_ret_p ++;
		}
		ix0_ret_s = ix_ret_s;
	    }
	}
      }
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","caught exception");
    }
    ret_s[ix_ret_s] = '\0';
    ix_ret_s ++;
    if ( zero_str == true || ix0_ret_s + 1 < ix_ret_s ) {
	ret_p[ix_ret_p].offset = ix0_ret_s;
	ix_ret_p ++;
    }
    ret_p[ix_ret_p].ptr = NULL;
    for ( i=0 ; i < ix_ret_p ; i++ ) {
	size_t off = ret_p[i].offset;
	ret_p[i].ptr = ret_s + off;
    }

    try {
	retobjp->erase();
	retobjp->append((char **)ret_p);
    }
    catch (...) {
	if ( ret_p_heap != NULL ) {
	    free(ret_p_heap);
	    ret_p_heap = NULL;
	}
	if ( ret_s_heap != NULL ) {
	    free(ret_s_heap);
	    ret_s_heap = NULL;
	}
	err_throw(__FUNCTION__,"FATAL","this->append() failed");
    }


 quit:
    if ( ret_p_heap != NULL ) free(ret_p_heap);
    if ( ret_s_heap != NULL ) free(ret_s_heap);
    return;
}

/* �ʰ���explode (��®��) */

/**
 * @brief  ���ꤵ�줿ʸ�������ڤ�ʸ�����ʬ�䤷�����η�̤򼫿Ȥ����������
 *
 *  ʸ����src_str����ڤ�ʸ����delim��ʬ�䤷�����η�̤�ʸ��������Ȥ��Ƽ��Ȥ�
 *  �������ޤ���ʸ����Ĺ��������Ǥ�����ޤ�(csv������ʬ��˻Ȥ��ޤ�)��<br>
 *  ����Ū�ˤϡ�::�פΤ褦��2ʸ���ʾ�ζ��ڤ�ʸ����λ��˻Ȥ���ΤǤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delim ���ڤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       �������Ƥ���ꤷ�Ƥ��뤿�ᡤ����Ū�˹�®��ư��ޤ���
 */
tarray_tstring &tarray_tstring::explode(const char *src_str, const char *delim,
					bool zero_str )
{
    explode_php(src_str, 0, delim, zero_str, this);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ�������ڤ�ʸ�����ʬ�䤷�����η�̤򼫿Ȥ����������
 *
 *  ʸ����src_str����ڤ�ʸ����delim��ʬ�䤷�����η�̤�ʸ��������Ȥ��Ƽ��Ȥ�
 *  �������ޤ���ʸ����Ĺ��������Ǥ�����ޤ�(csv������ʬ��˻Ȥ��ޤ�)��<br>
 *  ����Ū�ˤϡ�::�פΤ褦��2ʸ���ʾ�ζ��ڤ�ʸ����λ��˻Ȥ���ΤǤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delim ���ڤ�ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       �������Ƥ���ꤷ�Ƥ��뤿�ᡤ����Ū�˹�®��ư��ޤ���
 */
tarray_tstring &tarray_tstring::explode(const tstring &src_str, const char *delim,
					bool zero_str )
{
    explode_php(src_str.cstr(), src_str.length(), delim, zero_str, this);
    return *this;
}

/* ¿��ǽ��explode */

/*
 * New member functions of 1.3.0
 */

/**
 * @brief  ʸ�������ڤ�ʸ�����ʬ�䤷�����η�̤򼫿Ȥ����������(¿��ǽ��)
 *
 *  ʸ����src_str����ڤ�ʸ����delim��ʬ�䤷�����η�̤�ʸ��������Ȥ��Ƽ��Ȥ�
 *  �������ޤ���ʸ����Ĺ��������Ǥ�����ޤ�(csv������ʬ��˻Ȥ��ޤ�)��<br>
 *  ����Ū�ˤϡ�::�פΤ褦��2ʸ���ʾ�ζ��ڤ�ʸ����λ��˻Ȥ���ΤǤ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quot_bkt �ˡ������ʸ���פ���ꤷ�ޤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delim ���ڤ�ʸ����
 * @param      quot_bkt �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param      escape ����������ʸ��
 * @param      rm_escape ����������ʸ���������뤫�ɤ����Υե饰(true/false)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray_tstring &tarray_tstring::explode(const char *src_str, const char *delim,
	      bool zero_str, const char *quot_bkt, int escape, bool rm_escape )
{
    explode_advanced(src_str, 0, delim, zero_str,
		     quot_bkt, escape, rm_escape, this);
    return *this;
}

/**
 * @brief  ʸ�������ڤ�ʸ�����ʬ�䤷�����η�̤򼫿Ȥ����������(¿��ǽ��)
 *
 *  ʸ����src_str����ڤ�ʸ����delim��ʬ�䤷�����η�̤�ʸ��������Ȥ��Ƽ��Ȥ�
 *  �������ޤ���ʸ����Ĺ��������Ǥ�����ޤ�(csv������ʬ��˻Ȥ��ޤ�)��<br>
 *  ����Ū�ˤϡ�::�פΤ褦��2ʸ���ʾ�ζ��ڤ�ʸ����λ��˻Ȥ���ΤǤ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quot_bkt �ˡ������ʸ���פ���ꤷ�ޤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delim ���ڤ�ʸ����
 * @param      quot_bkt �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param      escape ����������ʸ��
 * @param      rm_escape ����������ʸ���������뤫�ɤ����Υե饰(true/false)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray_tstring &tarray_tstring::explode(const tstring &src_str, const char *delim,
		bool zero_str, const char *quot_bkt, int escape, bool rm_escape )
{
    explode_advanced(src_str.cstr(), src_str.length(), delim, 
		     zero_str, quot_bkt, escape, rm_escape, this);
    return *this;
}


/* �ʰ���split (��®��) */

/**
 * @brief  ���ꤵ�줿ʸ�������ڤ�ʸ����ʬ�䤷�����η�̤򼫿Ȥ����������
 *
 *  ʸ���� src_str ����ڤ�ʸ�� delims ��ʬ�䤷�����η�̤�ʸ��������Ȥ���
 *  ���Ȥ��������ޤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delims ���ڤ�ʸ�����ޤ�ʸ����[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @param      zero_str ʬ����Ĺ��0 ��ʸ�������Ǥ�������ݤ�(true/false)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       �������Ƥ���ꤷ�Ƥ��뤿�ᡤ����Ū�˹�®��ư��ޤ���
 *
 */
tarray_tstring &tarray_tstring::split( const char *src_str,
				       const char *delims, bool zero_str )
{
    split_simple( src_str, 0, delims, zero_str, this );
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ�������ڤ�ʸ����ʬ�䤷�����η�̤򼫿Ȥ����������
 *
 *  ʸ���� src_str ����ڤ�ʸ�� delims ��ʬ�䤷�����η�̤�ʸ��������Ȥ���
 *  ���Ȥ��������ޤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delims ���ڤ�ʸ�����ޤ�ʸ����[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @param      zero_str ʬ����Ĺ��0 ��ʸ�������Ǥ�������ݤ�(true/false)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @note       �������Ƥ���ꤷ�Ƥ��뤿�ᡤ����Ū�˹�®��ư��ޤ���
 *
 */
tarray_tstring &tarray_tstring::split( const tstring &src_str,
				       const char *delims, bool zero_str )
{
    split_simple( src_str.cstr(), src_str.length(), delims, zero_str, this );
    return *this;
}


/* ¿��ǽ��split */

/*
 * ʸ���� line ����ڤ�ʸ�� delims ��ʬ�䤹�롥[A-Z] �Ȥ��ä�ɽ���� OK��
 * [...]��ɽ���ˤĤ��Ƥϡ�tstring ���饹�� strpbrk() �򻲾ȤΤ��ȡ�
 *
 * �������ơ������ȥ���������ʸ���� quot_bkt �� escape �����ꤹ�롥
 *
 * [ʬ�����]
 *   foo name="bar \"hoge\" <xyz>"
 *   �� ��foo�ס�name="bar \"hoge\" <xyz>"�� ��ʬ��
 *
 * zero_str �� length=0 ��ʸ�����������ɤ�����true �ξ�硤"a,,c" �ξ�硤
 * "a", "", "c" �Τ褦��ʬ�䤵��롥
 *
 * quot_bkt �� Version 1.3.0 ����֥饱�åȤˤ��б����������Τ褦��ʬ���OK
 *   1,2,{3,4,5}
 *   �� ��1�ס�2�ס�{3,4,5}��
 * �֥饱�åȤμ���ϡ�[] {} () <> ��4����� quot_bkt �� "[]{}" �Τ褦�˻���
 * ���롥���̤Υ������ơ������Ⱥ����ơ�"'\"[]{}" �Τ褦�ˤ��Ƥ��ɤ�������
 * ���ϡ��������ơ�������ͥ���٤��⤤��ΤȤ��ư����롥
 *
 * split() �κǽ�ΰ����ϡ�NULL �Ǥ��ɤ������ξ�硤el=cstringarray() �Ȥ���
 * ��硤el[0] �� "" �ˤʤ롥
 *
 * [������1]
 *   tarray_tstring sobj;
 *   sobj.split(str, " \t", false, "\"", '\\');
 *   for ( i=0 ; i < sobj.length() ; i++ ) {
 *       printf("%d: [%s]\n", i, sobj.cstr(i));
 *   }
 */

/**
 * @brief  ʸ�������ڤ�ʸ����ʬ�䤷�����η�̤򼫿Ȥ����������(¿��ǽ��)
 *
 *  ʸ���� src_str ����ڤ�ʸ�� delims ��ʬ�䤷�����η�̤�ʸ��������Ȥ���
 *  ���Ȥ��������ޤ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quot_bkt �ˡ������ʸ���פ���ꤷ�ޤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delims ���ڤ�ʸ�����ޤ�ʸ����[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @param      zero_str ʬ����Ĺ��0 ��ʸ�������Ǥ�������ݤ�(true/false)
 * @param      quot_bkt �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param      escape ����������ʸ��
 * @param      rm_escape ����������ʸ���������뤫�ɤ����Υե饰(true/false)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::split( const char *src_str, const char *delims,
				       bool zero_str, const char *quot_bkt,
				       int escape, bool rm_escape )
{
    split_advanced( src_str, 0, delims, zero_str, 
		    quot_bkt, escape, rm_escape, this );
    return *this;
}

/**
 * @brief  ʸ�������ڤ�ʸ����ʬ�䤷�����η�̤򼫿Ȥ����������(¿��ǽ��)
 *
 *  ʸ���� src_str ����ڤ�ʸ�� delims ��ʬ�䤷�����η�̤�ʸ��������Ȥ���
 *  ���Ȥ��������ޤ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quot_bkt �ˡ������ʸ���פ���ꤷ�ޤ���
 *
 * @param      src_str ʬ���оݤ�ʸ����
 * @param      delims ���ڤ�ʸ�����ޤ�ʸ����[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @param      zero_str ʬ����Ĺ��0 ��ʸ�������Ǥ�������ݤ�(true/false)
 * @param      quot_bkt �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param      escape ����������ʸ��
 * @param      rm_escape ����������ʸ���������뤫�ɤ����Υե饰(true/false)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::split( const tstring &src_str, 
				       const char *delims,
				       bool zero_str, const char *quot_bkt,
				       int escape, bool rm_escape )
{
    split_advanced( src_str.cstr(), src_str.length(), delims, zero_str, 
		    quot_bkt, escape, rm_escape, this );
    return *this;
}


/* regassign */

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, const char *pat )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, 0, this->regex_rec, NULL);
    return *this;
}    

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const char *pat )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      nextpos �����pos
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const char *pat, size_t *nextpos )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign(const tstring &src_str, const char *pat)
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, 0, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const char *pat )
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      nextpos �����pos
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const char *pat, size_t *nextpos )
{
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign(const char *src_str, const tstring &pat)
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, 0, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const tstring &pat )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      nextpos �����pos
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					  const tstring &pat, size_t *nextpos )
{
    tstring one = src_str;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(one, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, 
					   const tstring &pat)
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, 0, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const tstring &pat )
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      nextpos �����pos
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					  const tstring &pat, size_t *nextpos )
{
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    this->regexp_match_advanced(src_str, pos, this->regex_rec, nextpos);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      tregex ���饹������ɽ������ѥ���ѥ��֥�������
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign(const char *src_str, const tregex &pat)
{
    tstring one = src_str;
    this->regexp_match_advanced(one, 0, pat, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      tregex ���饹������ɽ������ѥ���ѥ��֥�������
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const tregex &pat )
{
    tstring one = src_str;
    this->regexp_match_advanced(one, pos, pat, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      tregex ���饹������ɽ������ѥ���ѥ��֥�������
 * @param      nextpos �����pos
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const char *src_str, size_t pos,
					   const tregex &pat, size_t *nextpos )
{
    tstring one = src_str;
    this->regexp_match_advanced(one, pos, pat, nextpos);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      tregex ���饹������ɽ������ѥ���ѥ��֥�������
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str,
					   const tregex &pat )
{
    this->regexp_match_advanced(src_str, 0, pat, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      tregex ���饹������ɽ������ѥ���ѥ��֥�������
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const tregex &pat )
{
    this->regexp_match_advanced(src_str, pos, pat, NULL);
    return *this;
}

/**
 * @brief  ����ʸ���������ɽ���ޥå����Ԥ����������Ȥ�ޤ��̤򼫿Ȥ˳�Ǽ
 *
 *  ʸ����src_str���Ф���pat�ǻ��ꤵ�줿POSIX ��ĥ����ɽ��(�ʲ�������ɽ��)
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 *
 * @param      src_str �ޥå����оݤ�ʸ����
 * @param      pos ʸ����ޥå��γ��ϰ���
 * @param      tregex ���饹������ɽ������ѥ���ѥ��֥�������
 * @param      nextpos �����pos
 * @return     ���Ȥλ���
 * @throw      regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      �����˲��򵯤��������
 *
 */
tarray_tstring &tarray_tstring::regassign( const tstring &src_str, size_t pos,
					   const tregex &pat, size_t *nextpos )
{
    this->regexp_match_advanced(src_str, pos, pat, nextpos);
    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
tarray_tstring &tarray_tstring::append( size_t n )
{
    return this->replace(this->length(),0, n);
}

/**
 * @brief  ���ꤵ�줿ʸ��������(const char *[])�򡤼��Ȥ�����κǸ���ɲ�
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param      elements ���Ǥ�����ʸ����Υݥ�������(NULL �ǽ�ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append( const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->append(elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->append(elements,i);
}

/**
 * @brief  ʸ��������(const char *[])��n���Ǥ򡤼��Ȥ�����κǸ���ɲ�
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param      elements ���Ǥ�����ʸ����Υݥ�������
 * @param      n ����elements �����ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append( const char *const *elements, size_t n )
{
    return this->replace(this->length(), 0, elements, n);
}

/**
 * @brief  ʸ��������(tarray_tstring)��(����)���Ǥ򡤼��Ȥ�����κǸ���ɲ�
 *
 *  src�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param     src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param     idx2 src ������Ǥγ��ϰ���
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append(const tarray_tstring &src, size_t idx2)
{
    return this->append(src, idx2, src.length());
}

/**
 * @brief  ʸ��������(tarray_tstring)��(����)���Ǥ򡤼��Ȥ�����κǸ���ɲ�
 *
 *  src�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param    src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param    idx2 src������Ǥγ��ϰ���
 * @param    n2 src ������ǤθĿ�
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append( const tarray_tstring &src, 
					size_t idx2, size_t n2 )
{
    return this->replace(this->length(), 0, src, idx2, n2);
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʣ����ʸ������ɲ�
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʣ����ʸ����򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ... �����Ȥʤ�ʸ����(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append( const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vappend( el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","this->vappend() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʣ����ʸ������ɲ�
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʣ����ʸ����򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ap �����Ȥʤ�ʸ����β���Ĺ�����Υꥹ��(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::vappend( const char *el0, const char *el1,
					 va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->append(tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ����(const char *)��n���ɲ�
 *
 *  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ������������n�Ĥ��ɲä��ޤ���
 *
 * @param      str �����Ȥʤ�ʸ����
 * @param      n �ɲ����ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append( const char *str, size_t n )
{
    return this->replace(this->length(), 0, str, n);
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ����(tstring)��n���ɲ�
 *
 *  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ������������n�Ĥ��ɲä��ޤ���
 *
 * @param      one �����Ȥʤ�ʸ����(�ޥ˥奢��Ǥ�str)
 * @param      n �ɲ����ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::append( const tstring &one, size_t n )
{
    return this->replace(this->length(), 0, one, n);
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ����(printf()�ε�ˡ�ǻ���)��n���ɲ�
 *
 *  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ������������n�Ĥ��ɲä��ޤ���
 *
 * @param      n �ɲ����ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::appendf( size_t n, const char *format, ... )
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vappendf(n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vappendf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ����(printf()�ε�ˡ�ǻ���)��n���ɲ�
 *
 *  ���Ȥ�����κǸ�ˡ����ꤵ�줿ʸ������������n�Ĥ��ɲä��ޤ���
 *
 * @param      n �ɲ����ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ��������ꤵ�줿�Ѵ��ե����ޥåȤ�
 *             �Ѵ��Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::vappendf(size_t n, const char *format, va_list ap)
{
    tstring one;

    if ( format == NULL ) return this->append(format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->append(one,n);

    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
tarray_tstring &tarray_tstring::insert( size_t index, size_t n )
{
    return this->replace(index,0, n);
}

/**
 * @brief  ���ꤵ�줿ʸ��������(const char *[])�򡤼��Ȥ�����λ�����֤�����
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�ʸ��������λ������index��
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      elements �����Ȥʤ�ʸ����Υݥ�������(NULL �ǽ�ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->insert(index,elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->insert(index,elements,i);
}

/**
 * @brief  ʸ��������(const char *[])��n���Ǥ򡤼��Ȥ�����λ�����֤�����
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�ʸ��������λ������index��
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      elements �����Ȥʤ�ʸ����Υݥ�������
 * @param      n ����elements �����ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *const *elements, size_t n )
{
    return this->replace(index, 0, elements, n);
}

/**
 * @brief  ʸ��������(tarray_tstring)��(����)���Ǥ򡤼��Ȥ�����λ�����֤�����
 *
 *  src�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�ʸ��������λ������index���������ޤ���
 *
 * @param     index ���Ȥ��������������
 * @param     src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param     idx2 src ������Ǥγ��ϰ���
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index,
					const tarray_tstring &src, size_t idx2)
{
    return this->insert(index, src, idx2, src.length());
}

/**
 * @brief  ʸ��������(tarray_tstring)��(����)���Ǥ򡤼��Ȥ�����λ�����֤�����
 *
 *  src�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�ʸ��������λ������index���������ޤ���
 *
 * @param     index ���Ȥ��������������
 * @param     src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param     idx2 src ������Ǥγ��ϰ���
 * @param     n2 src ������ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const tarray_tstring &src, 
					size_t idx2, size_t n2 )
{
    return this->replace(index, 0, src, idx2, n2);
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�����λ�����֤�����
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�ʸ��������λ������index��
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ... �����Ȥʤ�ʸ����(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vinsert( index, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsert() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�����λ�����֤�����
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʸ��������򡤼��Ȥ�ʸ��������λ������index��
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ap �����Ȥʤ�ʸ����β���Ĺ�����Υꥹ��(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::vinsert( size_t index, 
				 const char *el0, const char *el1, va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->insert(index, tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  ���Ȥ�����λ�����֤ˡ����ꤵ�줿ʸ����(const char *)��n������
 *
 *  ���Ȥ�ʸ��������������ֹ�index�ΰ��֤ˡ����ꤵ�줿ʸ������������n�Ĥ�
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      str �����Ȥʤ�ʸ����
 * @param      n �ɲ����ǤθĿ�
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const char *str, size_t n )
{
    return this->replace(index, 0, str, n);
}

/**
 * @brief  ���Ȥ�����λ�����֤ˡ����ꤵ�줿ʸ����(tstring)��n������
 *
 *  ���Ȥ�ʸ��������������ֹ�index�ΰ��֤ˡ����ꤵ�줿ʸ������������n�Ĥ�
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      one �����Ȥʤ�ʸ����(�ޥ˥奢��Ǥ�str)
 * @param      n �ɲ����ǤθĿ�
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::insert( size_t index, 
					const tstring &one, size_t n )
{
    return this->replace(index, 0, one, n);
}

/**
 * @brief  ���Ȥ�����λ�����֤ˡ����ꤵ�줿ʸ����(printf()�ε�ˡ)��n������
 *
 *  ���Ȥ�ʸ��������������ֹ�index�ΰ��֤ˡ����ꤵ�줿ʸ������������n�Ĥ�
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      n �ɲ����ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format ���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::insertf( size_t index, 
					 size_t n, const char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vinsertf(index,n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsertf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����λ�����֤ˡ����ꤵ�줿ʸ����(printf()�ε�ˡ)��n������
 *
 *  ���Ȥ�ʸ��������������ֹ�index�ΰ��֤ˡ����ꤵ�줿ʸ������������n�Ĥ�
 *  �������ޤ���
 *
 * @param      index ���Ȥ��������������
 * @param      n �ɲ����ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format ���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::vinsertf( size_t index, 
					size_t n, const char *format, va_list ap )
{
    tstring one;

    if ( format == NULL ) return this->insert(index,format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->insert(index,one,n);

    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ� 
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, size_t n2 )
{
    static tstring one(true);

    this->arrs_rec.replace(idx1,n1, one,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ��������(const char *[])���ִ�
 *
 *  ���Ȥ�ʸ��������������ֹ�idx1����n1�Ĥ����Ǥ�elements�ǻ��ꤵ�줿
 *  ʸ����������ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      elements �����Ȥʤ�ʸ����Υݥ�������(NULL �ǽ�ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
					 const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->replace(idx1,n1, elements,0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->replace(idx1,n1, elements,i);
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ��������(const char *[])���ִ�
 *
 *  ���Ȥ�ʸ��������������ֹ�idx1����n1�Ĥ����Ǥ�elements�ǻ��ꤵ�줿
 *  ʸ����������ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      elements �����Ȥʤ�ʸ����Υݥ�������(NULL �ǽ�ü)
 * @param      n2 ����elements �����ǿ����ޤ���src ������ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
				       const char *const *elements, size_t n2 )
{
    size_t i;
    tarray<tstring> tmp_src;

    if ( 0 < n2 && elements == NULL ) return *this;

    /* n2 ��꾮���������б� */
    for ( i=0 ; i < n2 && elements[i] != NULL ; i++ );
    n2 = i;

    /* elements ���ʸ����˼��Ȥ��������Ƥ���ʸ�������äƤ��뤫�ɤ�����
       this->is_my_buffer(elements) �Ǥϴ�����Ĵ�٤��ʤ��Τ��������ԡ����� */
    if ( 0 < n2 ) {
	tmp_src.resize(n2);
	for ( i=0 ; i < n2 ; i++ ) tmp_src[i] = elements[i];
    }

    this->arrs_rec.replace(idx1,n1, tmp_src,0,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ��������(tarray_tstring)���ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ�src�ǻ��ꤵ�줿ʸ���������
 *  �ִ����ޤ���
 *
 * @param     idx1 ���Ȥ�����γ��ϰ���
 * @param     n1 �ִ���������ǿ�
 * @param     src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param     idx2 src ������Ǥγ��ϰ���
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
				       const tarray_tstring &src, size_t idx2 )
{
    return this->replace(idx1, n1, src, idx2, src.length());
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ��������(tarray_tstring)���ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ�src�ǻ��ꤵ�줿ʸ���������
 *  �ִ����ޤ���
 *
 * @param     idx1 ���Ȥ�����γ��ϰ���
 * @param     n1 �ִ���������ǿ�
 * @param     src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param     idx2 src������Ǥγ��ϰ���
 * @param     n2 src������ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1,
					 const tarray_tstring &src, 
					 size_t idx2, size_t n2 )
{
    this->arrs_rec.replace(idx1,n1, src.arrs_rec,idx2,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʣ����ʸ������ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤲��Ѱ����ǻ��ꤵ�줿
 *  ʣ����ʸ����ǻ��ꤵ�줿ʸ����������ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ... �����Ȥʤ�ʸ����(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, 
					const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vreplace( idx1, n1, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vreplace() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʣ����ʸ������ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤲��Ѱ����ǻ��ꤵ�줿
 *  ʣ����ʸ����ǻ��ꤵ�줿ʸ����������ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ap �����Ȥʤ�ʸ����β���Ĺ�����Υꥹ��(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::vreplace( size_t idx1, size_t n1, 
				 const char *el0, const char *el1, va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->replace(idx1,n1, tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ����(const char *)���ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤻��ꤵ�줿ʸ������������
 *  n2�Ĥ��ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      str �����Ȥʤ�ʸ����
 * @param      n2 ���ꤵ�줿ʸ����������������ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, 
					 const char *str, size_t n2 )
{
    tstring one = str;
    return this->replace(idx1,n1, one,n2);
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ����(tstring)���ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤻��ꤵ�줿ʸ������������
 *  n2�Ĥ��ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      one �����Ȥʤ�ʸ����(�ޥ˥奢��Ǥ�str)
 * @param      n2 ���ꤵ�줿ʸ����������������ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::replace( size_t idx1, size_t n1, 
					 const tstring &one, size_t n2 )
{
    if ( one.cstr() != NULL ) {
	this->arrs_rec.replace(idx1,n1, one,n2);

	this->update_cstr_ptrs_rec();
    }
    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ����(printf()�ε�ˡ)���ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤻��ꤵ�줿ʸ������������
 *  n2�Ĥ��ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      n2 ���ꤵ�줿ʸ����������������ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ����������ꤵ�줿�Ѵ��ե����ޥåȤ�
 *             �Ѵ��Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::replacef( size_t idx1, size_t n1, 
					  size_t n2, const char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vreplacef(idx1,n1,n2,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vreplacef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿ʸ����(printf()�ε�ˡ)���ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤻��ꤵ�줿ʸ������������
 *  n2�Ĥ��ִ����ޤ���
 *
 * @param      idx1 ���Ȥ�����γ��ϰ���
 * @param      n1 �ִ���������ǿ�
 * @param      n2 ���ꤵ�줿ʸ����������������ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ����������ꤵ�줿�Ѵ��ե����ޥåȤ�
 *             �Ѵ��Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::vreplacef( size_t idx1, size_t n1, size_t n2,
					   const char *format, va_list ap )
{
    tstring one;

    if ( format == NULL ) return this->replace(idx1,n1,format,n2);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->replace(idx1,n1,one,n2);

    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
tarray_tstring &tarray_tstring::put( size_t index, size_t n )
{
    static tstring one(true);

    this->arrs_rec.put(index, one,n);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ��������(const char *[])�򡤼��Ȥ�����λ�����֤˾��
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ���������ǰ���index������
 *  ���ޤ���
 *
 * @param      index ���Ȥ�����ν񤭹��߰���
 * @param      elements �����Ȥʤ�ʸ����Υݥ�������(NULL�ǽ�ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const char *const *elements )
{
    size_t i;
    if ( elements == NULL ) return this->put(index,elements, 0);
    for ( i=0 ; elements[i] != NULL ; i++ );
    return this->put(index,elements,i);
}

/**
 * @brief  ���ꤵ�줿ʸ��������(const char *[])�򡤼��Ȥ�����λ�����֤˾��
 *
 *  elements�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ���������ǰ���index������
 *  ���ޤ���
 *
 * @param      index ���Ȥ�����ν񤭹��߰���
 * @param      elements �����Ȥʤ�ʸ����Υݥ�������
 * @param      n ����elements �����ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const char *const *elements, size_t n )
{
    heap_mem<const char *> tmp_ptbl;
    size_t i;

    if ( 0 < n && elements == NULL ) return *this;

    /* n ��꾮���������б� */
    for ( i=0 ; i < n && elements[i] != NULL ; i++ );
    n = i;

    if ( 0 < n && this->is_my_buffer(elements) == true ) {
	/* elements �� this->cstr_ptrs_rec ͳ��ξ���
	   ���ɥ쥹�ơ��֥�򥳥ԡ����� (step1 �ǤΥꥵ�����Τ���) */
	if ( tmp_ptbl.allocate(n) < 0 ) {
	    err_throw(__FUNCTION__,"FATAL","malloc() failed");
	}
	tmp_ptbl.copyfrom(elements, n);
	elements = tmp_ptbl.ptr();
    }

    if ( this->length() < index + n ) {
	this->replace(this->length(),0, (index + n) - this->length());
    }
    this->replace(index, n, elements, n);

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ��������(tarray_tstring)�򡤼��Ȥ�����λ�����֤˾��
 *
 *  src�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ���������ǰ���index�����񤭤��ޤ���
 *
 * @param    index ���Ȥ�����ν񤭹��߰���
 * @param    src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param    idx2 src������Ǥγ��ϰ���
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index,
				     const tarray_tstring &src, size_t idx2)
{
    return this->put(index, src, idx2, src.length());
}

/**
 * @brief  ���ꤵ�줿ʸ��������(tarray_tstring)�򡤼��Ȥ�����λ�����֤˾��
 *
 *  src�ǻ��ꤵ�줿ʸ��������򡤼��Ȥ���������ǰ���index�����񤭤��ޤ���
 *
 * @param    index ���Ȥ�����ν񤭹��߰���
 * @param    src �����Ȥʤ�ʸ������������tarray_tstring ���饹�Υ��֥�������
 * @param    idx2 src������Ǥγ��ϰ���
 * @param    n2 src ������ǤθĿ�
 * @return   ���Ȥλ���
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const tarray_tstring &src, 
				     size_t idx2, size_t n2 )
{
    this->arrs_rec.put(index, src.arrs_rec,idx2,n2);

    this->update_cstr_ptrs_rec();

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�����λ�����֤˾��
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʣ����ʸ����򡤼��Ȥ���������ǰ��� index ����
 *  ��񤭤��ޤ���
 *
 * @param      idx1 ���Ȥ�����ν񤭹��߰���
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ... �����Ȥʤ�ʸ����(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::put( size_t index, 
				     const char *el0, const char *el1, ... )
{
    va_list ap;
    va_start(ap, el1);
    try {
	this->vput( index, el0, el1, ap );
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vput() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�����λ�����֤˾��
 *
 *  ���Ѱ����ǻ��ꤵ�줿ʣ����ʸ����򡤼��Ȥ���������ǰ��� index ����
 *  ��񤭤��ޤ���
 *
 * @param      idx1 ���Ȥ�����ν񤭹��߰���
 * @param      el0 �����Ȥʤ�ʸ����(0 ����)
 * @param      el1 �����Ȥʤ�ʸ����(1 ����)
 * @param      ap �����Ȥʤ�ʸ����β���Ĺ�����Υꥹ��(2 ���ܰʹ�; ��NULL ��ü)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::vput( size_t index, 
				 const char *el0, const char *el1, va_list ap )
{
    heap_mem<const char *> tmp_ptbl;
    size_t element_count, i;
    va_list aq;

    va_copy(aq, ap);

    element_count = 0;
    if ( el0 != NULL ) {
        element_count++;
	if ( el1 != NULL ) {
	    element_count++;
	    while ( 1 ) {
	        const char *elem = va_arg(aq,char *);
		if ( elem == NULL ) break;
		element_count++;
	    }
	}
    }
    va_end(aq);

    if ( tmp_ptbl.allocate(element_count + 1) < 0 ) {
	err_throw(__FUNCTION__,"FATAL","malloc() failed");
    }

    i = 0;
    if ( el0 != NULL ) {
        tmp_ptbl[i] = el0;
        i++;
	if ( el1 != NULL ) {
	    tmp_ptbl[i] = el1;
	    i++;
	    while ( 1 ) {
	        const char *elem = va_arg(ap,char *);
		if ( elem == NULL ) break;
		tmp_ptbl[i] = elem;
		i++;
	    }
	}
    }
    tmp_ptbl[i] = NULL;

    this->put(index, tmp_ptbl.ptr());

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ����(const char *)�ǡ����Ȥ�����λ�����֤���n�ľ��
 *
 *  ���Ȥ�����������ֹ�index�ΰ��֤��顤���ꤵ�줿ʸ������������n�Ĥ�
 *  ��񤭤��ޤ���
 *
 * @param      index ���Ȥ�����ν񤭹��߰���
 * @param      str �����Ȥʤ�ʸ����
 * @param      n ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::put( size_t index, const char *str, size_t n )
{
    tstring one = str;
    return this->put(index, one, n);
}

/**
 * @brief  ���ꤵ�줿ʸ����(tstring)�ǡ����Ȥ�����λ�����֤���n�ľ��
 *
 *  ���Ȥ�����������ֹ�index�ΰ��֤��顤���ꤵ�줿ʸ������������n�Ĥ�
 *  ��񤭤��ޤ���
 *
 * @param      index ���Ȥ�����ν񤭹��߰���
 * @param      one �����Ȥʤ�ʸ����(�ޥ˥奢��Ǥ�str)
 * @param      n ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::put(size_t index, const tstring &one, size_t n)
{
    if ( one.cstr() != NULL ) {
	this->arrs_rec.put(index, one,n);

	this->update_cstr_ptrs_rec();
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ����(printf()�ε�ˡ)�ǡ�����λ�����֤���n�ľ��
 *
 *  ���Ȥ�����������ֹ�index�ΰ��֤��顤���ꤵ�줿ʸ������������n�Ĥ�
 *  ��񤭤��ޤ���
 *
 * @param      index ���Ȥ�����ν񤭹��߰���
 * @param      n ���ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ... format���б���������Ĺ�����γ����ǥǡ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ����������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::putf( size_t index, 
				      size_t n, const char *format, ...)
{
    va_list ap;
    va_start(ap,format);
    try {
	this->vputf(index,n,format,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vputf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ����(printf()�ε�ˡ)�ǡ�����λ�����֤���n�ľ��
 *
 *  ���Ȥ�����������ֹ�index�ΰ��֤��顤���ꤵ�줿ʸ������������n�Ĥ�
 *  ��񤭤��ޤ���
 *
 * @param      index ���Ȥ�����ν񤭹��߰���
 * @param      n ���ǤθĿ�
 * @param      format �����Ȥʤ�ʸ����Τ���Υե����ޥåȻ���
 * @param      ap format ���б���������Ĺ�����Υꥹ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * @throw      ����Ĺ�����γ����ǥǡ����������ꤵ�줿�Ѵ��ե����ޥåȤ��Ѵ�
 *             �Ǥ��ʤ��ͤξ��
 *
 */
tarray_tstring &tarray_tstring::vputf( size_t index, size_t n,
				       const char *format, va_list ap )
{
    tstring one;

    if ( format == NULL ) return this->put(index,format,n);

    try {
	one.vprintf(format,ap);
    }
    catch (...) {
	err_throw(__FUNCTION__,"FATAL","one.vprintf() failed");
    }
    this->put(index,one,n);

    return *this;
}

/**
 * @brief  �����������ʬ�ξõ�
 *
 *  ���Ȥ�����������ֹ� idx ���� len �Ĥ����Ǥ����ˤ��ޤ���
 *
 * @param      idx �ڤ�Ф����Ǥγ��ϰ���
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::crop( size_t idx, size_t len )
{
    size_t max_n;
    if ( this->length() < idx ) idx = this->length();
    max_n = this->length() - idx;
    if ( max_n < len ) len = max_n;
    this->erase(0, idx);
    this->erase(len, this->length() - len);
    return *this;
}

/**
 * @brief  �����������ʬ�ξõ�
 *
 *  ���Ȥ�����������ֹ�idx �ʹߤ���������ˤ��ޤ���
 *
 * @param      idx �ڤ�Ф����Ǥγ��ϰ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::crop( size_t idx )
{
    if ( this->length() < idx ) idx = this->length();
    return this->crop(idx, this->length() - idx);
}

/**
 * @brief  ���������Ǥκ��
 *
 *  ���Ȥ�����ʸ��������������Ǥ������ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::erase()
{
    return this->erase(0,this->length());
}

/**
 * @brief  �������Ǥκ��
 *
 *  ���Ȥ�����ʸ������������Ǥ������ޤ���
 *
 * @param      index �����ֹ�
 * @param      num_elements ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::erase( size_t index, size_t num_elements )
{
    return this->replace(index,num_elements, (size_t)0);
}

/**
 * @brief  ����Ĺ���ѹ�
 *
 *  ���Ȥ�ʸ���������Ĺ���� new_num_elements ���ѹ����ޤ���
 *
 * @param      new_num_elements �ѹ����ʸ��������Ĺ
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::resize( size_t new_num_elements )
{
    if ( new_num_elements < this->length() ) {
        this->replace( new_num_elements, 
		       this->length() - new_num_elements,
		       (size_t)0 );
    }
    else {
	this->replace( this->length(), (size_t)0,
		       new_num_elements - this->length() );
    }
    return *this;
}

/**
 * @brief  ����Ĺ������Ū���ѹ�
 *
 *  ���Ȥ�ʸ���������Ĺ���� len ��Ĺ��ʬ�����ѹ����ޤ���
 *
 * @param      len ����Ĺ����ʬ����ʬ
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
tarray_tstring &tarray_tstring::resizeby( ssize_t len )
{
    size_t new_len, a_len;
    a_len = ((len < 0) ? -len : len);
    if ( len < 0 ) {
	if ( a_len < this->length() ) 
	    new_len = this->length() - a_len;
	else
	    new_len = 0;
    }
    else {
	new_len = this->length() + a_len;
    }

    return this->resize(new_len);
}

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�ʸ��������Τ��٤Ƥޤ��ϰ�����dest �ǻ��ꤵ�줿���֥������Ȥ�
 *  ���ԡ����ޤ���
 *
 * @param      index ���ԡ���(����)���������Ǥΰ���
 * @param      n ���ԡ��������ǿ�
 * @param      dest ���ԡ����tarray_tstring ���饹�Υ��֥�������
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
ssize_t tarray_tstring::copy( size_t index, size_t n,
			      tarray_tstring *dest ) const
{
    if ( dest == NULL ) return -1;

    size_t len = this->length();

    if ( len < index ) {
	dest->erase();
	return -1;
    }
    if ( this->cstrarray() == NULL ) {
	dest->erase();
	return 0;
    }

    if ( len - index < n ) n = len - index;

    dest->replace(0, dest->length(), *this, index, n);

    return n;
}

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�ʸ��������Τ��٤Ƥޤ��ϰ�����dest �ǻ��ꤵ�줿���֥������Ȥ�
 *  ���ԡ����ޤ���
 *
 * @param      index ���ԡ���(����)���������Ǥΰ���
 * @param      dest ���ԡ����tarray_tstring ���饹�Υ��֥�������
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
ssize_t tarray_tstring::copy( size_t index, tarray_tstring *dest ) const
{
    if ( dest == NULL ) return -1;
    return this->copy(index, this->length(), dest);
}

/**
 * @brief  ���Ȥ����Ƥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�ʸ��������Τ��٤Ƥ�dest �ǻ��ꤵ�줿���֥������Ȥ˥��ԡ����ޤ���
 *
 * @param      dest ���ԡ����tarray_tstring ���饹�Υ��֥�������
 * @return     �����������������<br>
 *             ���Ԥ�����������
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
ssize_t tarray_tstring::copy( tarray_tstring *dest ) const
{
    if ( dest == NULL ) return -1;
    return this->copy(0, this->length(), dest);
}

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��<br>
 * ssize_t tarray_tstring::copy(size_t, size_t, tarray_tstring *) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t tarray_tstring::copy( size_t index, size_t n, 
			      tarray_tstring &dest ) const
{
    return this->copy(index, n, &dest);
}

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��<br>
 * ssize_t tarray_tstring::copy(size_t, tarray_tstring *) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t tarray_tstring::copy( size_t index, tarray_tstring &dest ) const
{
    return this->copy(index, this->length(), &dest);
}

/**
 * @brief  ���Ȥ����Ƥ��̥��֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��<br>
 * ssize_t tarray_tstring::copy(tarray_tstring *) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t tarray_tstring::copy( tarray_tstring &dest ) const
{
    return this->copy(0, this->length(), &dest);
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���֥�������sobj �����Ƥȼ��Ȥ����ƤȤ������ؤ��ޤ���
 *
 * @param      sobj ���Ƥ������ؤ��� tarray_tstring ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * 
 */
tarray_tstring &tarray_tstring::swap( tarray_tstring &sobj )
{
    if ( &sobj == this ) return *this;

    this->arrs_rec.swap(sobj.arrs_rec);
    this->cstr_ptrs_rec.swap(sobj.cstr_ptrs_rec);
    this->reg_pos_rec.swap(sobj.reg_pos_rec);
    this->reg_length_rec.swap(sobj.reg_length_rec);
    this->regex_rec.swap(sobj.regex_rec);

    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str �򸡺�����
 *  ���Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strreplace( const char *org_str,
					    const char *new_str, bool all )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).strreplace(org_str,new_str,all);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str �򸡺�����
 *  ���Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strreplace( const tstring &org_str,
					    const char *new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str, all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str �򸡺�����
 *  ���Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strreplace( const char *org_str,
					    const tstring &new_str, bool all )
{
    return this->strreplace(org_str, new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str �򸡺�����
 *  ���Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 *
 * @param      org_str ���Ф���ʸ����
 * @param      new_str �ִ��θ����Ȥʤ�ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strreplace( const tstring &org_str,
					    const tstring &new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ʸ���ν���
 *
 *  ���Ȥ����������Ǥ�������ʸ��������ޤ���
 * 
 * @return ���Ȥλ���
 * @throw �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray_tstring &tarray_tstring::chop()
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).chop();
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥα�ü�β���ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������Ǥα�ü�β���ʸ�������ޤ���
 *
 * @param      rs ����ʸ���� (��ά��)
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::chomp( const char *rs )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).chomp(rs);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥα�ü�β���ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������Ǥα�ü�β���ʸ�������ޤ���
 *
 * @param      rs ����ʸ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::chomp( const tstring &rs )
{
    return this->chomp(rs.cstr());
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::trim( const char *side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).trim(side_spaces);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::trim( const tstring &side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).trim(side_spaces);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::trim( int side_space )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).trim(side_space);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::ltrim( const char *side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).ltrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::ltrim( const tstring &side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).ltrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::ltrim( int side_space )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).ltrim(side_space);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::rtrim( const char *side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).rtrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::rtrim( const tstring &side_spaces )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).rtrim(side_spaces);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::rtrim( int side_space )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).rtrim(side_space);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strtrim( const char *side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strtrim( const tstring &side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param      side_space �����оݤ�ʸ��
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::strtrim( int side_space )
{
    return this->trim(side_space);
}

/**
 * @brief  ����������ǤˤĤ�����ʸ����ʸ�����Ѵ�
 *
 *  ���Ȥ�ʸ��������������ǤΥ���ե��٥åȤ���ʸ����ʸ�����Ѵ����ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::tolower()
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).tolower();
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��ƾ�ʸ������ʸ�����Ѵ�
 *
 *  ���Ȥ�ʸ��������������ǤΥ���ե��٥åȤξ�ʸ������ʸ�����Ѵ����ޤ���
 *
 * @return     ���Ȥλ���
 *
 */
tarray_tstring &tarray_tstring::toupper()
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).toupper();
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��ƥ���ʸ�����·�����ƶ���ʸ�����ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ���ʿ����ʸ�� '\t' ��tab_width ���ͤ�
 *  ��·���򤷤ƶ���ʸ�����ִ����ޤ���
 *
 * @param      tab_width ����������ά����8
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::expand_tabs( size_t tab_width )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).expand_tabs(tab_width);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��ƶ���ʸ�����·�����ƥ���ʸ�����ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�2ʸ���ʾ�Ϣ³��������ʸ�� ' ' ���٤Ƥ�
 *  �оݤˤ������ꤵ�줿������ tab_width �Ƿ�·������ '\t' ���ִ����ޤ���
 *
 * @param      tab_width ����������ά����8
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::contract_spaces( size_t tab_width )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).contract_spaces(tab_width);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  (�ʲ�������ɽ��) �ǥޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::regreplace( const char *pat, 
					    const char *new_str, bool all )
{
    size_t i;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).regreplace(this->regex_rec, new_str, all);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  (�ʲ�������ɽ��) �ǥޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tstring &pat, 
					    const char *new_str, bool all )
{
    size_t i;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).regreplace(this->regex_rec, new_str, all);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  (�ʲ�������ɽ��) �ǥޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      new_str �ִ����ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tregex &pat, 
					    const char *new_str, bool all )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).regreplace(pat,new_str,all);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  (�ʲ�������ɽ��) �ǥޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::regreplace( const char *pat, 
					    const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  (�ʲ�������ɽ��) �ǥޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat ʸ���ѥ�����(����ɽ��)
 * @param      new_str �ִ����ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tstring &pat, 
					    const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  (�ʲ�������ɽ��) �ǥޥå�������ʬ��ʸ���� new_str ���֤������ޤ���
 *
 * @param      pat tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param      new_str �ִ����ʸ����
 * @param      all ���ִ��Υե饰
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
tarray_tstring &tarray_tstring::regreplace( const tregex &pat, 
					    const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_elem( const char *str ) const
{
    return this->find_elem((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const char *str ) const
{
    return this->find_elem(idx, str, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū�ʺ�¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const char *str, 
				   size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).compare(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_elem( const tstring &str ) const
{
    return this->find_elem(str.cstr());
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const tstring &str ) const
{
    return this->find_elem(idx, str.cstr());
}

/**
 * @brief  Ϣ³Ū�ʺ�¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_elem( size_t idx, const tstring &str,
				   size_t *nextidx ) const
{
    return this->find_elem(idx, str.cstr(), nextidx);
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind_elem( const char *str ) const
{
    if ( this->length() < 1 ) return -1;
    return this->rfind_elem(this->length() - 1, str, (size_t *)NULL);
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const char *str ) const
{
    return this->rfind_elem(idx, str, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū�ʱ�¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const char *str,
				    size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx+1 ; 0 < i ; ) {
	    i--;
	    if ( this->at_cs(i).compare(str) == 0 ) {
		if ( nextidx != NULL ) {
		    if ( 0 < i ) *nextidx = i - 1;
		    else *nextidx = this->length();
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind_elem( const tstring &str ) const
{
    return this->rfind_elem(str.cstr());
}

/**
 * @brief  ��¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const tstring &str ) const
{
    return this->rfind_elem(idx, str.cstr());
}

/**
 * @brief  Ϣ³Ū�ʱ�¦������������Ǥδ������׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str �˴����˰��פ������Ǥ򸡺�����
 *  ���Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind_elem( size_t idx, const tstring &str,
				    size_t *nextidx ) const
{
    return this->rfind_elem(idx, str.cstr(), nextidx);
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find( const char *str, ssize_t *pos_r ) const
{
    return this->find((size_t)0, (size_t)0, str, 
		      pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const char *str,
			      ssize_t *pos_r ) const
{
    return this->find(idx, pos, str, pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū�ʺ�¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const char *str,
			      ssize_t *pos_r,
			      size_t *nextidx, size_t *nextpos ) const
{
    ssize_t f_pos = -1;
    size_t a_length = this->length();
    if ( idx < a_length && pos <= this->at_cs(idx).length() && str != NULL) {
	size_t i;
	for ( i=idx ; i < a_length ; i++ ) {
	    if ( 0 <= (f_pos=this->at_cs(i).find(pos, str, &pos)) ) {
		if ( pos_r != NULL ) *pos_r = f_pos;
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) *nextidx = i + 1;
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( i + 1 < a_length ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) *nextpos = 0;
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    pos = 0;	/* reset */
	}
    }
    /* not found */
    if ( pos_r != NULL ) *pos_r = f_pos;
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs(a_length - 1).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find( const tstring &str, ssize_t *pos_r ) const
{
    return this->find(str.cstr(), pos_r);
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const tstring &str,
			      ssize_t *pos_r) const
{
    return this->find(idx, pos, str.cstr(), pos_r);
}

/**
 * @brief  Ϣ³Ū�ʺ�¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥκ�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find( size_t idx, size_t pos, const tstring &str,
			      ssize_t *pos_r, 
			      size_t *nextidx, size_t *nextpos) const
{
    return this->find(idx, pos, str.cstr(), pos_r, nextidx, nextpos);
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind( const char *str, ssize_t *pos_r ) const
{
    size_t a_length = this->length();
    if ( a_length < 1 ) return -1;
    return this->rfind(a_length - 1, this->at_cs(a_length - 1).length(), str, 
		       pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const char *str,
			       ssize_t *pos_r ) const
{
    return this->rfind(idx, pos, str, pos_r, (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū�ʱ�¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const char *str,
			       ssize_t *pos_r,
			       size_t *nextidx, size_t *nextpos ) const
{
    ssize_t f_pos = -1;
    size_t a_length = this->length();
    if ( idx < a_length && pos <= this->at_cs(idx).length() && str != NULL) {
	size_t i;
	for ( i=idx+1 ; 0 < i ; ) {
	    i--;
	    if ( 0 <= (f_pos=this->at_cs(i).rfind(pos, str, &pos)) ) {
		if ( pos_r != NULL ) *pos_r = f_pos;
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) {
			if ( 0 < i ) *nextidx = i - 1;
			else *nextidx = a_length;
		    }
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( 0 < i ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) {
			    *nextpos = this->at_cs(i-1).length();
			}
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    if ( 0 < i ) pos = this->at_cs(i-1).length();	/* reset */
	}
    }
    /* not found */
    if ( pos_r != NULL ) *pos_r = f_pos;
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs((size_t)0).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind( const tstring &str, ssize_t *pos_r ) const
{
    return this->rfind(str.cstr(), pos_r);
}

/**
 * @brief  ��¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const tstring &str,
			       ssize_t *pos_r) const
{
    return this->rfind(idx, pos, str.cstr(), pos_r);
}

/**
 * @brief  Ϣ³Ū�ʱ�¦������������Ǥ���ʬ���׸���
 *
 *  ���Ȥ��������Ǥα�¦����ʸ���� str ��ޤ����Ǥ򸡺��������дؿ����֤��ͤ�
 *  ���ơ����Ĥ��ä����Ϥ��������ֹ���֤������Ĥ���ʤ����������֤��ޤ���
 *  Ʊ���ˡ����Ĥ��ä����� pos_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ʸ����ΰ���
 *  ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      str ���Ф���ʸ����
 * @param      pos_r ���Ĥ��ä���硤�������Ǥˤ�����ʸ����ΰ���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::rfind( size_t idx, size_t pos, const tstring &str,
			       ssize_t *pos_r, 
			       size_t *nextidx, size_t *nextpos) const
{
    return this->rfind(idx, pos, str.cstr(), pos_r, nextidx, nextpos);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_str( const char *str ) const
{
    return this->find_matched_str((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_str( size_t idx, const char *str ) const
{
    return this->find_matched_str(idx, str, (size_t *)NULL);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_str( size_t idx, const char *str, 
					  size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).strmatch(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_str( const tstring &str ) const
{
    return this->find_matched_str(str.cstr());
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_str(size_t idx, const tstring &str) const
{
    return this->find_matched_str(idx, str.cstr());
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_str( size_t idx, const tstring &str,
					  size_t *nextidx ) const
{
    return this->find_matched_str(idx, str.cstr(), nextidx);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_fn( const char *str ) const
{
    return this->find_matched_fn((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_fn( size_t idx, const char *str ) const
{
    return this->find_matched_fn(idx, str, (size_t *)NULL);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_fn( size_t idx, const char *str, 
					  size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).fnmatch(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_fn( const tstring &str ) const
{
    return this->find_matched_fn(str.cstr());
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_fn(size_t idx, const tstring &str) const
{
    return this->find_matched_fn(idx, str.cstr());
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ե�����̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_fn( size_t idx, const tstring &str,
					  size_t *nextidx ) const
{
    return this->find_matched_fn(idx, str.cstr(), nextidx);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_pn( const char *str ) const
{
    return this->find_matched_pn((size_t)0, str, (size_t *)NULL);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_pn( size_t idx, const char *str ) const
{
    return this->find_matched_pn(idx, str, (size_t *)NULL);
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_pn( size_t idx, const char *str, 
					  size_t *nextidx ) const
{
    if ( idx < this->length() && str != NULL ) {
	size_t i;
	for ( i=idx ; i < this->length() ; i++ ) {
	    if ( this->at_cs(i).pnmatch(str) == 0 ) {
		if ( nextidx != NULL ) {
		    *nextidx = i + 1;
		}
		return i;
	    }
	}
    }
    /* */
    if ( nextidx != NULL ) *nextidx = this->length();
    return -1;
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_pn( const tstring &str ) const
{
    return this->find_matched_pn(str.cstr());
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_pn(size_t idx, const tstring &str) const
{
    return this->find_matched_pn(idx, str.cstr());
}

/**
 * @brief  ������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����ޥå�(�ѥ�̾����)
 *
 *  ���Ȥ��������Ǥκ�¦�����ˡ�������Υ磻��ɥ����ɥѥ�������Ѥ���ʸ����
 *  �ޥå����Ԥ����ޥå��������������ֹ���֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      str ���Ф��������ͤ˰��פ���ʸ����(�ޥ˥奢��Ǥ�pat)
 * @param      nextidx �����idx
 * @return     �����������������<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::find_matched_pn( size_t idx, const tstring &str,
					  size_t *nextidx ) const
{
    return this->find_matched_pn(idx, str.cstr(), nextidx);
}

#ifdef REGFUNCS_USING_CACHE
/* �����˥��㥹�Ȥ���ؿ� */
static tregex *coerce_into_casting( const tregex *in )
{ 
    return (tregex *)in;
} 
#endif

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( const char *pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch((size_t)0, (size_t)0, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(idx, pos, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū���������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  ssize_t *pos_r, size_t *span_r,
				  size_t *nextidx, size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(idx, pos, *epat_p, pos_r, span_r,
			  nextidx, nextpos);
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( const tstring &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(0, 0, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(idx, pos, pat, pos_r, span_r,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū���������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  ssize_t *pos_r, size_t *span_r,
				  size_t *nextidx, size_t *nextpos ) const
{
    tregex *epat_p;
#ifdef REGFUNCS_USING_CACHE
    epat_p = coerce_into_casting(&(this->regex_rec));
#else
    tregex epat;
    epat_p = &epat;
#endif
    if ( epat_p->compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",epat_p->cerrstr());
    }
    return this->regmatch(idx, pos, *epat_p, pos_r, span_r,
			  nextidx, nextpos);
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( const tregex &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(0, 0, pat, pos_r, span_r, NULL, NULL);
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  ssize_t *pos_r, size_t *span_r ) const
{
    return this->regmatch(idx, pos, pat, pos_r, span_r, NULL, NULL);
}

/**
 * @brief  Ϣ³Ū���������Ǥ��Ф�������ɽ���ޥå�����
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå��������� pos_r��span_r ���ؤ��ѿ��ˡ��������Ǥˤ�����ޥå�����
 *  ��ʬ��ʸ�����֤�Ĺ�����֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      pos_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ������
 * @param      span_r �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ��ʸ����Ĺ
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  ssize_t *pos_r, size_t *span_r,
				  size_t *nextidx, size_t *nextpos ) const
{
    ssize_t f_pos = -1;
    size_t a_length = this->length();
    if ( idx < a_length && pos <= this->at_cs(idx).length() ) {
	size_t i;
	for ( i=idx ; i < a_length ; i++ ) {
	    if (0 <= (f_pos=this->at_cs(i).regmatch(pos, pat, span_r, &pos))) {
		if ( pos_r != NULL ) *pos_r = f_pos;
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) *nextidx = i + 1;
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( i + 1 < a_length ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) *nextpos = 0;
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    pos = 0;	/* reset */
	}
    }
    /* not found */
    if ( pos_r != NULL ) *pos_r = f_pos;
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs(a_length - 1).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( const char *pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(0, 0, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  �������Ǥ��Ф�������ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū���������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring *result,
				  size_t *nextidx, size_t *nextpos )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result, nextidx, nextpos);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( const tstring &pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(0, 0, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring *result )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū���������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring *result,
				  size_t *nextidx, size_t *nextpos )
{
    if ( result == NULL ) return -1;
    if ( this->regex_rec.compile(pat.cstr()) < 0 ) {
	err_report1(__FUNCTION__,"ERROR","%s",this->regex_rec.cerrstr());
    }
    return this->regmatch(idx, pos, this->regex_rec, result, nextidx, nextpos);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( const tregex &pat,
				  tarray_tstring *result ) const
{
    if ( result == NULL ) return -1;
    return this->regmatch(0, 0, pat, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring *result ) const
{
    if ( result == NULL ) return -1;
    return this->regmatch(idx, pos, pat, result,
			  (size_t *)NULL, (size_t *)NULL);
}

/**
 * @brief  Ϣ³Ū���������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������
 *
 *  ���Ȥ��������Ǥκ�¦���顤pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��(�ʲ�������
 *  ɽ��)�˥ޥå�������ʬ��ޤ����Ǥ򸡺��������дؿ����֤��ͤȤ��ơ��ޥå�
 *  �������Ϥ��������ֹ���֤����ޥå����ʤ����������֤��ޤ���Ʊ���ˡ�
 *  �ޥå���������ʸ�������󥪥֥������� result �ˡ��������Ǥˤ�����ޥå�
 *  ������ʬ�θ������Ȥ�ޤ������֤��ޤ���
 *
 * @param      idx �������Ǥθ������ϰ���
 * @param      pos ʸ����θ������ϰ���
 * @param      pat �����˻Ȥ�����ɽ��
 * @param      result �ޥå�������硤�������Ǥˤ�����ޥå�������ʬ�η�̾���
 * @param      nextidx �����idx
 * @param      nextpos �����pos
 * @return     ����������������������ֹ�<br>
 *             ���Ԥ�����������
 *
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring *result,
				  size_t *nextidx, size_t *nextpos ) const
{
    if ( result == NULL ) return -1;
    size_t a_length = this->length();
    if ( result != this && idx < a_length && pos <= this->at_cs(idx).length() ) {
	size_t i;
	for ( i=idx ; i < a_length ; i++ ) {
	    result->regassign(this->at_cs(i), pos, pat, &pos);
	    if ( 0 < result->length() ) {
		if ( nextidx != NULL ) {
		    if ( this->at_cs(i).length() < pos ) *nextidx = i + 1;
		    else *nextidx = i;		/* keep */
		}
		if ( nextpos != NULL ) {
		    if ( i + 1 < a_length ) {	/* not last */
			if ( this->at_cs(i).length() < pos ) *nextpos = 0;
			else *nextpos = pos;
		    }
		    else *nextpos = pos;
		}
		return i;
	    }
	    pos = 0;	/* reset */
	}
    }
    /* not found */
    if ( nextidx != NULL ) *nextidx = a_length;
    if ( nextpos != NULL ) {
	*nextpos = (0 < a_length) ? this->at_cs(a_length - 1).length() + 1 : 1;
    }
    return -1;
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( const char *pat,
				  tarray_tstring &result )
{
    return this->regmatch(pat, &result);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring &result )
{
    return this->regmatch(idx, pos, pat, &result);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const char *pat,
				  tarray_tstring &result,
				  size_t *nextidx, size_t *nextpos )
{
    return this->regmatch(idx, pos, pat, &result, nextidx, nextpos);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( const tstring &pat,
				  tarray_tstring &result )
{
    return this->regmatch(pat, &result);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring &result )
{
    return this->regmatch(idx, pos, pat, &result);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tstring &pat,
				  tarray_tstring &result,
				  size_t *nextidx, size_t *nextpos )
{
    return this->regmatch(idx, pos, pat, &result, nextidx, nextpos);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( const tregex &pat,
				  tarray_tstring &result ) const
{
    return this->regmatch(pat, &result);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring &result ) const
{
    return this->regmatch(idx, pos, pat, &result);
}

/**
 * @brief  �������Ǥؤ�����ɽ���ޥå����Ԥ����������Ȥ�ޤ��������(��侩)
 * @deprecated  ��侩
 */
ssize_t tarray_tstring::regmatch( size_t idx, size_t pos, const tregex &pat,
				  tarray_tstring &result,
				  size_t *nextidx, size_t *nextpos ) const
{
    return this->regmatch(idx, pos, pat, &result, nextidx, nextpos);
}

/**
 * @brief  ����ʸ����γ����Хåե��ؤΥ��ԡ�
 *
 *  index�ǻ��ꤵ�줿���Ǥ�ʸ����򡤳����Хåե��˥��ԡ����ޤ���
 * 
 * @param   index �����ֹ�
 * @param   dest_buf �����Хåե��Υ��ɥ쥹
 * @param   buf_size �����Хåե��Υ�����
 * 
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ���,���ԡ��Ǥ���ʸ������<br>
 *          �����(���顼): dest_str��NULL�����ꤷ��buf_size��0�ʳ����ͤ�����
 *          �������.
 * @throw   ����Ĺ�ʾ��Index�����ꤵ�줿���
 */
ssize_t tarray_tstring::getstr( size_t index, 
				char *dest_buf, size_t buf_size ) const
{
    if ( index < 0 || this->length() <= index ) {
	return -1;
    }
    else {
        return this->arrs_rec.at_cs(index).getstr(dest_buf,buf_size);
    }
}

static void dump_tstring( const tstring &src )
{
    if ( 0 <= src.strchr('"') ) {
	size_t j;
	sli__eprintf("\"");
	for ( j=0 ; j < src.length() ; j++ ) {
	    int ch = src.cchr(j);
	    if ( ch == '"' ) sli__eprintf("\\\"");
	    else sli__eprintf("%c", ch);
	}
	sli__eprintf("\"");
    }
    else {
	sli__eprintf("\"%s\"", src.cstr());
    }
    return;
}

/**
 * @brief  ����ɽ���ޥå���̤Υޥå����֤����
 *
 *  ��¸���줿����ɽ���ޥå���̤Ρ��ޥå�������ʬ�ΰ��֤��֤��ޤ���
 * 
 * @param  idx 0: �ޥå�����ʸ�������Τξ���<br>
 *             1�ʹ�: ����ɽ�����줾��˥ޥå�������ʬʸ����ξ���
 * 
 * @return ��¸���줿�ޥå�������ʬ�ΰ���
 */
size_t tarray_tstring::reg_pos( size_t idx ) const
{
    if ( idx < this->reg_pos_rec.length() )
	return this->reg_pos_rec.z_cs(idx);
    else 
	return 0;
}

/**
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ (̤����)
 * @deprecated  ̤����
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void tarray_tstring::set_scopy_flag()
{
    return;
}

/**
 * @brief  ���֥������Ⱦ����ɸ�२�顼���Ϥؽ���
 *
 *  ���ȤΥ��֥������Ⱦ����ɸ�२�顼���Ϥؽ��Ϥ��ޤ���
 *
 */
void tarray_tstring::dprint( const char *msg ) const
{
    size_t i;
    if ( msg != NULL ) {
	sli__eprintf("%s sli::%s[obj=0x%zx] = {",
		     msg, CLASS_NAME, (const size_t)this);
    }
    else {
	sli__eprintf("sli::%s[obj=0x%zx] = {",CLASS_NAME,(const size_t)this);
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( i != 0 ) sli__eprintf(", ");
	dump_tstring(this->at_cs(i));
    }
    sli__eprintf("}\n");
    return;
}


/*
 * private member functions
 */
 
/**
 * @brief   cstr_ptrs_rec �򹹿�
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ���private�Ǥ� 
 */
 tarray_tstring &tarray_tstring::update_cstr_ptrs_rec()
{
    size_t i;
    const char **ptrs;
    this->cstr_ptrs_rec.resize(this->arrs_rec.length() + 1);
    ptrs = (const char **)(this->cstr_ptrs_rec.data_ptr());
    for ( i=0 ; i < this->arrs_rec.length() ; i++ ) {
	this->arrs_rec[i].register_extptr(ptrs + i);
	ptrs[i] = this->arrs_rec[i].cstr();
    }
    ptrs[i] = NULL;

    return *this;
}

/**
 * @brief   ptr���ؤ��������ɥ쥹�����Ȥ�ʸ�������Ǥ�������å�
 * 
 * @param   ptr ���ɥ쥹
 * @return  ���Ȥ��ΰ���λ��Ͽ�<br>
 *          ����ʳ��λ��ϵ�
 * @note    ���Υ��дؿ���private�Ǥ� 
 */
bool tarray_tstring::is_my_buffer( const char *const *ptr ) const
{
    const char *const *cstr_ptrs = this->cstrarray();
    if ( ptr == NULL || cstr_ptrs == NULL ) return false;
    if ( cstr_ptrs <= ptr && ptr < cstr_ptrs + this->length() + 1 )
	return true;
    else
	return false;
}

/**
 * @brief  ����ɽ���ˤ��ޥå�����(private)
 *
 *  ʸ���� target ���Ф���regex_ref �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ��
 *  �ˤ��ʸ����ޥå����Ԥ���ʸ��������Ȥ��Ƽ��Ȥ˳�Ǽ���ޤ���
 * 
 * @param   target �ޥå����оݤ�tstring���֥�������
 * @param   pos ʸ����ޥå��γ��ϰ���
 * @param   regex_ref tregex ���饹�Υ���ѥ���ѥ��֥�������
 * @param   nextpos �����pos
 * @return  �ǽ�˥ޥå���������<br>
 *          ���Ԥ�����������
 * @throw   regex�롼���󤬥����Ȥ��̤����Ƥ�����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 * @note    ���Υ��дؿ���private�Ǥ�
 */
ssize_t tarray_tstring::regexp_match_advanced( const tstring &target,
					       size_t pos,
					       const tregex &regex_ref,
					       size_t *nextpos )
{
    int status;
    size_t to_append, n_result, i, len_target_str = 0;
    const char *str_ptr;
    const char *target_str;
    tstring bak_target;

    /* ���Ȥ��ݻ�����ʸ����ξ��ϥ��ԡ�������Τ��оݤˤ��� */
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( &(this->at_cs(i)) == &target ) break;
    }
    if ( i < this->length() ) {	/* ���Ĥ��ä���� */
	bak_target.init(target);
	target_str = bak_target.cstr();
	len_target_str = bak_target.length();
    }
    else {
	target_str = target.cstr();
	len_target_str = target.length();
    }

    this->erase();

    if ( target_str == NULL ) goto quit;
    if ( len_target_str < pos ) goto quit;
    if ( regex_ref.cregex() == NULL ) goto quit;

    to_append = 1;
    str_ptr = regex_ref.cstr();
    /* '(' �θĿ��������������ɲ�ʬ�ˤ��롥'\\(' �⥫����Ȥ���뤬OK */
    if ( str_ptr != NULL ) {
	for ( i=0 ; str_ptr[i] != '\0' ; i++ ) {
	    if ( str_ptr[i] == '(' ) to_append++;
	}
    }

    this->reg_pos_rec.resize(1);
    this->reg_length_rec.resize(1);
    do {
	this->reg_pos_rec.resizeby(to_append);
	this->reg_length_rec.resizeby(to_append);
	/* */
	status = c_regsearchx(regex_ref.cregex(), target_str + pos, 
			      (0 < pos), false, 
			      this->reg_pos_rec.length(),
			      (size_t *)(this->reg_pos_rec.data_ptr()),
			      (size_t *)(this->reg_length_rec.data_ptr()),
			      &n_result);
	if ( status != 0 ) {
	   this->init_reg_results();
	   if ( c_regfatal(status) ) {
	     err_throw(__FUNCTION__,"FATAL","Out of memory in c_regsearchx()");
	   }
	   goto quit;
	}
	if ( n_result == 0 ) {	/* never */
	    goto quit;
	}
    } while ( this->reg_pos_rec.length() <= n_result );

    this->reg_pos_rec.resize(n_result);
    this->reg_length_rec.resize(n_result);
    this->resize(n_result);

    for ( i=0 ; i < n_result ; i++ ) {
	this->reg_pos_rec.z(i) += pos;
	this->at(i).assign(target_str + this->reg_pos_rec.z(i),
			   this->reg_length_rec.z(i));
    }

    if ( 0 < n_result ) {
	ssize_t ret = this->reg_pos_rec.z(0);
	size_t span = this->reg_length_rec.z(0);
	if ( nextpos != NULL ) {
	    size_t add_span = ((span == 0) ? 1 : span);
	    if ( 0 <= ret ) *nextpos = ret + add_span;
	    else *nextpos = len_target_str + 1;
	}
	return ret;
    }

 quit:
    if ( nextpos != NULL ) *nextpos = len_target_str + 1;
    this->init_reg_results();
    return -1;
}

/**
 * @brief  ����ɽ���η�̤γ�Ǽ�������
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
void tarray_tstring::init_reg_results()
{
    this->reg_pos_rec.init(sizeof(size_t), true);
    this->reg_length_rec.init(sizeof(size_t), true);
    return;
}

static const tstring junk;

/**
 * @brief  at() ���ѤΥ��дؿ� (private)
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
tstring &tarray_tstring::tstring_ref_after_resize( size_t index )
{
    this->resize(index + 1);
    return this->arrs_rec.at(index);
}

/**
 * @brief  at() ���ѤΥ��дؿ� (private)
 *
 * @note   ���Υ��дؿ���private�Ǥ� 
 */
const tstring &tarray_tstring::err_throw_const_tstring_ref( const char *fnc,
					const char *lv, const char *mes ) const
{
    err_throw(fnc,lv,mes);
    return junk;		/* never */
}

}	/* namespace sli */


#include "private/c_memcpy.cc"
#include "private/c_strncmp.cc"
#include "private/c_isalpha.cc"
#include "private/c_regsearchx.c"
#include "private/c_regfatal.c"

