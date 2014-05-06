/*
 * これは mdarray_math.cc から include して使う
 * (引数の片方が配列，もう片方がスカラーの場合)
 * namespace sli の外側で使い，次のマクロを定義する事
 *  MD_NAME_NAMESPC   ... ネームスペース名
 *  MD_NAME_FUNCTION  ... 関数名
 *  MD_NAME_MFNC      ... double型用の数学関数
 *  MD_NAME_MFNCF     ... float型用の数学関数(未定義でも良い)
 *  MD_NAME_MFNCL     ... long double型用の数学関数(未定義でも良い)
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
static void fncname( const void *org_val_ptr, long double s_v, bool rv, \
		     void *new_val_ptr, size_t len, \
		     bool func_ok, bool funcf_ok, bool funcl_ok ) \
{ \
  size_t i; \
  new_type *dst = (new_type *)new_val_ptr; \
  const org_type *src = (const org_type *)org_val_ptr; \
  if ( rv == false ) { \
    if ( func_ok == true ) { \
       for (i=0 ; i < len ; i++) dst[i] = (new_type)(MD_NAME_MFNC(src[i], s_v)); \
    } \
    else if ( funcf_ok == true ) { \
       for (i=0 ; i < len ; i++) dst[i] = (new_type)(__MD_NM_MFNCF(src[i], s_v)); \
    } \
    else if ( funcl_ok == true ) { \
       for (i=0 ; i < len ; i++) dst[i] = (new_type)(__MD_NM_MFNCL(src[i], s_v)); \
    } \
  } \
  else { \
    if ( func_ok == true ) { \
       for (i=0 ; i < len ; i++) dst[i] = (new_type)(MD_NAME_MFNC(s_v, src[i])); \
    } \
    else if ( funcf_ok == true ) { \
       for (i=0 ; i < len ; i++) dst[i] = (new_type)(__MD_NM_MFNCF(s_v, src[i])); \
    } \
    else if ( funcl_ok == true ) { \
       for (i=0 ; i < len ; i++) dst[i] = (new_type)(__MD_NM_MFNCL(s_v, src[i])); \
    } \
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

static void MD_NAME_FUNCTION( const mdarray &src0, 
		   long double src1, ssize_t szt_src1, bool rv, mdarray *dest )
{
    ssize_t zt0, zt1, dest_sz_type;
    void (*fnc_calc)(const void *, long double, bool, void *, size_t,
		     bool, bool, bool);
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

    /* 実数型でない場合は例外を返す */
    switch( src0.size_type() ) {
      /* 実数型 */
      case FLOAT_ZT     :
      case DOUBLE_ZT    :
      case LDOUBLE_ZT   :
      case UCHAR_ZT     :
      case INT16_ZT     :
      case INT32_ZT     :
      case INT64_ZT     :
	/* NO PROBLEM */
	break;
      /* 複素数型 */
      case FCOMPLEX_ZT  :
      case DCOMPLEX_ZT  :
      case LDCOMPLEX_ZT :
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
	break;
      /* その他の型 */
      default           :
	err_throw(__FUNCTION__,"ERROR","unsupported type");
	break;
    }

    if ( src0.length() == 0 ) goto quit;

    fnc_calc = NULL;
    zt0 = (0 < src0.size_type()) ? -8 : src0.size_type();
    zt1 = szt_src1;
    dest_sz_type = (zt0 < zt1) ? zt0 : zt1;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if (src0.size_type() == org_sz_type && dest_sz_type == new_sz_type) { \
        fnc_calc = &MD_NAME_NAMESPC::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    if ( fnc_calc != NULL ) {
	const size_t dim_len = src0.dim_length();
	const size_t *szs = src0.cdimarray();
	dest->init(dest_sz_type, true, szs, dim_len, false);
	if ( dest_sz_type < -8 ) {
	    if ( funcl_ok == true ) {
		func_ok = false;
		funcf_ok = false;
	    }
	}
	if ( dest_sz_type == -4 ) {
	    if ( funcf_ok == true ) {
		func_ok = false;
		funcl_ok = false;
	    }
	}
	(*fnc_calc)(src0.data_ptr_cs(), src1, rv, dest->data_ptr(),
		    dest->length(), func_ok, funcf_ok, funcl_ok);
    } else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }
 quit:
    dest->set_auto_resize(src0.auto_resize());
    dest->set_auto_init(src0.auto_init());
    dest->set_rounding(src0.rounding());
    return;
}

}	/* namespace */
