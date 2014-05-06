/* -*- Mode: C++ ; Coding: euc-japan -*- */
/* Time-stamp: <2013-04-27 21:00:12 cyamauch> */

/**
 * @file   mdarray_complex.cc
 * @brief  mdarray�Ȥ��ηѾ����饹�Υ��֥������Ȥǻ��Ѳ�ǽ��ʣ�Ǵؿ��Υ�����
 * @note   �ѥե����ޥ󥹤�Ż뤷���ؿ��ݥ��󥿤�Ȥ�ʤ������򤷤Ƥ��ꡤ
 *         ���֥������ȥե��������粽���װ��ȤʤäƤ��롥
 */

#define CLASS_NAME "-"

#include "config.h"

#include <math.h>
#include "mdarray_complex.h"
#include "private/err_report.h"

/* creal() */
#define MD_NAME_FUNCTION creal
#define MD_NAME_MFNC creal
#define MD_NAME_MFNCF creal
#define MD_NAME_MFNCL creal
#include "skel/mdarray_complex_calc1r_noptr.cc"
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
#undef MD_NAME_MFNCL
/* cimag() */
#define MD_NAME_FUNCTION cimag
#define MD_NAME_MFNC cimag
#define MD_NAME_MFNCF cimag
#define MD_NAME_MFNCL cimag
#include "skel/mdarray_complex_calc1r_noptr.cc"
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
#undef MD_NAME_MFNCL

/* conj() */
#define MD_NAME_NAMESPC __sli__conj
#define MD_NAME_FUNCTION conj
#define MD_NAME_MFNC conj
#define MD_NAME_MFNCF conj
#define MD_NAME_MFNCL conj
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
#undef MD_NAME_MFNCF
#undef MD_NAME_MFNCL

/* cabs() */
#define MD_NAME_FUNCTION cabs
#define MD_NAME_MFNC cabs
#include "skel/mdarray_complex_calc1r_noptr.cc"
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* carg() */
#define MD_NAME_FUNCTION carg
#define MD_NAME_MFNC carg
#include "skel/mdarray_complex_calc1r_noptr.cc"
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC

/* cexp() */
#define MD_NAME_NAMESPC __sli__cexp
#define MD_NAME_FUNCTION cexp
#define MD_NAME_MFNC cexp
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* csqrt() */
#define MD_NAME_NAMESPC __sli__csqrt
#define MD_NAME_FUNCTION csqrt
#define MD_NAME_MFNC csqrt
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* clog() */
#define MD_NAME_NAMESPC __sli__clog
#define MD_NAME_FUNCTION clog
#define MD_NAME_MFNC clog
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* cproj() */
#define MD_NAME_NAMESPC __sli__cproj
#define MD_NAME_FUNCTION cproj
#define MD_NAME_MFNC cproj
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* csin() */
#define MD_NAME_NAMESPC __sli__csin
#define MD_NAME_FUNCTION csin
#define MD_NAME_MFNC csin
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* ccos() */
#define MD_NAME_NAMESPC __sli__ccos
#define MD_NAME_FUNCTION ccos
#define MD_NAME_MFNC ccos
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* ctan() */
#define MD_NAME_NAMESPC __sli__ctan
#define MD_NAME_FUNCTION ctan
#define MD_NAME_MFNC ctan
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* csinh() */
#define MD_NAME_NAMESPC __sli__csinh
#define MD_NAME_FUNCTION csinh
#define MD_NAME_MFNC csinh
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* ccosh() */
#define MD_NAME_NAMESPC __sli__ccosh
#define MD_NAME_FUNCTION ccosh
#define MD_NAME_MFNC ccosh
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* ctanh() */
#define MD_NAME_NAMESPC __sli__ctanh
#define MD_NAME_FUNCTION ctanh
#define MD_NAME_MFNC ctanh
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* casin() */
#define MD_NAME_NAMESPC __sli__casin
#define MD_NAME_FUNCTION casin
#define MD_NAME_MFNC casin
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* cacos() */
#define MD_NAME_NAMESPC __sli__cacos
#define MD_NAME_FUNCTION cacos
#define MD_NAME_MFNC cacos
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* catan() */
#define MD_NAME_NAMESPC __sli__catan
#define MD_NAME_FUNCTION catan
#define MD_NAME_MFNC catan
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* casinh() */
#define MD_NAME_NAMESPC __sli__casinh
#define MD_NAME_FUNCTION casinh
#define MD_NAME_MFNC casinh
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* cacosh() */
#define MD_NAME_NAMESPC __sli__cacosh
#define MD_NAME_FUNCTION cacosh
#define MD_NAME_MFNC cacosh
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC
/* catanh() */
#define MD_NAME_NAMESPC __sli__catanh
#define MD_NAME_FUNCTION catanh
#define MD_NAME_MFNC catanh
#include "skel/mdarray_complex_calc1_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC

/* cpow() */
#define MD_NAME_NAMESPC __sli__calc2_cpow
#define MD_NAME_FUNCTION calc2_cpow
#define MD_NAME_MFNC cpow
#include "skel/mdarray_complex_calc2_noptr.cc"
namespace sli
{
mdarray cpow( const mdarray &obj, dcomplex v )
{
    mdarray dest;
    MD_NAME_FUNCTION(obj, v, DOUBLE_ZT, false, &dest);
    dest.set_scopy_flag();
    return dest;
}
mdarray cpow( dcomplex v, const mdarray &obj )
{
    mdarray dest;
    MD_NAME_FUNCTION(obj, v, DOUBLE_ZT, true, &dest);
    dest.set_scopy_flag();
    return dest;
}
}	/* namespace */
#undef MD_NAME_NAMESPC
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC

/* cpow() */
#define MD_NAME_NAMESPC __sli__calc2s_cpow
#define MD_NAME_R_FUNCTION r_calc2s_cpow
#define MD_NAME_S_FUNCTION s_calc2s_cpow
#define MD_NAME_FUNCTION cpow
#define MD_NAME_MFNC cpow
#include "skel/mdarray_complex_calc2s_noptr.cc"
#undef MD_NAME_NAMESPC
#undef MD_NAME_R_FUNCTION
#undef MD_NAME_S_FUNCTION
#undef MD_NAME_FUNCTION
#undef MD_NAME_MFNC


namespace sli
{

namespace _calcx1
{
#define MAKE_FUNC(fncname,sz_type,op_type) \
static void fncname( const void *org_val_ptr, void *new_val_ptr, size_t len, \
		     dcomplex (*func)(dcomplex), fcomplex (*funcf)(fcomplex), \
		     ldcomplex (*funcl)(ldcomplex) ) \
{ \
    size_t i; \
    op_type *dest = (op_type *)new_val_ptr; \
    const op_type *src = (const op_type *)org_val_ptr; \
    if ( func != NULL ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (op_type)((*func)((dcomplex)src[i])); \
    } \
    else if ( funcf != NULL ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (op_type)((*funcf)((fcomplex)src[i])); \
    } \
    else if ( funcl != NULL ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (op_type)((*funcl)((ldcomplex)src[i])); \
    } \
    return; \
}
SLI__MDARRAY__DO_OPERATION_1TYPE_COMPLEX(MAKE_FUNC,);
#undef MAKE_FUNC
}


/**
 * @brief  mdarray �α黻 (̤����)
 *
 *  mdarray �򡤻��ꤵ�줿�黻�Ѵؿ�(func��funcf��funcl)�ˤ��黻����
 *  ���η�̤��֤���
 *
 * @param      src     �黻�Ѥ� mdarray
 * @param      func    dcomplex ���α黻�Ѵؿ��ݥ���
 * @param      funcf   fcomplex ���α黻�Ѵؿ��ݥ���
 * @param      funcl   ldcomplex ���α黻�Ѵؿ��ݥ���
 * @return     �黻��� mdarray
 * @note       ���ߤ�̤����
 */
mdarray calcx1( const mdarray &src,
	       dcomplex (*func)(dcomplex), fcomplex (*funcf)(fcomplex),
	       ldcomplex (*funcl)(ldcomplex) )
{
    const ssize_t dest_sz_type = src.size_type();
    mdarray dest;
    void (*fnc_calc)(const void *, void *, size_t,
	   dcomplex (*)(dcomplex), fcomplex (*)(fcomplex), ldcomplex (*)(ldcomplex));

    /* �¿��ξ����㳰���֤� */
    switch( src.size_type() ) {
      /* ʣ�ǿ��� */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	/* NO PROBLEM */
	break;
      /* �¿��� */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
	break;
      /* ����¾�η� */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    if ( src.length() == 0 ) goto quit;

    fnc_calc = NULL;

    /* �ؿ������� */
#define SEL_FUNC(fncname,sz_type,op_type) \
    if ( src.size_type() == sz_type ) { \
        fnc_calc = &_calcx1::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_1TYPE_COMPLEX(SEL_FUNC,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src.dim_length();
	const size_t *szs = src.cdimarray();
	dest.init(dest_sz_type, true, szs, dim_len, false);
	if ( dest_sz_type < DCOMPLEX_ZT ) {
	    if ( funcl != NULL ) {
		func = NULL;
		funcf = NULL;
	    }
	}
	if ( dest_sz_type == FCOMPLEX_ZT ) {
	    if ( funcf != NULL ) {
		func = NULL;
		funcl = NULL;
	    }
	}
	(*fnc_calc)(src.data_ptr_cs(), dest.data_ptr(), dest.length(),
		    func, funcf, funcl);
    } else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
 quit:
    dest.set_auto_resize(src.auto_resize());
    dest.set_auto_init(src.auto_init());
    dest.set_rounding(src.rounding());

    dest.set_scopy_flag();
    return dest;
}


namespace _calcx2
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type) \
static void fncname( const void *org_val_ptr, ldcomplex s_v, bool rv, \
		     void *new_val_ptr, size_t len, \
		     dcomplex (*func)(dcomplex,dcomplex), \
		     fcomplex (*funcf)(fcomplex,fcomplex), \
		     ldcomplex (*funcl)(ldcomplex,ldcomplex) ) \
{ \
  size_t i; \
  new_type *dest = (new_type *)new_val_ptr; \
  const org_type *src = (const org_type *)org_val_ptr; \
  if ( rv == false ) { \
    if ( func != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*func)((dcomplex)src[i], s_v)); \
    } \
    else if ( funcf != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcf)((fcomplex)src[i], s_v)); \
    } \
    else if ( funcl != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcl)((ldcomplex)src[i], s_v)); \
    } \
  } \
  else { \
    if ( func != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*func)(s_v, (dcomplex)src[i])); \
    } \
    else if ( funcf != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcf)(s_v, (fcomplex)src[i])); \
    } \
    else if ( funcl != NULL ) { \
       for (i=0 ; i < len ; i++) dest[i] = (new_type)((*funcl)(s_v, (ldcomplex)src[i])); \
    } \
  } \
  return; \
}
SLI__MDARRAY__DO_OPERATION_2TYPES_COMPLEX(MAKE_FUNC,);
#undef MAKE_FUNC
}

/**
 * @brief  mdarray �α黻 (̤����)
 *
 *  mdarray ��ʣ�ǿ����򡤻��ꤵ�줿�黻�Ѵؿ�(func��funcf��funcl)�ˤ��黻
 *  �������η�̤��֤���
 *
 * @param      src0     �黻�Ѥ� mdarray
 * @param      src1     �黻�Ѥ�ʣ�ǿ���
 * @param      szt_src1 ʣ�ǿ����Υ�����
 * @param      rv       true �ξ��� src0, src1 �ν���� func �����¹�
 * @param      func     dcomplex ���α黻�Ѵؿ��ݥ���
 * @param      funcf    fcomplex ���α黻�Ѵؿ��ݥ���
 * @param      funcl    ldcomplex ���α黻�Ѵؿ��ݥ���
 * @return     �黻��� mdarray
 * @note       ���ߤ�̤����
 */
mdarray calcx2( const mdarray &src0, 
	        ldcomplex src1, ssize_t szt_src1, bool rv,
	        dcomplex (*func)(dcomplex,dcomplex), fcomplex (*funcf)(fcomplex,fcomplex),
	        ldcomplex (*funcl)(ldcomplex,ldcomplex))
{
    mdarray dest;
    ssize_t zt0, zt1, dest_sz_type;
    void (*fnc_calc)(const void *, ldcomplex, bool, void *, size_t,
		     dcomplex (*)(dcomplex,dcomplex), fcomplex (*)(fcomplex,fcomplex), 
		     ldcomplex (*)(ldcomplex,ldcomplex));

    /* �¿��ξ����㳰���֤� */
    switch( src0.size_type() ) {
      /* ʣ�ǿ��� */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	/* NO PROBLEM */
	break;
      /* �¿��� */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
	break;
      /* ����¾�η� */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }
    switch( szt_src1 ) {
      /* ʣ�ǿ��� */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	/* NO PROBLEM */
	break;
      /* �¿��� */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
	break;
      /* ����¾�η� */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    if ( src0.length() == 0 ) goto quit;

    fnc_calc = NULL;
    zt0 = src0.size_type();
    zt1 = szt_src1;
    dest_sz_type = (zt0 < zt1) ? zt0 : zt1;

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type) \
    if (src0.size_type() == org_sz_type && dest_sz_type == new_sz_type) { \
        fnc_calc = &_calcx2::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_COMPLEX(SEL_FUNC,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src0.dim_length();
	const size_t *szs = src0.cdimarray();
	dest.init(dest_sz_type, true, szs, dim_len, false);
	if ( dest_sz_type < DCOMPLEX_ZT ) {
	    if ( funcl != NULL ) {
		func = NULL;
		funcf = NULL;
	    }
	}
	if ( dest_sz_type == FCOMPLEX_ZT ) {
	    if ( funcf != NULL ) {
		func = NULL;
		funcl = NULL;
	    }
	}
	(*fnc_calc)(src0.data_ptr_cs(), src1, rv, dest.data_ptr(),
		    dest.length(), func, funcf, funcl);
    } else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
 quit:
    dest.set_auto_resize(src0.auto_resize());
    dest.set_auto_init(src0.auto_init());
    dest.set_rounding(src0.rounding());

    dest.set_scopy_flag();
    return dest;
}

/**
 * @brief  mdarray �α黻 (̤����)
 *
 *  mdarray Ʊ�Τ򡤻��ꤵ�줿�黻�Ѵؿ�(func��funcf��funcl)�ˤ��黻����
 *  ���η�̤��֤���
 *
 * @param      src0     �黻�Ѥ� mdarray 1
 * @param      src1     �黻�Ѥ� mdaaray 2
 * @param      func     dcomplex ���α黻�Ѵؿ��ݥ���
 * @param      funcf    fcomplex ���α黻�Ѵؿ��ݥ���
 * @param      funcl    ldcomplex ���α黻�Ѵؿ��ݥ���
 * @return     �黻��� mdarray
 * @note       ���ߤ�̤����
 */
mdarray calcx2( const mdarray &src0, const mdarray &src1,
	        dcomplex (*func)(dcomplex,dcomplex), fcomplex (*funcf)(fcomplex,fcomplex),
	        ldcomplex (*funcl)(ldcomplex,ldcomplex) )
{
    const ssize_t zt0 = src0.size_type();
    const ssize_t zt1 = src1.size_type();
    const ssize_t dest_sz_type = (zt0 < zt1) ? zt0 : zt1;
    size_t dim_len = src0.dim_length();
    mdarray ret(dest_sz_type, true);
    mdarray_size nx;
    size_t i;
    if ( dim_len < src1.dim_length() ) dim_len = src1.dim_length();
    nx.resize(dim_len);
    for ( i=0 ; i < dim_len ; i++ ) {
	size_t n = src0.length(i);
	if ( n < src1.length(i) ) n = src1.length(i);
	nx[i] = n;
    }

    /* �¿��ξ����㳰���֤� */
    switch( src0.size_type() ) {
      /* ʣ�ǿ��� */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	/* NO PROBLEM */
	break;
      /* �¿��� */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
	break;
      /* ����¾�η� */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }
    switch( src1.size_type() ) {
      /* ʣ�ǿ��� */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	/* NO PROBLEM */
	break;
      /* �¿��� */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
	break;
      /* ����¾�η� */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    ret.reallocate(nx.array_ptr(), dim_len, true);
    ret.set_auto_resize(true);
    ret += src0;
    ret.calcx2(src1, false, func, funcf, funcl);
    ret.set_auto_resize(src0.auto_resize());
    ret.set_auto_init(src0.auto_init());
    ret.set_rounding(src0.rounding());

    ret.set_scopy_flag();
    return ret;
}

#if 0
/**
 * @brief  ʣ�ǿ��� sin ��黻����
 *
 *  ʣ�ǿ��� sin ��黻���롥
 *
 * @param  obj �黻���� mdarray ���֥�������
 * @return �黻���� mdarray ���֥�������
 */
//mdarray csin( const mdarray &obj )
//{
//  return calcx1(obj,&sli::csin,NULL,NULL);
//}

/**
 * @brief  ʣ�ǿ��� cos ��黻����
 *
 *  ʣ�ǿ��� cos ��黻���롥
 *
 * @param  obj �黻���� mdarray ���֥�������
 * @return �黻���� mdarray ���֥�������
 */
//mdarray ccos( const mdarray &obj )
//{
//  return calcx1(obj,&sli::ccos,NULL,NULL);
//}

/**
 * @brief  ʣ�ǿ��� tan ��黻����
 *
 *  ʣ�ǿ��� tan ��黻���롥
 *
 * @param  obj �黻���� mdarray ���֥�������
 * @return �黻���� mdarray ���֥�������
 */
//mdarray ctan( const mdarray &obj )
//{
//  return calcx1(obj,&sli::ctan,NULL,NULL);
//}

/**
 * @brief  ʣ�ǿ��� exp ��黻����
 *
 *  ʣ�ǿ��� exp ��黻���롥
 *
 * @param  obj �黻���� mdarray ���֥�������
 * @return �黻���� mdarray ���֥�������
 */
//mdarray cexp( const mdarray &obj )
//{
//  return calcx1(obj,&sli::cexp,NULL,NULL);
//}
#endif	/* 0 */

}

