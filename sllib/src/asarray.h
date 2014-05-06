/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:23:23 cyamauch> */

#ifndef _SLI__ASARRAY_H
#define _SLI__ASARRAY_H 1

/**
 * @file   asarray.h
 * @brief  Ǥ�ե��饹�Υ��֥�������Ϣ������򰷤� asarray ���饹�Υ�����
 */

#include "sli_config.h"
#include "tarray_tstring.h"
#include "tarray.h"
#include "ctindex.h"

namespace sli
{

/*
 * sli::asarray template class can handle the associative arrays of arbitrary
 * data types or classes.  Its function is almost the same as that of 
 * asarray_tstring, except that this class does not include member functions
 * specializing in string.
 */

/**
 * @class  sli::asarray
 * @brief  Ǥ�ե��饹�Υ��֥�������Ϣ������򰷤�����Υƥ�ץ졼�ȥ��饹
 *
 *   asarray �ƥ�ץ졼�ȥ��饹�ϡ�Ǥ�ե��饹�Υ��֥�������Ϣ������򰷤�����
 *   �Ǥ��ޤ���
 *
 * @note  asarray ���֥������������Ǥϡ��ͤȤ��ƤΥ��֥������Ȥ�ñ�������Ȥ�
 *        ����¸����ޤ���Ʊ���ˡ�����ʸ����������ֹ�Ȥδط�����¸���졤����
 *        �ˤ� ctindex ���饹���Ѥ��Ƥ��ޤ���ctindex ���饹�ϥ���ʸ�����������
 *        ��Ȥδط��򡤡��ڡ׹�¤�ǵ�Ͽ������®�ʸ���(����ʸ���󤫤������ֹ��
 *        ���Ѵ�)���ǽ�ˤ��Ƥ��ޤ��������������ڡ׹�¤�Ǥ��Τǡ������ˤϤ���
 *        �֤�������ޤ���
 * @author Chisato YAMAUCHI
 * @date 2013-04-01 00:00:00
 */

  template <class datatype> class asarray
  {

  public:
    /* constructor and destructor */
    asarray();
    asarray(const asarray &obj);
    virtual ~asarray();

    /*
     * operator
     */
    /* same as .init(sobj) */
    virtual asarray &operator=(const asarray &obj);

    /* same as .at(key) */
    virtual datatype &operator[]( const char *key );
    virtual const datatype &operator[]( const char *key ) const;

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual asarray &init();
    virtual asarray &init(const asarray &obj);

    /* padding of the total existing values in an a ssociated array */
    virtual asarray &clean(const datatype &one);

    /* set an associative array */
    virtual asarray &assign( const asarray &src );
    virtual asarray &assign( const char *key, const datatype &val );

    /* assign multiple strings or arrays of strings to the key */
    virtual asarray &assign_keys( const char *key0, ... );
    virtual asarray &vassign_keys( const char *key0, va_list ap );
    virtual asarray &assign_keys( const char *const *keys );
    virtual asarray &assign_keys( const tarray_tstring &keys );

    /* split a string and assign to the key */
    virtual asarray &split_keys( const char *src_str, const char *delims, 
				 bool zero_str, const char *quotations,
				 int escape, bool rm_escape );
    virtual asarray &split_keys( const char *src_str, const char *delims,
				 bool zero_str = false );
    virtual asarray &split_keys( const tstring &src_str, const char *delims, 
				 bool zero_str, const char *quotations,
				 int escape, bool rm_escape );
    virtual asarray &split_keys( const tstring &src_str, const char *delims,
				 bool zero_str = false );

    /* append an element */
    virtual asarray &append( const asarray &src );
    virtual asarray &append( const char *key, const datatype &val );

    /* insert an element */
    virtual asarray &insert( const char *key, const asarray &src );
    virtual asarray &insert( const char *key,
			     const char *newkey, const datatype &newval );

    /* erase element(s) */
    virtual asarray &erase();
    virtual asarray &erase( const char *key, size_t num_elements = 1 );

    /* swap self and another */
    virtual asarray &swap( asarray &sobj );

    /* change a key string */
    virtual asarray &rename_a_key( const char *org_key, const char *new_key );

    /* a reference to the element value corresponding to specified key or */
    /* element number                                                     */
    virtual datatype &at( const char *key );
    virtual datatype &atf( const char *fmt, ... );
    virtual datatype &vatf( const char *fmt, va_list ap );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const datatype &at( const char *key ) const;
    virtual const datatype &atf( const char *fmt, ... ) const;
    virtual const datatype &vatf( const char *fmt, va_list ap ) const;
#endif
    virtual const datatype &at_cs( const char *key ) const;
    virtual const datatype &atf_cs( const char *fmt, ... ) const;
    virtual const datatype &vatf_cs( const char *fmt, va_list ap ) const;

    /* length of an associated array */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;

    /* acquire the element number corresponding to the key string */
    virtual ssize_t index( const char *key ) const;
    virtual ssize_t indexf( const char *fmt, ... ) const;
    virtual ssize_t vindexf( const char *fmt, va_list ap ) const;

    /* acquire the key string corresponding to the element number */
    virtual const char *key( size_t index ) const;

    /* a reference to the element value corresponding to specified key or */
    /* element number                                                     */
    virtual datatype &at( size_t index );
    virtual const datatype &at_cs( size_t index ) const;

    /* a reference to the array object of the key string (read only) */
    virtual const tarray_tstring &keys() const;

    /* a reference to the array object of the value (read only) */
    virtual const tarray<datatype> &values() const;

    /* register a function to create object of datatype class */
    virtual void register_creator( datatype *(*func)(void *), void *user_ptr );

    /* ������֥������Ȥ� return ��ľ���˻Ȥ���shallow copy ����Ĥ������ */
    /* �Ȥ� (̤����)                                                         */
    virtual void set_scopy_flag();

#if 0	/* ���������Ȥ����ϤǤ��ʤ��餷����orz */
    virtual asarray &assign( const char *key0, const datatype &val0,
			     const char *key1, ... );
    virtual asarray &vassign( const char *key0, const datatype &val0,
			      const char *key1, va_list ap );
    virtual asarray &append( const char *key0, const datatype &val0, 
			     const char *key1, ... );
    virtual asarray &vappend( const char *key0, const datatype &val0, 
			      const char *key1, va_list ap );
    virtual asarray &insert( const char *key, 
			     const char *key0, const datatype &val0, 
			     const char *key1, ... );
    virtual asarray &vinsert( const char *key,
			      const char *key0, const datatype &val0, 
			      const char *key1, va_list ap );
#endif

  private:
    tarray_tstring key_rec;
    tarray<datatype> value_rec;
    ctindex index_rec;

    /* for error handling */
    void err_throw1( const char *func0, const char *level0, 
		     const char *message0, const char *arg ) const;
    void err_report1( const char *func0, const char *level0, 
		      const char *message0, const char *arg ) const;

  };
}

namespace sli
{

/* constructor */
/**
 * @brief  ���󥹥ȥ饯��
 */
template <class datatype>
asarray<datatype>::asarray()
{
    return;
}

/* copy constructor */

/**
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  ���Ȥ�obj�����Ƥǽ�������ޤ�
 * 
 * @param  obj asarray���饹�Υ��֥�������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
template <class datatype>
asarray<datatype>::asarray(const asarray<datatype> &obj)
{
    this->init(obj);
    return;
}

/* destructor */

/**
 * @brief  �ǥ��ȥ饯��
 *
 */
template <class datatype>
asarray<datatype>::~asarray()
{
    return;
}

template <class datatype>
/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿Ʊ���������饹�򰷤����֥������Ȥ򼫿Ȥ�
 *  ���ԡ����ޤ���
 * 
 * @param   obj asarray���饹�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 */
asarray<datatype> &asarray<datatype>::operator=(const asarray<datatype> &obj)
{
    this->init(obj);
    return *this;
}

template <class datatype>
/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤�
 *
 *  ���ꤵ�줿�������б�����Ϣ������������ͤλ��Ȥ��֤��ޤ���
 * 
 * @param   key Ϣ������Υ���ʸ����
 * @return  �������б�����Ϣ������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
inline datatype &asarray<datatype>::operator[]( const char *key )
{
    return this->at(key);
}

template <class datatype>
/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤� (�ɼ�����)
 *
 *  ���ꤵ�줿�������б�����Ϣ������������ͤλ��Ȥ��֤��ޤ���
 * 
 * @param   key Ϣ������Υ���ʸ����
 * @return  �������б�����Ϣ������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   ¸�ߤ��ʤ�����ʸ���󤬻��ꤵ�줿���
 */
inline const datatype &asarray<datatype>::operator[]( const char *key ) const
{
    return this->at_cs(key);
}

/*
 * public member functions
 */

template <class datatype>
/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�Ϣ�������õ�����֥������Ȥν������Ԥ��ޤ���
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::init()
{
    this->index_rec.init();
    this->key_rec.init();
    this->value_rec.init();
    return *this;
}

template <class datatype>
/**
 * @brief  ���֥������ȤΥ��ԡ�
 * 
 *  ���ꤵ�줿���֥������� obj �����Ƥ򼫿Ȥ˥��ԡ����ޤ���
 * 
 * @param   obj asarray���饹�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @throw   �����˲��򵯤��������
 */
asarray<datatype> &asarray<datatype>::init(const asarray<datatype> &obj)
{
    if ( this == &obj ) return *this;
    this->value_rec.init(obj.value_rec);
    this->key_rec.init(obj.key_rec);
    this->index_rec.init(obj.index_rec);
    return *this;
}

template <class datatype>
/**
 * @brief  ��¸�������ͤ��٤Ƥ���ꤵ�줿�ͤǥѥǥ���
 *
 *  ���Ȥ�Ϣ����������������Ƥ�Ǥ�դ��ͤǥѥǥ��󥰤��ޤ���
 * 
 * @param   one Ϣ������������Ǥ�ѥǥ��󥰤�����
 * @return  ���Ȥλ���
 * @throw   ���ǤΥХåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::clean(const datatype &one)
{
    this->value_rec.clean(one);
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿Ϣ������򼫿Ȥ�����
 *
 *  src�ǻ��ꤵ�줿Ϣ������������򡤼��Ȥ��������ޤ���
 * 
 * @param   src asarray���饹�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::assign(const asarray<datatype> &src)
{
    if ( this == &src ) return *this;
    this->erase();
    return this->append(src);
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿����(key,value) 1�ĤΤߤ�Ϣ������Ȥ���
 *
 *  ���Ȥ򡤻��ꤵ�줿 1�Ĥ����Ǥ��������Ϣ������ˤ��ޤ���
 * 
 * @param   key Ϣ����������ꤹ�륭��ʸ����
 * @param   val Ϣ����������ꤹ����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::assign( const char *key, 
					      const datatype &val )
{
    /* ���Ȥ��������Ƥ��� key, val ����Ƥ⤤���褦�ˤ��� */
    if ( key != NULL ) {
	size_t org_len = this->length();
	/* �ޤ����Ǹ���ɲä��� */
	this->value_rec.append(val,1);
	this->key_rec.append(key,1);
	/* ���Τ�Ĥ�õ�� */
	this->value_rec.erase(0, org_len);
	this->key_rec.erase(0, org_len);
	/* */
	this->index_rec.init();
	this->index_rec.append(this->key_rec.cstr(0), 0);
    }
    else {
	this->erase();
    }
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿ʣ����ʸ�����Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *  ���Ѱ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 * 
 * @param   key0 ����ʸ����
 * @param   ... ����ʸ����β���Ĺ�����Υꥹ�� (NULL��ü)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::assign_keys( const char *key0, ... )
{
    va_list ap;
    va_start(ap,key0);
    try {
	this->vassign_keys(key0,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","%s","this->vassign_keys() failed");
    }
    va_end(ap);
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿ʣ����ʸ�����Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ����򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *  ���Ѱ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 * 
 * @param   key0 ����ʸ����
 * @param   ap ����ʸ����β���Ĺ�����Υꥹ�� (NULL��ü)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::vassign_keys( const char *key0, va_list ap )
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

template <class datatype>
/**
 * @brief  ���ꤵ�줿ʸ���������Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ�������� keys �򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 *  ����ν�ü�� NULL �Ǥʤ���Фʤ�ޤ���
 * 
 * @param   keys ����ʸ��������ꤹ��ʸ�������� (NULL��ü)
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::assign_keys( const char *const *keys )
{
    tarray_tstring tmp_keys;
    if ( this->key_rec.cstrarray() == keys ) return *this;
    tmp_keys.assign(keys);
    this->assign_keys(tmp_keys);
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿ʸ���������Ϣ������Υ����Ȥ��Ƽ��Ȥ�����
 *
 *  ���ꤵ�줿ʣ����ʸ�������� keys �򼫿Ȥ�Ϣ������Υ��������ꤷ�ޤ���<br>
 * 
 * @param   keys ����ʸ��������ꤹ��ʸ��������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::assign_keys( const tarray_tstring &keys )
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

template <class datatype>
/**
 * @brief  ʸ�����ʬ�䤷�ơ����������� (¿��ǽ��)
 *
 *  ʸ�����ʬ�䤷��,���������ꤷ�ޤ���ʸ�����ʬ��������줿�����θĿ���
 *  Ķ����Ϣ���������ǤϺ������ޤ�. <br>
 *  delims �ˤϡ�" \t" �Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���<br>
 *  �������ơ��������̤Ρ������ʸ���פǰϤޤ줿ʸ�����ʬ�䤷�ʤ���硤
 *  quotations �ˡ������ʸ���פ���ꤷ�ޤ���
 * 
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @param   quotations �������ơ�����󡦥֥饱�å�ʸ����ޤ�ʸ����
 * @param   escape ����������ʸ��
 * @param   rm_escape ����������ʸ���������뤫�ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::split_keys( const char *src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

template <class datatype>
/**
 * @brief  ʸ�����ʬ�䤷�ơ�����������
 *
 *  ʸ�����ʬ�䤷��,���������ꤷ�ޤ���ʸ�����ʬ��������줿�����θĿ���
 *  Ķ����Ϣ���������ǤϺ������ޤ�. <br>
 *  delims �ˤϡ�" \t"�Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���
 * 
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::split_keys( const char *src_str, 
						  const char *delims,
						  bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}

template <class datatype>
/**
 * @brief  ʸ�����ʬ�䤷�ơ����������� (¿��ǽ��)
 *
 *  ʸ�����ʬ�䤷�ơ����������ꤷ�ޤ���ʸ�����ʬ��������줿�����θĿ���
 *  Ķ����Ϣ���������ǤϺ������ޤ���<br>
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
asarray<datatype> &asarray<datatype>::split_keys( const tstring &src_str, 
					 const char *delims, 
					 bool zero_str, const char *quotations,
					 int escape, bool rm_escape )
{
    tarray_tstring tmp_keys;

    tmp_keys.split(src_str,delims,zero_str,quotations,escape,rm_escape);
    this->assign_keys(tmp_keys);

    return *this;
}

template <class datatype>
/**
 * @brief  ʸ�����ʬ�䤷�ơ�����������
 *
 *  ʸ�����ʬ�䤷�ơ����������ꤷ�ޤ���ʸ�����ʬ��������줿�����θĿ���
 *  Ķ����Ϣ���������ǤϺ������ޤ�. <br>
 *  delims �ˤϡ�" \t"�Τ褦��ñ���ʸ���ꥹ�Ȥ˲ä�������ɽ�����Ѥ�����
 *  "[A-Z]" ���뤤�� "[^A-Z]" �Τ褦�ʻ��꤬��ǽ�Ǥ���
 * 
 * @param   src_str ʬ���оݤ�ʸ����
 * @param   delims ���ڤ�ʸ����ޤ�ʸ����
 * @param   zero_str Ĺ��0�ζ��ڤ��̤�ʸ�����������ɤ����Υե饰
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::split_keys( const tstring &src_str, 
						  const char *delims,
						  bool zero_str )
{
    return this->split_keys(src_str,delims,zero_str,NULL,'\0',false);
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿Ϣ������򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿ʣ�������Ǥ��ɲä��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   src �����Ȥʤ����Ǥ����asarary���饹�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::append( const asarray<datatype> &src )
{
    size_t i;
    if ( this == &src ) {
	/* ��������ʣ����ΤǼ��Ȥ��ɲä��뤳�ȤϤǤ��ʤ� */
	this->err_report1(__FUNCTION__,"WARNING", 
			  "%s", "cannot append self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->append(src.key(i),src.at_cs(i));
    }
 quit:
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿1���Ǥ򡤼��Ȥ�Ϣ��������ɲ�
 *
 *  ���Ȥ�Ϣ������ˡ����ꤵ�줿 1�Ĥ����Ǥ��ɲä��ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key Ϣ��������ɲä��륭��ʸ����
 * @param   val Ϣ��������ɲä�����
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::append( const char *key, 
					      const datatype &val )
{
    if ( key != NULL ) {
	if ( this->index_rec.index(key,0) < 0 ) {
	    this->value_rec.append(val,1);
	    this->key_rec.append(key,1);
	    this->index_rec.append(key,this->key_rec.length()-1);
	}
	else {
	    this->err_report1(__FUNCTION__,"WARNING",
			      "cannot append key '%s'",key);
	}
    }
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿Ϣ������򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿ʣ�������Ǥ�����
 *  ���ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ��뼫�Ȥ�Ϣ������Υ���ʸ����
 * @param   src �����Ȥʤ����Ǥ����asarary���饹�Υ��֥�������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::insert( const char *key,
					      const asarray<datatype> &src )
{
    size_t i;
    if ( this == &src ) {
	/* ��������ʣ����ΤǼ��Ȥ򥤥󥵡��Ȥ��뤳�ȤϤǤ��ʤ� */
	this->err_report1(__FUNCTION__,"WARNING", 
			  "%s", "cannot insert self-object");
	goto quit;
    }
    for ( i=0 ; i < src.length() ; i++ ) {
	this->insert(key,src.key(i),src.at_cs(i));
    }
 quit:
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿1���Ǥ򡤼��Ȥ�Ϣ�����������
 *
 *  ���Ȥ�Ϣ������Υ��� key �����ǰ��֤����ˡ����ꤵ�줿 1�Ĥ����Ǥ�����
 *  ���ޤ���<br>
 *  ��������ʣ������硤�¹Ի��˷ٹ𤬽��Ϥ��졤�������Ԥ��ޤ���
 * 
 * @param   key �������֤ˤ��뼫�Ȥ�Ϣ������Υ���ʸ����
 * @param   newkey Ϣ��������������륭��ʸ����
 * @param   newval Ϣ�����������������
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::insert( const char *key,
				   const char *newkey, const datatype &newval )
{
    if ( key != NULL && newkey != NULL ) {
	size_t i;
	ssize_t idx = this->index_rec.index(key,0);
	if ( idx < 0 ) {
	    this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	    goto quit;
	}
	if ( 0 <= this->index_rec.index(newkey,0) ) {
	    this->err_report1(__FUNCTION__,"WARNING",
			      "cannot insert key '%s'",newkey);
	    goto quit;
	}
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

template <class datatype>
/**
 * @brief  ���������Ǥκ��
 *
 *  ���Ȥ�Ϣ����������Ƥ����Ǥ�õ�ޤ���
 * 
 * @return  ���Ȥλ���
 * @hrow    �����Хåե��γ��ݤ˼��Ԥ������
 */
asarray<datatype> &asarray<datatype>::erase()
{
    this->index_rec.init();
    this->key_rec.erase();
    this->value_rec.erase();
    return *this;
}

template <class datatype>
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
asarray<datatype> &asarray<datatype>::erase( const char *key, 
					     size_t num_elements )
{
    size_t i, maxels;
    ssize_t idx;
    if ( key == NULL ) goto quit;
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
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

template <class datatype>
/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���֥�������sobj �����Ƥȼ��Ȥ����ƤȤ������ؤ��ޤ���
 * 
 * @param   sobj ���Ƥ������ؤ���asarray���֥�������
 * @return  ���Ȥλ���
 */
asarray<datatype> &asarray<datatype>::swap( asarray<datatype> &sobj )
{
    if ( this == &sobj ) return *this;
    this->key_rec.swap( sobj.key_rec );
    this->value_rec.swap( sobj.value_rec );
    this->index_rec.swap( sobj.index_rec );
    return *this;
}

template <class datatype>
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
asarray<datatype> &asarray<datatype>::rename_a_key( const char *org_key, 
						    const char *new_key )
{
    ssize_t idx;
    idx = this->index_rec.index(org_key,0);
    if ( idx < 0 ) {
	this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",org_key);
	goto quit;
    }
    if ( new_key == NULL || this->key_rec.at(idx).compare(new_key) == 0 )
	goto quit;
    if ( 0 <= this->index_rec.index(new_key,0) ) {
	this->err_report1(__FUNCTION__,"WARNING",
			  "already used: key '%s'. Not renamed.",new_key);
	goto quit;
    }
    this->index_rec.erase(org_key, idx);
    this->index_rec.append(new_key, idx);
    this->key_rec.at(idx).assign(new_key);
 quit:
    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿����ʸ������б������������Ǥλ��Ȥ��֤�
 * 
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
inline datatype &asarray<datatype>::at( const char *key )
{
    ssize_t idx;
    if ( key == NULL ) {
        this->err_throw1(__FUNCTION__,"ERROR","NULL key...?","");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
	this->append(key,this->value_rec.initial_one());
	idx = this->index_rec.index(key,0);
    }
    if ( idx < 0 ) {
        this->err_throw1(__FUNCTION__,"ERROR","not found key: %s",key);
    }
    return this->value_rec.at(idx);
}

template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б�����Ϣ������������ͤλ��Ȥ��֤��ޤ���<br>
 *  ���Υ��дؿ��Ǥϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥå�
 *  �Ȳ��Ѱ����ǥ��åȤǤ��ޤ�.
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
datatype &asarray<datatype>::atf( const char *fmt, ... )
{
    datatype *ret_p = NULL;
    va_list ap;
    va_start(ap,fmt);
    try {
	datatype &ret = this->vatf(fmt,ap);
	va_end(ap);
	ret_p = &ret;
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vatf() failed","");
    }
    return *(ret_p);
}

template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б�����Ϣ������������ͤλ��Ȥ��֤��ޤ�.<br>
 *  ���Υ��дؿ��Ǥϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥå�
 *  �Ȳ��Ѱ����ǥ��åȤǤ��ޤ�.
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
datatype &asarray<datatype>::vatf( const char *fmt, va_list ap )
{
    tstring buf;

    if ( fmt == NULL ) return this->at(fmt);

    buf.vprintf(fmt,ap);
    return this->at(buf.cstr());
}

#ifdef SLI__OVERLOAD_CONST_AT
template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ�����)
 *
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 */
inline const datatype &asarray<datatype>::at( const char *key ) const
{
    return this->at_cs(key);
}

template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const datatype &asarray<datatype>::atf( const char *fmt, ... ) const
{
    const datatype *ret_p = NULL;
    va_list ap;
    va_start(ap,fmt);
    try {
	const datatype &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	ret_p = &ret;
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vatf_cs() failed","");
    }
    return *(ret_p);
}
template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const datatype &asarray<datatype>::vatf( const char *fmt, va_list ap ) const
{
    return this->vatf_cs(fmt, ap);
}
#endif

template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ�����)
 * 
 * @param   key Ϣ������Υ���ʸ����
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ¸�ߤ��ʤ�����ʸ���󤬻��ꤵ�줿���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 */
inline const datatype &asarray<datatype>::at_cs( const char *key ) const
{
    ssize_t idx;
    if ( key == NULL ) {
        this->err_throw1(__FUNCTION__,"ERROR","NULL key...?","");
    }
    idx = this->index_rec.index(key,0);
    if ( idx < 0 ) {
        this->err_throw1(__FUNCTION__,"ERROR","not found key '%s'",key);
    }
    return this->value_rec.at_cs(idx);
}

template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ... fmt���б���������Ĺ�����γ����ǥǡ���
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ¸�ߤ��ʤ�����ʸ���󤬻��ꤵ�줿���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const datatype &asarray<datatype>::atf_cs( const char *fmt, ... ) const
{
    const datatype *ret_p = NULL;
    va_list ap;
    va_start(ap,fmt);
    try {
	const datatype &ret = this->vatf_cs(fmt,ap);
	va_end(ap);
	ret_p = &ret;
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vatf_cs() failed","");
    }
    return *(ret_p);
}

template <class datatype>
/**
 * @brief  ����ʸ������б������������Ǥλ��Ȥ��֤� (�ɼ����ѡ�printf()�ε�ˡ)
 *
 *  �����Υ���ʸ������б����������ͤλ���(�ɤ߼������)���֤��ޤ���<br>
 *  ���Υ��дؿ��ϻ��ꤷ��������ʸ����� printf() �ؿ���Ʊ�ͤΥե����ޥåȤ�
 *  ���Ѱ����ǥ��åȤǤ��ޤ���
 * 
 * @param   fmt ����ʸ����Τ���Υե����ޥåȻ���
 * @param   ap fmt���б���������Ĺ�����Υꥹ��
 * @return  ���ꤵ�줿�����˳������������ͤλ���
 * @throw   ¸�ߤ��ʤ�����ʸ���󤬻��ꤵ�줿���
 * @throw   ���ꤵ�줿����ʸ����NULL�ξ��
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 */
const datatype &asarray<datatype>::vatf_cs( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->at_cs(fmt);

    buf.vprintf(fmt,ap);
    return this->at_cs(buf.cstr());
}

template <class datatype>
/**
 * @brief  ����ʸ��������󥪥֥������Ȥλ��Ȥ���� (�ɼ�����)
 *
 *  ���֥���������Ǵ������Ƥ��륭��ʸ��������󥪥֥������Ȥλ���(�ɤ߼��
 *  ����)���֤��ޤ���
 * 
 * @return  ����ʸ��������󥪥֥�������
 */
inline const tarray_tstring &asarray<datatype>::keys() const
{
    return this->key_rec;
}

template <class datatype>
/**
 * @brief  �ͤ����󥪥֥������Ȥλ��Ȥ���� (�ɼ�����)
 *
 *  ���֥���������Ǵ������Ƥ����ͤ����󥪥֥������Ȥλ���(�ɤ߼������)
 *  ���֤��ޤ���
 * 
 * @return  �ͤ����󥪥֥������Ȥλ���
 */
inline const tarray<datatype> &asarray<datatype>::values() const
{
    return this->value_rec;
}

template <class datatype>
/**
 * @brief  Ϣ�������Ĺ��(�Ŀ�)�����
 * 
 * @return  ���Ȥ�Ϣ������θĿ�
 */
inline size_t asarray<datatype>::length() const
{
    return this->key_rec.length();
}

template <class datatype>
/**
 * @brief  Ϣ�������Ĺ��(�Ŀ�)�����
 * 
 * @return  ���Ȥ�Ϣ������θĿ�
 * @note    asarray<datatype>::length() �Ȥΰ㤤�Ϥ���ޤ���
 */
inline size_t asarray<datatype>::size() const
{
    return this->key_rec.size();
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿����ʸ������б����������ֹ���֤�
 * 
 * @param   key ����ʸ����
 * @return  �������: ���ꤵ�줿����ʸ���󤬸��Ĥ��ä���硥<br>
 *          �����: ���Ĥ���ʤ��ä���硥
 */
ssize_t asarray<datatype>::index( const char *key ) const
{
    if ( key == NULL ) return -1;
    return this->index_rec.index(key,0);
}

template <class datatype>
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
ssize_t asarray<datatype>::indexf( const char *fmt, ... ) const
{
    ssize_t ret = -1;
    va_list ap;
    va_start(ap,fmt);
    try {
	ret = this->vindexf(fmt,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vindexf() failed","");
    }
    va_end(ap);
    return ret;
}

template <class datatype>
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
ssize_t asarray<datatype>::vindexf( const char *fmt, va_list ap ) const
{
    tstring buf;

    if ( fmt == NULL ) return this->index(fmt);

    buf.vprintf(fmt,ap);
    return this->index(buf.cstr());
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿�����ֹ�˳������륭��ʸ������֤�
 * 
 *  ���ꤵ�줿�����ֹ�˳������륭��ʸ����Υ��ɥ쥹���֤��ޤ���
 * 
 * @param   index �����ֹ�
 * @return  ����ʸ����������Хåե��Υ��ɥ쥹��<br>
 *          �����Ĺ���ʾ�������ֹ椬���ꤵ�줿����NULL
 */
const char *asarray<datatype>::key( size_t index ) const
{
    if ( index < 0 || this->length() <= index ) return NULL;
    else return this->key_rec.cstr(index);
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿�����ֹ�˳������������ͤλ��Ȥ��֤�
 * 
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳������������ͤλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 */
inline datatype &asarray<datatype>::at( size_t index )
{
    static tstring buf(32);
    if ( index < 0 || this->length() <= index ) {
	buf.printf("%d",(int)index);
        this->err_throw1(__FUNCTION__,"ERROR","invalid index: %s",buf.cstr());
    }
    return this->value_rec.at(index);
}


template <class datatype>
/**
 * @brief  ���ꤵ�줿�����ֹ�˳������������ͤλ���(tstring)���֤� (�ɼ�����)
 * 
 * @param   index �����ֹ�
 * @return  ���ꤵ�줿�����ֹ�˳������������ͤλ���
 * @throw   ���ꤵ�줿�����ֹ椬�����ʾ��
 */
inline const datatype &asarray<datatype>::at_cs( size_t index ) const
{
    static tstring buf(32);
    if ( index < 0 || this->length() <= index ) {
	buf.printf("%d",(int)index);
        this->err_throw1(__FUNCTION__,"ERROR","invalid index: %s",buf.cstr());
    }
    return this->value_rec.at_cs(index);
}

/* new datatype ������ˡ����󥹥ȥ饯���˰�����Ϳ���������˻Ȥ� */

/**
 * @brief  datatype���饹�Υ��֥������Ȥ���ؿ�����Ͽ
 *
 *  asarray ���饹�����ǡ�new datatype�פ�����ˡ����󥹥ȥ饯���˰�����
 *  Ϳ���������˻Ȥ��ޤ�.
 *
 * @param     func �桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr �桼���ؿ��κǸ��Ϳ������桼���Υݥ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
template <class datatype>
void asarray<datatype>::register_creator( datatype *(*func)(void *), 
					  void *user_ptr )
{
    this->value_rec.register_creator(func, user_ptr);
    return;
}

/**
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ (̤����)
 * @deprecated  ̤����
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
template <class datatype>
void asarray<datatype>::set_scopy_flag()
{
    return;
}


/* �����󡤤��������Ȥ����ϤǤ��ʤ��餷����orz */
#if 0
template <class datatype>
asarray<datatype> &asarray<datatype>::assign( const char *key0, 
					      const datatype &val0,
					      const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vassign(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vassign() failed","");
    }
    va_end(ap);
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::vassign( const char *key0, 
					       const datatype &val0,
					       const char *key1, va_list ap )
{
    this->assign(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const datatype &val1 = va_arg(ap,const datatype &);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,const char *);
	    if ( keyx == NULL ) break;
	    const datatype &valx = va_arg(ap,const datatype &);
	    this->append(keyx,valx);
	}
    }
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::append( const char *key0, 
					      const datatype &val0,
					      const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vappend(key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vappend() failed","");
    }
    va_end(ap);
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::vappend( const char *key0, 
					       const datatype &val0,
					       const char *key1, va_list ap )
{
    this->append(key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const datatype &val1 = va_arg(ap,const datatype &);
	this->append(key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    if ( keyx == NULL ) break;
	    const datatype &valx = va_arg(ap,const datatype &);
	    this->append(keyx,valx);
	}
    }
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::insert( const char *key,
					const char *key0, const datatype &val0,
					const char *key1, ... )
{
    va_list ap;
    va_start(ap,key1);
    try {
	this->vinsert(key,key0,val0,key1,ap);
    }
    catch (...) {
	va_end(ap);
        this->err_throw1(__FUNCTION__,"FATAL","this->vinsert() failed","");
    }
    va_end(ap);
    return *this;
}
template <class datatype>
asarray<datatype> &asarray<datatype>::vinsert( const char *key,
					const char *key0, const datatype &val0,
					const char *key1, va_list ap )
{
    if ( key == NULL ) goto quit;
    if ( this->index_rec.index(key,0) < 0 ) {
	this->err_report1(__FUNCTION__,"WARNING","unknown key '%s'",key);
	goto quit;
    }
    this->insert(key,key0,val0);
    if ( key0 != NULL && key1 != NULL ) {
	const datatype &val1 = va_arg(ap,const datatype &);
	this->insert(key,key1,val1);
	while ( 1 ) {
	    const char *keyx = va_arg(ap,char *);
	    if ( keyx == NULL ) break;
	    const datatype &valx = va_arg(ap,const datatype &);
	    this->insert(key,keyx,valx);
	}
    }
 quit:
    return *this;
}
#endif


/* private member functions */

/**
 * @brief  ���顼���㳰ȯ������ err_rec �� throw ����
 *
 *  asarray���饹�δؿ���ǥ��顼���㳰ȯ�����˸ƤӽФ���ޤ���
 *  ɸ�२�顼���Ϥ˥��顼�����ɽ������err_rec �� throw���ޤ���
 *
 * @param	func0 �ƤӽФ����ؿ�̾
 * @param	level0 ���顼����<BR>
 *			�㳰ȯ����:"FATAL"<BR>
 *			���顼��:"ERROR"
 * @param	message0 ���顼��å�������
 * @param	arg ���顼��å��������ǥǡ���
 * @throw	err_rec:���顼�������Ĺ�¤��.
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
void asarray<datatype>::err_throw1( const char *func0, const char *level0, 
				  const char *message0, const char *arg ) const
{
    static stdstreamio sio;
    static tstring mes(128);
    const char *classname = "asarray";
    sio.eprintf("%s::%s(): [%s] ",classname,func0,level0);
    mes.printf(message0, arg);
    sio.eprintf("%s\n", mes.cstr());
    {
	size_t i, len;
	const char *src;
	char *dst;
	sli::err_rec st;
	/* */
	src = classname;  dst = st.class_name;   len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = func0;      dst = st.func_name;    len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = level0;     dst = st.level;        len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = mes.cstr(); dst = st.message;      len = 128;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	throw st;
    }
}


/**
 * @brief  �ٹ����Ϥ����뤿��Υ��дؿ�
 *
 *  asarray���饹�Υ��дؿ���Ƿٹ��ȯ��������Ȥ��˸ƤӽФ���ޤ���
 *  ɸ�२�顼���Ϥ˥��顼��å���������Ϥ��ޤ���
 *
 * @param	func0 �ƤӽФ������дؿ�̾
 * @param	level0 �ٹ�:"WARNING"
 * @param	message0 ���顼��å������ν�
 * @param	arg	���顼��å��������ǥǡ���
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
void asarray<datatype>::err_report1( const char *func0, const char *level0, 
				  const char *message0, const char *arg ) const
{
    static stdstreamio sio;
    const char *classname = "asarray";
    sio.eprintf("%s::%s(): [%s] ",classname,func0,level0);
    sio.eprintf(message0, arg);
    sio.eprintf("\n");
}

}	/* namespace sli */

#endif	/* _SLI__ASARRAY_H */
