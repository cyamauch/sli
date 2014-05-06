/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-05-14 17:50:41 cyamauch> */

/**
 * @file   mdarray_uintptr.cc
 * @brief  uintptr_t����¿��������򰷤� mdarray_uintptr ���饹�Υ�����
 */

#define CLASS_NAME "mdarray_uintptr"

#include "config.h"

#include "mdarray_uintptr.h"
#include "tarray_tstring.h"

#include <stdarg.h>

#include "sli_funcs.h"

#include "private/s_memset.h"
#include "private/s_memmove.h"
#include "private/err_report.h"

#ifndef debug_report
//#define debug_report(xx) err_report(__FUNCTION__,"DEBUG",xx)
#define debug_report(xx)
#endif

namespace sli
{

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���Ȥ�ư�ꥵ�����⡼�ɤǽ�������ޤ���
 *
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr()
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, true);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true.
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @param      auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true.
 * @param      extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, uintptr_t **extptr_address )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    this->mdarray::register_extptr((void *)extptr_address);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @param      auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true.
 * @param      extptr2d_address �桼���Υݥ����ѿ�(2d��)�Υ��ɥ쥹
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, uintptr_t *const **extptr2d_address )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    this->mdarray::register_extptr_2d((void *)extptr2d_address);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 * @param      auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true.
 * @param      extptr3d_address �桼���Υݥ����ѿ�(3d��)�Υ��ɥ쥹
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, uintptr_t *const *const **extptr3d_address )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r);
    this->mdarray::register_extptr_3d((void *)extptr3d_address);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ndim�Ǽ�������naxisx[]�ǳƼ��������ǿ�����ꤷ�ƽ������Ԥ��ޤ���
 *
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxisx[] �Ƽ��������ǿ�
 * @param  ndim ������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, ndim, init_buf);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0��1�����ܤ����ǿ�����ꤷ�ƽ������Ԥ��ޤ���
 *
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0��1������)�μ�����
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
*/
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, 1, true);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0�ǣ������ܤ����ǿ���naxis1��2�����ܤ����ǿ�����ꤷ�ƽ������
 *  �Ԥ��ޤ���
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0��1������)�����ǿ�
 * @param  naxis1 �����ֹ�1 ��2������)�����ǿ�
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, 2, true);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0�ǣ������ܤ����ǿ���naxis1��2�����ܤ����ǿ���naxis2��3�����ܤ�
 *  ���ǿ�����ꤷ�ƽ������Ԥ��ޤ���
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0(1������)�����ǿ�
 * @param  naxis1 �����ֹ�1(2������)�����ǿ�
 * @param  naxis2 �����ֹ�2(3������)�����ǿ�
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1, size_t naxis2 )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    this->class_level_rec ++;
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_r, naxisx, 3, true);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0��1�����ܤ����ǿ�����ꤷ��vals�����Ƥǽ������Ԥ��ޤ���
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0(1������)�����ǿ�
 * @param  vals[] ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, 
			      const uintptr_t vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, vals);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0��1�����ܤ����ǿ���naxis1��2�����ܤ����ǿ�����ꤷ��
 *  vals�����Ƥǽ������Ԥ��ޤ�.
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0(1������)�����ǿ�
 * @param  naxis1 �����ֹ�1(2������)�����ǿ�
 * @param  vals[] ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1,
			      const uintptr_t vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, vals);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0��1�����ܤ����ǿ���naxis1��2�����ܤ����ǿ�����ꤷ��
 *  vals�����Ƥǽ������Ԥ��ޤ���
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0(1������)�����ǿ�
 * @param  naxis1 �����ֹ�1(2������)�����ǿ�
 * @param  vals[] ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1,
			      const uintptr_t *const vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, vals);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0��1�����ܤ����ǿ���naxis2��2�����ܤ����ǿ���naxis2�ǻ������ܤ�
 *  ���ǿ�����ꤷ��vals�����Ƥǽ������Ԥ��ޤ���
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0(1������)�����ǿ�
 * @param  naxis1 �����ֹ�1(2������)�����ǿ�
 * @param  naxis2 �����ֹ�2(3������)�����ǿ�
 * @param  vals[] ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1, size_t naxis2,
			      const uintptr_t vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, naxis2, vals);
    return;
}

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  naxis0��1�����ܤ����ǿ���naxis2��2�����ܤ����ǿ���naxis2�ǻ������ܤ�
 *  ���ǿ�����ꤷ��vals�����Ƥǽ������Ԥ��ޤ���
 * 
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  naxis0 �����ֹ�0(1������)�����ǿ�
 * @param  naxis1 �����ֹ�1(2������)�����ǿ�
 * @param  naxis2 �����ֹ�2(3������)�����ǿ�
 * @param  vals[] ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, size_t naxis0, size_t naxis1, size_t naxis2,
			      const uintptr_t *const *const vals[] )
{
    this->class_level_rec ++;
    this->init(auto_r, naxis0, naxis1, naxis2, vals);
    return;
}

#if 0

/**
 * @brief  ���󥹥ȥ饯��
 *
 *  ���Ȥ�����Ȥ���Ϳ����줿�����ǥǡ����ǽ�������ޤ���
 * 
 * @deprecated  �����Բġ�
 * @param  auto_r ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param  dim_info  �Ƽ��������ǿ�
 * @param  v0 �ͥǡ���
 * @param  ... �ͥǡ����β���Ĺ�����ꥹ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( bool auto_r, const char *dim_info,
			      uintptr_t v0, ... )
{
    va_list ap;

    this->class_level_rec ++;

    va_start(ap, v0);
    try {
	this->vinit(auto_r,dim_info,v0,ap);
    }
    catch (...) {
	va_end(ap);
 	err_throw(__FUNCTION__,"FATAL","this->vinit()");
    }
    va_end(ap);

    return;
}
#endif

/**
 * @brief  ���ԡ����󥹥ȥ饯��
 *
 *  ���Ȥ�obj�����Ƥ򥳥ԡ����ޤ���
 * 
 * @param  obj ���ԡ����Υ��֥�������
 * @throw  �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr::mdarray_uintptr( const mdarray_uintptr &obj )
{
    this->class_level_rec ++;
    this->mdarray::init(obj);
    return;
}

/**
 * @brief  ���֥������ȤΥ��ԡ��ޤ�������
 *
 *  ���֥������Ȥ���������obj �����Ƥ򤹤٤�(����Ĺ��Ƽ�°��)�� ���Ȥ˥��ԡ�
 *  ���ޤ���obj ���ؿ��ˤ�ä��֤����ƥ�ݥ�ꥪ�֥������Ȥξ�硤���Ĥ����
 *  ����� shallow  copy ���Ԥʤ��ޤ���<br>
 *  obj�η������Ȥη��Ȥϰۤʤ��硤���ʳ��Τ��٤�(����Ĺ��Ƽ�°��)�򥳥ԡ�
 *  ���������ͤΥ��ԡ��ϥ����顼�ͤΡ�=�ױ黻�Ҥ�Ʊ�ͤε�§�ǹԤʤ��ޤ���
 *
 * @param      obj mdarray(�Ѿ�) ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    if ( &obj == this ) return *this;

    if ( obj.size_type() == (ssize_t)UINTPTR_ZT ) {
	this->mdarray::init(obj);
    }
    else {

	if ( obj.dim_length() == 0 ) this->init();
	else {
	    this->init(true, obj.cdimarray(), obj.dim_length(), true);
	    this->ope_plus_equal(obj);
	}
	this->init_properties(obj);

    }

    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����û�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray(�Ѿ�) ���饹�Υ��֥������Ȥ������
 *  ���Ȥ˲û����ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 * �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param      obj mdarray(�Ѿ�) ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_plus_equal(obj);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����Ǹ���
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray(�Ѿ�) ���饹�Υ��֥������Ȥ������
 *  ���Ȥ��鸺�����ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param      obj mdarray(�Ѿ�) ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_minus_equal(obj);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray(�Ѿ�) ���饹�Υ��֥������Ȥ������
 *  ���Ȥ˾軻���ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param      obj mdarray(�Ѿ�) ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_star_equal(obj);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿����ǽ���
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿mdarray(�Ѿ�) ���饹�Υ��֥������Ȥ������
 *  ���Ȥ���������ޤ���<br>
 *  ���ȤȤϰۤʤ뷿�����ꤵ�줿���֥������Ȥ����ꤵ�줿��硤�̾�Υ����顼
 *  �黻�ξ���Ʊ�ͤη��Ѵ��������Ԥʤ��ޤ���<br>
 *  �ּ�ư�ꥵ�����⡼�ɡפ����ꤵ��Ƥ����硤�Ƽ����������˴ؤ��Ƽ��Ȥ���
 *  obj �������礭����硤��ưŪ�˥ꥵ�������ޤ���
 *
 * @param      obj mdarray(�Ѿ�) ���饹�Υ��֥�������
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(const mdarray &obj)
{
    debug_report("(const mdarray &obj)");
    this->ope_slash_equal(obj);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ�.
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ�.
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(double v)
{
    debug_report("(double v)");
    this->ope_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ�.
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(long long v)
{
    debug_report("(long long v)");
    this->ope_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ�.
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(long v)
{
    debug_report("(long v)");
    this->ope_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�����
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿����(�����顼��) �������򤷤ޤ�.
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator=(int v)
{
    debug_report("(int v)");
    this->ope_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_plus_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(double v)
{
    debug_report("(double v)");
    this->ope_plus_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(long long v)
{
    debug_report("(long long v)");
    this->ope_plus_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(long v)
{
    debug_report("(long v)");
    this->ope_plus_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǲû�
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��Ʋû�
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator+=(int v)
{
    debug_report("(int v)");
    this->ope_plus_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_minus_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(double v)
{
    debug_report("(double v)");
    this->ope_minus_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(long long v)
{
    debug_report("(long long v)");
    this->ope_minus_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(long v)
{
    debug_report("(long v)");
    this->ope_minus_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǸ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǸ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator-=(int v)
{
    debug_report("(int v)");
    this->ope_minus_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_star_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(double v)
{
    debug_report("(double v)");
    this->ope_star_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(long long v)
{
    debug_report("(long long v)");
    this->ope_star_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(long v)
{
    debug_report("(long v)");
    this->ope_star_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ������顼�ͤ�軻
 *
 *  �黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤ򼫿Ȥ����Ǥ��٤Ƥ��Ф��ƾ軻
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator*=(int v)
{
    debug_report("(int v)");
    this->ope_star_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(dcomplex v)
{
    debug_report("(dcomplex v)");
    this->ope_slash_equal(DCOMPLEX_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(double v)
{
    debug_report("(double v)");
    this->ope_slash_equal(DOUBLE_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(long long v)
{
    debug_report("(long long v)");
    this->ope_slash_equal(LLONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(long v)
{
    debug_report("(long v)");
    this->ope_slash_equal(LONG_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤥����顼�ͤǽ���
 *
 *  ���Ȥ����Ǥ��٤Ƥ��Ф��ơ��黻�Ҥα�¦(����) �ǻ��ꤵ�줿�����顼�ͤǽ���
 *  ���ޤ���<br>
 *  ���ȤȤϷ����ۤʤ�����ξ�硤�̾�Υ����顼�黻�ξ���Ʊ�ͤη��Ѵ�������
 *  �Ԥʤ��ޤ���
 *
 * @param      v �����顼��
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::operator/=(int v)
{
    debug_report("(int v)");
    this->ope_slash_equal(INT_ZT,(const void *)&v);
    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�����°�����������ޤ���<br>
 *  ���󥵥��� 0 �Ȥ��ƥ��֥������Ȥ��������ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init()
{
    debug_report("()");
    this->mdarray::init();
    return *this;
}

/**
 * @brief  ���֥������Ȥν����
 *
 *  ���Ȥ�����°�����������ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize )
{
    this->mdarray::init();
    this->mdarray::set_auto_resize(auto_resize);
    return *this;
}

/**
 * @brief  ���֥������Ȥν���� (n����)
 *
 *  ���Ȥ�����°�����������ޤ���<br>
 *  ndim �Ǽ�������naxisx[] �ǳƼ��������ǿ�����ꤷ�ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      ndim ���󼡸���
 * @param      naxisx[] �Ƽ��������ǿ�
 * @param      init_buf �����ͤ�ǥե�����ͤ�����ʤ� true
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, ndim, init_buf);
    return *this;
}

/**
 * @brief  ����ν���� (1����)
 *
 *  ���Ȥ�������������ޤ���<br>
 *  naxis0 �� 1 �����ܤ����ǿ�����ꤷ��1 �������������ĥ��֥������Ȥ�
 *  �������ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0 )
{
    const size_t naxisx[] = {naxis0};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 1, true);
    return *this;
}

/**
 * @brief  ����ν���� (2����)
 *
 *  ���Ȥ�������������ޤ���<br>
 *  naxis0 �� 1 �����ܤ����ǿ���naxis1 �� 2 �����ܤ����ǿ����ꤷ��2 ����������
 *  ����ĥ��֥������Ȥ�������ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1 )
{
    const size_t naxisx[] = {naxis0, naxis1};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 2, true);
    return *this;
}

/**
 * @brief  ����ν���� (3����)
 *
 *  ���Ȥ�������������ޤ���<br>
 *  naxis0 �� 1 �����ܤ����ǿ���naxis1 �� 2 �����ܤ����ǿ���naxis2 �� 3 ������
 *  �����ǿ�����ꤷ��3 �������������ĥ��֥������Ȥ�������ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @param      naxis2 �����ֹ�2 �μ���(3 ������) �����ǿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2 )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 3, true);
    return *this;
}

/**
 * @brief  ���󥹥ȥ饯�������뤤�� init() �ΰ����μ�������� parse (̤����)
 * 
 * @deprecated  ̤����
 * @param   dim_info �Ƽ��������ǿ��ξ���
 * @return  ���ꤵ�줿�����ο�
 * @throw   �����Хåե��γ��ݤ˼��Ԥ������
 * @note    ���Υ��дؿ���protected�Ǥ���<br>
 *          3�����ޤǤ������ݡ��Ȥ��Ƥ��ޤ���.
 */
size_t mdarray_uintptr::parse_dim_info( const char *dim_info )
{
    size_t i;
    tarray_tstring arr0;
    this->dinfo_dlen[0] = 0;
    this->dinfo_dlen[1] = 0;
    this->dinfo_dlen[2] = 0;
    arr0.split(dim_info,", ",false);
    /* 3�����ޤǤ������ݡ��Ȥ��ʤ� */
    for ( i=0 ; i < arr0.length() && i < 3 ; i++ ) {
	this->dinfo_dlen[i] = arr0[i].atoll();
	if ( this->dinfo_dlen[i] == 0 ) break;
    }
    return i;
}

/**
 * @brief  ����ν����
 *
 *  naxis0��1�����ܤ����ǿ�����ꤷ��vals�����Ƥǽ������Ԥ��ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      vals ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, 
				    const uintptr_t vals[] )
{
    const size_t naxisx[] = {naxis0};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 1, false);
    this->mdarray::putdata((const void *)vals, this->byte_length());
    return *this;
}

/**
 * @brief  ����ν����
 *
 *  naxis0��1�����ܤ����ǿ���naxis1��2�����ܤ����ǿ�����ꤷ��
 *  vals�����Ƥǽ������Ԥ��ޤ�.
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @param      vals ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, 
				    const uintptr_t vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 2, false);
    this->mdarray::putdata((const void *)vals, this->byte_length());
    return *this;
}

/**
 * @brief  ����ν����
 *
 *  naxis0��1�����ܤ����ǿ���naxis1��2�����ܤ����ǿ�����ꤷ��
 *  vals�����Ƥǽ������Ԥ��ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @param      vals ���ԡ����Ȥʤ�ݥ�������Υ��ɥ쥹
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, 
				    const uintptr_t *const vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1};
    size_t blen0, i;
    char *p_dst;
    /* */
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 2, false);
    /* */
    blen0 = this->byte_length(0);
    p_dst = (char *)(this->mdarray::data_ptr());
    for ( i=0 ; i < naxis1 ; i++ ) {
	const void *p_src = (const void *)(vals[i]);
	if ( p_src == NULL ) break;
	s_memmove(p_dst, p_src, blen0);
	p_dst += blen0;
    }
    if ( i < naxis1 ) s_memset(p_dst, 0, blen0 * (naxis1 - i), 0);

    return *this;
}

/**
 * @brief  ����ν����
 *
 *  naxis0��1�����ܤ����ǿ���naxis2��2�����ܤ����ǿ���naxis2�ǻ������ܤ�
 *  ���ǿ�����ꤷ��vals�����Ƥǽ������Ԥ��ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @param      naxis2 �����ֹ�2 �μ���(3 ������) �����ǿ�
 * @param      vals ���ԡ����Ȥʤ�����Υ��ɥ쥹
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2, 
				    const uintptr_t vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 3, false);
    this->mdarray::putdata((const void *)vals, this->byte_length());
    return *this;
}

/**
 * @brief  ����ν����
 *
 *  naxis0��1�����ܤ����ǿ���naxis2��2�����ܤ����ǿ���naxis2�ǻ������ܤ�
 *  ���ǿ�����ꤷ��vals�����Ƥǽ������Ԥ��ޤ���
 *
 * @param      auto_resize ��ư�ꥵ�����⡼�ɤ����ꤹ����� true
 * @param      naxis0 �����ֹ�0 �μ���(1 ������) �����ǿ�
 * @param      naxis1 �����ֹ�1 �μ���(2 ������) �����ǿ�
 * @param      naxis2 �����ֹ�2 �μ���(3 ������) �����ǿ�
 * @param      vals ���ԡ����Ȥʤ�ݥ�������Υ��ɥ쥹
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, size_t naxis0, size_t naxis1, size_t naxis2, 
				    const uintptr_t *const *const vals[] )
{
    const size_t naxisx[] = {naxis0, naxis1, naxis2};
    size_t blen0, i, j;
    char *p_dst;
    /* */
    this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, naxisx, 3, false);
    /* */
    blen0 = this->byte_length(0);
    p_dst = (char *)(this->mdarray::data_ptr());
    for ( i=0 ; i < naxis2 ; i++ ) {		/* layer */
	j = 0;					/* row */
	if ( vals[i] != NULL ) {
	    for ( ; j < naxis1 ; j++ ) {
		const void *p_src = (const void *)(vals[i][j]);
		if ( p_src == NULL ) break;
		s_memmove(p_dst, p_src, blen0);
		p_dst += blen0;
	    }
	}
	if ( j < naxis1 ) {
	    s_memset(p_dst, 0, blen0 * (naxis1 - j), 0);
	    p_dst += blen0 * (naxis1 - j);
	}
    }

    return *this;
}

#if 0

/**
 * @brief  ����ν����
 *
 *  ���Ȥ�����Ȥ���Ϳ����줿�����ǥǡ����ǽ�������ޤ�.
 * 
 * @deprecated  ̤����
 * @param   auto_resize ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param   dim_info �Ƽ��������ǿ�
 * @param   v0 �ͥǡ���
 * @param   ... �ͥǡ����γ�������
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr &mdarray_uintptr::init( bool auto_resize, const char *dim_info, uintptr_t v0, ... )
{
    va_list ap;
    va_start(ap, v0);
    try {
	this->vinit(auto_resize, dim_info, v0, ap);
    }
    catch (...) {
	va_end(ap);
 	err_throw(__FUNCTION__,"FATAL","failed this->init()");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ����ν����
 *
 *  ���Ȥ�����Ȥ���Ϳ����줿�����ǥǡ����ǽ�������ޤ�.
 * 
 * @deprecated  ̤����
 * @param   auto_resize ��ư�ꥵ�����⡼�ɤǽ������Ԥ�����true
 * @param   dim_info �Ƽ��������ǿ�
 * @param   v0 �ͥǡ���
 * @param   ap �ͥǡ����β���Ĺ�����ꥹ��
 * 
 * @return  ���Ȥλ���
 * @throw   �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr &mdarray_uintptr::vinit( bool auto_resize, const char *dim_info, uintptr_t v0, va_list ap )
{
    size_t ndim = this->parse_dim_info(dim_info);
    if ( ndim < 1 ) {
	this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize);
    }
    else {
	size_t i;
	this->mdarray::init((ssize_t)UINTPTR_ZT, auto_resize, this->dinfo_dlen, ndim, true);
	i = 0;
	if ( i < this->length() ) this->p(i) = v0;
	i++;
	for ( ; i < this->length() ; i++ ) {
	    this->p(i) = va_arg(ap,uintptr_t);
	}
    }
    return *this;
}
#endif

/**
 * @brief  ���֥������ȤΥ��ԡ�
 *
 *  obj ����������Ƥ�°�������٤Ƥ򼫿Ȥ˥��ԡ����ޤ������Ȥ�����η��� obj
 *  ������η��ϰ��פ��Ƥ���ɬ�פ�����ޤ�(�����ۤʤ���ϡ�=�ױ黻�Ҥ����
 *  ���ޤ�)��<br>
 *  obj ���ؿ��ˤ�ä��֤��줿�ƥ�ݥ�ꥪ�֥������Ȥξ�硤���Ĥ���Ƥ����
 *  shallow copy ���Ԥʤ��ޤ���
 *
 * @param      obj ���ԡ����Ȥʤ륪�֥������� (���ϼ��Ȥ�Ʊ��)
 * @return     ���Ȥλ���
 * @throw      �����ۤʤ�����򥳥ԡ����褦�Ȥ�����
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::init( const mdarray &obj )
{
    debug_report("( const mdarray &obj )");
    if ( obj.size_type() == this->size_type() ) {
	this->mdarray::init(obj);
    }
    else {
 	err_throw(__FUNCTION__,"FATAL",
		  "cannot use init() between different size_type");
    }
    return *this;
}

/* copy properties (auto_resize, auto_init, and rounding) */
/**
 * @brief  ��ư�ꥵ��������ư��������ͼθ��������������ˡ������򥳥ԡ�
 *
 *  src_obj �� auto_resize, auto_init, rounding, alloc_strategy �������
 *  ���ԡ����ޤ���
 *
 * @param   src_obj ���ԡ�����륪�֥�������
 * @return  ���Ȥλ���
 *            
 */
mdarray_uintptr &mdarray_uintptr::init_properties( const mdarray &src_obj )
{
    this->mdarray::init_properties(src_obj);
    return *this;
}

/**
 * @brief  ���ߤμ�ư�ꥵ�����β��ݤ�������ѹ�
 *
 *  �ꥵ�����⡼�ɤ�(true)���ޤ��ϵ�(false) �����ꤷ�ޤ���<br>
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ��� true��
 *  ư��⡼�ɤ��ּ�ư�ꥵ�����⡼�ɡפξ��� false �Ǥ���
 *
 * @param     tf �ꥵ�����⡼��
 * @return    ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::set_auto_resize( bool tf )
{
    this->mdarray::set_auto_resize(tf);
    return *this;
}

/**
 * @brief  ���ߤμ�ư������β��ݤ�������ѹ�
 *
 *  ��ư������⡼�ɤ�(true)���ޤ��ϵ�(false) �����ꤷ�ޤ���
 *
 * @param     tf ��ư������⡼��
 * @return    ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::set_auto_init( bool tf )
{
    this->mdarray::set_auto_init(tf);
    return *this;
}

/**
 * @brief  ���ߤλͼθ����β��ݤ�������ѹ�
 *
 *  �����Ĥ��ι��٥���дؿ��ˤ����ơ���ư�����������������Ѵ�������ˡ�
 *  �ͼθ�����Ԥ����ݤ����ꤷ�ޤ���<br>
 *  �ͼθ�������褦�����ꤵ��Ƥ�����Ͽ�(true)��
 *  �ͼθ������ʤ��褦�����ꤵ��Ƥ�����ϵ�(false) ����ꤷ�ޤ���
 *
 * @param   tf �ͼθ���������
 * @return  ���Ȥλ���
 * @note    �ͼθ�����°������ǽ������дؿ��ϡ�
 *          lvalue()��llvalue() ���дؿ���assign default() ���дؿ���
 *          assign() ���дؿ���convert() ���дؿ��������������дؿ����̡�
 *
 */
mdarray_uintptr &mdarray_uintptr::set_rounding( bool tf )
{
    this->mdarray::set_rounding(tf);
    return *this;
}

/**
 * @brief  ���������ˡ��������ѹ�
 *
 *  �����ѥ������ݤ�����ˡ��ɤΤ褦����ˡ�ǹԤʤ�������ꤷ�ޤ���
 *  ���� 3 �Ĥ������򤷤ޤ���<br>
 *    "min"  ... �Ǿ��¤���ݡ��ꥵ��������ɬ�� realloc() ���ƤФ�ޤ���<br>
 *    "pow"  ... 2��n��ǳ��ݡ�<br>
 *    "auto" ... ��ư�ꥵ�����⡼�ɤλ��ˤ� "pow"�������Ǥʤ����� "min" ��
 *               �ꥵ����������¹Ԥ��ޤ���<br>
 *    NULL��¾ ... ���ߤ���ˡ��ݻ����ޤ���
 *
 * @param   strategy ���������ˡ������
 * @return  ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::set_alloc_strategy( const char *strategy )
{
    this->mdarray::set_alloc_strategy(strategy);
    return *this;
}

/*
static void fnc_for_convert( const void *v0p, void *v1p, void *p )
{
    mdarray_uintptr *this_p = (mdarray_uintptr *)p;
    uintptr_t v0 = *((const uintptr_t *)v0p);
    *((uintptr_t *)v1p) = (*(this_p->convert_func_rec))(v0, this_p->convert_user_ptr_rec);
}
*/

/*
mdarray_uintptr &mdarray_uintptr::convert( uintptr_t (*func)(uintptr_t,void *),
				       void *user_ptr )
{
    this->convert_func_rec = func;
    this->convert_user_ptr_rec = user_ptr;
    this->mdarray::convert((ssize_t)UINTPTR_ZT, &fnc_for_convert, (void *)this);
    return *this;
}
*/

/**
 * @brief  ����������Ǥ��ͤν񤭴��� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ������Ǥ��ͤ� �桼������ؿ� func ��ͳ�ǽ񤭴����ޤ���<br>
 *  �桼������ؿ�����1�����ˤ�����θ��γ����ǥ��ɥ쥹������2�����ˤ��Ѵ����
 *  �����ǤΥ��ɥ쥹������3�����ˤ���1����2������Ϳ����줿���ǤθĿ�����
 *  ��4�����ˤ��Ѵ�������(������ʤ����������ʤ���)����5�����ˤ� user_ptr 
 *  ��Ϳ�����ޤ���
 *
 * @param      func     �桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr �桼���ؿ��κǸ��Ϳ������桼���Υݥ���
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::convert_via_udf( void (*func)(const uintptr_t *,uintptr_t *,size_t,int,void *),
				       void *user_ptr )
{
    this->mdarray::convert_via_udf((ssize_t)UINTPTR_ZT,
		   (void (*)(const void *,void *,size_t,int,void *))func,
		   user_ptr);
    return *this;
}

/**
 * @brief  2�ĤΥ��֥������ȴ֤Ǥ����ƤΥ���å�
 *
 *  ���ꤵ�줿���֥�������sobj �����Ƥȼ��Ȥ����Ƥ������ؤ��ޤ���
 *  ��������ơ�����Υ�������°�������٤Ƥξ��֤������ؤ��ޤ���<br>
 *  sobj �η��ϼ��Ȥη���Ʊ��Ǥ���ɬ�פ�����ޤ���
 *
 * @param     sobj ���Ȥ�Ʊ��������� mdarray(�Ѿ�) ���饹�Υ��֥�������
 * @return    ���Ȥλ���    
 * @throw     sobj �η������Ȥη��Ȱۤʤ���
 */
mdarray_uintptr &mdarray_uintptr::swap( mdarray &sobj )
{
    if ( sobj.size_type() == this->size_type() ) {
	this->mdarray::swap(sobj);
    }
    else {
 	err_throw(__FUNCTION__,"FATAL",
		  "cannot swap objects between different size_type");
    }
    return *this;
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ�
 *
 *  ���Ȥ����Ƥ����Ƥ���ꤵ�줿���֥������� dest �إ��ԡ����ޤ���
 *  ��������Ĺ���������Ƥ�°�������ԡ�����ޤ�������(���ԡ���) ������ϲ��Ѥ���
 *  �ޤ���<br>
 *  ����������Ĺ�� dest �ȼ��ȤȤ����������ϡ������ѥХåե��κƳ��ݤ�
 *  �Ԥʤ�줺����¸�ΥХåե�����������Ƥ����ԡ�����ޤ���<br>
 *  ��=�ױ黻�Ҥ� .init(obj) �Ȥϰۤʤꡤ��� deep copy ���¹Ԥ���ޤ���<br>
 *  dest �ˤϡ�mdarray_uintptr ���饹�Υ��֥������Ȥ��뤤�� mdarray ���饹��
 *  ���֥������Ȥ����ꤵ��ʤ���Фʤ�ޤ���
 *
 * @param     dest ���ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @return    ���ԡ��������ǿ�(����߹Կ��ߥ쥤���)��<br>
 *            �����������ʾ��������
 * @throw     dest �ǻ��ꤵ�줿���֥������Ȥ������ʾ��
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
ssize_t mdarray_uintptr::copy( mdarray *dest ) const
{
    debug_report("( mdarray *dest )");
    return this->mdarray::copy(dest);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥإ��ԡ� (��侩)
 * @deprecated ��侩��<br>
 * ssize_t mdarray_uintptr::copy( mdarray *dest ) const <br>
 * �򤪻Ȥ�����������
 */
ssize_t mdarray_uintptr::copy( mdarray &dest ) const
{
    debug_report("( mdarray &dest )");
    return this->mdarray::copy(dest);
}

/**
 * @brief  ���Ȥ����Ƥ���ꥪ�֥������Ȥذܴ�
 *
 *  ���Ȥ���������Ƥ�dest �ˤ����ꤵ�줿���֥������Ȥءְܴɡפ��ޤ�
 *  (��������Ĺ���Ƽ�°�������ꤵ�줿���֥������Ȥ˥��ԡ�����ޤ�)���ܴɤ�
 *  ��̡����Ȥ�����Ĺ�ϥ���ˤʤ�ޤ���<br>
 *  dest �ˤĤ��Ƥ������ѥХåե��κƳ��ݤϹԤʤ�줺�����Ȥ������ѥХåե�
 *  �ˤĤ��Ƥδ������¤� dest �˾��Ϥ�������ˤʤäƤ��ꡤ��®��ư��ޤ���
 *  dest �ˤϡ�mdarray_uintptr ���饹�Υ��֥������Ȥ��뤤�� mdarray ���饹��
 *  ���֥������Ȥ����ꤵ��ʤ���Фʤ�ޤ���
 *
 * @param     dest �ܴɡ����ԡ���Υ��֥������ȤΥ��ɥ쥹
 * @return    ���Ȥλ���
 * @throw     dest �ǻ��ꤵ�줿���֥������Ȥ������ʾ��
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr &mdarray_uintptr::cut( mdarray *dest )
{
    debug_report("( mdarray *dest )");
    this->mdarray::cut(dest);
    return *this;
}

/* interchange rows and columns */
/**
 * @brief  ���Ȥ������ (x,y) �ǤΥȥ�󥹥ݡ���
 *
 *  ���Ȥ�����Υ����ȥ��Ȥ������ؤ��ޤ���
 * 
 * @note  ��®�ʥ��르�ꥺ�����Ѥ��Ƥ��ޤ���
 */
mdarray_uintptr &mdarray_uintptr::transpose_xy()
{
    this->mdarray::transpose_xy();
    return *this;
}

/* interchange xyz to zxy */
/**
 * @brief  ���Ȥ������ (x,y,z)��(z,x,y) �Υȥ�󥹥ݡ���
 *
 *  ���Ȥ�����μ� (x,y,z) �� (z,x,y) ���Ѵ����ޤ���
 * 
 * @note  ��®�ʥ��르�ꥺ�����Ѥ��Ƥ��ޤ���
 */
mdarray_uintptr &mdarray_uintptr::transpose_xyz2zxy()
{
    this->mdarray::transpose_xyz2zxy();
    return *this;
}

/* rotate image                                              */
/*   angle: 90,-90, or 180                                   */
/*          (anticlockwise when image is bottom-left origin) */
/**
 * @brief  ���Ȥ������ (x,y) �Ǥβ�ž (90��ñ��)
 *
 *  ���Ȥ������ (x,y) �̤ˤĤ��Ƥβ�ž(90��ñ��)��Ԥʤ��ޤ���<br>
 *  ���������Ȥ�����硤ȿ���פޤ��ǻ��ꤷ�ޤ���
 *
 * @param  angle 90,-90, 180 �Τ����줫�����
 * 
 */
mdarray_uintptr &mdarray_uintptr::rotate_xy( int angle )
{
    this->mdarray::rotate_xy( angle );
    return *this;
}


/**
 * @brief  ���Ȥ���������Ƥ�桼�����Хåե��إ��ԡ� (���ǿ��Ǥλ���)
 *
 *  ���Ȥ���������Ƥ� dest_buf�ǻ��ꤵ�줿�桼�����Хåե��إ��ԡ����ޤ���<br>
 *  �Хåե����礭�� elem_size �����ǤθĿ���Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param      dest_buf �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param      elem_size ���ԡ��������ǤθĿ�
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�(���ԡ�������ά��)
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(���ԡ�������ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(���ԡ�������ά��)
 * @return     �桼���ΥХåե�Ĺ����ʬ�ʾ��˥��ԡ���������ǿ�
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t mdarray_uintptr::get_elements( uintptr_t *dest_buf, size_t elem_size, 
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 ) const
{
    return this->mdarray::get_elements((void *)dest_buf,
				       elem_size, idx0,idx1,idx2);
}

/**
 * @brief  �桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ� (���ǿ��Ǥλ���)
 *
 *  src_buf �ǻ��ꤵ�줿�桼�����Хåե������Ƥ򼫿Ȥ�����إ��ԡ����ޤ���<br>
 *  �Хåե����礭�� elem_size �ϡ����ǤθĿ���Ϳ���ޤ���idx0, idx1, idx2 ��
 *  ���Ȥ�����Ǥγ���������ꤷ�ޤ���<br>
 *  �����ˡ�MDARRAY_INDEF ������Ū��Ϳ���ʤ��Ǥ���������
 *
 * @param      src_buf �桼�����Хåե��Υ��ɥ쥹 (���ԡ���)
 * @param      elem_size ���ԡ��������ǤθĿ�
 * @param      idx0 �����ֹ�0 �μ���(1 ������) �������ֹ�(���ԡ��衤��ά��)
 * @param      idx1 �����ֹ�1 �μ���(2 ������) �������ֹ�(���ԡ��衤��ά��)
 * @param      idx2 �����ֹ�2 �μ���(3 ������) �������ֹ�(���ԡ��衤��ά��)
 * @return     �桼���ΥХåե�Ĺ����ʬ�ʾ��˥��ԡ���������ǿ�
 * @throw      �����˲��򵯤��������
 *
 */
ssize_t mdarray_uintptr::put_elements( const uintptr_t *src_buf, size_t elem_size,
				     ssize_t idx0, ssize_t idx1, ssize_t idx2 )
{
    return this->mdarray::put_elements((const void *)src_buf, 
				       elem_size, idx0,idx1,idx2);
}

/**
 * @brief  ��������1�ĳ�ĥ
 *
 *  ���Ȥ���������μ�������1 �ĳ�ĥ���ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::increase_dim()
{
    this->mdarray::increase_dim();
    return *this;
}

/**
 * @brief  ��������1�Ľ̾�
 *
 *  ���Ȥ���������μ�����1 �Ľ̾����ޤ���
 *
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
mdarray_uintptr &mdarray_uintptr::decrease_dim()
{
    this->mdarray::decrease_dim();
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ��Ƥ�����Ĺ���ѹ�
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤len�ʹߤ����ǤϺ������ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::resize( size_t len )
{
    this->mdarray::resize(len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��Ƥ�����Ĺ���ѹ�
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤len�ʹߤ����ǤϺ������ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::resize( size_t dim_index, size_t len )
{
    this->mdarray::resize(dim_index,len);
    return *this;
}

/**
 * @brief  ����Ĺ���ѹ� (¾���֥������Ȥ��饳�ԡ�)
 *
 *  ���Ȥμ�����������Ĺ�򡤥��֥�������src �����Ĥ�Τ�Ʊ���礭���ˤ��ޤ���
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 *  ����Ĺ����̤����硤����Ĺ�������ʤ���ʬ�����ǤϺ������ޤ���<br>
 *
 * @param     src ����Ĺ�Υ��ԡ���
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::resize( const mdarray &src )
{
    debug_report("(const mdarray &src)");
    this->mdarray::resize(src);
    return *this;
}

/* change the length of the 1-d array */
/**
 * @brief  ����Ĺ���ѹ� (1����)
 *
 *  ���Ȥ����������Ĺ�����ѹ���������Ĺ x_len ��1��������Ȥ��ޤ���
 */
mdarray_uintptr &mdarray_uintptr::resize_1d( size_t x_len )
{
    this->mdarray::resize_1d(x_len);
    return *this;
}

/* change the length of the 2-d array */
/**
 * @brief  ����Ĺ���ѹ� (2����)
 *
 *  ���Ȥ����������Ĺ�����ѹ���������Ĺ (x_len, y_len) ��2��������Ȥ��ޤ���
 */
mdarray_uintptr &mdarray_uintptr::resize_2d( size_t x_len, size_t y_len )
{
    this->mdarray::resize_2d(x_len, y_len);
    return *this;
}

/* change the length of the 3-d array */
/**
 * @brief  ����Ĺ���ѹ� (3����)
 *
 *  ���Ȥ����������Ĺ�����ѹ���������Ĺ (x_len, y_len, z_len) ��3���������
 *  ���ޤ���
 */
mdarray_uintptr &mdarray_uintptr::resize_3d( size_t x_len, size_t y_len, size_t z_len )
{
    this->mdarray::resize_3d(x_len, y_len, z_len);
    return *this;
}

/**
 * @brief  ����Ĺ���ѹ� (ʣ���μ���������)
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���<br>
 *  ����Ĺ���ĥ�����硤���Ǥ��ͤ�ǥե�����ͤ���뤫�ɤ����� init_buf ��
 *  ����Ǥ��ޤ���<br>
 *  ����Ĺ����̤����硤����Ĺ�������ʤ���ʬ�����ǤϺ������ޤ���<br>
 *
 * @param     naxisx[] �Ƽ��������ǿ�
 * @param     ndim ���󼡸���
 * @param     init_buf ����Ĺ�γ�ĥ���������ͤ�ǥե�����ͤ�����ʤ� true
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @note      2�����ʾ�Υꥵ�����ξ�硤�㥳���ȤǹԤʤ��ޤ���1���������ꥵ��
 *            ������ʤ顤resize(dim_index, ...) �������㥳���ȤǤ���
 */
mdarray_uintptr &mdarray_uintptr::resize( const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::resize(naxisx,ndim,init_buf);
    return *this;
}

/**
 * @brief  ����Ĺ���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���Ĺ���λ���ϡ�printf() �����β���Ĺ������
 *  �����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,5")�ˤ�äƹԤʤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param  exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param  ...     format���б���������Ĺ�����γ����ǥǡ���
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::resizef( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vresizef(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vresizef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ����Ĺ���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�����ѹ����ޤ���Ĺ���λ���ϡ�printf() �����β���Ĺ������
 *  �����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,5")�ˤ�äƹԤʤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param  exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param  ap      format���б���������Ĺ�����Υꥹ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::vresizef( const char *exp_fmt, va_list ap )
{
    this->mdarray::vresizef(exp_fmt,ap);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ�������Ĺ������Ū���ѹ�
 *
 *  ���Ȥ����������Ĺ���� len �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� len ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ�len �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param      len ���ǸĿ�����ʬ����ʬ
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::resizeby( ssize_t len )
{
    this->mdarray::resizeby(len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ�������Ĺ������Ū���ѹ�
 *
 *  ���Ȥ����������Ĺ���� len �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� len ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ�len �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      len ���ǸĿ�����ʬ����ʬ
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::resizeby( size_t dim_index, ssize_t len )
{
    this->mdarray::resizeby(dim_index,len);
    return *this;
}

/* change the length of the 1-d array relatively */
/**
 * @brief  ����Ĺ������Ū���ѹ� (1������)
 */
mdarray_uintptr &mdarray_uintptr::resizeby_1d( ssize_t x_len )
{
    this->mdarray::resizeby_1d(x_len);
    return *this;
}

/* change the length of the 2-d array relatively */
/**
 * @brief  ����Ĺ������Ū���ѹ� (2������)
 */
mdarray_uintptr &mdarray_uintptr::resizeby_2d( ssize_t x_len, ssize_t y_len )
{
    this->mdarray::resizeby_2d(x_len, y_len);
    return *this;
}

/* change the length of the 3-d array relatively */
/**
 * @brief  ����Ĺ������Ū���ѹ� (3������)
 */
mdarray_uintptr &mdarray_uintptr::resizeby_3d(ssize_t x_len, ssize_t y_len, ssize_t z_len)
{
    this->mdarray::resizeby_3d(x_len, y_len, z_len);
    return *this;
}

/**
 * @brief  ����Ĺ������Ū���ѹ� (ʣ���μ���������)
 *
 *  ���Ȥ����������Ĺ���� naxisx_rel[] �λ���ʬ����ĥ���̾����ޤ���<br>
 *  resizeby() ������󥵥����ϡ����������Ĺ���� naxisx_rel[] ��ä������
 *  �Ȥʤ�ޤ����������ν̾��ϡ�naxisx_rel[] �˥ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�ä�
 *  �Ԥ��ޤ���<br>
 *  init_buf �ǥ�������ĥ���˿����˺�����������Ǥν�����򤹤뤫�ɤ�����
 *  ����Ǥ��ޤ���
 *
 * @param     naxisx_rel ���ǸĿ�����ʬ����ʬ
 * @param     ndim naxisx_rel[] �θĿ�
 * @param     init_buf �����˺�����������Ǥν������Ԥʤ����� true
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::resizeby( const ssize_t naxisx_rel[], size_t ndim, 
			    bool init_buf )
{
    this->mdarray::resizeby(naxisx_rel,ndim,init_buf);
    return *this;
}

/**
 * @brief  ����Ĺ������Ū���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�������ʬ����ĥ���̾����ޤ���Ĺ���λ���ϡ�printf() ����
 *  �β���Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,-5")�ˤ�äƹԤʤ�
 *  �ޤ���<br>
 *  resizebyf()������󥵥����ϡ����������Ĺ���˻���ʬ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ��ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param  exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param  ...     format���б���������Ĺ�����γ����ǥǡ���
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::resizebyf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vresizebyf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vresizebyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ����Ĺ������Ū���ѹ� (IDL�ε�ˡ)
 *
 *  ���Ȥ����������Ĺ�������ʬ����ĥ���̾����ޤ���Ĺ���λ���ϡ�printf() ����
 *  �β���Ĺ�����ǻ����ǽ�ʥ���޶��ڤ��ʸ����(�㤨��"10,-5")�ˤ�äƹԤʤ�
 *  �ޤ���<br>
 *  resizebyf()������󥵥����ϡ����������Ĺ���˻���ʬ��ä�����ΤȤʤ�ޤ���
 *  �������ν̾��ϡ��ޥ��ʥ��ͤ���ꤹ�뤳�Ȥˤ�äƹԤ��ޤ���<br>
 *  ���ꤵ�줿ʸ����������Ǥ������ǤϤʤ���硤���μ�����Ĺ�����ѹ����ޤ���
 *  �㤨�С� ",2" �����ꤵ�줿��硤�ǽ�μ�����Ĺ�����ѹ�����ޤ���
 *
 * @param  exp_fmt csv������Ĺ������ʸ����Τ���Υե����ޥåȻ���
 * @param  ap      format���б���������Ĺ�����Υꥹ��
 * @throw  �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::vresizebyf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vresizebyf(exp_fmt,ap);
    return *this;
}

/* change length of array without adjusting buffer contents */
/**
 * @brief  ����ΥХåե�����֤�Ĵ���򤻤��ˡ������ѥХåե���Ƴ���
 *
 *  ����ΥХåե�����֤�Ĵ���򤻤��ˡ����Ȥ����ļ������礭���������Ĺ����
 *  �ѹ����ޤ����Ĥޤꡤ�����ѥХåե����Ф��Ƥ�ñ���realloc()��Ƥ֤����ν���
 *  ��Ԥʤ��ޤ���
 *
 * @param     naxisx[] �Ƽ��������ǿ�
 * @param     ndim ���󼡸���
 * @param    init_buf �����˳��ݤ�����ʬ�������ͤ�ǥե�����ͤ�����ʤ� true
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::reallocate( const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::reallocate(naxisx,ndim,init_buf);
    return *this;
}

/* free current buffer and alloc new memory */
/**
 * @brief  �����ѥХåե����ö�������������˳���
 *
 *  ��������Ƥ��ö�˴����������Ĺ�����ѹ����ޤ����Ĥޤꡤ�����ѥХåե���
 *  �Ф��� free()��malloc() ��Ƥ֤����ν�����Ԥʤ��ޤ���
 *
 * @param    naxisx[] �Ƽ��������ǿ�
 * @param    ndim ���󼡸���
 * @param    init_buf �����˳��ݤ�����ʬ�������ͤ�ǥե�����ͤ�����ʤ� true
 * @throw    �����Хåե��γ��ݤ˼��Ԥ������
 */
mdarray_uintptr &mdarray_uintptr::allocate( const size_t naxisx[], size_t ndim, bool init_buf )
{
    this->mdarray::allocate(naxisx,ndim,init_buf);
    return *this;
}

/**
 * @brief  ��������ĥ���ν���ͤ�����
 *
 *  ���󥵥�����ĥ���ν���ͤ����ꤷ�ޤ������ꤵ�줿�ͤϴ�¸�����Ǥˤ�
 *  ���Ѥ�������������ĥ����ͭ���Ȥʤ�ޤ���
 *
 * @param     value ���󥵥�����ĥ���ν����
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::assign_default( uintptr_t value )
{
    this->mdarray::assign_default((const void *)&value);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ��ƿ����������Ǥ�����
 *
 *  ���Ȥ���������ǰ��� idx �ˡ�len ��ʬ�����Ǥ��������ޤ����ʤ������������
 *  ���Ǥ��ͤϥǥե�����ͤǤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param      idx �������֤������ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::insert( ssize_t idx, size_t len )
{
    this->mdarray::insert(idx,len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��ƿ����������Ǥ�����
 *
 *  ���Ȥ���������ǰ��� idx �ˡ�len ��ʬ�����Ǥ��������ޤ����ʤ������������
 *  ���Ǥ��ͤϥǥե�����ͤǤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      idx �������֤������ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::insert( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::insert(dim_index,idx,len);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ����������Ǥκ��
 *
 *  ���Ȥ����󤫤���ꤵ�줿��ʬ�����Ǥ������ޤ����������ʬ��Ĺ����û���ʤ�
 *  �ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ�.
 *
 * @param      idx ���ϰ��֤������ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::erase( ssize_t idx, size_t len )
{
    this->mdarray::erase(idx,len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǥκ��
 *
 *  ���Ȥ����󤫤���ꤵ�줿��ʬ�����Ǥ������ޤ����������ʬ��Ĺ����û���ʤ�
 *  �ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      idx ���ϰ��֤������ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::erase( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::erase(dim_index,idx,len);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ�
 *
 * ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 * ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 * ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 * idx_dst �˴�¸������Ĺ����礭���ͤ����ꤷ�Ƥ⡤���󥵥������Ѥ��ޤ���
 * ������������ cpy() ���дؿ��Ȥϰۤʤ�ޤ���<br>
 * ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param      idx_src ���ԡ����������ֹ�
 * @param      len ���ԡ��������Ǥ�Ĺ��
 * @param      idx_dst ���ԡ���������ֹ�
 * @param      clr ���ԡ������ͤΥ��ꥢ����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::move( ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr )
{
    this->mdarray::move(idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ�
 *
 *  ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst �˴�¸������Ĺ����礭���ͤ����ꤷ�Ƥ⡤���󥵥������Ѥ��ޤ���
 *  ������������ cpy() ���дؿ��Ȥϰۤʤ�ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      idx_src ���ԡ����������ֹ�
 * @param      len ���ԡ��������Ǥ�Ĺ��
 * @param      idx_dst ���ԡ���������ֹ�
 * @param      clr ���ԡ������ͤΥ��ꥢ����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::move( size_t dim_index, 
			   ssize_t idx_src, size_t len, ssize_t idx_dst,
			   bool clr )
{
    this->mdarray::move(dim_index,idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ� (������ϼ�ư��ĥ)
 *
 *  ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst + len ����¸������Ĺ����礭����硤���󥵥����ϼ�ư��ĥ�����
 *  ����<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param      idx_src ���ԡ����������ֹ�
 * @param      len ���ԡ��������Ǥ�Ĺ��
 * @param      idx_dst ���ԡ���������ֹ�
 * @param      clr ���ԡ������ͤΥ��ꥢ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::cpy( ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr )
{
    this->mdarray::cpy(idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤΥ��ԡ� (������ϼ�ư��ĥ)
 *
 *  ���Ȥ��������Ǵ֤��ͤ򥳥ԡ����ޤ���<br>
 *  ���� clr �� false �����ꤵ�줿���ϥ��ԡ������ͤϻĤ�ޤ�����true ������
 *  ���줿���ϥ��ԡ������ͤϻĤ餺�ǥե�����ͤ������ޤ���<br>
 *  idx_dst + len ����¸������Ĺ����礭����硤���󥵥����ϼ�ư��ĥ�����
 *  ����<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      idx_src ���ԡ����������ֹ�
 * @param      len ���ԡ��������Ǥ�Ĺ��
 * @param      idx_dst ���ԡ���������ֹ�
 * @param      clr ���ԡ������ͤΥ��ꥢ����
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::cpy( size_t dim_index, 
			  ssize_t idx_src, size_t len, ssize_t idx_dst,
			  bool clr )
{
    this->mdarray::cpy(dim_index,idx_src,len,idx_dst,clr);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ����������Ǵ֤Ǥ��ͤ����촹��
 *
 *  ���Ȥ��������Ǵ֤��ͤ������ؤ��ޤ���<br>
 *  �����ֹ� idx_src ���� len ��ʬ�����Ǥ������ֹ� idx_dst ���� len ��ʬ����
 *  �Ǥ������ؤ��ޤ���<br>
 *  idx_dst + len �����󥵥�����Ķ������ϡ����󥵥����ޤǤν������Ԥ���
 *  ���������ؤ����ΰ褬�Ťʤä���硤�ŤʤäƤ��ʤ� src ���ΰ���Ф��ƤΤ���
 *  ���ؤ��������Ԥ��ޤ���
 *
 * @param      idx_src �����ؤ����������ֹ�
 * @param      len �����ؤ��������Ǥ�Ĺ��
 * @param      idx_dst �����ؤ���������ֹ�
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::swap( ssize_t idx_src, size_t len, ssize_t idx_dst )
{

    this->mdarray::swap(idx_src,len,idx_dst);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ����������Ǵ֤Ǥ��ͤ����촹��
 *
 *  ���Ȥ��������Ǵ֤��ͤ������ؤ��ޤ���<br>
 *  �����ֹ� dim_index �������ֹ� idx_src ���� len ��ʬ�����Ǥ������ֹ� 
 *  idx_dst ���� len ��ʬ�����Ǥ������ؤ��ޤ���<br>
 *  idx_dst + len �����󥵥�����Ķ������ϡ����󥵥����ޤǤν������Ԥ���
 *  ���������ؤ����ΰ褬�Ťʤä���硤�ŤʤäƤ��ʤ� src ���ΰ���Ф��ƤΤ���
 *  ���ؤ��������Ԥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      idx_src �����ؤ����������ֹ�
 * @param      len �����ؤ��������Ǥ�Ĺ��
 * @param      idx_dst �����ؤ���������ֹ�
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::swap( size_t dim_index, 
		      ssize_t idx_src, size_t len, ssize_t idx_dst )
{
    this->mdarray::swap(dim_index,idx_src,len,idx_dst);
    return *this;
}

/**
 * @brief  �ǽ�μ����ˤĤ��������������ʬ�ξõ�
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ����ˤ��ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param      idx �ڤ�Ф����ϰ��֤������ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::crop( ssize_t idx, size_t len )
{
    this->mdarray::crop(idx,len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ��������������ʬ�ξõ�
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ����ˤ��ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param      dim_index �����ֹ�
 * @param      idx �ڤ�Ф����ϰ��֤������ֹ�
 * @param      len ���ǤθĿ�
 * @return     ���Ȥλ���
 * @throw      �����Хåե��γ��ݤ˼��Ԥ������
 *
 */
mdarray_uintptr &mdarray_uintptr::crop( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::crop(dim_index,idx,len);
    return *this;
}

#if 0
/* crop(), vcrop() : ̤���� */
/**
 *
 * @deprecated ̤�����Ǥ�.
 */
mdarray_uintptr &mdarray_uintptr::cropf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vcropf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcropf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 *
 * @deprecated ̤�����Ǥ�.
 */
mdarray_uintptr &mdarray_uintptr::vcropf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vcropf(exp_fmt,ap);
    return *this;
}
#endif	/* 0 */

/**
 * @brief  �ǽ�μ����ˤĤ���������¤Ӥ�ȿž
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ�ȿž�����ޤ���<br>
 *  ��˼����ֹ� 0 �μ���(1������)������оݤȤ��ޤ���
 *
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 */
mdarray_uintptr &mdarray_uintptr::flip( ssize_t idx, size_t len )
{
    this->mdarray::flip(idx,len);
    return *this;
}

/**
 * @brief  Ǥ�դ�1�Ĥμ����ˤĤ���������¤Ӥ�ȿž
 *
 *  ���Ȥ���������ǰ��� idx ���� len �Ĥ����Ǥ�ȿž�����ޤ���<br>
 *  �����ֹ� dim_index �ǽ����оݤȤ��뼡�������Ǥ��ޤ���
 *
 * @param     dim_index �����ֹ�
 * @param     idx ���ϰ��֤������ֹ�
 * @param     len ���ǤθĿ�
 * @return    ���Ȥλ���
 */
mdarray_uintptr &mdarray_uintptr::flip( size_t dim_index, ssize_t idx, size_t len )
{
    this->mdarray::flip(dim_index,idx,len);
    return *this;
}


/* 
 * member functions for image processing using IDL style argument 
 * such as "0:99,*".  The expression is set to exp_fmt argument in 
 * member functions.
 *
 * Number of dimension in the expression is unlimited.
 * Note that integer numbers in the string is 0-indexed. 
 *
 * [example]
 *   array1 = array0.sectionf("0:99,*");
 *   array1 = array0.sectionf("%ld:%ld,*", begin, end);
 *
 * Flipping elements in dimensions is supported in sectionf(), copyf(),
 * trimf(), and flipf().  Here is an example to perform flipping elements
 * in first 2 dimensions:
 *   array1 = array0.sectionf("99:0,-*");
 * 
 */

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ����ꤵ�줿���֥���
 *  ���� dest_obj �إ��ԡ����ޤ���<br>
 *  ���ԡ�������ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL �� IRAF ��
 *  Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���<br>
 *  ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°����
 *  �ɤ��٤Ƥ򥳥ԡ����ޤ�������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_obj �˼��Ȥ�Ϳ������硤trimf()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param  dest_obj ���ԡ���Υ��֥�������
 * @param  exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param  ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    ���ԡ��������ǿ�(��� �� �Կ� �� �쥤��� �� ...)
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
ssize_t mdarray_uintptr::copyf( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vcopyf(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcopyf() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ� (IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ����ꤵ�줿���֥���
 *  ���� dest_obj �إ��ԡ����ޤ���<br>
 *  ���ԡ�������ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL �� IRAF ��
 *  Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���<br>
 *  ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°����
 *  �ɤ��٤Ƥ򥳥ԡ����ޤ�������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_obj �˼��Ȥ�Ϳ������硤trimf()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param  dest_obj ���ԡ���Υ��֥�������
 * @param  exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param  ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    ���ԡ��������ǿ�(��� �� �Կ� �� �쥤��� �� ...)
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
ssize_t mdarray_uintptr::vcopyf( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vcopyf(dest_obj, exp_fmt, ap);
}

/* copy an convet all or a section to another mdarray object */
/* Flipping elements is supported                            */
/* [not implemented] */
/**
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤� (IDL�ε�ˡ)
 * @deprecated  ̤����
 */
ssize_t mdarray_uintptr::convertf_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vconvertf_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vconvertf_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤� (IDL�ε�ˡ)
 * @deprecated  ̤����
 */
ssize_t mdarray_uintptr::vconvertf_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vconvertf_copy(dest_obj, exp_fmt, ap);
}

/**
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ󥰡�IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������Ĥ���
 *  ���곰����ʬ��õ�ޤ���<br>
 *  �ڤ�Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 *  IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param  exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param  ...      exp_fmt���б���������Ĺ�����γ����ǥǡ���
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr &mdarray_uintptr::trimf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vtrimf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtrimf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ󥰡�IDL�ε�ˡ)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������Ĥ���
 *  ���곰����ʬ��õ�ޤ���<br>
 *  �ڤ�Ф���뤵����ϰϤ�Ǥ�դμ����ޤǻ����ǽ�� "0:99,*" �Τ褦�� IDL ��
 *  IRAF ��Ʊ�ͤ�ʸ����ǻ��ꤷ�ޤ�������ʸ������������ֹ�� 0-indexed �Ǥ���
 *
 * @param  exp_fmt  csv�������ϰϻ���ʸ����Τ���Υե����ޥåȻ���
 * @param  ap       exp_fmt���б���������Ĺ�����Υꥹ��
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr &mdarray_uintptr::vtrimf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vtrimf(exp_fmt, ap);
    return *this;
}

/* flip elements in a section specified by IDL-style expression */
/**
 * @brief  Ǥ�դμ���(ʣ�����)��������¤Ӥ�ȿž (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::flipf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vflipf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vflipf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  Ǥ�դμ���(ʣ�����)��������¤Ӥ�ȿž (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vflipf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vflipf(exp_fmt, ap);
    return *this;
}

/**
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
ssize_t mdarray_uintptr::transposef_xy_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vtransposef_xy_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xy_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
ssize_t mdarray_uintptr::vtransposef_xy_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vtransposef_xy_copy(dest_obj, exp_fmt, ap);
}

/**
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
ssize_t mdarray_uintptr::transposef_xyz2zxy_copy( mdarray *dest_obj, const char *exp_fmt, ... ) const
{
    ssize_t ret;
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	ret = this->mdarray::vtransposef_xyz2zxy_copy(dest_obj, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vtransposef_xyz2zxy_copy() failed");
    }
    va_end(ap);
    return ret;
}

/**
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä��������� (IDL�ε�ˡ)
 */
ssize_t mdarray_uintptr::vtransposef_xyz2zxy_copy( mdarray *dest_obj, const char *exp_fmt, va_list ap ) const
{
    return this->mdarray::vtransposef_xyz2zxy_copy(dest_obj, exp_fmt, ap);
}

/* padding existing values in an array */
/**
 * @brief  ���Ȥ������ǥե�����ͤǥѥǥ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::cleanf( const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vcleanf(exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vcleanf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ������ǥե�����ͤǥѥǥ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vcleanf( const char *exp_fmt, va_list ap )
{
    this->mdarray::vcleanf(exp_fmt, ap);
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::fillf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vfillf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vfillf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vfillf(value, exp_fmt, ap);
    return *this;
}

/* rewrite element values with a value in a section */
/**
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::fillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
        void *user_ptr, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vfillf_via_udf(value, 
		(void (*)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
		user_ptr, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vfillf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����򥹥��顼�ͤǽ񤭴��� (�桼���ؿ���ͳ��IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vfillf_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
        void *user_ptr, const char *exp_fmt, va_list ap )
{
    this->mdarray::vfillf_via_udf(value, 
	(void (*)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
	user_ptr, exp_fmt, ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤǲû� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::addf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vaddf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤǲû� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vaddf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vaddf(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǸ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::subtractf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vsubtractf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǸ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vsubtractf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vsubtractf(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�軻 (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::multiplyf( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vmultiplyf(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�軻 (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vmultiplyf( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vmultiplyf(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǽ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::dividef( double value, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vdividef(value, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǽ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vdividef( double value, const char *exp_fmt, va_list ap )
{
    this->mdarray::vdividef(value, exp_fmt, ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::pastef( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vpastef(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vpastef( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vpastef(src, exp_fmt, ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::pastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
        const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vpastef_via_udf(src, 
	       (void (*)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func, user_ptr, 
		exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vpastef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿�����Ž���դ� (�桼���ؿ���ͳ��IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vpastef_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
        const char *exp_fmt, va_list ap )
{
    this->mdarray::vpastef_via_udf(src, 
	(void (*)(double [],double [],size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func, user_ptr, 
	exp_fmt, ap);
    return *this;
}

/* add an array object */
/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������û� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::addf( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vaddf(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vaddf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������û� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vaddf( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vaddf(src, exp_fmt, ap);
    return *this;
}

/* subtract an array object */
/**
 * @brief ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����Ǹ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::subtractf( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vsubtractf(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vsubtractf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����Ǹ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vsubtractf( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vsubtractf(src, exp_fmt, ap);
    return *this;
}

/* multiply an array object */
/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������軻 (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::multiplyf( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vmultiplyf(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vmultiplyf() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������軻 (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vmultiplyf( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vmultiplyf(src, exp_fmt, ap);
    return *this;
}

/* divide an array object */
/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����ǽ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::dividef( const mdarray &src, const char *exp_fmt, ... )
{
    va_list ap;
    va_start(ap, exp_fmt);
    try {
	this->mdarray::vdividef(src, exp_fmt, ap);
    }
    catch (...) {
	va_end(ap);
	err_throw(__FUNCTION__,"FATAL","vdividef() failed");
    }
    va_end(ap);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����ǽ��� (IDL�ε�ˡ)
 */
mdarray_uintptr &mdarray_uintptr::vdividef( const mdarray &src, const char *exp_fmt, va_list ap )
{
    this->mdarray::vdividef(src, exp_fmt, ap);
    return *this;
}


/* 
 * member functions for image processing
 */


/**
 * @brief  �������Ǥ��̥��֥������Ȥإ��ԡ�
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ����ꤵ�줿���֥���
 *  ���� dest �إ��ԡ����ޤ���<br>
 *  ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°����
 *  �ɤ��٤Ƥ򥳥ԡ����ޤ�������(���ԡ���) ������ϲ��Ѥ���ޤ���<br>
 *  dest_obj �˼��Ȥ�Ϳ������硤trim()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param      dest ���ԡ���Υ��֥�������
 * @param      col_idx ���ԡ����������
 * @param      col_len ���ԡ������󥵥���
 * @param      row_idx ���ԡ����ι԰���
 * @param      row_len ���ԡ����ιԥ�����
 * @param      layer_idx ���ԡ����Υ쥤�����
 * @param      layer_len ���ԡ����Υ쥤�䥵����
 * @return     ���ԡ��������ǿ�
 * @throw      �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 *
 */
ssize_t mdarray_uintptr::copy( mdarray *dest,
			     ssize_t col_idx, size_t col_len,
			     ssize_t row_idx, size_t row_len, 
			     ssize_t layer_idx, size_t layer_len ) const
{
    debug_report("( mdarray *dest, ssize_t, ... )");
    return this->mdarray::copy(dest, col_idx,col_len,
			       row_idx, row_len, 
			       layer_idx, layer_len);
}

/* convert and copy the value of selected array element. */
/* [not implemented] */
/**
 * @brief  ���Ȥΰ������Ǥ��Ѵ�����������֤�
 * @deprecated  ̤����
 */
ssize_t mdarray_uintptr::convert_copy( mdarray *dest,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    debug_report("( mdarray *dest )");
    return this->mdarray::convert_copy(dest, col_idx, col_len,
				       row_idx, row_len, layer_idx, layer_len);
}

/* set a section to be copied by move_to() */
/**
 * @brief  ����� move_to() �ˤ���ư�������ΰ�λ���
 */
mdarray_uintptr &mdarray_uintptr::move_from(ssize_t col_idx, size_t col_len,
					  ssize_t row_idx, size_t row_len, 
					  ssize_t layer_idx, size_t layer_len)
{
    this->mdarray::move_from(col_idx,col_len, row_idx,row_len,
			     layer_idx,layer_len);
    return *this;
}

/* copy a section specified by move_from() */
/**
 * @brief  move_from() �ǻ��ꤵ�줿�ΰ�ΰ�ư
 */
mdarray_uintptr &mdarray_uintptr::move_to( 
		       ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::move_to(dest_col, dest_row, dest_layer);
    return *this;
}

/* trim a section */
/**
 * @brief  ������ʬ�ʳ����������Ǥξõ� (�����ǡ����Υȥ�ߥ�)
 *
 *  �����ǡ��������Υ��дؿ��ǡ����Ȥ�����Τ������ꤵ�줿��ʬ������Ĥ���
 *  ���곰����ʬ��õ�ޤ���<br>
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ�����硥�����˲��򵯤�������硥
 */
mdarray_uintptr &mdarray_uintptr::trim( ssize_t col_idx, size_t col_len,
				      ssize_t row_idx, size_t row_len, 
				      ssize_t layer_idx, size_t layer_len )
{
    this->mdarray::trim(col_idx,col_len, row_idx,row_len, layer_idx,layer_len);
    return *this;
}


/* flip horizontal within a rectangular section */
/**
 * @brief  ����ο�ʿ�����Ǥ�ȿž
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����ǤˤĤ��ơ��������Ƥ��ʿ������ȿž�����ޤ���
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
 */
mdarray_uintptr &mdarray_uintptr::flip_cols( ssize_t col_idx, size_t col_len,
					  ssize_t row_idx, size_t row_len, 
					  ssize_t layer_idx, size_t layer_len )
{
    this->mdarray::flip_cols(col_idx,col_len, row_idx,row_len, 
			     layer_idx,layer_len);
    return *this;
}

/* flip vertical within a rectangular section */
/**
 * @brief  ����ο�ľ�����Ǥ�ȿž
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����ǤˤĤ��ơ��������Ƥ��ľ������ȿž�����ޤ���
 *
 * @param     col_idx �����
 * @param     col_len �󥵥���
 * @param     row_idx �԰���
 * @param     row_len �ԥ�����
 * @param     layer_idx �쥤�����
 * @param     layer_len �쥤�䥵����
 * @return    ���Ȥλ���
 */
mdarray_uintptr &mdarray_uintptr::flip_rows( ssize_t col_idx, size_t col_len,
					  ssize_t row_idx, size_t row_len, 
					  ssize_t layer_idx, size_t layer_len )
{
    this->mdarray::flip_rows(col_idx,col_len, row_idx,row_len, 
			     layer_idx,layer_len);
    return *this;
}

/* interchange rows and columns and copy */
/**
 * @brief  (x,y)�Υȥ�󥹥ݡ�����Ԥʤä���������
 */
ssize_t mdarray_uintptr::transpose_xy_copy( mdarray *dest,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return this->mdarray::transpose_xy_copy( (mdarray *)dest, 
				       col_idx, col_len, row_idx, row_len, 
				       layer_idx, layer_len );
}

/* interchange x and z and copy */
/**
 * @brief  (x,y,z)��(z,x,y)�Υȥ�󥹥ݡ�����Ԥʤä���������
 */
ssize_t mdarray_uintptr::transpose_xyz2zxy_copy( mdarray *dest,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return this->mdarray::transpose_xyz2zxy_copy( (mdarray *)dest, 
				       col_idx, col_len, row_idx, row_len, 
				       layer_idx, layer_len );
}

/* rotate and copy a section to another mdarray object       */
/*   angle: 90,-90, or 180                                   */
/*          (anticlockwise when image is bottom-left origin) */
/**
 * @brief  (x,y)�ǲ�ž�������������� (��ž��90��ñ��)
 */
ssize_t mdarray_uintptr::rotate_xy_copy( mdarray *dest, int angle,
				    ssize_t col_idx, size_t col_len,
				    ssize_t row_idx, size_t row_len, 
				    ssize_t layer_idx, size_t layer_len ) const
{
    return this->mdarray::rotate_xy_copy( (mdarray *)dest, angle, 
					  col_idx, col_len, row_idx, row_len, 
					  layer_idx, layer_len);
}


/**
 * @brief  ���Ȥ������ǥե�����ͤǥѥǥ���
 *
 *  ���Ȥ���������Ǥ�ǥե�����ͤǥѥǥ��󥰤��ޤ���<br>
 *  �����ϻ��ꤷ�ʤ��Ƥ���ѤǤ��ޤ������ξ��ϡ������Ǥ������оݤǤ���
 *  clean() ��¹Ԥ��Ƥ�����Ĺ���Ѳ����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���<br>
 *
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::clean( ssize_t col_index, size_t col_size,
				       ssize_t row_index, size_t row_size,
				       ssize_t layer_index, size_t layer_size )
{
    this->mdarray::clean(col_index,col_size,row_index,row_size,
			 layer_index,layer_size);
    return *this;
}


/**
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴���
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤǽ񴹤��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value �񤭹�����
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @param      func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::fill( double value, 
			 ssize_t col_index, size_t col_size,
			 ssize_t row_index, size_t row_size,
			 ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, ssize_t ... )");
    this->mdarray::fill(value, col_index, col_size, 
			row_index, row_size, layer_index, layer_size);
    return *this;
}

/**
 * @brief  ���Ȥ��������ꤵ�줿�����顼�ͤǽ񤭴��� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ򡤻��ꤵ�줿�ͤȥ桼������ؿ���ͳ�ǽ�
 *  ���ޤ���<br>
 *  �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ�������(����)��value ��Ϳ����줿
 *  �͡�����(�ǽ�ΰ���)�θĿ�������֡��԰��֡��쥤����֡����ȤΥ��֥�������
 *  �Υ��ɥ쥹���桼���ݥ��� user_ptr ���͡���Ϳ�����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value �񤭹�����
 * @param      func ���Ѵ��ΰ٤Υ桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::fill_via_udf( double value, 
	void (*func)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
	ssize_t col_index, size_t col_size,
	ssize_t row_index, size_t row_size,
	ssize_t layer_index, size_t layer_size )
{
    debug_report("( double value, double (*func)() ... )");
    this->mdarray::fill_via_udf(value, 
			(void (*)(double [],double,size_t, ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
			user_ptr, col_index, col_size,
			row_index, row_size,
			layer_index, layer_size);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤǲû�
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ� value ��û����ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value �û�������
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::add( double value, 
		    ssize_t col_index, size_t col_size,
		    ssize_t row_index, size_t row_size,
		    ssize_t layer_index, size_t layer_size )
{
    this->mdarray::add(value,col_index,col_size,row_index,row_size,
		       layer_index,layer_size);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǸ���
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ��� value �򸺻����ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value ����������
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::subtract( double value, 
				       ssize_t col_index, size_t col_size,
				       ssize_t row_index, size_t row_size,
				       ssize_t layer_index, size_t layer_size )
{
    this->mdarray::subtract(value,col_index,col_size,row_index,row_size,
			    layer_index,layer_size);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿�����顼�ͤ�軻
 *
 * ���Ȥ�����λ��ꤵ�줿�ϰϤ����Ǥ��ͤ�value ��軻���ޤ���
 * �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value �軻������
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::multiply( double value, 
		    ssize_t col_index, size_t col_size,
		    ssize_t row_index, size_t row_size,
		    ssize_t layer_index, size_t layer_size )
{
    this->mdarray::multiply(value,col_index,col_size,row_index,row_size,
			    layer_index,layer_size);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿�����顼�ͤǽ���
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ����ǤˤĤ��� value �ǽ������ޤ���
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      value ����������
 * @param      col_index �����
 * @param      col_size �󥵥���
 * @param      row_index �԰���
 * @param      row_size �ԥ�����
 * @param      layer_index �쥤�����
 * @param      layer_size �쥤�䥵����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::divide( double value, 
				       ssize_t col_index, size_t col_size,
				       ssize_t row_index, size_t row_size,
				       ssize_t layer_index, size_t layer_size )
{
    this->mdarray::divide(value,col_index,col_size,row_index,row_size,
			  layer_index,layer_size);
    return *this;
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ�
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src �ǻ��ꤵ�줿���֥������Ȥ�
 *  �������ͤ�Ž���դ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src �����Ȥʤ��������ĥ��֥�������
 * @param      dest_col �����
 * @param      dest_row �԰���
 * @param      dest_layer �쥤�����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::paste( const mdarray &src,
	ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    debug_report("( const mdarray &src, ssize_t ... )");
    this->mdarray::paste(src,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  ���Ȥ�����˻��ꤵ�줿���֥������Ȥ������Ž���դ� (�桼���ؿ���ͳ)
 *
 *  ���Ȥ�����λ��ꤵ�줿�ϰϤ������ͤˡ�src �ǻ��ꤵ�줿���֥������Ȥγ�����
 *  �ͤ�桼������ؿ���ͳ��Ž���դ��ޤ����桼������ؿ���Ϳ����Ž���դ�����
 *  ��ư���Ѥ��뤳�Ȥ��Ǥ��ޤ���<br>
 *  �桼������ؿ� func �ΰ����ˤϽ�ˡ����Ȥ�������(����)�����֥������� src ��
 *  ������(����)���ǽ��2�Ĥΰ���������θĿ�������֡��԰��֡��쥤����֡�����
 *  �Υ��֥������ȤΥ��ɥ쥹���桼���ݥ��� user_ptr ���� ��Ϳ�����ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src �����Ȥʤ��������ĥ��֥�������
 * @param      func ���Ѵ��Τ���Υ桼���ؿ��Υ��ɥ쥹
 * @param      user_ptr func �κǸ��Ϳ������桼���Υݥ���
 * @param      dest_col �����
 * @param      dest_row �԰���
 * @param      dest_layer �쥤�����
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::paste_via_udf( const mdarray &src,
	void (*func)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray_uintptr *,void *),
	void *user_ptr,
	ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    debug_report("( const mdarray &src, double (*func)() ... )");
    this->mdarray::paste_via_udf(src,
		(void (*)(double [],double [],size_t,ssize_t,ssize_t,ssize_t,mdarray *,void *))func,
		user_ptr,
		dest_col, dest_row, dest_layer);
    return *this;
}


/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������û�
 *
 *  ���Ȥ����Ǥ˥��֥������� src_img �����������û����ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��βû�Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param      dest_col �û����ϰ���(��)
 * @param      dest_row �û����ϰ���(��)
 * @param      dest_layer �û����ϰ���(�쥤��)
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::add( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::add(src_img,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����Ǹ���
 *
 *  ���Ȥ�����������ͤ��饪�֥������� src_img ����������������ͤ򸺻����ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��θ���Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param      dest_col �������ϰ���(��)
 * @param      dest_row �������ϰ���(��)
 * @param      dest_layer �������ϰ���(�쥤��)
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::subtract( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::subtract(src_img,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  ���Ȥ�����ˡ����ꤵ�줿���֥������Ȥ������軻
 *
 *  ���Ȥ�����������ͤ˥��֥������� src_img �����������軻���ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��ξ軻Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param      dest_col �軻���ϰ���(��)
 * @param      dest_row �軻���ϰ���(��)
 * @param      dest_layer �軻���ϰ���(�쥤��)
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::multiply( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::multiply(src_img,dest_col,dest_row,dest_layer);
    return *this;
}

/**
 * @brief  ���Ȥ�����򡤻��ꤵ�줿���֥������Ȥ�����ǽ���
 *
 *  ���Ȥ�����������ͤ��饪�֥������� src_img �����������������ޤ���
 *  �󡦹ԡ��쥤��ˤĤ��Ƥ��줾��ν���Ŭ�ѳ��ϰ��֤����Ǥ��ޤ���<br>
 *  �����ǡ��������Υ��дؿ��Ǥ���
 *
 * @param      src_img �黻�˻Ȥ��������ĥ��֥�������
 * @param      dest_col �������ϰ���(��)
 * @param      dest_row �������ϰ���(��)
 * @param      dest_layer �������ϰ���(�쥤��)
 * @return     ���Ȥλ���
 *
 */
mdarray_uintptr &mdarray_uintptr::divide( const mdarray &src_img,
	  ssize_t dest_col, ssize_t dest_row, ssize_t dest_layer )
{
    this->mdarray::divide(src_img,dest_col,dest_row,dest_layer);
    return *this;
}


/**
 * @brief  �桼���Υݥ����ѿ�����Ͽ
 *
 *  �桼���Υݥ����ѿ��򥪥֥������Ȥ���Ͽ���ޤ���<br>
 *  ���Υ��дؿ���Ȥäƥ桼���Υݥ����ѿ��Υ��ɥ쥹����Ͽ����С����֥���
 *  ���Ȥ���������Хåե�����Ƭ���ɥ쥹���˥桼���Υݥ����ѿ����ݻ�������
 *  ���������Ǥ��ޤ���
 *
 * @param   extptr_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @note    ������Ϳ������ˡ��ݥ����ѿ��ˡ�&�פ�Ĥ���Τ�˺��ʤ��褦��
 *          ���ޤ��礦
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray_uintptr &mdarray_uintptr::register_extptr( uintptr_t **extptr_address )
{
    this->mdarray::register_extptr((void *)extptr_address);
    return *this;
}

/**
 * @brief  �桼���Υݥ����ѿ�(2d��)����Ͽ
 *
 *  �桼���Υݥ����ѿ�(2d��)�򥪥֥������Ȥ���Ͽ���ޤ���<br>
 *  ���Υ��дؿ���Ȥäƥ桼���Υݥ����ѿ��Υ��ɥ쥹����Ͽ����С����֥���
 *  ���Ȥ��������륢�ɥ쥹�ơ��֥����Ƭ���ɥ쥹���˥桼���Υݥ����ѿ��ˤ�
 *  �ݻ����Ƥ��������Ǥ��ޤ���
 *
 * @param   extptr2d_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @note    ������Ϳ������ˡ��ݥ����ѿ��ˡ�&�פ�Ĥ���Τ�˺��ʤ��褦��
 *          ���ޤ��礦
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray_uintptr &mdarray_uintptr::register_extptr_2d( uintptr_t *const **extptr2d_address )
{
    this->mdarray::register_extptr_2d((void *)extptr2d_address);
    return *this;
}

/**
 * @brief  �桼���Υݥ����ѿ�(3d��)����Ͽ
 *
 *  �桼���Υݥ����ѿ�(3d��)�򥪥֥������Ȥ���Ͽ���ޤ���<br>
 *  ���Υ��дؿ���Ȥäƥ桼���Υݥ����ѿ��Υ��ɥ쥹����Ͽ����С����֥���
 *  ���Ȥ��������륢�ɥ쥹�ơ��֥����Ƭ���ɥ쥹���˥桼���Υݥ����ѿ��ˤ�
 *  �ݻ����Ƥ��������Ǥ��ޤ���
 *
 * @param   extptr3d_address �桼���Υݥ����ѿ��Υ��ɥ쥹
 * @return  ���Ȥλ���
 * @note    ������Ϳ������ˡ��ݥ����ѿ��ˡ�&�פ�Ĥ���Τ�˺��ʤ��褦��
 *          ���ޤ��礦
 * @attention  �桼���Υݥ����ѿ�����Ͽ����� shallow copy ����ǽ���ʤ��ʤ�
 *             �ޤ���
 */
mdarray_uintptr &mdarray_uintptr::register_extptr_3d( uintptr_t *const *const **extptr3d_address )
{
    this->mdarray::register_extptr_3d((void *)extptr3d_address);
    return *this;
}

/**
 * @brief  �Х��ȥ�������Ĵ��
 *
 *  ���Υ��дؿ��ϡ����Ȥ������Х��ʥ�ǡ����Ȥ��ƥե��������¸����������
 *  ���뤤�ϥե�����ΥХ��ʥ�ǡ����򼫿Ȥ�����˼����ߤ������˻Ȥ��ޤ���
 *  (�ܺ٤� reverse_endian( bool, size_t, size_t ) �򻲾�)
 *
 * @param     is_little_endian 1 ���ܤν�����Υ����Υ���ǥ�����
 * @return    ���Ȥλ���
 * @note      ��®���Τ��ᡤSIMD̿���ȤäƤ��ޤ���
 *
 */
mdarray_uintptr &mdarray_uintptr::reverse_endian( bool is_little_endian )
{
    this->mdarray::reverse_byte_order(is_little_endian, 0, 0, this->length());
    return *this;
}

/**
 * @brief  �Х��ȥ�������Ĵ��
 *
 * ���Υ��дؿ��ϡ����Ȥ������Х��ʥ�ǡ����Ȥ��ƥե��������¸����������
 * ���뤤�ϥե�����ΥХ��ʥ�ǡ����򼫿Ȥ�����˼����ߤ������˻Ȥ��ޤ���<br>
 * �ե�����˥ǡ�������¸���������ϡ����Υ��дؿ���ƤӽФ��ƥե�������¸��Ŭ
 * ��������ǥ�������Ѵ�����array_ptr() ���дؿ��ʤɤǼ����������ɥ쥹�򥹥�
 * �꡼��񤭹����Ѥδؿ���Ϳ�������Ƥ�񤭹�����塤���٤��Υ��дؿ���Ƥӽ�
 * ���ơ�����ǥ�����򸵤��ᤷ�ޤ���<br>
 * �ե����뤫��ǡ������ɤ߹��ߤ������ϡ�array_ptr() ���дؿ��ʤɤǼ���������
 * �ɥ쥹�򥹥ȥ꡼���ɤ߼���Ѥδؿ���Ϳ�������Ƥ��ɤ߹�����塤���Υ��дؿ�
 * ��ƤӽФ��ƽ����Ϥ�Ŭ��������ǥ�������Ѵ����ޤ���<br>
 * �嵭�Τ�����ξ��⡤�ե��������¸�����٤��ǡ������ӥå�����ǥ�����ʤ�
 * �С���1������ false �򥻥åȤ��ޤ�(��ȥ륨��ǥ�����ʤ� true �Ǥ�)��<br>
 * ���Υ��дؿ��ϡ������Ϥˤ�äƻȤ�ʬ����ɬ�פȤʤ�ʤ��褦�˺���Ƥ���
 * �����㤨�С��ե�����˥ӥå�����ǥ�����Υǡ�������¸�������Τǡ�
 * is_little_endian �� false ����ꤷ�����Υ��дؿ���ƤӽФ����Ȥ��ޤ���
 * ���λ����ޥ��󤬥ӥå�����ǥ�����Ǥ���С��ºݤˤ�ȿž�����ϹԤ��ޤ���
 * (�ޥ��󤬥�ȥ륨��ǥ�����Ǥ���С�ȿž�������Ԥ��ޤ�)��
 * ���ˡ����֥���������ΥХ��ʥ�ǡ����򤽤Τޤޥե��������¸����С�
 * ���ꤵ�줿�Х��ȥ��������ΥХ��ʥ�ե����뤬�Ǥ��ޤ���
 * ���θ塤����Ʊ�������Ǥ��Υ��дؿ���ƤӽФ��ơ�����ǥ�����ȿž����Ƥ�
 * ����ϸ����᤹������Ԥ��ޤ���<br>
 * ���äơ��ե�����ؤ���¸�˺ݤ��Ƥϡ����Υ��дؿ���Ʊ�������ǣ���ƤӽФ���
 * ������Ǥ���
 *
 * @param     is_little_endian 1 ���ܤν�����Υ����Υ���ǥ�����
 * @param     begin �����򳫻Ϥ��������ֹ�
 * @param     length �����оݤȤʤ�Ĺ��
 * @return    ���Ȥλ���
 * @note      ��®���Τ��ᡤSIMD̿���ȤäƤ��ޤ���
 *
 */
mdarray_uintptr &mdarray_uintptr::reverse_endian( bool is_little_endian, 
						size_t begin, size_t length )
{
    this->mdarray::reverse_byte_order(is_little_endian, 0, begin, length);
    return *this;
}

/* protected */

/**
 * @brief  �Ѿ����饹�ˤ����롤�ǥե���ȷ�������
 *
 *  ���Ȥ������٤�������쥯�饹(mdarray)�Υ��дؿ������Τ��ޤ���
 * 
 * @return  ������ (������: uintptr_t)
 * @note    ���Υ��дؿ���protected�Ǥ���
 */
ssize_t mdarray_uintptr::default_size_type()
{
    return (ssize_t)UINTPTR_ZT;
}

/**
 * @brief  �Ѿ����饹�ˤ����롤���������ǽ�ʷ�������
 *
 *  ��������뷿���ɤ�������쥯�饹(mdarray)�Υ��дؿ������Τ��ޤ���
 * 
 * @param   sz_type ������
 * @return  ���������ǽ�ʷ�(uintptr_t)�ʤ鿿
 * @note    ���Υ��дؿ���protected�Ǥ���
 */
bool mdarray_uintptr::is_acceptable_size_type( ssize_t sz_type )
{
    if ( sz_type == (ssize_t)UINTPTR_ZT ) return true;
    else return false;
}

}	/* namespace */

#include "private/s_memset.cc"
#include "private/s_memmove.cc"

#undef CLASS_NAME

