/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:25:27 cyamauch> */

#ifndef _SLI__TARRAY_H
#define _SLI__TARRAY_H 1

/**
 * @file   tarray.h
 * @brief  Ǥ�ե��饹�Υ��֥�����������򰷤� tarray ���饹�Υ�����
 */

#include <stddef.h>
#include <sys/types.h>

#include "sli_config.h"
#include "stdstreamio.h"
#include "mdarray.h"

#include "slierr.h"

namespace sli
{

/*
 * tarray template class can handle arrays of arbitrary data types or classes.
 * Its function is almost the same as that of tarray_tstring,  except that this
 * class does not include member functions specializing in string.
 */

/**
 * @class  sli::tarray
 * @brief  Ǥ�ե��饹�Υ��֥�����������򰷤�����Υƥ�ץ졼�ȥ��饹
 *
 *   tarray �ƥ�ץ졼�ȥ��饹�ϡ�Ǥ�ե��饹�Υ��֥�����������򰷤������Ǥ���
 *   ����<br>
 *   ���������ˤ����ơ��������٤ε��ϤΥ��饹���ꤷ�Ƥ��ꡤ���Υƥ�ץ졼��
 *   ���饹�ǥץ�ߥƥ��ַ��򰷤����åȤϤ���ޤ��󡥥ץ�ߥƥ��ַ�������ˤ�
 *   mdarray��mdarray_int��mdarray_float �ʤɤ����Ѥ���������
 *
 * @author Chisato YAMAUCHI
 * @date 2013-04-01 00:00:00
 */

  template <class datatype> class tarray
  {

  public:
    /* constructor and destructor */
    tarray();
    tarray(const tarray &obj);
    virtual ~tarray();

    /*
     * operator
     */
    /* same as .init(sobj) */
    virtual tarray &operator=(const tarray &obj);

    /* same as .append(sobj,0) */
    virtual tarray &operator+=(const tarray &obj);

    /* same as .append(sobj,1) */
    virtual tarray &operator+=(const datatype &one);

    /* same as .at(key) */
    virtual datatype &operator[]( size_t index );
    virtual const datatype &operator[]( size_t index ) const;

    /*
     * member functions
     */
    /* complete initialization of objects */
    virtual tarray &init();
    virtual tarray &init(const tarray &obj);

    /* Padding of the existing values in an array by arbitrary ones */
    virtual tarray &clean(const datatype &one);

    /* substitute the object */
    virtual tarray &assign( const datatype &one, size_t n );
    virtual tarray &assign( const tarray &src, size_t idx2 = 0 );
    virtual tarray &assign( const tarray &src, 
			    size_t idx2, size_t n2 );

    /* append an element */
    virtual tarray &append( const datatype &one, size_t n );
    virtual tarray &append( const tarray &src, size_t idx2 = 0 );
    virtual tarray &append( const tarray &src, size_t idx2, size_t n2 );

    /* insert an element */
    virtual tarray &insert(size_t index, const datatype &one, size_t n);
    virtual tarray &insert( size_t index, 
			    const tarray &src, size_t idx2 = 0 );
    virtual tarray &insert( size_t index, 
			    const tarray &src, size_t idx2, size_t n2 );

    /* replace an element */
    virtual tarray &replace( size_t idx1, size_t n1, 
			     const datatype &one, size_t n2 );
    virtual tarray &replace( size_t idx1, size_t n1,
			     const tarray &src, size_t idx2 = 0 );
    virtual tarray &replace( size_t idx1, size_t n1,
			     const tarray &src, size_t idx2, size_t n2 );

    /* put a value to an arbitrary element's point */
    virtual tarray &put( size_t index, const datatype &one, size_t n );
    virtual tarray &put( size_t index, 
			 const tarray &src, size_t idx2 = 0);
    virtual tarray &put( size_t index, 
			 const tarray &src, size_t idx2, size_t n2 );

    /* change the length of the array */
    virtual tarray &resize( size_t new_num_elements );

    /* change the length of the array relatively */
    virtual tarray &resizeby( ssize_t len );

    /* cutout elements */
    virtual tarray &crop( size_t idx, size_t len );
    virtual tarray &crop( size_t idx );

    /* erase element(s) */
    virtual tarray &erase();
    virtual tarray &erase( size_t index, size_t num_elements = 1 );

    /* copy all or some elements to another object */
    virtual ssize_t copy( size_t index, size_t n, tarray *dest ) const;
    virtual ssize_t copy( size_t index, tarray *dest ) const;
    virtual ssize_t copy( tarray *dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t index, size_t n, tarray &dest ) const;
    /* not recommended */
    virtual ssize_t copy( size_t index, tarray &dest ) const;
    /* not recommended */
    virtual ssize_t copy( tarray &dest ) const;

    /* swap self and another */
    virtual tarray &swap( tarray &sobj );

    /* a reference to the specified element */
    virtual datatype &at( size_t index );
#ifdef SLI__OVERLOAD_CONST_AT
    virtual const datatype &at( size_t index ) const;
#endif
    virtual const datatype &at_cs( size_t index ) const;

    /* length of the array (the number of elements) */
    virtual size_t length() const;
    /* not recommended */
    virtual size_t size() const;

    /* register a function to create object of datatype class */
    virtual void register_creator( datatype *(*func)(void *), void *user_ptr );
    virtual const datatype &initial_one() const;

    /* ������֥������Ȥ� return ��ľ���˻Ȥ���shallow copy ����Ĥ������ */
    /* �Ȥ� (̤����)                                                         */
    virtual void set_scopy_flag();

  private:
    bool request_shallow_copy( datatype *from_obj ) const;

    /* ���顼�ϥ�ɥ�� */
    void err_throw( const char *func0, const char *level0, 
		    const char *message0 ) const;

  private:
    mdarray arrs_rec;
    /* ����ͤ򥭡��פ���(��delete) */
    datatype *initial_one_rec;
    /* datatype���饹�Υ��֥������Ȥ���ؿ� */
    datatype *(*creator)(void *);
    void *creator_arg;

    /* shallow copy �Τ���Υե饰��= ���Ǥϥ��ԡ�����ʤ� */
    /* ���ԡ��� src ¦�Υե饰 */
    bool shallow_copy_ok;		/* set_scopy_flag() �ǥ��åȤ���� */
    datatype *shallow_copy_dest_obj;	/* ���ԡ���: ͭ���ʤ� non-NULL */

    /* ���ԡ��� dest ¦�Υե饰(���ԡ�������񤭹��ޤ��) */
    datatype *shallow_copy_src_obj;	/* ���ԡ���: �����Ƥ���� non-NULL */

    /* __shallow_init(), __deep_init() ������˹Ԥʤ�����Υե饰 */
    bool __copying;

    /*
      gcc �ǥ��顼��Ф�����Ρ����ߡ��Υ��дؿ��� 

      ���Ȥ������ȤΥ����С����ɤϤ��ʤ��Ф��褦����
      �Ȥ����櫓�ǡ�private �����򡥡���
     */
    tarray &assign( size_t n );
    tarray &append( size_t n );
    tarray &insert( size_t index, size_t n );
    tarray &replace( size_t idx1, size_t n1, size_t n2 );
    tarray &put( size_t index, size_t n );

  };
}

/* SOURCE */

namespace sli
{

/* constructor */
template <class datatype>
/**
 * @brief  ���󥹥ȥ饯��
 *
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
tarray<datatype>::tarray()
{
    static datatype one;
    try {
	this->initial_one_rec = new datatype;
	*(this->initial_one_rec) = one;
    }
    catch (...) {
	this->err_throw(__FUNCTION__,"FATAL","new failed");
    }

    this->creator = NULL;
    this->creator_arg = NULL;

    this->arrs_rec.init(sizeof(datatype *), true);

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    return;
}

/* copy constructor */
template <class datatype>
/**
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  ���Ȥ�obj�����Ƥǽ�������ޤ�
 *
 * @param  obj tarray<datatype>���֥�������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 * @throw  �����˲��򵯤������Ȥ�
 */
tarray<datatype>::tarray(const tarray<datatype> &obj)
{
    static datatype one;
    try {
	this->initial_one_rec = new datatype;
	*(this->initial_one_rec) = one;
    }
    catch (...) {
	this->err_throw(__FUNCTION__,"FATAL","new failed");
    }

    this->creator = NULL;
    this->creator_arg = NULL;

    this->arrs_rec.init(sizeof(datatype *), true);

    this->shallow_copy_ok = false;
    this->shallow_copy_dest_obj = NULL;
    this->shallow_copy_src_obj = NULL;
    this->__copying = false;

    this->init(obj);

    return;
}

/* destructor */
template <class datatype>
/**
 * @brief  �ǥ��ȥ饯��
 *
 */
tarray<datatype>::~tarray()
{
    size_t i;
    for ( i=0 ; i < this->arrs_rec.length() ; i++ ) {
	datatype *objp = (datatype *)(this->arrs_rec.p(i));
	if ( objp != NULL ) {
	    delete objp;
	}
    }
    if ( this->initial_one_rec != NULL ) {
	delete this->initial_one_rec;
    }
    return;
}

template <class datatype>
/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿Ʊ���������饹�򰷤����֥������Ȥ򼫿Ȥ�
 *  ���ԡ����ޤ���
 *
 * @param     obj Ʊ���������饹�򰷤��������ĥ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @throw     �����˲��򵯤��������
 */
tarray<datatype> &tarray<datatype>::operator=(const tarray<datatype> &obj)
{
    this->init(obj);
    return *this;
}


template <class datatype>
/**
 * @brief  ���Ȥ�����κǸ�ˡ�������ɲ�
 *
 *  ���Ȥ�����ˡ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿������ɲä�Ԥ��ޤ�.
 *
 * @param     obj Ʊ���������饹�򰷤��������ĥ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @throw     �����˲��򵯤��������
 */
tarray<datatype> &tarray<datatype>::operator+=(const tarray<datatype> &obj)
{
    this->append(obj,0);
    return *this;
}

template <class datatype>
/**
 * @brief  ���Ȥ�����κǸ�ˡ�1 ���Ǥ��ɲ�
 *
 *  ���Ȥ�����ˡ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿1 ���Ǥ��ɲä��ޤ�.
 *
 * @param     one �ɲä�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::operator+=(const datatype &one)
{
    this->append(one,1);
    return *this;
}

template <class datatype>
/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤�
 *
 *  ź�����ǻ��ꤵ�줿�������Ǥλ��Ȥ��֤��ޤ���<br>
 *  "[]"��ľ��� "." ����³����type ���饹�Υ��дؿ���Ȥ������Ǥ��ޤ���<br>
 *  ����Ĺ�ʾ�� index �����ꤵ�줿���ϡ��������������Ǥ�����ޤ���
 *  �Хåե��γ��ݤ˼��Ԥ�������������㳰��ȯ�����ޤ���
 *
 * @param     index 0 ����Ϥޤ������ֹ�
 * @return    ź�����ǻ��ꤵ�줿�������Ǥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
inline datatype &tarray<datatype>::operator[]( size_t index )
{
    return this->at(index);
}

template <class datatype>
/**
 * @brief  [] �ǻ��ꤵ�줿�����ͤλ��Ȥ��֤� (�ɼ�����)
 *
 *  ź�����ǻ��ꤵ�줿�������Ǥλ��Ȥ��֤��ޤ���<br>
 *  "[]"��ľ��� "." ����³����type ���饹�Υ��дؿ���Ȥ������Ǥ��ޤ���<br>
 *  �ɤ߼�����Ѥǡ�index �������Ĺ���ʾ���ͤ����ꤵ�줿��硤�㳰��ȯ��
 *  ���ޤ���
 *
 * @param     index 0 ����Ϥޤ������ֹ�
 * @return    ź�����ǻ��ꤵ�줿�������Ǥλ���
 * @throw     index ������Ĺ�ʾ���ͤ����ꤵ�줿���
 */
inline const datatype &tarray<datatype>::operator[]( size_t index ) const
{
    return this->at_cs(index);
}

/*
 * public member functions
 */

template <class datatype>
/**
 * @brief  ���֥������Ȥν����
 *
 *  ���֥������Ȥ�������������Хåե��˳�����Ƥ�줿�����ΰ�ϳ�������
 *  �ޤ���
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::init()
{
    size_t i;
    for ( i=0 ; i < this->arrs_rec.length() ; i++ ) {
	datatype *objp = (datatype *)(this->arrs_rec.p(i));
	if ( objp != NULL ) {
	    delete objp;
	}
    }
    this->arrs_rec.init(sizeof(datatype *), true);
    this->register_creator(NULL,NULL);
    return *this;
}

template <class datatype>
/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  ���ꤵ�줿���֥������� obj �����Ƥǽ�������ޤ� (obj �����Ƥ��٤Ƥ򼫿Ȥ�
 *  ���ԡ����ޤ�)��
 *
 * @param     obj tarray ���饹�Υ��֥�������(���ԡ���)
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @throw     �����˲��򵯤��������
 */
tarray<datatype> &tarray<datatype>::init(const tarray<datatype> &obj)
{
    if ( &obj == this ) return *this;
    tarray<datatype>::init();
    this->register_creator(obj.creator, obj.creator_arg);

    return this->append(obj,0);
}

template <class datatype>
/**
 * @brief  ��¸���������Τ���ꤵ�줿�ͤǥѥǥ���
 *
 *  ���Ȥ���������������Ǥ��� one �ǥѥǥ��󥰤��ޤ���
 *  clean() ��¹Ԥ��Ƥ�����Ĺ���Ѳ����ޤ���
 *
 * @param     one �����ѥǥ��󥰤��뤿�����
 * @return    ���Ȥλ���
 * @throw     ������Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::clean(const datatype &one)
{
    size_t i;
    for ( i=0 ; i < this->length() ; i++ ) {
	datatype *objp = (datatype *)(this->arrs_rec.p(i));
	*(objp) = one;
    }
    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::assign( size_t n )
{
    return this->replace(0, this->length(), n);
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿�ͤ�n�� ���Ȥ�����
 *
 *  ���Ȥ��������Ǥ�n�ĤȤ������٤Ƥ����Ǥ˻��ꤵ�줿�ͤ��������ޤ���
 *
 * @param     one �����Ȥʤ���
 * @param     n ��one ��񤭹���Ŀ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::assign( const datatype &one, size_t n )
{
    return this->replace(0, this->length(), one, n);
}


template <class datatype>
/**
 * @brief  ���ꤵ�줿�����(����)���Ǥ򼫿Ȥ�����
 *
 *  src�ǻ��ꤵ�줿����������ޤ��ϰ����򡤼��Ȥ��������ޤ���
 *
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::assign(const tarray<datatype> &src,
					   size_t idx2)
{
    return this->replace(0, this->length(), src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿�����(����)���Ǥ򼫿Ȥ�����
 *
 *  src�ǻ��ꤵ�줿����������ޤ��ϰ����򡤼��Ȥ��������ޤ���
 *
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @param     n2 src ������Ф����ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::assign( const tarray<datatype> &src, 
					    size_t idx2, size_t n2 )
{
    return this->replace(0, this->length(), src, idx2, n2);
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::append( size_t n )
{
    return this->replace(this->length(), 0, n);
}

template <class datatype>
/**
 * @brief  ���Ȥ�����κǸ�ˡ����ꤵ�줿�ͤ�n���ɲ�
 *
 *  ���Ȥ�����κǸ�ˡ����ꤵ�줿�ͤ��������n�Ĥ��ɲä��ޤ���
 *
 * @param     one �����Ȥʤ���
 * @param     n ��one ��񤭹���Ŀ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::append( const datatype &one, size_t n )
{
    return this->replace(this->length(), 0, one, n);
}

template <class datatype>
/**
 * @brief  �����(����)���Ǥ򡤼��Ȥ�����κǸ���ɲ�
 *
 *  src�ǻ��ꤵ�줿����򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::append(const tarray<datatype> &src,
					   size_t idx2)
{
    return this->replace(this->length(), 0, src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  �����(����)���Ǥ򡤼��Ȥ�����κǸ���ɲ�
 *
 *  src�ǻ��ꤵ�줿����򡤼��Ȥ�����κǸ����ʹߤ��ɲä��ޤ���
 *
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @param     n2 src ������Ф����ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::append( const tarray<datatype> &src, 
					    size_t idx2, size_t n2 )
{
    return this->replace(this->length(), 0, src, idx2, n2);
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::insert( size_t index, size_t n )
{
    return this->replace(index,0, n);
}

template <class datatype>
/**
 * @brief  ���Ȥ�����λ�����֤ˡ����ꤵ�줿�ͤ�n������
 *
 *  ���Ȥ�����������ֹ�index�ΰ��֤ˡ����ꤵ�줿�ͤ��������n�Ĥ�����
 *  ���ޤ���<br>
 *  index �������Ĺ���ʾ���ͤ���ꤹ���硤index �˼��Ȥ������Ĺ��
 *  ��Ϳ������ΤȤߤʤ��ޤ���
 *
 * @param     index ���Ȥ��������������
 * @param     one �����Ȥʤ���
 * @param     n ��one ��񤭹���Ŀ�
 * @return    ���Ȥλ���
 */
tarray<datatype> &tarray<datatype>::insert( size_t index, 
					    const datatype &one, size_t n )
{
    return this->replace(index,0, one,n);
}

template <class datatype>
/**
 * @brief  �����(����)���Ǥ򡤼��Ȥ�����λ�����֤�����
 *
 *  src�ǻ��ꤵ�줿����򡤼��Ȥ�����λ������index���������ޤ���<br>
 *  index �������Ĺ���ʾ���ͤ���ꤹ���硤index �˼��Ȥ������Ĺ����
 *  Ϳ������ΤȤߤʤ��ޤ���
 *
 * @param     index ���Ȥ��������������
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @return    ���Ȥλ���
 */
tarray<datatype> &tarray<datatype>::insert( size_t index,
				      const tarray<datatype> &src, size_t idx2)
{
    return this->replace(index,0, src,idx2,src.length());
}

template <class datatype>
/**
 * @brief  �����(����)���Ǥ򡤼��Ȥ�����λ�����֤�����
 *
 *  src�ǻ��ꤵ�줿����򡤼��Ȥ�����λ������index���������ޤ���<br>
 *  index �������Ĺ���ʾ���ͤ���ꤹ���硤index �˼��Ȥ������Ĺ����
 *  Ϳ������ΤȤߤʤ��ޤ���
 *
 * @param     index ���Ȥ��������������
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @param     n2 src ������Ф����ǤθĿ�
 * @return    ���Ȥλ���
 */
tarray<datatype> &tarray<datatype>::insert( size_t index, 
					    const tarray<datatype> &src, 
					    size_t idx2, size_t n2 )
{
    return this->replace(index,0, src,idx2,n2);
}

template <class datatype>
/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿�ͤ��ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ򡤻��ꤵ�줿�ͤ��������
 *  n2�Ĥ��ִ����ޤ���<br>
 *  idx1 ���������ǿ��ʾ���ͤ����ꤵ�줿��硤append() ���дؿ���Ʊ�ͤ�
 *  ������Ԥ��ޤ���<br>
 *  idx1 ��n1 ���¤���������ǿ������礭�����䡤�ޤ�n1��n2 ���羮�ط���
 *  �������γ�ĥ�����̤�ɬ�פʾ������ǿ���ưŪ��Ĵ�����ޤ���
 *
 * @param     idx1 ���Ȥ�����γ��ϰ���
 * @param     n1 �ִ���������ǿ�
 * @param     one �����Ȥʤ���
 * @param     n ��one ��񤭹���Ŀ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1,
					     const datatype &one, size_t n2 )
{
    size_t new_length, crt_length = this->length();
    size_t i;

    /* ��� */
    /* ���� src �˼��Ȥ���ꤵ��Ƥ��ɤ��褦�ˡ��ΰ褬�������ʤ���� */
    /* �����������褦�ˤ��Ƥ��� */

    if ( crt_length < idx1 ) idx1 = crt_length;
    if ( crt_length < idx1 + n1 ) n1 = crt_length - idx1;

    if ( n1 < n2 ) {
	new_length = crt_length + (n2 - n1);
    }
    else {	/* (n2 <= n1)	to be small or no-sizechange */
	/* ����������Ƥ��ޤ�  */
	try {
	    datatype *objp_dst;
	    for ( i=0 ; i < n2 ; i++ ) {
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + i));
		*(objp_dst) = one;
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL","*(objp_dst) = one; failed");
	}
	new_length = crt_length - (n1 - n2);
	for ( i=0 ; i < (n1 - n2) ; i++ ) {
	    datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n2) + i));
	    delete objp;
	}
	if ( n2 < n1 ) {
	    this->arrs_rec.move(idx1 + n1, crt_length - idx1 - n1, idx1 + n2,
				false);
	}
    }

    if ( new_length != crt_length ) {
	this->arrs_rec.resize(new_length);
    }
    if ( n1 < n2 ) {		/* to be large */
	ssize_t src_p = idx1 + n1;
	ssize_t dest_p = idx1 + n2;
	size_t sz = crt_length - idx1 - n1;
	/* ���ɥ쥹�ơ��֥����ʬŪ��ư */
	this->arrs_rec.move(src_p, sz, dest_p, true);
	/* �������֥����������� */
	try {
	    datatype *objp;
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
		if ( this->creator != NULL ) 
		    objp = (*(this->creator))(this->creator_arg);
		else objp = new datatype;
		this->arrs_rec.p((idx1 + n1) + i) = (uintptr_t)objp;
	    }
	}
	catch (...) {
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
	      datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n1) + i));
	      if ( objp != NULL ) {
		  delete objp;
	      }
	    }
	    this->arrs_rec.move(dest_p, sz, src_p, true);
	    this->err_throw(__FUNCTION__,"FATAL","new failed");
	}
	/* ���� */
	try {
	    datatype *objp_dst;
	    for ( i=0 ; i < n2 ; i++ ) {
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + i));
		*(objp_dst) = one;
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL","*(objp_dst) = one; failed");
	}
    }

    return *this;
}

template <class datatype>
/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿������ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ�src�ǻ��ꤵ�줿�����
 *  �ִ����ޤ���<br>
 *  idx1 ���������ǿ��ʾ���ͤ����ꤵ�줿��硤append() ���дؿ���Ʊ�ͤ�
 *  ������Ԥ��ޤ���<br>
 *  idx1 ��n1 ���¤���������ǿ������礭�����䡤�ޤ�n1��n2 ���羮�ط���
 *  �������γ�ĥ�����̤�ɬ�פʾ������ǿ���ưŪ��Ĵ�����ޤ���
 *
 * @param     idx1 ���Ȥ�����γ��ϰ���
 * @param     n1 �ִ���������ǿ�
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1,
					     const tarray<datatype> &src,
					     size_t idx2 )
{
    return this->replace(idx1, n1, src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  ����ΰ����ޤ��������򡤻��ꤵ�줿������ִ�
 *
 *  ���Ȥ���������Ǥΰ���idx1����n1�Ĥ����Ǥ�src�ǻ��ꤵ�줿�����
 *  �ִ����ޤ���<br>
 *  idx1 ���������ǿ��ʾ���ͤ����ꤵ�줿��硤append() ���дؿ���Ʊ�ͤ�
 *  ������Ԥ��ޤ���<br>
 *  idx1 ��n1 ���¤���������ǿ������礭�����䡤�ޤ�n1��n2 ���羮�ط���
 *  �������γ�ĥ�����̤�ɬ�פʾ������ǿ���ưŪ��Ĵ�����ޤ���
 *
 * @param     idx1 ���Ȥ�����γ��ϰ���
 * @param     n1 �ִ���������ǿ�
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     n2 src ������Ф����ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1,
					     const tarray<datatype> &src, 
					     size_t idx2, size_t n2 )
{
    size_t new_length, crt_length = this->length();
    size_t i;

    /* ��� */
    /* ���� src �˼��Ȥ���ꤵ��Ƥ��ɤ��褦�ˡ�������ȹ��פ��Ƥ��� */
    /* - �ΰ褬�������ʤ���ϡ�������� */
    /* - �ΰ褬�礭���ʤ���ϡ����ɥ쥹�ơ��֥�ꥵ�����塤*/
    /*   ���ɥ쥹�ơ��֥��Υ��ɥ쥹����¸���Ƥ��顤NULL�ʥ��ɥ쥹�ơ��֥� */
    /*   �� new ����ᡤ�Ǹ����¸�������ɥ쥹��Υ��֥������Ȥ򥳥ԡ� */

    if ( src.length() < idx2 ) idx2 = src.length();
    if ( src.length() < idx2 + n2 ) n2 = src.length() - idx2;

    if ( crt_length < idx1 ) idx1 = crt_length;
    if ( crt_length < idx1 + n1 ) n1 = crt_length - idx1;

    if ( n1 < n2 ) {
	new_length = crt_length + (n2 - n1);
    }
    else {	/* (n2 <= n1)	to be small or no-sizechange */
	/* ����������Ƥ��ޤ�  */
	try {
	    const datatype *objp_src;
	    datatype *objp_dst;
	    size_t j;
	    for ( i=0 ; i < n2 ; i++ ) {
		if ( idx1 <= idx2 ) j = i;
		else j = n2 - i - 1;
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + j));
		objp_src = (const datatype *)(src.arrs_rec.p_cs(idx2 + j));
		*(objp_dst) = *(objp_src);
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL",
			    "*(objp_dst) = *(objp_src) failed");
	}
	new_length = crt_length - (n1 - n2);
	for ( i=0 ; i < (n1 - n2) ; i++ ) {
	    datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n2) + i));
	    delete objp;
	}
	if ( n2 < n1 ) {
	    this->arrs_rec.move(idx1 + n1, crt_length - idx1 - n1, idx1 + n2,
				false);
	}
    }

    if ( new_length != crt_length ) {
	this->arrs_rec.resize(new_length);
    }

    if ( n1 < n2 ) {		/* to be large */
	ssize_t src_p = idx1 + n1;
	ssize_t dest_p = idx1 + n2;
	size_t sz = crt_length - idx1 - n1;
	const size_t nx[] = {n2};
	mdarray adr_save(sizeof(datatype *), true, nx, 1, false);
	/* src �˼��Ȥ����ꤵ�줿�������� .move() �ǥ��ɥ쥹�������Τ���¸ */
	for ( i=0 ; i < n2 ; i++ ) {
	    adr_save.p(i) = src.arrs_rec.p_cs(idx2 + i);
	}
	/* ���ɥ쥹�ơ��֥����ʬŪ��ư */
	this->arrs_rec.move(src_p, sz, dest_p, true);
	/* �������֥����������� */
	try {
	    datatype *objp;
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
		if ( this->creator != NULL ) 
		    objp = (*(this->creator))(this->creator_arg);
		else objp = new datatype;
		this->arrs_rec.p((idx1 + n1) + i) = (uintptr_t)objp;
	    }
	}
	catch (...) {
	    for ( i=0 ; i < (n2 - n1) ; i++ ) {
	      datatype *objp = (datatype *)(this->arrs_rec.p((idx1 + n1) + i));
	      if ( objp != NULL ) {
		  delete objp;
	      }
	    }
	    this->arrs_rec.move(dest_p, sz, src_p, true);
	    this->err_throw(__FUNCTION__,"FATAL","new failed");
	}
	/* adr_save ����¸�������󥹥��󥹤Υ��ɥ쥹��Ȥäơ����� */
	try {
	    const datatype *objp_src;
	    datatype *objp_dst;
	    size_t j;
	    for ( i=0 ; i < n2 ; i++ ) {
		if ( idx1 <= idx2 ) j = i;
		else j = n2 - i - 1;
		objp_dst = (datatype *)(this->arrs_rec.p(idx1 + j));
		objp_src = (const datatype *)(adr_save.p(j));
		*(objp_dst) = *(objp_src);
	    }
	}
	catch (...) {
	    this->err_throw(__FUNCTION__,"FATAL",
			    "*(objp_dst) = *(objp_src) failed");
	}
    }

    return *this;
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::replace( size_t idx1, size_t n1, 
					     size_t n2 )
{
    return this->replace(idx1,n1,this->initial_one(),n2);
}

/**
 * @brief  ����ѥ�����˥��顼����Ϥ����뤿��Υ��ߡ��Υ��дؿ�
 * @deprecated  �����Բġ�
 * @note	���Υ��дؿ���private�Ǥ�
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::put( size_t index, size_t n )
{
    if ( this->length() < index + n ) {
	this->replace(this->length(), 0, 
		      this->initial_one(), (index + n) - this->length());
    }
    return this->replace(index, n, n);
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿�ͤǡ����Ȥ�����λ�����֤���n�ľ��
 *
 *  ���Ȥ�����������ֹ�index�ΰ��֤��顤���ꤵ�줿�ͤ��������n�Ĥ�
 *  ��񤭤��ޤ���<br>
 *  index ��Ǥ�դ��ͤ�������Ǥ��ޤ��������λ�����Ф�������������ǿ���
 *  ��­���Ƥ�����ϡ���ưŪ�˥��������ĥ���ޤ���<br>
 *  ���Ǥ����ξ��֤ˤ����ơ��㤨�С�my_arr.put(0,value,6) �� 
 *  my_arr.put(2,value,4) �η�̤�Ʊ���Ǥ���<br>
 *  ����ˡ����ǿ� 4 �Ĥ�������Ф��� my_arr.put(2,value,4) �Ȥ���ȡ����ǿ���
 *  6 �ĤȤʤꡤ�����ֹ� 2 �ʹߤ����Ǥ� value �ǻ��ꤵ�줿��ΤȤʤ�ޤ���
 *
 * @param     index ���Ȥ�����ν񤭹��߰���
 * @param     one �����Ȥʤ���
 * @param     n ��one ��񤭹���Ŀ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::put( size_t index, 
					 const datatype &one, size_t n )
{
    if ( this->length() < index + n ) {
	this->replace(this->length(), 0, 
		      this->initial_one(), (index + n) - this->length());
    }
    return this->replace(index,n, one,n);
}

/**
 * @brief  ���ꤵ�줿����򡤼��Ȥ�����λ�����֤˾��
 *
 *  src�ǻ��ꤵ�줿����򡤼��Ȥ���������ǰ���index�����񤭤��ޤ���
 *  index ��Ǥ�դ��ͤ�������Ǥ��ޤ��������λ�����Ф�������������ǿ���
 *  ��­���Ƥ�����ϡ���ưŪ�˥��������ĥ���ޤ���<br>
 *  ���Ǥ����ξ��֤ˤ����ơ��㤨�С�my_arr.put(0,value,6) �� 
 *  my_arr.put(2,value,4) �η�̤�Ʊ���Ǥ���<br>
 *  ����ˡ����ǿ� 4 �Ĥ�������Ф��� my_arr.put(2,value,4) �Ȥ���ȡ����ǿ���
 *  6 �ĤȤʤꡤ�����ֹ� 2 �ʹߤ����Ǥ� value �ǻ��ꤵ�줿��ΤȤʤ�ޤ���
 *
 * @param     index ���Ȥ�����ν񤭹��߰���
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
template <class datatype>
tarray<datatype> &tarray<datatype>::put( size_t index,
					 const tarray<datatype> &src,
					 size_t idx2)
{
    return this->put(index, src, idx2, src.length());
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿����򡤼��Ȥ�����λ�����֤˾��
 *
 *  src�ǻ��ꤵ�줿����򡤼��Ȥ���������ǰ���index�����񤭤��ޤ���
 *  index ��Ǥ�դ��ͤ�������Ǥ��ޤ��������λ�����Ф�������������ǿ���
 *  ��­���Ƥ�����ϡ���ưŪ�˥��������ĥ���ޤ���<br>
 *  ���Ǥ����ξ��֤ˤ����ơ��㤨�С�my_arr.put(0,value,6) �� 
 *  my_arr.put(2,value,4) �η�̤�Ʊ���Ǥ���<br>
 *  ����ˡ����ǿ� 4 �Ĥ�������Ф��� my_arr.put(2,value,4) �Ȥ���ȡ����ǿ���
 *  6 �ĤȤʤꡤ�����ֹ� 2 �ʹߤ����Ǥ� value �ǻ��ꤵ�줿��ΤȤʤ�ޤ���
 *
 * @param     index ���Ȥ�����ν񤭹��߰���
 * @param     src �����Ȥʤ��������Ǥ���ĥ��֥�������
 * @param     idx2 src �����Ǥγ����ֹ�
 * @param     n2 src ������Ф����ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::put( size_t index, 
					 const tarray<datatype> &src, 
					 size_t idx2, size_t n2 )
{
    size_t len = src.length();
    if ( idx2 <= len ) {
	size_t max = len - idx2;
	if ( max < n2 ) n2 = max;
	if ( this->length() < index + n2 ) {
	    this->replace(this->length(), 0, 
			  this->initial_one(), (index + n2) - this->length());
	}
	return this->replace(index,n2, src,idx2,n2);
    }
    return *this;
}

template <class datatype>
/**
 * @brief  �����������ʬ�ξõ�
 *
 *  ���Ȥ�����������ֹ�idx ����len �Ĥ����Ǥ����ˤ��ޤ���
 *
 * @param     idx �ڤ�Ф����Ǥγ��ϰ���
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::crop( size_t idx, size_t len )
{
    size_t max_n;
    if ( this->length() < idx ) idx = this->length();
    max_n = this->length() - idx;
    if ( max_n < len ) len = max_n;
    this->erase(0, idx);
    this->erase(len, this->length() - len);
    return *this;
}

template <class datatype>
/**
 * @brief  �����������ʬ�ξõ�
 *
 *  ���Ȥ�����������ֹ�idx �ʹߤ���������ˤ��ޤ���
 *
 * @param     idx �ڤ�Ф����Ǥγ��ϰ���
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::crop( size_t idx )
{
    if ( this->length() < idx ) idx = this->length();
    return this->crop(idx, this->length() - idx);
}

template <class datatype>
/**
 * @brief  ���������Ǥκ��
 *
 *  ���Ȥ���������������Ǥ������ޤ�(����Ĺ�ϥ���ˤʤ�ޤ�)��
 *
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::erase()
{
    return this->erase(0,this->length());
}

template <class datatype>
/**
 * @brief  �������Ǥκ��
 *
 *  �����ֹ� index �����Ǥ��� num_el �Ĥ����Ǥ������ޤ���<br>
 *  num_el �����ꤵ��ʤ����ϡ�1 �Ĥ����Ǥ������ޤ���<br>
 *  index �������Ĺ���ʾ���ͤ����ꤵ�줿��硤̵�뤵��ޤ���
 *
 * @param     index �����ֹ�
 * @param     num_elements ���ǤθĿ�
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::erase( size_t index, size_t num_el )
{
    return this->replace(index,num_el, this->initial_one(),(size_t)0);
}

template <class datatype>
/**
 * @brief  ����Ĺ���ѹ�
 *
 *  ���Ȥ������Ĺ���� new_num_elements ���ѹ����ޤ�������Ĺ����̤����硤
 *  new_num_elements �ʹߤ����ǤϺ������ޤ���
 *
 * @param     new_num_elements �ѹ��������Ĺ
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::resize( size_t new_num_elements )
{
    if ( new_num_elements < this->length() ) {
        this->replace( new_num_elements, this->length() - new_num_elements,
		       this->initial_one(), (size_t)0 );
    }
    else {
	this->replace( this->length(), 0,
		       this->initial_one(), new_num_elements - this->length() );
    }
    return *this;
}

template <class datatype>
/**
 * @brief  ����Ĺ������Ū���ѹ�
 *
 *  ���Ȥ������Ĺ���� len ��Ĺ��ʬ�����ѹ����ޤ���<br>
 *  ����Ĺ����̤����硤�Ǹ�� abs(len) �Ĥ����ǤϺ������ޤ���
 *
 * @param     len ����Ĺ����ʬ����ʬ
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
tarray<datatype> &tarray<datatype>::resizeby( ssize_t len )
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

template <class datatype> 
/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�����Τ��٤Ƥޤ��ϰ�����dest �ǻ��ꤵ�줿���֥������Ȥ˥��ԡ�
 *  ���ޤ���<br>
 *  index + n ���ͤ����ԡ��������ǿ���Ķ�����硤�����ֹ�index ����Ǹ������
 *  �ޤǥ��ԡ�����ޤ���<br>
 *  index ���ͤ����ԡ��������ǿ���Ķ�����硤dest �����ƤϾõ�졤�֤��ͤ�
 *  -1�Ȥʤ�ޤ���
 *
 * @param     index ���ԡ������֥������Ȥ�����γ��������ֹ�
 * @param     n ���ԡ��������ǿ�
 * @param     dest ���ԡ����tarray ���饹�Υ��֥�������
 * @return    �������: ���ԡ��������ǿ�<br>
 *            �����(���顼) : index ������Ĺ�ʾ���ͤ����ꤵ�줿���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tarray<datatype>::copy( size_t index, size_t n,
				tarray<datatype> *dest ) const
{
    if ( dest == NULL ) return -1;

    size_t len = this->length();

    if ( len < index ) {
	dest->erase();
	return -1;
    }
    if ( len == 0 ) {
	dest->erase();
	return 0;
    }

    if ( len - index < n ) n = len - index;

    dest->replace(0, dest->length(), *this, index, n);

    return n;
}

template <class datatype>
/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�����Τ��٤Ƥޤ��ϰ�����dest �ǻ��ꤵ�줿���֥������Ȥ˥��ԡ�
 *  ���ޤ���<br>
 *  index + n ���ͤ����ԡ��������ǿ���Ķ�����硤�����ֹ�index ����Ǹ������
 *  �ޤǥ��ԡ�����ޤ���<br>
 *  index ���ͤ����ԡ��������ǿ���Ķ�����硤dest �����ƤϾõ�졤�֤��ͤ�
 *  -1�Ȥʤ�ޤ���
 *
 * @param     index ���ԡ������֥������Ȥ�����γ��������ֹ�
 * @param     dest ���ԡ����tarray ���饹�Υ��֥�������
 * @return    �������: ���ԡ��������ǿ�<br>
 *            �����(���顼) : index ������Ĺ�ʾ���ͤ����ꤵ�줿���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tarray<datatype>::copy( size_t index, tarray<datatype> *dest ) const
{
    if ( dest == NULL ) return -1;

    return this->copy(index, this->length(), dest);
}

template <class datatype> 
/**
 * @brief  ���Ȥ����Ƥ��̥��֥������Ȥإ��ԡ�
 *
 *  ���Ȥ�����Τ��٤Ƥ�dest �ǻ��ꤵ�줿���֥������Ȥ˥��ԡ����ޤ���
 *
 * @param     dest ���ԡ����tarray ���饹�Υ��֥�������
 * @return    �������: ���ԡ��������ǿ�<br>
 *            �����(���顼) : index ������Ĺ�ʾ���ͤ����ꤵ�줿���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
ssize_t tarray<datatype>::copy( tarray<datatype> *dest ) const
{
    if ( dest == NULL ) return -1;

    return this->copy(0, this->length(), dest);
}

template <class datatype>
/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��
 */
ssize_t tarray<datatype>::copy( size_t index, size_t n, 
				tarray<datatype> &dest ) const
{
    return this->copy(index, n, &dest);
}

template <class datatype>
/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��
 */
ssize_t tarray<datatype>::copy( size_t index, tarray<datatype> &dest ) const
{
    return this->copy(index, &dest);
}

template <class datatype>
/**
 * @brief  ���Ȥ����Ƥ��̥��֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��
 */
ssize_t tarray<datatype>::copy( tarray<datatype> &dest ) const
{
    return this->copy(&dest);
}

template <class datatype>
/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���֥�������sobj �����Ƥȼ��Ȥ����ƤȤ������ؤ��ޤ���
 *
 * @param     sobj ���Ƥ������ؤ���tarray ���饹�Υ��֥�������
 * @return    ���Ȥλ���
 */
tarray<datatype> &tarray<datatype>::swap( tarray<datatype> &sobj )
{
    datatype *tmp__initial_one_rec;
    datatype *(*tmp__creator)(void *);
    void *tmp__creator_arg;

    if ( &sobj == this ) return *this;

    this->arrs_rec.swap(sobj.arrs_rec);

    tmp__initial_one_rec = this->initial_one_rec;
    this->initial_one_rec = sobj.initial_one_rec;
    sobj.initial_one_rec = tmp__initial_one_rec;

    tmp__creator = this->creator;
    this->creator = sobj.creator;
    sobj.creator = tmp__creator;

    tmp__creator_arg = this->creator_arg;
    this->creator_arg = sobj.creator_arg;
    sobj.creator_arg = tmp__creator_arg;

    return *this;
}

template <class datatype>
/**
 * @brief  ���ꤵ�줿�������Ǥλ��Ȥ��֤�
 *
 *  index �ǻ��ꤵ�줿�������Ǥλ��Ȥ��֤��ޤ���<br>
 *  �������дؿ���ľ��� "." ����³����type���饹�Υ��дؿ���Ȥ�����
 *  �Ǥ��ޤ�<br>
 *  ����Ĺ�ʾ��index �����ꤵ�줿���ϡ��������������Ǥ�����ޤ���
 *  �Хåե��γ��ݤ˼��Ԥ�������������㳰��ȯ�����ޤ���
 *
 * @param     index �����ֹ�
 * @return    ���ꤵ�줿�����ֹ�˳��������ͤޤ��ϥ��֥������Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
inline datatype &tarray<datatype>::at( size_t index )
{
    datatype *objp = NULL;
#if 1
    if ( this->length() <= index ) {
	this->resize(index + 1);
    }
#endif
    if ( index < 0 || this->length() <= index ) {
        this->err_throw(__FUNCTION__,"ERROR","Invalid index");
    }
    else {
	objp = (datatype *)(this->arrs_rec.p(index));
    }
    return *(objp);
}

#ifdef SLI__OVERLOAD_CONST_AT
template <class datatype>
/**
 * @brief  ���ꤵ�줿�������Ǥλ��Ȥ��֤� (�ɼ�����)
 *
 *  index �ǻ��ꤵ�줿�������Ǥλ��Ȥ��֤��ޤ���<br>
 *  �������дؿ���ľ��� "." ����³����type���饹�Υ��дؿ���Ȥ�����
 *  �Ǥ��ޤ���<br>
 *  �ɤ߼�����Ѥǡ�����Ĺ�ʾ�� index �����ꤵ�줿���ϡ��㳰��ȯ�����ޤ���
 *
 * @param     index �����ֹ�
 * @return    ���ꤵ�줿�����ֹ�˳��������ͤޤ��ϥ��֥������Ȥλ���
 * @throw     ����Ĺ�ʾ��index �����ꤵ�줿���
 */
inline const datatype &tarray<datatype>::at( size_t index ) const
{
    return this->at_cs(index);
}
#endif

template <class datatype>
/**
 * @brief  ���ꤵ�줿�������Ǥλ��Ȥ��֤� (�ɼ�����)
 *
 *  index �ǻ��ꤵ�줿�������Ǥλ��Ȥ��֤��ޤ���<br>
 *  �������дؿ���ľ��� "." ����³����type���饹�Υ��дؿ���Ȥ�����
 *  �Ǥ��ޤ���<br>
 *  �ɤ߼�����Ѥǡ�����Ĺ�ʾ�� index �����ꤵ�줿���ϡ��㳰��ȯ�����ޤ���
 *
 * @param     index �����ֹ�
 * @return    ���ꤵ�줿�����ֹ�˳��������ͤޤ��ϥ��֥������Ȥλ���
 * @throw     ����Ĺ�ʾ��index �����ꤵ�줿���
 */
inline const datatype &tarray<datatype>::at_cs( size_t index ) const
{
    const datatype *objp = NULL;
    if ( index < 0 || this->length() <= index ) {
        this->err_throw(__FUNCTION__,"ERROR","Invalid index");
    }
    else {
	objp = (const datatype *)(this->arrs_rec.p_cs(index));
    }
    return *(objp);
}

template <class datatype>
/**
 * @brief  �����Ĺ��(�Ŀ�)�����
 *
 * @return    �����
 */
inline size_t tarray<datatype>::length() const
{
    return this->arrs_rec.length();
}

/**
 * @brief  �����Ĺ��(�Ŀ�)�����
 *
 * @return    �����
 */
template <class datatype>
inline size_t tarray<datatype>::size() const
{
    return this->arrs_rec.length();
}

/* new datatype ������ˡ����󥹥ȥ饯���˰�����Ϳ���������˻Ȥ� */
template <class datatype>
/**
 * @brief  datatype���饹�Υ��֥������Ȥ���ؿ�����Ͽ
 *
 *  tarray ���饹�����ǡ�new datatype�פ�����ˡ����󥹥ȥ饯���˰�����Ϳ������
 *  ���˻Ȥ��ޤ�.
 *
 * @param     func �桼���ؿ��Υ��ɥ쥹
 * @param     user_ptr �桼���ؿ��κǸ��Ϳ������桼���Υݥ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
void tarray<datatype>::register_creator( datatype *(*func)(void *), 
					 void *user_ptr )
{
    static datatype one;
    if ( this->initial_one_rec != NULL ) {
	delete this->initial_one_rec;
	this->initial_one_rec = NULL;
    }
    try {
	if ( func == NULL ) {
	    this->initial_one_rec = new datatype;
	    *(this->initial_one_rec) = one;
	}
	else {
	    this->initial_one_rec = (*func)(user_ptr);	/* new */
	}
    }
    catch (...) {
	this->err_throw(__FUNCTION__,"FATAL","new failed");
    }
    this->creator = func;
    this->creator_arg = user_ptr;
    return;
}

template <class datatype>
/**
 * @brief  initial_one_rec�λ���(�ɼ�����)���֤�
 *
 * @return    initial_one_rec�λ���
 */
const datatype &tarray<datatype>::initial_one() const
{
    return *(this->initial_one_rec);
}

/**
 * @brief  �ƥ�ݥ�ꥪ�֥������ȤΤ����shallow copy°������Ϳ (̤����)
 * @deprecated  ̤����
 */
/* ���: ���Ȥ��֤������Ȥ��������return ʸ�Ǥ������ȥ��󥹥��� */
/*       ��2�󥳥ԡ������Ȥ����������ʻ��ˤʤ�Τ� void �ˤʤäƤ��� */
template <class datatype>
void tarray<datatype>::set_scopy_flag()
{
    this->shallow_copy_ok = true;
    return;
}


/* private member functions */

/**
 * @brief  shallow copy ����ǽ������� (̤����)
 * 
 *  src¦�Ǽ¹Ԥ��롥<br>
 *  SLLIB�μ����Ǥϡ�shallow copy ����������ȤΤ߲ġ�
 * 
 * @deprecated  ̤����
 * @param   from_obj �ꥯ�����Ȥ���������¦�Υ��֥������ȤΥ��ɥ쥹
 * @return  shallow copy����ǽ�ʤ鿿<br>
 *          ����ʳ��λ��ϵ�
 * @note ���Υ��дؿ��� private �Ǥ�
 */
template <class datatype>
bool tarray<datatype>::request_shallow_copy( datatype *from_obj ) const
{
    return false;
}

/**
 * @brief  ���顼���㳰ȯ������ err_rec �� throw ����
 *
 *  tarray���饹�δؿ���ǥ��顼���㳰ȯ�����˸ƤӽФ���ޤ���
 *  ɸ�२�顼���Ϥ˥��顼�����ɽ������err_rec �� throw���ޤ���
 * 
 * @param	func0 �ƤӽФ����ؿ�̾
 * @param	level0 ���顼����<br>
 *			�㳰ȯ����:"FATAL"<BR>
 *			���顼��:"ERROR"
 * @param	message0 ���顼��å�����
 * @throw	err_rec:���顼�������Ĺ�¤��
 * @note	���Υ��дؿ���private�Ǥ�
 */

template <class datatype>
void tarray<datatype>::err_throw( const char *func0, const char *level0, 
				  const char *message0 ) const
{
    static stdstreamio sio;
    const char *classname = "tarray";
    sio.eprintf("%s::%s(): [%s] ",classname,func0,level0);
    sio.eprintf("%s\n", message0);
    {
	size_t i, len;
	const char *src;
	char *dst;
	sli::err_rec st;
	/* */
	src = classname;  dst = st.class_name;  len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = func0;     dst = st.func_name;    len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = level0;    dst = st.level;        len = 64;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	src = message0;  dst = st.message;      len = 128;
	for ( i=0 ; i < len && src[i] != '\0' ; i++ ) dst[i] = src[i];
	dst[i] = '\0';
	/* */
	throw st;
    }
}

}	/* namespace sli */

#endif	/* _SLI__TARRAY_H */
