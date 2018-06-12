/*
 * ����� mdarray_complex.cc ���� include ���ƻȤ�
 * (1�Ĥ�ʣ�ǿ����ΰ�����Ȥꡤʣ�ǿ����֤����)
 * namespace sli �γ�¦�ǻȤ������Υޥ������������
 *  MD_NAME_NAMESPC   ... �͡��ॹ�ڡ���̾
 *  MD_NAME_FUNCTION  ... �ؿ�̾
 *  MD_NAME_MFNC      ... dcomplex���Ѥο��شؿ�
 *  MD_NAME_MFNCF     ... ffomplex���Ѥο��شؿ�(̤����Ǥ��ɤ�)
 *  MD_NAME_MFNCL     ... ldcomplex���Ѥο��شؿ�(̤����Ǥ��ɤ�)
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

namespace sli
{

namespace MD_NAME_NAMESPC
{
#define MAKE_FUNC(fncname,sz_type,op_type) \
static void fncname( const void *org_val_ptr, void *new_val_ptr, size_t len, \
		     bool func_ok, bool funcf_ok, bool funcl_ok ) \
{ \
    size_t i; \
    op_type *dest = (op_type *)new_val_ptr; \
    const op_type *src = (const op_type *)org_val_ptr; \
    if ( func_ok == true ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (op_type)(MD_NAME_MFNC((dcomplex)src[i])); \
    } \
    else if ( funcf_ok == true ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (op_type)(__MD_NM_MFNCF((fcomplex)src[i])); \
    } \
    else if ( funcl_ok == true ) { \
	for ( i=0 ; i < len ; i++ ) dest[i] = (op_type)(__MD_NM_MFNCL((ldcomplex)src[i])); \
    } \
    return; \
}
SLI__MDARRAY__DO_OPERATION_1TYPE_COMPLEX(MAKE_FUNC,);
#undef MAKE_FUNC
}

#undef __MD_NM_MFNCF
#undef __MD_NM_MFNCL

mdarray MD_NAME_FUNCTION( const mdarray &src )
{
    const ssize_t src_szt = src.size_type();
    const ssize_t dest_sz_type = src.size_type();
    mdarray dest;
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

    /* ʣ�ǿ����Ǥʤ������㳰���֤� */
    /* ʣ�ǿ��� */
    if ( src_szt == FCOMPLEX_ZT || src_szt == DCOMPLEX_ZT ||
	 src_szt == LDCOMPLEX_ZT ) {
	/* NO PROBLEM */
    }
    /* �¿��� */
    else if ( src_szt == FLOAT_ZT || src_szt == DOUBLE_ZT ||
	      src_szt == LDOUBLE_ZT || src_szt == UCHAR_ZT ||
	      src_szt == INT16_ZT || src_szt == INT32_ZT ||
	      src_szt == INT64_ZT ) {
	err_throw(__FUNCTION__,"ERROR","real type cannot be used");
    }
    /* ����¾�η� */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    if ( src.length() == 0 ) goto quit;

    fnc_calc = NULL;

    /* �ؿ������� */
#define SEL_FUNC(fncname,sz_type,op_type) \
    if ( src_szt == sz_type ) { \
        fnc_calc = &MD_NAME_NAMESPC::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_1TYPE_COMPLEX(SEL_FUNC,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src.dim_length();
	const size_t *szs = src.cdimarray();
	dest.init(dest_sz_type, true, szs, dim_len, false);
	if ( dest_sz_type < DCOMPLEX_ZT ) {
	    if ( funcl_ok == true ) {
		func_ok = false;
		funcf_ok = false;
	    }
	}
	if ( dest_sz_type == FCOMPLEX_ZT ) {
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
