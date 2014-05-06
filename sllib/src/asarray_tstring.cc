/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:15:47 cyamauch> */

/**
 * @file   asarray_tstring.cc
 * @brief  ʸ�����Ϣ������򰷤� asarray_tstring ���饹�Υ�����
 */

#define CLASS_NAME "asarray_tstring"

#include "config.h"

#include "asarray_tstring.h"

#include <stdlib.h>

#include "private/err_report.h"


namespace sli
{

/* constructor */

/**
 * @brief  ���󥹥ȥ饯��
 *
 */
asarray_tstring::asarray_tstring()
{
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���Ȥ� elements[] �����ͤȤ��ƽ�������ޤ���<br>
 *  ��¤�Τ�����κǽ����Ǥˤ�ɬ�� {NULL,NULL} ��Ϳ���Ƥ���������
 * 
 * @param  elements asarrdef_tstring��¤�Τ�����
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring::asarray_tstring(const asarrdef_tstring elements[])
{
    this->assign(elements);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  Ϳ����줿����ʸ�������ʸ����Ǽ��Ȥ��������ޤ���<br>
 *  �����κǸ�ˤ�ɬ�� NULL ��Ϳ���Ƥ���������
 *  
 * @param  key0 ����ʸ����
 * @param  val0 ��ʸ����
 * @param  key1 ����ʸ����
 * @param  ... �������ͤȤʤ�ʸ����β���Ĺ�����γ����ǥǡ���
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring::asarray_tstring(const char *key0, const char *val0,
				 const char *key1, ...)
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
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
 *  ���Ȥ�obj�����Ƥǽ�������ޤ���
 * 
 * @param  obj asarray_tstring���֥�������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
asarray_tstring::asarray_tstring(const asarray_tstring &obj)
{
    this->init(obj);
    return;
}

/* destructor */

/**
 * @brief  �ǥ��ȥ饯��
 */
asarray_tstring::~asarray_tstring()
{
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  �黻�Ҥα�¦�ǻ��ꤵ�줿 asarray_tstring ���饹�Υ��֥������Ȥ򼫿Ȥ˥��ԡ�
 *  ���ޤ���
 * 
 * @param   obj asarray_tstring�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������ 
 * @throw   �����˲��򵯤������Ȥ�
 */
asarray_tstring &asarray_tstring::operator=(const asarray_tstring &obj)
{
    this->init(obj);
    return *this;
}

/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ���(tstring)���֤�
 *
 *  �������б�����Ϣ������������ͤλ��Ȥ��֤��ޤ���
 * 
 * @param   key Ϣ������Υ���ʸ����
 * @return  �������б�����Ϣ������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
tstring &asarray_tstring::operator[]( const char *key )
{
    return this->at(key);
}

/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ���(tstring)���֤� (�ɼ�����)
 *
 *  �������б�����Ϣ������������ͤλ��Ȥ��֤��ޤ���
 * 
 * @param   key Ϣ������Υ���ʸ���� 
 * @return  �������б�����Ϣ�������������
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   ¸�ߤ��ʤ�����ʸ���󤬻��ꤵ�줿���
 */
const tstring &asarray_tstring::operator[]( const char *key ) const
{
    return this->at_cs(key);
}

/*
 * public member functions
 */

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�Ϣ�������õ�����֥������Ȥν������Ԥ��ޤ���
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::init()
{
    this->index_rec.init();
    this->key_rec.init();
    this->value_rec.init();
    this->regex_rec.init();
    return *this;
}

/**
 * @brief  ���֥������ȤΥ��ԡ�
 * 
 *  ���ꤵ�줿���֥������� obj �����Ƥ򼫿Ȥ˥��ԡ����ޤ���
 *
 * @param   obj asarray_tstring�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 */
asarray_tstring &asarray_tstring::init(const asarray_tstring &obj)
{
    if ( &obj == this ) return *this;
    this->value_rec.init(obj.value_rec);
    this->key_rec.init(obj.key_rec);
    this->index_rec.init(obj.index_rec);
    /* this->regex_rec.init(obj.regex_rec); */
    this->regex_rec.init();
    return *this;
}

/**
 * @brief  ��¸�������ͤ��٤Ƥ���ꤵ�줿ʸ����(const char *)�ǥѥǥ���
 *
 *  ���Ȥ�Ϣ������������ͤ��٤Ƥ�ʸ���� str �ǥѥǥ��󥰤��ޤ���<br>
 *  str ���ά������硤Ĺ��0��ʸ���󤬻Ȥ��ޤ���
 * 
 * @param   str Ϣ��������ͤ�ѥǥ��󥰤���ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::clean(const char *str)
{
    this->value_rec.clean(str);
    return *this;
}

/**
 * @brief  ��¸�������ͤ��٤Ƥ���ꤵ�줿ʸ����(tstring)�ǥѥǥ���
 *
 *  ���Ȥ�Ϣ������������ͤ��٤Ƥ�ʸ���� str �ǥѥǥ��󥰤��ޤ���<br>
 * 
 * @param   str Ϣ��������ͤ�ѥǥ��󥰤���ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::clean(const tstring &str)
{
    this->value_rec.clean(str);
    return *this;
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarray_tstring)�򼫿Ȥ�����
 *
 *  src�ǻ��ꤵ�줿ʸ����Ϣ������������򡤼��Ȥ��������ޤ���
 * 
 * @param   src asarray_tstring���֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * 
 */
asarray_tstring &asarray_tstring::assign(const asarray_tstring &src)
{
    if ( &src == this ) return *this;
    this->erase();
    return this->append(src);
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarrdef_tstring [])�򼫿Ȥ�����
 *
 *  elements �ǻ��ꤵ�줿ʸ����Ϣ������������򡤼��Ȥ��������ޤ���<br>
 *  elements �� {NULL,NULL} �ǽ�ü���Ƥ���ɬ�פ�����ޤ���
 * 
 * @param   elements[] asarrdef_tstring��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign( const asarrdef_tstring elements[] )
{
    if ( elements != NULL ) {
	size_t i;
	for ( i=0 ; elements[i].key != NULL ; i++ );
	this->assign(elements, i);
    }
    else {
	this->erase();
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarrdef_tstring [])��n�Ĥ򼫿Ȥ�����
 *
 *  elements �ǻ��ꤵ�줿ʸ����Ϣ���������Ƭ���� n ��ʬ�����Ƥ򡤼��Ȥ�����
 *  ���ޤ���
 * 
 * @param   elements asarray_tstring���֥�������
 * @param   n ����elements�ο�
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign( const asarrdef_tstring elements[],
					  size_t n )
{
    /* ���Ȥ��������Ƥ��� key, val ����Ƥ⤤���褦�ˤ��� */
    if ( 0 < n && elements != NULL ) { 
	size_t i, len_elem, org_len = this->length();
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ );
	len_elem = i;
	/* �ޤ����Ǹ���ɲä��� */
	for ( i=0 ; i < len_elem ; i++ ) {
	    const char *val = elements[i].value;
	    if ( val == NULL ) val = "";
	    this->value_rec.append(val, 1);
	    this->key_rec.append(elements[i].key, 1);
	} 
	/* ���Τ�Ĥ�õ�� */
	this->value_rec.erase(0, org_len);
	this->key_rec.erase(0, org_len);
	/* ����ǥå�����Ϥ�Ϥ��� */
	this->index_rec.init();
	for ( i=0 ; i < this->key_rec.length() ; ) {
	    if ( this->index_rec.index(this->key_rec.cstr(i),0) < 0 ) {
		this->index_rec.append(this->key_rec.cstr(i),i);
		i++;
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING","cannot append key '%s'",
			    this->key_rec.cstr(i));
		this->value_rec.erase(i, 1);
		this->key_rec.erase(i, 1);
	    }
	} 
    } 
    else {
	this->erase();
    } 
    return *this;
}

/**
 * @brief  ���ꤵ�줿����(key,value) 1�ĤΤߤ�Ϣ������Ȥ���
 *
 *  ���Ȥ򡤻��ꤵ�줿 1�Ĥ����Ǥ��������Ϣ������ˤ��ޤ���
 *  
 * @param   key Ϣ����������ꤹ�륭��ʸ����
 * @param   val Ϣ����������ꤹ����ʸ���� 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign( const char *key, const char *val )
{
    asarrdef_tstring el[] = { {key,val},{NULL,NULL} };
    return this->assign(el);
}

/**
 * @brief  ���ꤵ�줿����(key,value) 1�ĤΤߤ�Ϣ������Ȥ���
 *
 *  ���Ȥ򡤻��ꤵ�줿 1�Ĥ����Ǥ��������Ϣ������ˤ��ޤ���
 *  
 * @param   key Ϣ����������ꤹ�륭��ʸ����
 * @param   val Ϣ����������ꤹ����ʸ���� 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign( const char *key, const tstring &val )
{
    asarrdef_tstring el[] = { {key,val.cstr()},{NULL,NULL} };
    return this->assign(el);
}

/**
 * @brief  ���ꤵ�줿����(key,value) 1�ĤΤߤ�Ϣ������Ȥ���
 *
 *  ���Ȥ򡤻��ꤵ�줿 1�Ĥ����Ǥ��������Ϣ������ˤ��ޤ���<br>
 *  ���Υ��дؿ��Ǥϡ����ꤷ������ʸ�����printf()�ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ�.
 * 
 * @param   key Ϣ����������ꤹ�륭��ʸ����
 * @param   fmt ���ꤹ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assignf( const char *key, 
					   const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	this->vassignf(key,fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassignf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿����(key,value) 1�ĤΤߤ�Ϣ������Ȥ���
 *
 *  ���Ȥ򡤻��ꤵ�줿 1�Ĥ����Ǥ��������Ϣ������ˤ��ޤ���<br>
 *  ���Υ��дؿ��Ǥϡ����ꤷ������ʸ�����printf()�ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ�.
 *
 * @param   key Ϣ����������ꤹ�륭��ʸ����
 * @param   fmt ���ꤹ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vassignf( const char *key,
					    const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->assign(key,fmt);

    buf.vprintf(fmt,ap);
    this->assign(key,buf.cstr());

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����Ϣ���������Ǥ򼫿Ȥ�����
 *
 *  ���ꤵ�줿ʣ����Ϣ���������Ǥ򼫿Ȥ��������ޤ���<br>
 *  ����Ĺ�����ν�ü�� NULL �ǽ�ü���Ƥ���ɬ�פ�����ޤ���
 *
 * @param   key0 Ϣ����������ꤹ�륭��ʸ����
 * @param   key1 Ϣ����������ꤹ�륭��ʸ����
 * @param   val0 Ϣ����������ꤹ�륭��ʸ����
 * @param   ... ����,�ͤȤʤ�ʸ����β���Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign( const char *key0, const char *val0,
					  const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����Ϣ���������Ǥ򼫿Ȥ�����
 *
 *  ���ꤵ�줿ʣ����Ϣ���������Ǥ򼫿Ȥ��������ޤ���<br>
 *  ����Ĺ�����ν�ü�� NULL �ǽ�ü���Ƥ���ɬ�פ�����ޤ���
 *
 * @param   key0 Ϣ����������ꤹ�륭��ʸ����
 * @param   val0 Ϣ����������ꤹ�륭��ʸ����
 * @param   ap ����,�ͤȤʤ�ʸ����β���Ĺ�����γ����ǥǡ���
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vassign( const char *key0, const char *val0,
					   const char *key1, va_list ap )
{
    /* ���Ȥ��������Ƥ��� key, val ����Ƥ⤤���褦�ˤ��� */
    if ( key0 != NULL ) { 
	size_t i, org_len = this->length();
	/* �ޤ����Ǹ���ɲä��� */
	this->value_rec.append((val0 == NULL) ? "" : val0, 1);
	this->key_rec.append(key0, 1);
	if ( key1 != NULL ) {
	    const char *val1 = va_arg(ap,char *);
	    this->value_rec.append((val1 == NULL) ? "" : val1, 1);
	    this->key_rec.append(key1, 1);
	    while ( 1 ) {
		const char *keyx = va_arg(ap,char *);
		const char *valx;
		if ( keyx == NULL ) break;
		valx = va_arg(ap,char *);
		this->value_rec.append((valx == NULL) ? "" : valx, 1);
		this->key_rec.append(keyx, 1);
	    }
	} 
	/* ���Τ�Ĥ�õ�� */
	this->value_rec.erase(0, org_len);
	this->key_rec.erase(0, org_len);
	/* ����ǥå�����Ϥ�Ϥ��� */
	this->index_rec.init();
	for ( i=0 ; i < this->key_rec.length() ; ) {
	    if ( this->index_rec.index(this->key_rec.cstr(i),0) < 0 ) {
		this->index_rec.append(this->key_rec.cstr(i),i);
		i++;
	    }
	    else {
		err_report1(__FUNCTION__,"WARNING","cannot append key '%s'",
			    this->key_rec.cstr(i));
		this->value_rec.erase(i, 1);
		this->key_rec.erase(i, 1);
	    }
	} 
    } 
    else {
	this->erase();
    } 
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ�����Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *  ���Ѱ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 * 
 * @param   key0 ����ʸ����
 * @param   ... ����ʸ����β���Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign_keys( const char *key0, ... )
{
    va_list ap;
    va_start(ap,key0);
    try {
	this->vassign_keys(key0,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign_keys() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ�����Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *  ���Ѱ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 * 
 * @param   key0 ����ʸ����
 * @param   ap ����ʸ����β���Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vassign_keys( const char *key0, va_list ap )
{
    tarray_tstring tmp_keys;
    if ( key0 != NULL ) {
	tmp_keys.append(key0,1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    if ( keyx == NULL ) break;
	    tmp_keys.append(keyx,1);
	}
    }
    this->assign_keys(tmp_keys);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ���������Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ�������� keys �򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *  ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 * 
 * @param   keys ����ʸ��������ꤹ��ʸ��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign_keys( const char *const *keys )
{
    tarray_tstring tmp_keys;
    if ( this->key_rec.cstrarray() == keys ) return *this;
    tmp_keys.assign(keys);
    this->assign_keys(tmp_keys);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ���������Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ�������� keys �򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *
 * @param   keys ����ʸ��������ꤹ��ʸ��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign_keys( const tarray_tstring &keys )
{
    size_t i;

    if ( &(this->key_rec) == &keys ) return *this;

    this->key_rec.erase();
    this->index_rec.init();
    for ( i=0 ; i < keys.length() ; i++ ) {
	/* Ʊ������̾��������硤name, name(1), name(2), ... �Τ褦�ˤ��� */
	int ix=0;
	tstring r_key;
	r_key.assign(keys.cstr(i));
	while ( 0 <= this->index_rec.index(r_key.cstr(),0) ) {
	    ix ++;
	    r_key.printf("%s(%d)",keys.cstr(i),ix);
	}
	this->key_rec.append(r_key.cstr(), 1);
	this->index_rec.append(r_key.cstr(), i);
    }
    this->value_rec.resize(this->key_rec.length());

    return *this;
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ����������� (¿��ǽ��)
 *
 *  ʸ�����ʬ�䤷�ơ����������ꤷ�ޤ���<br>
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quotations �ˡ������ʸ���פ���ꤷ�ޤ���
 * 
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�η�̤�ʸ�����������ɤ����Υե饰
 * @param   quotations �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param   escape ����������ʸ��
 * @param   rm_escape ����������ʸ���������뤫�ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_keys( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ�����������
 *
 *  ʸ�����ʬ�䤷�ơ����������ꤷ�ޤ���<br>
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���
 *
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�η�̤�ʸ�����������ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_keys( const char *src_str, 
					      const char *delims,
					      bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}


/**
 * @brief  ʸ�����ʬ�䤷�ơ����������� (¿��ǽ��)
 *
 *  ʸ�����ʬ�䤷�ơ����������ꤷ�ޤ���<br>
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quotations �ˡ������ʸ���פ���ꤷ�ޤ���
 * 
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�η�̤�ʸ�����������ɤ����Υե饰
 * @param   quotations �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param   escape ����������ʸ��
 * @param   rm_escape ����������ʸ���������뤫�ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_keys( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ�����������
 *
 *  ʸ�����ʬ�䤷�ơ����������ꤷ�ޤ���
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ����� 
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���
 * 
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�η�̤�ʸ�����������ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_keys( const tstring &src_str, 
					      const char *delims,
					      bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ�����Ϣ���������ʸ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�Ϣ��������ͤ����ꤷ�ޤ���<br>
 *  ���Ѱ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 *
 * @param   value0 ��ʸ����
 * @param   ... ��ʸ����β���Ĺ�����γ����ǥǡ��� 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign_values( const char *value0, ... )
{
    va_list ap;
    va_start(ap,value0);
    try {
	this->vassign_values(value0,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vassign_values() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ����ʸ�����Ϣ���������ʸ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ�����,���Ȥ�Ϣ��������ͤ����ꤷ�ޤ���<br>
 *  ���Ѱ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 *
 * @param   value0 ��ʸ����
 * @param   ap ��ʸ����β���Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vassign_values( const char *value0, va_list ap )
{
    tarray_tstring tmp_values;
    if ( value0 != NULL ) {
	tmp_values.append(value0,1);
	while ( 1 ) {
	    const char *valuex = va_arg(ap,char *);
	    if ( valuex == NULL ) break;
	    tmp_values.append(valuex,1);
	}
    }
    this->assign_values(tmp_values);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ���������Ϣ���������ʸ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�Ϣ��������ͤ����ꤷ�ޤ���<br>
 *  ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 *
 * @param   values ��ʸ��������ꤹ��ʸ��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign_values( const char *const *values )
{
    if ( this->value_rec.cstrarray() == values ) return *this;
    this->value_rec.assign(values);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʸ���������Ϣ���������ʸ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򡤼��Ȥ�Ϣ��������ͤ����ꤷ�ޤ���
 *
 * @param   values ��ʸ��������ꤹ��ʸ��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::assign_values( const tarray_tstring &values )
{
    if ( &(this->value_rec) == &values ) return *this;
    this->value_rec.assign(values);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ��ͤ����� (¿��ǽ��)
 *
 *  ʸ�����ʬ�䤷�ơ��ͤ����ꤷ�ޤ���
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quotations �ˡ������ʸ���פ���ꤷ�ޤ���
 *
 * @param   src_str ʬ���оȤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @param   quotations �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param   escape ����������ʸ��
 * @param   rm_escape ����������ʸ���������뤫�ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_values( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    this->value_rec.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ��ͤ�����
 *
 *  ʸ�����ʬ�䤷�ơ��ͤ����ꤷ�ޤ���
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���
 * 
 * @param   src_str ʬ���оȤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_values( const char *src_str, 
						const char *delims,
						bool zero_str )
{
    return this->split_values(src_str,delims,zero_str,NULL,'\0',false);
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ��ͤ����� (¿��ǽ��)
 *
 *  ʸ�����ʬ�䤷�ơ��ͤ����ꤷ�ޤ���
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quotations �ˡ������ʸ���פ���ꤷ�ޤ���
 * 
 * @param   src_str ʬ���оȤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @param   quotations �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param   escape ����������ʸ��
 * @param   rm_escape ����������ʸ���������뤫�ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_values( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    this->value_rec.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->value_rec.resize(this->key_rec.length());
    return *this;
}

/**
 * @brief  ʸ�����ʬ�䤷�ơ��ͤ�����
 *
 *  ʸ�����ʬ�䤷��,�ͤ����ꤷ�ޤ�.
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���
 * 
 * @param   src_str ʬ���оȤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::split_values( const tstring &src_str, 
						const char *delims,
						bool zero_str )
{
    return this->split_values(src_str,delims,zero_str,NULL,'\0',false);
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarray_tstring)�򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿ʣ�������Ǥ��ɲä��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   src �����Ȥʤ����Ǥ����asarray_tstring�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::append( const asarray_tstring &src )
{
    size_t i;
    if ( &src == this ) {
	/* ��������ʣ����ΤǼ��Ȥ��ɲä��뤳�ȤϤǤ��ʤ� */
	err_report(__FUNCTION__,"WARNING", "cannot append self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->append(src.key(i),src.cstr(i));
    }
 quit:
    return *this;
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarrdef_tstring)�򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿ʣ�������Ǥ��ɲä��ޤ���<br>
 *  elements �� {NULL,NULL} �ǽ�ü���Ƥ���ɬ�פ�����ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   elements[] �����Ȥʤ�asarrdef_tstring��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::append( const asarrdef_tstring elements[] )
{
    if ( elements != NULL ) {
	size_t i;
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    this->append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarrdef_tstring)��n�Ĥ򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ�elements ����Ƭ���� n�Ĥ����Ǥ��ɲä��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   elements[] �����Ȥʤ�asarrdef_tstring��������
 * @param   n ����elements�θĿ�
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::append( const asarrdef_tstring elements[],
					  size_t n )
{
    if ( 0 < n && elements != NULL ) {
	size_t i;
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    this->append(elements[i].key,elements[i].value);
	}
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿1����(const char *)�򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿 1�Ĥ����Ǥ��ɲä��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key Ϣ��������ɲä��륭��ʸ����
 * @param   val Ϣ��������ɲä�����ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::append( const char *key, const char *val )
{
    if ( key != NULL ) {
	if ( this->index_rec.index(key,0) < 0 ) {
	    if ( val == NULL ) val = "";
	    this->value_rec.append(val,1);
	    this->key_rec.append(key,1);
	    this->index_rec.append(key,this->key_rec.length()-1);
	}
	else {
	    err_report1(__FUNCTION__,"WARNING","cannot append key '%s'",key);
	}
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿1����(tstring)�򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿 1�Ĥ����Ǥ��ɲä��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key Ϣ��������ɲä��륭��ʸ����
 * @param   val Ϣ��������ɲä�����ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::append( const char *key, const tstring &val )
{
    return this->append(key, val.cstr());
}

/**
 * @brief  ���ꤵ�줿1����(printf()�ε�ˡ�ǻ���)�򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿1�Ĥ����Ǥ��ɲä��ޤ���<br>
 *  ���Υ��дؿ��Ǥϡ����ꤷ������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥå�
 *  �Ȳ��Ѱ����ǥ��åȤǤ��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key Ϣ��������ɲä��륭��ʸ����
 * @param   fmt �ɲä�����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б��������Ѱ����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::appendf( const char *key, 
					   const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	this->vappendf(key,fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vappendf() failed");
    }
    va_end(ap);
    return *this;
}


/**
 * @brief  ���ꤵ�줿1����(printf()�ε�ˡ�ǻ���)�򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿1�Ĥ����Ǥ��ɲä��ޤ���<br>
 *  ���Υ��дؿ��Ǥϡ����ꤷ������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥå�
 *  �Ȳ��Ѱ����ǥ��åȤǤ��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key Ϣ��������ɲä��륭��ʸ����
 * @param   fmt �ɲä�����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б��������Ѱ����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vappendf( const char *key,
					    const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->append(key,fmt);

    buf.vprintf(fmt,ap);
    this->append(key,buf.cstr());

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ�������Ǥ򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿ʣ�������Ǥ��ɲä��ޤ���<br>
 *  ����Ĺ���������ǥǡ����κǸ�� NULL �Ǥ���ɬ�פ�����ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 *
 * @param   key0 Ϣ��������ɲä��륭��ʸ����
 * @param   val0 Ϣ��������ɲä�����ʸ����
 * @param   key1 Ϣ��������ɲä��륭��ʸ����
 * @param   ... ����,�ͤȤʤ�ʸ����β���Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::append( const char *key0, const char *val0,
					  const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vappend(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vappend() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ�������Ǥ򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿ʣ�������Ǥ��ɲä��ޤ���<br>
 *  ����Ĺ�����Υꥹ�Ȥν�ü�Υǡ�����NULL�Ǥ���ɬ�פ�����ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 *
 * @param   key0 Ϣ��������ɲä��륭��ʸ����
 * @param   val0 Ϣ��������ɲä�����ʸ����
 * @param   key1 Ϣ��������ɲä��륭��ʸ����
 * @param   ap ����,�ͤȤʤ�ʸ����β���Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vappend( const char *key0, const char *val0,
					   const char *key1, va_list ap )
{
    this->append(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const char *val1 = va_arg(ap,char *);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    const char *valx;
	    if ( keyx == NULL ) break;
	    valx = va_arg(ap,const char *);
	    this->append(keyx,valx);
	}
    }
    return *this;
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarray_tstring)�򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿ʣ�������Ǥ�����
 *  ���ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ���Ϣ������Υ���ʸ����
 * @param   src �����Ȥʤ����Ǥ����asarray_tstring���饹�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insert( const char *key,
					  const asarray_tstring &src )
{
    size_t i;
    if ( &src == this ) {
	/* ��������ʣ����ΤǼ��Ȥ򥤥󥵡��Ȥ��뤳�ȤϤǤ��ʤ� */
	err_report(__FUNCTION__,"WARNING", "cannot insert self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->insert(key,src.key(i),src.cstr(i));
    }
 quit:
    return *this;
}


/**
 * @brief  ���ꤵ�줿Ϣ������(asarrdef_tstring)�򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿ʣ�������Ǥ�����
 *  ���ޤ���<br>
 *  elements �κǸ�����Ǥϡ�{NULL,NULL} �Ǥ���ɬ�פ�����ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 *
 * @param   key �������֤ˤ���Ϣ������Υ���ʸ����
 * @param   elements[] �����Ȥʤ����Ǥ����asarrdef_tstring��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insert( const char *key, 
					  const asarrdef_tstring elements[] )
{
    if ( key != NULL && elements != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; elements[i].key != NULL ; i++ ) {
	    this->insert(key,elements[i].key,elements[i].value);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  ���ꤵ�줿Ϣ������(asarrdef_tstring)��n�Ĥ򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ�elements ����Ƭ���� n�Ĥ����Ǥ�
 *  �������ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ���Ϣ������Υ���ʸ����
 * @param   elements[] �����Ȥʤ����Ǥ����asarrdef_tstring��������
 * @param   n ����elements�θĿ�
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insert( const char *key, 
					  const asarrdef_tstring elements[],
					  size_t n )
{
    if ( key != NULL && 0 < n && elements != NULL ) {
	size_t i;
	if ( this->index_rec.index(key,0) < 0 ) {
	    err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	for ( i=0 ; i < n && elements[i].key != NULL ; i++ ) {
	    this->insert(key,elements[i].key,elements[i].value);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  ���ꤵ�줿1����(const char *)�򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿 1�Ĥ����Ǥ�����
 *  ���ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ����Ͽ̤�Ϣ������Υ���ʸ����
 * @param   newkey Ϣ��������������륭��ʸ����
 * @param   newval Ϣ�����������������ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insert( const char *key,
				       const char *newkey, const char *newval )
{
    if ( key != NULL && newkey != NULL ) {
	size_t i;
	ssize_t idx = this->index_rec.index(key,0);
	if ( idx < 0 ) {
	    err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	if ( 0 <= this->index_rec.index(newkey,0) ) {
	    err_report1(__FUNCTION__,"WARNING","cannot insert key '%s'",newkey);
	    goto quit;
	}
	if ( newval == NULL ) newval = "";
	this->value_rec.insert(idx,newval,1);
	this->key_rec.insert(idx,newkey,1);
	this->index_rec.append(newkey,idx);
	for ( i=idx+1 ; i < this->key_rec.length() ; i++ ) {
	    this->index_rec.update(this->key_rec.cstr(i),i-1,i);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  ���ꤵ�줿1����(tstring)�򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿 1�Ĥ����Ǥ�����
 *  ���ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ����Ͽ̤�Ϣ������Υ���ʸ����
 * @param   newkey Ϣ��������������륭��ʸ����
 * @param   newval Ϣ�����������������ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insert( const char *key,
				    const char *newkey, const tstring &newval )
{
    return this->insert(key, newkey, newval.cstr());
}

/**
 * @brief  ���ꤵ�줿1����(printf()�ε�ˡ�ǻ���)�򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿 1�Ĥ����Ǥ�����
 *  ���ޤ���<br>
 *  ���Υ��дؿ��Ǥϡ����ꤷ������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥå�
 *  �Ȳ��Ѱ����ǥ��åȤǤ��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ����Ͽ̤�Ϣ������Υ�����ʸ����
 * @param   newkey Ϣ��������������륭��ʸ����
 * @param   fmt ����������ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insertf( const char *key,
					   const char *newkey, 
					   const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	this->vinsertf(key,newkey,fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsertf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿1����(printf()�ε�ˡ�ǻ���)�򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿 1�Ĥ����Ǥ�����
 *  ���ޤ���<br>
 *  ���Υ��дؿ��Ǥϡ����ꤷ������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥå�
 *  �Ȳ��Ѱ����ǥ��åȤǤ��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ��뼫�Ȥ�Ϣ������Υ�����ʸ����
 * @param   newkey Ϣ��������������륭��ʸ����
 * @param   fmt ����������ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vinsertf( const char *key,
					    const char *newkey,
					    const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->insert(key,newkey,fmt);

    buf.vprintf(fmt,ap);
    this->insert(key,newkey,buf.cstr());

    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ�������Ǥ򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿ʣ�������Ǥ�����
 *  ���ޤ���<br>
 *  ����Ĺ�����κǸ�����Ǥ� NULL �Ǥ���ɬ�פ�����ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 *
 * @param   key �������֤ˤ���Ϣ������Υ���ʸ����
 * @param   key0 Ϣ��������������륭��ʸ����
 * @param   val0 Ϣ�����������������ʸ��
 * @param   key1 Ϣ��������������륭��ʸ����
 * @param   ... ����,�ͤȤʤ�ʸ����β���Ĺ�����γ����ǥǡ���
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::insert( const char *key,
					  const char *key0, const char *val0,
					  const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vinsert(key,key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vinsert() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���ꤵ�줿ʣ�������Ǥ򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿ʣ�������Ǥ�����
 *  ���ޤ���<br>
 *  ����Ĺ�����κǸ�����Ǥ� NULL �Ǥ���ɬ�פ�����ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 *
 * @param   key �������֤ˤ���Ϣ������Υ���ʸ����
 * @param   key0 Ϣ��������������륭��ʸ����
 * @param   val0 Ϣ�����������������ʸ��
 * @param   key1 Ϣ��������������륭��ʸ����
 * @param   ap ����,�ͤȤʤ�ʸ����β���Ĺ�����Υꥹ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::vinsert( const char *key,
					   const char *key0, const char *val0,
					   const char *key1, va_list ap )
{
    if ( key == NULL ) goto quit;
    if ( this->index_rec.index(key,0) < 0 ) {
	err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    this->insert(key,key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const char *val1 = va_arg(ap,char *);
	this->insert(key,key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    const char *valx;
	    if ( keyx == NULL ) break;
	    valx = va_arg(ap,const char *);
	    this->insert(key,keyx,valx);
	}
    }
 quit:
    return *this;
}

/**
 * @brief  ���������Ǥκ��
 *
 *  ���Ȥ�Ϣ������Τ��٤Ƥ����Ǥ������ޤ���
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ����Ԥ������
 */
asarray_tstring &asarray_tstring::erase()
{
    this->index_rec.init();
    this->key_rec.erase();
    this->value_rec.erase();
    return *this;
}

/**
 * @brief  �������Ǥκ��
 *
 *  ���Ȥ�Ϣ������� key �ǻ��ꤵ�줿�������б��������Ǥ��� num_elements �Ĥ�
 *  ���Ǥ�õ�ޤ���<br>
 *  �õ�줿ʬ��������Ĺ��û���ʤ�ޤ���
 * 
 * @param   key ����ʸ����
 * @param   num_elements ����������ǤθĿ� (��ά�� 1)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::erase( const char *key, size_t num_elements )
{
    size_t i, maxels;
    ssize_t idx;
    if ( key == NULL ) goto quit;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    maxels = this->key_rec.length() - idx;
    if ( maxels < num_elements ) num_elements = maxels;

    for ( i=idx ; i < idx + num_elements ; i++ ) {
	this->index_rec.erase(this->key_rec.cstr(i),i);
    }
    for ( ; i < this->key_rec.length() ; i++ ) {
	this->index_rec.update(this->key_rec.cstr(i),i,i-num_elements);
    }

    this->key_rec.erase(idx,num_elements);
    this->value_rec.erase(idx,num_elements);

 quit:
    return *this;
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���֥�������sobj �����Ƥȼ��Ȥ����ƤȤ������ؤ��ޤ���
 * 
 * @param   sobj ���Ƥ������ؤ��� asarray_tstring ���饹�Υ��֥�������
 * @return  ���Ȥλ���
 */
asarray_tstring &asarray_tstring::swap( asarray_tstring &sobj )
{
    if ( &sobj == this ) return *this;
    this->key_rec.swap( sobj.key_rec );
    this->value_rec.swap( sobj.value_rec );
    this->index_rec.swap( sobj.index_rec );
    this->regex_rec.swap( sobj.regex_rec );
    return *this;
}

/**
 * @brief  ����ʸ������ѹ�
 *
 *  ���Ȥ�Ϣ������Υ���ʸ���� org_key �� new_key �ǻ��ꤵ�줿ʸ������ѹ�
 *  ���ޤ���<br>
 *  org_key ��¸�ߤ��ʤ����䡤new_key ����ʣ���륭��ʸ������ä����Ϸٹ�
 *  ���Ϥ���ޤ���
 * 
 * @param   org_key ���Υ���ʸ����
 * @param   new_key �ѹ���Υ���ʸ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::rename_a_key( const char *org_key, 
						const char *new_key )
{
    ssize_t idx;
    idx = this->index_rec.index(org_key,0);
    if ( idx < 0 ) {
	err_report1(__FUNCTION__,"WARNING","unknown key '%s'",org_key);
	goto quit;
    }
    if ( new_key == NULL || this->key_rec.at(idx).compare(new_key) == 0 )
	goto quit;
    if ( 0 <= this->index_rec.index(new_key,0) ) {
	err_report1(__FUNCTION__,"WARNING",
		    "already used: key '%s'. Not renamed.",new_key);
	goto quit;
    }
    this->index_rec.erase(org_key, idx);
    this->index_rec.append(new_key, idx);
    this->key_rec.at(idx).assign(new_key);
 quit:
    return *this;
}

/* */

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�Ϣ������������Ǥ���ʸ����ˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str ��
 *  �����������Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 * 
 * @param   org_str ���Ф���ʸ����
 * @param   new_str �ִ��θ����Ȥʤ�ʸ����
 * @param   all ���ִ��Υե饰(��ά��false)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strreplace( const char *org_str,
					      const char *new_str, bool all )
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	this->at(i).strreplace(org_str, new_str, all);
    }
    return *this;
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�Ϣ������������Ǥ���ʸ����ˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str ��
 *  �����������Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 * 
 * @param   org_str ���Ф���ʸ����
 * @param   new_str �ִ��θ����Ȥʤ�ʸ����
 * @param   all ���ִ��Υե饰(��ά��false)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strreplace( const tstring &org_str,
					      const char *new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str, all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�Ϣ������������Ǥ���ʸ����ˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str ��
 *  �����������Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 * 
 * @param   org_str ���Ф���ʸ����
 * @param   new_str �ִ��θ����Ȥʤ�ʸ����
 * @param   all ���ִ��Υե饰(��ά��false)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strreplace( const char *org_str,
					     const tstring &new_str, bool all )
{
    return this->strreplace(org_str, new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ñ���ʸ���󸡺����ִ�
 *
 *  ���Ȥ�Ϣ������������Ǥ���ʸ����ˤĤ��ơ�ʸ����κ�¦����ʸ���� org_str ��
 *  �����������Ĥ��ä�����ʸ���� new_str ���֤������ޤ���
 * 
 * @param   org_str ���Ф���ʸ����
 * @param   new_str �ִ��θ����Ȥʤ�ʸ����
 * @param   all ���ִ��Υե饰(��ά��false)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strreplace( const tstring &org_str,
					     const tstring &new_str, bool all )
{
    return this->strreplace(org_str.cstr(), new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ʸ���ν���
 *
 *  ���Ȥ������Ǥ���ʸ�����������ʸ���������ޤ���
 * 
 * @return  ���Ȥλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::chop()
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
 *  ���Ȥ�Ϣ������������Ǥ���ʸ����α�ü�β���ʸ�������ޤ���
 * 
 * @param   rs ����ʸ���� (��ά��)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::chomp( const char *rs )
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
 *  ���Ȥ�Ϣ������������Ǥ���ʸ����α�ü�β���ʸ�������ޤ���
 * 
 * @param   rs ����ʸ���� (��ά��)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::chomp( const tstring &rs )
{
    return this->chomp(rs.cstr());
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���<br>
 *  side_spaces���ά������硤" \t\n\r\f\v" ����ꤷ����ΤȤߤʤ��ޤ���
 * 
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::trim( const char *side_spaces )
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
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���
 * 
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::trim( const tstring &side_spaces )
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
 *  ���Ȥ����Ƥ����Ǥ���ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 * 
 * @param   side_space �����оݤ�ʸ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::trim( int side_space )
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
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���<br>
 *  side_spaces���ά������硤" \t\n\r\f\v" ����ꤷ����ΤȤߤʤ��ޤ���
 *
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::ltrim( const char *side_spaces )
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
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���<br>
 *
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::ltrim( const tstring &side_spaces )
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
 *  ���Ȥ����Ƥ����Ǥ���ʸ����κ�ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param   side_space �����оݤ�ʸ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::ltrim( int side_space )
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
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���<br>
 *  side_spaces���ά������硤" \t\n\r\f\v" ����ꤷ����ΤȤߤʤ��ޤ���
 *
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::rtrim( const char *side_spaces )
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
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���
 *
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::rtrim( const tstring &side_spaces )
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
 *  ���Ȥ����Ƥ����Ǥ���ʸ����α�ü�ˤ���Ǥ��ʸ�������ޤ���
 *
 * @param   side_space �����оݤ�ʸ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::rtrim( int side_space )
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
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���<br>
 *  side_spaces���ά������硤" \t\n\r\f\v" ����ꤷ����ΤȤߤʤ��ޤ���
 * 
 * @param   side_spaces �����оݤ�ʸ�����åȡ�[A-Z] �Ȥ��ä�ɽ������Ѳ�ǽ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strtrim( const char *side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ�ʸ��������������ǤˤĤ��ơ�ʸ����ξü�ˤ���Ǥ��ʸ��(side_spaces
 *  �ǻ���)�����ޤ���
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strtrim( const tstring &side_spaces )
{
    return this->trim(side_spaces);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�ʸ����ξü������ʸ���ν���
 *
 *  ���Ȥ����Ƥ����Ǥ���ʸ�����ξü�ˤ���Ǥ��ʸ�������ޤ���
 * 
 * @param   side_space �����оݤ�ʸ��
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::strtrim( int side_space )
{
    return this->trim(side_space);
}

/**
 * @brief  ����������ǤˤĤ�����ʸ����ʸ�����Ѵ�
 *
 *  ���Ȥ�Ϣ������������ǤΥ���ե��٥åȤ���ʸ����ʸ�����Ѵ����ޤ���
 * 
 * @return  ���Ȥλ���
 */
asarray_tstring &asarray_tstring::tolower()
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
 *  ���Ȥ�Ϣ������������ǤΥ���ե��٥åȤξ�ʸ������ʸ�����Ѵ����ޤ���
 * 
 * @return  ���Ȥλ���
 */
asarray_tstring &asarray_tstring::toupper()
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
 *  ���Ȥ�Ϣ������������ǤˤĤ��ơ���ʿ����ʸ�� '\t' ��tab_width ���ͤ�
 *  ��·���򤷤ƶ���ʸ�����ִ����ޤ���
 * 
 * @param   tab_width ����������ά����8
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::expand_tabs( size_t tab_width )
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
 * @param   tab_width ����������ά����8
 * @return  ���Ȥλ���
 * @throws  �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::contract_spaces( size_t tab_width )
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
 *  ���Ȥ�Ϣ������������Ǥ���ʸ�����pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ�.
 * 
 * @param   pat ʸ����ѥ�����(����ɽ��)
 * @param   new_str �ִ����ʸ����
 * @param   all ���ִ��Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::regreplace( const char *pat, 
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
 *  ���Ȥ�Ϣ������������Ǥ���ʸ�����pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ�.
 * 
 * @param   pat ʸ����ѥ�����(����ɽ��)
 * @param   new_str �ִ����ʸ����
 * @param   all ���ִ��Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::regreplace( const tstring &pat, 
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
 *  ���Ȥ�Ϣ������������Ǥ���ʸ�����pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ�.
 * 
 * @param   pat tregex�Υ���ѥ���Ѥߥ��֥�������
 * @param   new_str �ִ����ʸ����
 * @param   all ���ִ��Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::regreplace( const tregex &pat, 
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
 *  ���Ȥ�Ϣ������������Ǥ���ʸ�����pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ�.
 * 
 * @param   pat ʸ����ѥ�����
 * @param   new_str �ִ����ʸ����
 * @param   all ���ִ��Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::regreplace( const char *pat, 
					     const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�Ϣ������������Ǥ���ʸ�����pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ�.
 * 
 * @param   pat ʸ����ѥ�����
 * @param   new_str �ִ����ʸ����
 * @param   all ���ִ��Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::regreplace( const tstring &pat, 
					     const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/**
 * @brief  ����������ǤˤĤ��Ƥ�����ɽ���ˤ���ִ�
 *
 *  ���Ȥ�Ϣ������������Ǥ���ʸ�����pat �ǻ��ꤵ�줿 POSIX ��ĥ����ɽ����
 *  �ޥå�������ʬ��ʸ���� new_str ���֤������ޤ�.
 * 
 * @param   pat tregex���饹�Υ���ѥ���Ѥߥ��֥�������
 * @param   new_str �ִ����ʸ����
 * @param   all ���ִ��Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray_tstring &asarray_tstring::regreplace( const tregex &pat, 
					     const tstring &new_str, bool all )
{
    return this->regreplace(pat, new_str.cstr(), all);
}

/* */

/**
 * @brief  Ϣ���������ʸ������Ф���ݥ����������� (�ɼ�����)
 *
 *  ���Ȥ�����Ϣ���������ʸ����Υݥ���������֤��ޤ���<br>
 *  �ݥ�������ϡ�ɬ�� NULL �ǽ�ü���Ƥ��ޤ���
 * 
 * @return  Ϣ���������ʸ����ؤΥݥ�������
 */
const char *const *asarray_tstring::cstrarray() const
{
    return this->value_rec.cstrarray();
}

/**
 * @brief  ���ꤵ�줿�������б�������ʸ������֤� (�ɼ�����)
 *
 *  ���ꤵ�줿�������б�����Ϣ���������Ǥ�ʸ����Υ��ɥ쥹���֤��ޤ���
 * 
 * @param   key Ϣ������Τ���Υ���ʸ����
 * @return  ���ｪλ��: ���ꤵ�줿�������б�����ʸ����Υ��ɥ쥹��<br>
 *          �����ͤ������ʾ��: NULL��
 * @note    asarray_tstring::cstr() �Ȥΰ㤤�Ϥ���ޤ���
 */
const char *asarray_tstring::c_str( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return NULL;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return NULL;
    return this->value_rec.c_str(idx);
}

/**
 * @brief  ���ꤵ�줿�������б�������ʸ������֤� (�ɼ�����)
 *
 *  ���ꤵ�줿�������б�����Ϣ���������Ǥ�ʸ����Υ��ɥ쥹���֤��ޤ���
 * 
 * @param   key Ϣ������Τ���Υ���ʸ����
 * @return  ���ｪλ��: ���ꤵ�줿�������б�����ʸ����Υ��ɥ쥹.<br>
 *          �����ͤ������ʾ��: NULL��
 * @note    asarray_tstring::c_str() �Ȥΰ㤤�Ϥ���ޤ���
 */
const char *asarray_tstring::cstr( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return NULL;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return NULL;
    return this->value_rec.cstr(idx);
}

/**
 * @brief  ���ꤵ�줿�������б�������ʸ������֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  ���ꤵ�줿�������б�����Ϣ���������Ǥ�ʸ����Υ��ɥ쥹���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥå�ʸ����
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ｪλ��: ���ꤵ�줿�������б�����ʸ����Υ��ɥ쥹��<br>
 *          �����ͤ������ʾ��: NULL��
 */
const char *asarray_tstring::cstrf( const char *fmt, ... ) const
{
    const char *ret;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vcstrf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vcstrf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  ���ꤵ�줿�������б�������ʸ������֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  ���ꤵ�줿�������б�����Ϣ���������Ǥ�ʸ����Υ��ɥ쥹���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥå�ʸ����
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ｪλ��: ���ꤵ�줿�������б�����ʸ����Υ��ɥ쥹��<br>
 *          �����ͤ������ʾ��: NULL��
 */
const char *asarray_tstring::vcstrf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->cstr(fmt);

    buf.vprintf(fmt,ap);
    return this->cstr(buf.cstr());
}

/**
 * @brief  ���ꤵ�줿����ʸ������б������������Ǥλ���(tstring)���֤�
 *
 * @param   key Ϣ������Τ���Υ���ʸ����
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    asarray_tstring::at() �Ȥΰ㤤�Ϥ���ޤ���
 */
tstring &asarray_tstring::element( const char *key )
{
    return this->at(key);
}

/**
 * @brief  ���ꤵ�줿����ʸ������б������������Ǥλ���(tstring)���֤�
 *
 * @param   key Ϣ������Τ���Υ���ʸ����
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    asarray_tstring::element() �Ȥΰ㤤�Ϥ���ޤ���
 */
tstring &asarray_tstring::at( const char *key )
{
    ssize_t idx;
    if ( key == NULL ) {
        err_throw(__FUNCTION__,"ERROR","NULL key...?");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	this->append(key,"");
	idx = this->index_rec.index(key,0);
    }
    if ( idx < 0 ) {
        err_throw1(__FUNCTION__,"ERROR","not found key '%s'",key);
    }
    return this->value_rec.at(idx);
}

/**
 * @brief  ����ʸ������б������������Ǥλ���(tstring)���֤� (printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ��Ȥ��֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
tstring &asarray_tstring::atf( const char *fmt, ... )
{
    va_list ap;
    va_start(ap,fmt);
    try {
	tstring &ret = this->vatf(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vatf() failed");
    }
}

/**
 * @brief  ����ʸ������б������������Ǥλ���(tstring)���֤� (printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ��Ȥ��֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
tstring &asarray_tstring::vatf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->at(fmt);

    buf.vprintf(fmt,ap);
    return this->at(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT

/**
 * @brief  ����ʸ������б������������Ǥλ���(tstring)���֤� (�ɼ�����)
 *
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL���������ʾ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const tstring &asarray_tstring::at( const char *key ) const
{
    return this->at_cs(key);
}

/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL����¸�ߤ��ʤ����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const tstring &asarray_tstring::atf( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const tstring &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vatf_cs() failed");
    }
}

/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL����¸�ߤ��ʤ����
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const tstring &asarray_tstring::vatf( const char *fmt, va_list ap ) const
{
    return this->vatf_cs(fmt, ap);
}
#endif

/**
 * @brief  ����ʸ������б������������Ǥλ���(tstring)���֤� (�ɼ�����)
 *
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL����¸�ߤ��ʤ����
 */
const tstring &asarray_tstring::at_cs( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) {
        err_throw(__FUNCTION__,"ERROR","NULL key...?");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
        err_throw1(__FUNCTION__,"ERROR","not found key '%s'",key);
    }
    return this->value_rec.at_cs(idx);
}

/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL����¸�ߤ��ʤ����.
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const tstring &asarray_tstring::atf_cs( const char *fmt, ... ) const
{
    va_list ap;
    va_start(ap,fmt);
    try {
	const tstring &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	return ret;
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vatf_cs() failed");
    }
}

/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 *
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ꤵ�줿�������б����������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL����¸�ߤ��ʤ����.
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const tstring &asarray_tstring::vatf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->at_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->at_cs(buf.cstr());
}

/**
 * @brief  ����ʸ������б���������ʸ����γ����Хåե��ؤΥ��ԡ�
 *
 *  �����ǻ��ꤵ�줿�������б����������ͤ�ʸ����򡤳����Хåե��˥��ԡ�
 *  ���ޤ���
 *
 * @param   key ����ʸ����
 * @param   dest_buf ���ԡ���γ����Хåե����ɥ쥹
 * @param   buf_size �����Хåե��Υ�����
 * @return  �������: �Хåե�Ĺ����ʬ�ʾ��˥��ԡ��Ǥ���ʸ������<br>
 *          �����: dest_buf��NULL�����ꤷ��buf_size��0�ʳ����ͤ����ꤷ����硥
 */
ssize_t asarray_tstring::getstr( const char *key, 
				 char *dest_buf, size_t buf_size ) const
{
    ssize_t idx;
    if ( key == NULL ) return -1;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return -1;
    return this->value_rec.getstr(idx,dest_buf,buf_size);
}

/**
 * @brief  Ϣ�������Ĺ��(�Ŀ�)�����
 *
 * @return  ���Ȥ�Ϣ����������ǿ�
 */
size_t asarray_tstring::length() const
{
    return this->key_rec.length();
}

/**
 * @brief  Ϣ�������Ĺ��(�Ŀ�)�����
 * 
 * @return ���Ȥ�Ϣ����������ǿ�
 * @note   asarray_tstring::length() �Ȥΰ㤤�Ϥ���ޤ���
 */
size_t asarray_tstring::size() const
{
    return this->key_rec.size();
}

/**
 * @brief  ���ꤵ�줿����ʸ������б��������Ǥ�ʸ����Ĺ���֤�
 *
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�������б������ͤ�ʸ����Ĺ��
 */
size_t asarray_tstring::length( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return 0;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return 0;
    return this->value_rec.length(idx);
}

/**
 * @brief  ���ꤵ�줿����ʸ������б��������Ǥ�ʸ����Ĺ���֤�
 *
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�������б������ͤ�ʸ����Ĺ��
 */
size_t asarray_tstring::size( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) return 0;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return 0;
    return this->value_rec.size(idx);
}

/**
 * @brief  ���ꤵ�줿����ʸ������б���������ʸ����ΰ���pos�ˤ���ʸ�����֤�
 *
 *  �����ǻ��ꤵ�줿����ʸ������б����������ͤΡ�����pos�ˤ���ʸ�����֤��ޤ���
 * 
 * @param   key Ϣ������Υ���ʸ����
 * @param   pos ʸ����ΰ���
 * @return  �������: ���ꤷ�����֤ˤ���ʸ����<br>
 *          �����: ���顼��
 */
int asarray_tstring::cchr( const char *key, size_t pos ) const
{
    ssize_t idx;
    if ( key == NULL ) return -1;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) return -1;
    return this->value_rec.cchr(idx,pos);
}

/**
 * @brief  ���ꤵ�줿����ʸ������б����������ֹ���֤�
 * 
 * @param   key ����ʸ����
 * @return  �������: ���ꤵ�줿����ʸ���󤬸��Ĥ��ä���硥<br>
 *          �����: ���Ĥ���ʤ��ä���硥
 */
ssize_t asarray_tstring::index( const char *key ) const
{
    if ( key == NULL ) return -1;
    return this->index_rec.index(key,0);
}

/**
 * @brief  ���ꤵ�줿����ʸ������б����������ֹ���֤� (printf()�ε�ˡ)
 *
 *  ����ʸ������б����������ֹ���֤��ޤ���<br>
 *  ���Υ��дؿ��Ǥ� printf() �ؿ���Ʊ�ͤΥե����ޥåȤȲ��Ѱ����ǡ�����
 *  ʸ����򥻥åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  �������: ���ꤵ�줿����ʸ���󤬸��Ĥ��ä���硥<br>
 *          �����: ���Ĥ���ʤ��ä���硥
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t asarray_tstring::indexf( const char *fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vindexf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        err_throw(__FUNCTION__,"FATAL","this->vindexf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  ���ꤵ�줿����ʸ������б����������ֹ���֤� (printf()�ε�ˡ)
 *
 *  ����ʸ������б����������ֹ���֤��ޤ���<br>
 *  ���Υ��дؿ��Ǥ� printf() �ؿ���Ʊ�ͤΥե����ޥåȤȲ��Ѱ����ǡ�����
 *  ʸ����򥻥åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  �������: ���ꤵ�줿����ʸ���󤬸��Ĥ��ä���硥<br>
 *          �����: ���Ĥ���ʤ��ä���硥
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t asarray_tstring::vindexf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->index(fmt);

    buf.vprintf(fmt,ap);
    return this->index(buf.cstr());
}

/**
 * @brief  ���ꤵ�줿�����ֹ�˳������륭��ʸ������֤�
 * 
 *  ���ꤵ�줿�����ֹ�˳������륭��ʸ����Υ��ɥ쥹���֤��ޤ���
 * 
 * @param   index �����ֹ�
 * @return   ����ʸ����������Хåե��Υ��ɥ쥹��<br>
 *           �����Ĺ���ʾ�������ֹ椬���ꤵ�줿����NULL
 */
const char *asarray_tstring::key( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->key_rec.cstr(index);
}

/**
 * @brief  ���ꤵ�줿�����ֹ�˳���������ʸ������֤� (�ɼ�����)
 *
 *  ���ꤵ�줿�����ֹ�˳�������Ϣ���������ʸ����Υ��ɥ쥹���֤��ޤ���
 * 
 * @param   index �����ֹ�
 * @return  �����ֹ�˳�������ʸ����Υ��ɥ쥹��<br>
 *          �����ֹ椬�����ʾ��NULL��
 */
const char *asarray_tstring::cstr( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->value_rec.cstr(index);
}

/**
 * @brief  ���ꤵ�줿�����ֹ�˳������������ͤλ���(tstring)���֤�
 *
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳������������ͤλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 */
tstring &asarray_tstring::at( size_t index )
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at(index);
}

#ifdef SLI__OVERLOAD_CONST_AT

/**
 * @brief  ���ꤵ�줿�����ֹ�˳������������ͤλ���(tstring)���֤� (�ɼ�����)
 * 
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳������������ͤλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 */
const tstring &asarray_tstring::at( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at_cs(index);
}
#endif

/**
 * @brief  ���ꤵ�줿�����ֹ�˳������������ͤλ���(tstring)���֤�
 * 
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳������������ͤλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 */
tstring &asarray_tstring::element( size_t index )
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at(index);
}

/**
 * @brief  ���ꤵ�줿�����ֹ�˳������������ͤλ���(tstring)���֤� (�ɼ�����)
 * 
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳������������ͤλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 */
const tstring &asarray_tstring::at_cs( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) {
        err_throw1(__FUNCTION__,"ERROR","invalid index: %d",(int)index);
    }
    return this->value_rec.at_cs(index);
}

/**
 * @brief  ����ʸ��������󥪥֥������Ȥλ��Ȥ���� (�ɼ�����)
 *
 *  ���֥���������Ǵ������Ƥ��륭��ʸ��������󥪥֥������Ȥλ���(�ɤ߼��
 *  ����)���֤��ޤ���
 * 
 * @return  ����ʸ��������󥪥֥������Ȥλ���
 */
const tarray_tstring &asarray_tstring::keys() const
{
    return this->key_rec;
}

/**
 * @brief  ��ʸ��������󥪥֥������Ȥλ��Ȥ���� (�ɼ�����)
 *
 *  ���֥���������Ǵ������Ƥ�����ʸ��������󥪥֥������Ȥλ���(�ɤ߼������)
 *  ���֤��ޤ���
 * 
 * @return  ��ʸ��������󥪥֥������Ȥλ���
 */
const tarray_tstring &asarray_tstring::values() const
{
    return this->value_rec;
}

/**
 * @brief  ɸ�२�顼���Ϥ�src����� (������)
 * 
 * @param  src tstring���֥�������
 * @note   private �ʴؿ��Ǥ���
 */
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
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ (̤����)
 * @deprecated  ̤����
 *
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
void asarray_tstring::set_scopy_flag()
{
    return;
}

/**
 * @brief  ���֥������Ⱦ����ɸ�२�顼���Ϥؽ���
 *
 *  ���֥������Ⱦ����ɸ�२�顼���Ϥؽ��Ϥ��ޤ���
 */
void asarray_tstring::dprint( const char *msg ) const
{
    size_t i;
    if ( msg != NULL ) {
	sli__eprintf("%s sli::%s[obj=0x%zx] = { ",
		     msg, CLASS_NAME, (const size_t)this);
    }
    else {
	sli__eprintf("sli::%s[obj=0x%zx] = { ",CLASS_NAME,(const size_t)this);
    }
    for ( i=0 ; i < this->length() ; i++ ) {
	if ( i != 0 ) sli__eprintf(", ");
	sli__eprintf("{");
	dump_tstring(this->key_rec.at_cs(i));
	sli__eprintf(", ");
	dump_tstring(this->value_rec.at_cs(i));
	sli__eprintf("}");
    }
    sli__eprintf(" }\n");
    return;
}

}	/* namespace sli */
