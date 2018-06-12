/*
 * ����� mdarray_math.cc ���� include ���ƻȤ�
 * (1�Ĥμ¿����ΰ�����Ȥꡤ�¿����֤����)
 * namespace sli �γ�¦�ǻȤ������Υޥ������������
 *  MD_NAME_NAMESPC   ... �͡��ॹ�ڡ���̾
 *  MD_NAME_FUNCTION  ... �ؿ�̾
 *  MD_NAME_MFNC      ... double���Ѥο��شؿ�
 *  MD_NAME_MFNCF     ... float���Ѥο��شؿ�(̤����Ǥ��ɤ�)
 *  MD_NAME_MFNCL     ... long double���Ѥο��شؿ�(̤����Ǥ��ɤ�)
 */


#ifdef MD_NAME_MFNCF
#define __MD_NM_MFNCF MD_NAME_MFNCF
#else
#define __MD_NM_MFNCF MD_NAME_MFNC
#endif

#ifdef MD_NAME_MFNCL
#define __MD_NM_MFNCL MD_NAME_MFNCL
#else
#define __MD_NM_MFNCL MD_NAME_MFNC
#endif

namespace MD_NAME_NAMESPC
{
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname( const void *org_val_ptr, void *new_val_ptr, size_t len, \
		     bool func_ok, bool funcf_ok, bool funcl_ok ) \
{ \
    size_t i; \
    new_type *dst = (new_type *)new_val_ptr; \
    const org_type *src = (const org_type *)org_val_ptr; \
    if ( func_ok == true ) { \
      for ( i=0 ; i < len ; i++ ) dst[i] = (new_type)(MD_NAME_MFNC(src[i])); \
    } \
    else if ( funcf_ok == true ) { \
      for ( i=0 ; i < len ; i++ ) dst[i] = (new_type)(__MD_NM_MFNCF(src[i])); \
    } \
    else if ( funcl_ok == true ) { \
      for ( i=0 ; i < len ; i++ ) dst[i] = (new_type)(__MD_NM_MFNCL(src[i])); \
    } \
    return; \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,,,,,,,,,,,,,);
#undef MAKE_FUNC
}

#undef __MD_NM_MFNCF
#undef __MD_NM_MFNCL

namespace sli
{

mdarray MD_NAME_FUNCTION( const mdarray &src )
{
    const ssize_t src_szt = src.size_type();
    mdarray dest;
    ssize_t dest_szt;
    void (*fnc_calc)(const void *, void *, size_t, bool, bool, bool);
    bool func_ok = false;
    bool funcf_ok = false;
    bool funcl_ok = false;
#ifdef MD_NAME_MFNC
    func_ok = true;
#endif
#ifdef MD_NAME_MFNCF
    funcf_ok = true;
#endif
#ifdef MD_NAME_MFNCL
    funcl_ok = true;
#endif

    /* �¿����Ǥʤ������㳰���֤� */
    /* �¿��� */
    if ( src_szt == FLOAT_ZT || src_szt == DOUBLE_ZT ||
	 src_szt == LDOUBLE_ZT ||
	 src_szt == UCHAR_ZT || src_szt == INT16_ZT ||
	 src_szt == INT32_ZT || src_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    else if ( src_szt == FCOMPLEX_ZT || src_szt == DCOMPLEX_ZT ||
	      src_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    if ( src.length() == 0 ) goto quit;

    fnc_calc = NULL;
    dest_szt = (0 < src_szt) ? -8 : src_szt;

    /* �ؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if (src_szt == org_sz_type && dest_szt == new_sz_type) { \
        fnc_calc = &MD_NAME_NAMESPC::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src.dim_length();
	const size_t *szs = src.cdimarray();
	dest.init(dest_szt, true, szs, dim_len, false);
	if ( dest_szt < -8 ) {
	    if ( funcl_ok == true ) {
		func_ok = false;
		funcf_ok = false;
	    }
	}
	if ( dest_szt == -4 ) {
	    if ( funcf_ok == true ) {
		func_ok = false;
		funcl_ok = false;
	    }
	}
	(*fnc_calc)(src.data_ptr_cs(), dest.data_ptr(), dest.length(),
		    func_ok, funcf_ok, funcl_ok);
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

}	/* namespace */
