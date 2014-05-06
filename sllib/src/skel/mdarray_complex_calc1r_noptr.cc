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
    const size_t dim_len = src.dim_length();
    const size_t *szs = src.cdimarray();
    ssize_t dest_sz_type = DOUBLE_ZT;
    mdarray dest;
    size_t i, len;

    /* �¿��ξ����㳰���֤� */
    switch( src.size_type() ) {
      /* ʣ�ǿ��� */
      case FCOMPLEX_ZT  :
	  dest_sz_type = FLOAT_ZT;
	  break;
      case DCOMPLEX_ZT  :
	  dest_sz_type = DOUBLE_ZT;
	  break;
      case LDCOMPLEX_ZT :
	  dest_sz_type = LDOUBLE_ZT;
	  break;
      /* �¿��� */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	err_throw(__FUNCTION__,"ERROR","real type can not be used");
	break;
      /* ����¾�η� */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    dest.init(dest_sz_type, true, szs, dim_len, false);
    len = dest.length();

    if ( src.size_type() == DCOMPLEX_ZT ) {
	double *dp = (double *)dest.data_ptr();
	const dcomplex *sp = (const dcomplex *)src.data_ptr();
	for (i=0 ; i < len ; i++) dp[i] = MD_NAME_MFNC(sp[i]);
    }
    else if ( src.size_type() == FCOMPLEX_ZT ) {
	float *dp = (float *)dest.data_ptr();
	const fcomplex *sp = (const fcomplex *)src.data_ptr();
	for (i=0 ; i < len ; i++) dp[i] = __MD_NM_MFNCF(sp[i]);
    }
    else if ( src.size_type() == LDCOMPLEX_ZT ) {
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

