/*
 * これは mdarray_math.cc から include して使う
 * (引数の両方が配列の場合)
 * namespace sli の外側で使い，次のマクロを定義する事
 *  MD_NAME_NAMESPC     ... ネームスペース名
 *  MD_NAME_R_FUNCTION  ... 再帰呼び出し用static関数名
 *  MD_NAME_S_FUNCTION  ... static関数名
 *  MD_NAME_FUNCTION    ... 関数名
 *  MD_NAME_MFNC        ... double型用の数学関数
 *  MD_NAME_MFNCF       ... float型用の数学関数(未定義でも良い)
 *  MD_NAME_MFNCL       ... long double型用の数学関数(未定義でも良い)
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

/* mdarray.cc の namespace _calc2 のと本質的に同じ */
#define MAKE_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
static void fncname(const void *org_val_ptr, void *new_val_ptr, \
		    bool rv, size_t len, const void *default_v_ptr, \
		    bool func_ok, bool funcf_ok, bool funcl_ok) \
{ \
  size_t i; \
  new_type *dest = (new_type *)new_val_ptr; \
  const org_type *src = (const org_type *)org_val_ptr; \
  if ( src != NULL && rv == false ) { \
    if ( func_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(MD_NAME_MFNC((double)dest[i],(double)src[i])); \
    } \
    else if ( funcf_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCF((float)dest[i],(float)src[i])); \
    } \
    else if ( funcl_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCL((long double)dest[i],(long double)src[i])); \
    } \
  } \
  else if ( src != NULL && rv == true ) { \
    if ( func_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(MD_NAME_MFNC((double)src[i],(double)dest[i])); \
    } \
    else if ( funcf_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCF((float)src[i],(float)dest[i])); \
    } \
    else if ( funcl_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCL((long double)src[i],(long double)dest[i])); \
    } \
  } \
  else if ( src == NULL && rv == false ) { \
    org_type def_v = (org_type)0; \
    if ( default_v_ptr != NULL ) def_v = *((const org_type *)default_v_ptr); \
    if ( func_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(MD_NAME_MFNC((double)dest[i],(double)def_v)); \
    } \
    else if ( funcf_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCF((float)dest[i],(float)def_v)); \
    } \
    else if ( funcl_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCL((long double)dest[i],(long double)def_v)); \
    } \
    /* for (i=0 ; i<len ; i++) dest[i] = 0; */ \
  } \
  else if ( src == NULL && rv == true ) { \
    org_type def_v = (org_type)0; \
    if ( default_v_ptr != NULL ) def_v = *((const org_type *)default_v_ptr); \
    if ( func_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(MD_NAME_MFNC((double)def_v,(double)dest[i])); \
    } \
    else if ( funcf_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCF((float)def_v,(float)dest[i])); \
    } \
    else if ( funcl_ok == true ) { \
      for (i=0 ; i<len ; i++) dest[i] = (new_type)(__MD_NM_MFNCL((long double)def_v,(long double)dest[i])); \
    } \
    /* for (i=0 ; i<len ; i++) dest[i] = 0; */ \
  } \
}
SLI__MDARRAY__DO_OPERATION_2TYPES(MAKE_FUNC,,,,,,,,,,,,,);
#undef MAKE_FUNC

}	/* namespace */

namespace sli
{

/* mdarray::r_calc2() と本質的に同じ */
static void MD_NAME_R_FUNCTION(mdarray *thisp, const mdarray &obj, 
			  bool obj_1st, size_t dim_idx,
			  size_t len_src, size_t len_dest,
			  size_t pos_src, size_t pos_dest,
			  bool func_ok, bool funcf_ok, bool funcl_ok,
			  void (*func_calc)(const void *, void *, bool, size_t,
					    const void *, bool, bool, bool) )
{
    size_t len, len_large;
    len = thisp->length(dim_idx);
    len_large = len;
    if ( len_src == 0 ) len = 0;
    else if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= thisp->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    MD_NAME_R_FUNCTION( thisp, obj, obj_1st, dim_idx-1, len_src, len_dest,
			   pos_src + len_src * i, pos_dest + len_dest * i,
			   func_ok, funcf_ok, funcl_ok, func_calc);
	}
	/* default 値を用いた計算をする */
	for ( ; i < len_large ; i++ ) {
	    MD_NAME_R_FUNCTION( thisp, obj, obj_1st, dim_idx-1, 0, len_dest,
			   pos_src + 0 * i, pos_dest + len_dest * i,
			   func_ok, funcf_ok, funcl_ok, func_calc);
	}
    }
    else {	/* 次元=0 での処理 */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = thisp->bytes();
	char *dest = (char *)(thisp->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	/* 関数の浮動小数点引数2つは対等なので，    */
	/* すべての組合せについて計算する必要がある */
	(*func_calc)((const void *)src, (void *)dest, obj_1st, len,
		     obj.default_value_ptr(), func_ok,funcf_ok,funcl_ok);
	dest += dest_bytes * len;
	/* default 値を用いた計算をする */
	(*func_calc)(NULL, (void *)dest, obj_1st, len_large - len,
		     obj.default_value_ptr(), func_ok,funcf_ok,funcl_ok);
    }
    return;
}

/* mdarray::calc2() と本質的に同じ */
static void MD_NAME_S_FUNCTION(mdarray *thisp, const mdarray &obj, bool obj_1st)
{
    void (*func_calc)(const void *, void *, bool, size_t, const void *,
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

    if ( thisp->auto_resize() == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( thisp->length(i) < obj.length(i) ) {
		thisp->resize(i, obj.length(i));
	    }
	}
    }

    if ( thisp->dim_length() == 0 ) return;
    if ( thisp->data_ptr_cs() == NULL ) return;
    if ( obj.data_ptr_cs() == NULL ) return;

    func_calc = NULL;

    /* 関数を選択 */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.size_type() == org_sz_type && thisp->size_type() == new_sz_type ) { \
        func_calc = &MD_NAME_NAMESPC::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES(SEL_FUNC,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* 関数があれば代入を行なう */
    if ( func_calc != NULL ) {
	const ssize_t dest_sz_type = thisp->size_type();
	size_t len_src, len_dest, ndim;
	size_t i;

	if ( dest_sz_type < DOUBLE_ZT ) {
	    if ( funcl_ok == true ) {
		func_ok = false;
		funcf_ok = false;
	    }
	}
	if ( dest_sz_type == FLOAT_ZT ) {
	    if ( funcf_ok == true ) {
		func_ok = false;
		funcl_ok = false;
	    }
	}

	/* 大きい方をとる */
	ndim = thisp->dim_length();
	if ( ndim < obj.dim_length() ) ndim = obj.dim_length();

	/* 最初は，扱う次元内の全ピクセル数を len_xxx にセットする */
	len_src = 1;
	for ( i=0 ; i < ndim ; i++ ) len_src *= obj.length(i);
	len_dest = 1;
	for ( i=0 ; i < ndim ; i++ ) len_dest *= thisp->length(i);
	
	MD_NAME_R_FUNCTION(thisp, obj, obj_1st, ndim-1, len_src, len_dest, 0, 0, 
		      func_ok, funcf_ok, funcl_ok, func_calc);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return;
}

mdarray MD_NAME_FUNCTION( const mdarray &src0, const mdarray &src1 )
{
    const ssize_t src0_szt = src0.size_type();
    const ssize_t src1_szt = src1.size_type();
    const ssize_t zt0 = (0 < src0_szt) ? -8 : src0_szt;
    const ssize_t zt1 = (0 < src1_szt) ? -8 : src1_szt;
    const ssize_t dest_szt = (zt0 < zt1) ? zt0 : zt1;
    size_t dim_len = src0.dim_length();
    mdarray ret(dest_szt, true);
    mdarray_size nx;
    size_t i;
    if ( dim_len < src1.dim_length() ) dim_len = src1.dim_length();
    nx.resize(dim_len);
    for ( i=0 ; i < dim_len ; i++ ) {
	size_t n = src0.length(i);
	if ( n < src1.length(i) ) n = src1.length(i);
	nx[i] = n;
    }

    /* 実数型でない場合は例外を返す */
    /* 実数型 */
    if ( src0_szt == FLOAT_ZT || src0_szt == DOUBLE_ZT || 
	 src0_szt == LDOUBLE_ZT ||
	 src0_szt == UCHAR_ZT || src0_szt == INT16_ZT ||
	 src0_szt == INT32_ZT || src0_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    /* 複素数型 */
    else if ( src0_szt == FCOMPLEX_ZT || src0_szt == DCOMPLEX_ZT ||
	      src0_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    /* その他の型 */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }
    /* 実数型 */
    if ( src1_szt == FLOAT_ZT || src1_szt == DOUBLE_ZT || 
	 src1_szt == LDOUBLE_ZT ||
	 src1_szt == UCHAR_ZT || src1_szt == INT16_ZT ||
	 src1_szt == INT32_ZT || src1_szt == INT64_ZT ) {
	/* NO PROBLEM */
    }
    /* 複素数型 */
    else if ( src1_szt == FCOMPLEX_ZT || src1_szt == DCOMPLEX_ZT ||
	      src1_szt == LDCOMPLEX_ZT ) {
	err_throw(__FUNCTION__,"ERROR","complex type cannot be used");
    }
    /* その他の型 */
    else {
	err_throw(__FUNCTION__,"ERROR","unsupported type");
    }

    ret.reallocate(nx.array_ptr(), dim_len, true);
    ret.set_auto_resize(true);
    ret += src0;
    MD_NAME_S_FUNCTION(&ret, src1, false);
    ret.set_auto_resize(src0.auto_resize());
    ret.set_auto_init(src0.auto_init());
    ret.set_rounding(src0.rounding());

    ret.set_scopy_flag();
    return ret;
}

}	/* namespace */
