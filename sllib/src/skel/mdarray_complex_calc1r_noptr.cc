/*
 * ����� mdarray_complex.cc ���� include ���ƻȤ�
 * (1�Ĥ�ʣ�ǿ����ΰ�����Ȥꡤ�¿����֤����)
 * namespace sli �γ�¦�ǻȤ������Υޥ������������
 *  MD_NAME_FUNCTION  ... �ؿ�̾
 *  MD_NAME_MFNC      ... dcomplex���Ѥο��شؿ�
 *  MD_NAME_MFNCF     ... fcomplex���Ѥο��شؿ�(̤����Ǥ��ɤ�)
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

mdarray MD_NAME_FUNCTION( const mdarray &src )
{
    const ssize_t src_szt = src.size_type();
    const size_t dim_len = src.dim_length();
    const size_t *szs = src.cdimarray();
    ssize_t dest_sz_type = DOUBLE_ZT;
    mdarray dest;
    size_t i, len;

    /* �¿��ξ����㳰���֤� */
    /* ʣ�ǿ��� */
    if ( src_szt == FCOMPLEX_ZT ) {
	dest_sz_type = FLOAT_ZT;
    }
    else if ( src_szt == DCOMPLEX_ZT ) {
	dest_sz_type = DOUBLE_ZT;
    }
    else if ( src_szt == LDCOMPLEX_ZT ) {
	dest_sz_type = LDOUBLE_ZT;
    }
    /* �¿��� */
    else if ( src_szt == FLOAT_ZT || src_szt == DOUBLE_ZT ||
	      src_szt == LDOUBLE_ZT || src_szt == UCHAR_ZT ||
	      src_szt == INT16_ZT || src_szt == INT32_ZT ||
	      src_szt == INT64_ZT ) {
	err_throw(__FUNCTION__,"ERROR","real type can not be used");
    }
    /* ����¾�η� */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    dest.init(dest_sz_type, true, szs, dim_len, false);
    len = dest.length();

    if ( src_szt == DCOMPLEX_ZT ) {
	double *dp = (double *)dest.data_ptr();
	const dcomplex *sp = (const dcomplex *)src.data_ptr();
	for (i=0 ; i < len ; i++) dp[i] = MD_NAME_MFNC(sp[i]);
    }
    else if ( src_szt == FCOMPLEX_ZT ) {
	float *dp = (float *)dest.data_ptr();
	const fcomplex *sp = (const fcomplex *)src.data_ptr();
	for (i=0 ; i < len ; i++) dp[i] = __MD_NM_MFNCF(sp[i]);
    }
    else if ( src_szt == LDCOMPLEX_ZT ) {
	long double *dp = (long double *)dest.data_ptr();
	const ldcomplex *sp = (const ldcomplex *)src.data_ptr();
	for (i=0 ; i < len ; i++) dp[i] = __MD_NM_MFNCL(sp[i]);
    }

    dest.set_auto_resize(src.auto_resize());
    dest.set_auto_init(src.auto_init());
    dest.set_rounding(src.rounding());

    dest.set_scopy_flag();
    return dest;
}

}	/* namespace */

