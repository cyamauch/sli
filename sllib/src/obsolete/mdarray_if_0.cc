

#if 0
static size_t abs_sz2z( ssize_t val )
{
    ssize_t ret;
    if ( val < 0 ) ret = -val;
    else ret = val;
    return (size_t)ret;
}
#endif


#if 0
/** 
 * obj������Ĺ,���ʤɤ����Ƥ򡢼��Ȥ˥��ԡ����ޤ���
 * 
 * @param obj mdarray���֥�������
 * @param dim_idx �����ֹ�
 * @param len_src obj�����ǿ�
 * @param len_dest ���Ȥ����ǿ�
 * @param pos_src �ǡ����ɤ߹��߰���
 * @param pos_dest �ǡ����񤭹��߰���
 * @param func_src2dest ���Ѵ��˻Ȥ���ؿ��ؤΥ��ɥ쥹
 * @param f_idx 1:�ͼθ������Ѵ�
 *           2:�ڼΤƤ��Ѵ�
 * @return ���Ȥλ���
 * @throw �����˲��򵯤������Ȥ�
 * @note ���Υ��дؿ���private�Ǥ�
 */
/* ���δؿ��ϺƵ�Ū�˸ƤФ�� */
mdarray &mdarray::r_equal( const mdarray &obj, size_t dim_idx,
		      size_t len_src, size_t len_dest,
		      size_t pos_src, size_t pos_dest,
		      void (*func_src2dest)(const void *,void *,size_t,void *),
		      size_t f_idx )
{
    size_t len;
    len = this->length(dim_idx);
    if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_equal(obj, dim_idx-1, len_src, len_dest,
			  pos_src + len_src * i, pos_dest + len_dest * i,
			  func_src2dest, f_idx);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->arr_rec) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	if ( func_src2dest != NULL ) {
	    (*func_src2dest)((const void *)src, (void *)dest, len, (void *)&f_idx);
	}
	else {	/* operator=() �� dest_bytes == src_bytes ���ݾڤ���Ƥ��� */
	    c_memcpy((void *)dest, (const void *)src, dest_bytes * len);
	}
    }
    return *this;
}
#endif


#if 0
/**
 * ���֥������Ȥ���������obj �����Ƥ򤹤٤�(����Ĺ�����ʤ�) ���Ȥ˥��ԡ����ޤ���
 *
 * @param     obj mdarray���饹�Υ��֥�������
 * @return    ���Ȥλ���
 * @throw     �����Хåե��γ��ݤ˼��Ԥ������
 * @throw     �����˲��򵯤��������
 */
mdarray &mdarray::operator=(const mdarray &obj)
{
    void (*func_src2dest)(const void *,void *,size_t,void *);

    if ( &obj == this ) return *this;

    if ( this->auto_resize_rec == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->dim_length() == 0 ) return *this;
    if ( this->data_ptr_cs() == NULL ) return *this;
    if ( obj.data_ptr_cs() == NULL ) return *this;

    func_src2dest = NULL;

    /* ���Ѵ��Τ���δؿ������� */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && this->sz_type_rec == new_sz_type ) { \
        func_src2dest = &cnv_n::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ������뤫��1���ǤΥХ��ȿ������פ����������Ԥʤ� */
    if ( func_src2dest != NULL || this->bytes() == obj.bytes() ) {
	size_t len_src, len_dest, ndim;
	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */
	size_t f_idx;
	if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_equal(obj, ndim-1, len_src, len_dest, 0, 0,
		      func_src2dest, f_idx);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}
#endif

#if 0
/* ���δؿ��ϺƵ�Ū�˸ƤФ�� */
/**
 * @brief mdarray �βû�����
 *
 * 1.����!=0�ʤ�С��Ƶ�Ū�ˤ��δؿ���ƽФ���
 *
 * 2.src ���ͤ� double ���Ѵ����롣(=tmp0)
 *
 * 3.dest ���ͤ� double ���Ѵ����롣(=tmp1)
 *
 * 4.dest ���ͤ� src ���ͤ�û����롣
 *
 * 5.dest ���ͤ�����η����᤹��
 *
 * 6.2����5�ޤǤ����Ǥο����������֤���
 *
 * @attension  ���ߤ�̤����
 * 
 * @param      obj �û��оݤ� mdarray
 * @return     ���Ȥλ���
 */
mdarray &mdarray::r_plus_equal(const mdarray &obj, size_t dim_idx,
			       size_t len_src, size_t len_dest,
			       size_t pos_src, size_t pos_dest,
			       void (*func_src2d)(const void *,void *,void *),
			       void (*func_dest2d)(const void *,void *,void *),
			       void (*func_d2dest)(const void *,void *,void *),
			       size_t f_idx)
{
    size_t len;
    len = this->length(dim_idx);
    if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_plus_equal(obj, dim_idx-1, len_src, len_dest,
			       pos_src + len_src * i, pos_dest + len_dest * i,
			       func_src2d, func_dest2d, func_d2dest, f_idx);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	double tmp0, tmp1;
	size_t i;
	for ( i=0 ; i < len ; i++ ) {
	    (*func_src2d)((const void *)src,  (void *)&tmp0, (void *)&f_idx);
	    (*func_dest2d)((const void *)dest, (void *)&tmp1, (void *)&f_idx);
	    tmp1 += tmp0;
	    (*func_d2dest)((const void *)&tmp1, (void *)dest, (void *)&f_idx);
	    src += src_bytes;
	    dest += dest_bytes;
	}
    }
    return *this;
}

/**
 * @brief �黻�� += �ζ���ư��
 *
 * 1.�����ȥꥵ�����ξ�硢Ϳ����줿 mdarray �����ǿ��˹�碌�ơ�
 *   ���ǿ���ꥵ�������롣
 *
 * 2.���Ѵ��Τ���δؿ������򤹤롣(SEL_FUNC)
 *
 * 3.�û������δؿ���ƽФ���
 *
 * @attension  ���ߤ�̤����
 * 
 * @param      obj �û��оݤ� mdarray
 * @return     ���Ȥλ���
 */
mdarray &mdarray::ope_plus_equal(const mdarray &obj)
{
    void (*func_src2d)(const void *,void *,void *);
    void (*func_dest2d)(const void *,void *,void *);
    void (*func_d2dest)(const void *,void *,void *);

    if ( this->auto_resize_rec == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->dim_length() == 0 ) return *this;
    if ( this->data_ptr_cs() == NULL ) return *this;
    if ( obj.data_ptr_cs() == NULL ) return *this;

    func_src2d = NULL;
    func_dest2d = NULL;
    func_d2dest = NULL;

    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(double �� ���Ȥη�) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->sz_type_rec == new_sz_type ) { \
        func_d2dest = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_src2d != NULL && func_dest2d != NULL && func_d2dest != NULL ) {
	size_t len_src, len_dest, ndim;
	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */
	size_t f_idx;
	if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_plus_equal(obj, ndim-1, len_src, len_dest, 0, 0,
			   func_src2d, func_dest2d, func_d2dest, f_idx);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}
#endif


#if 0
/* ���δؿ��ϺƵ�Ū�˸ƤФ�� */
mdarray &mdarray::r_minus_equal(const mdarray &obj, size_t dim_idx,
			       size_t len_src, size_t len_dest,
			       size_t pos_src, size_t pos_dest,
			       void (*func_src2d)(const void *,void *,void *),
			       void (*func_dest2d)(const void *,void *,void *),
			       void (*func_d2dest)(const void *,void *,void *),
			       size_t f_idx)
{
    size_t len;
    len = this->length(dim_idx);
    if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_minus_equal(obj, dim_idx-1, len_src, len_dest,
				pos_src + len_src * i, pos_dest + len_dest * i,
				func_src2d, func_dest2d, func_d2dest, f_idx);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	double tmp0, tmp1;
	size_t i;
	for ( i=0 ; i < len ; i++ ) {
	    (*func_src2d)((const void *)src,  (void *)&tmp0, (void *)&f_idx);
	    (*func_dest2d)((const void *)dest, (void *)&tmp1, (void *)&f_idx);
	    tmp1 -= tmp0;
	    (*func_d2dest)((const void *)&tmp1, (void *)dest, (void *)&f_idx);
	    src += src_bytes;
	    dest += dest_bytes;
	}
    }
    return *this;
}

mdarray &mdarray::ope_minus_equal(const mdarray &obj)
{
    void (*func_src2d)(const void *,void *,void *);
    void (*func_dest2d)(const void *,void *,void *);
    void (*func_d2dest)(const void *,void *,void *);

    if ( this->auto_resize_rec == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->dim_length() == 0 ) return *this;
    if ( this->data_ptr_cs() == NULL ) return *this;
    if ( obj.data_ptr_cs() == NULL ) return *this;

    func_src2d = NULL;
    func_dest2d = NULL;
    func_d2dest = NULL;

    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(double �� ���Ȥη�) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->sz_type_rec == new_sz_type ) { \
        func_d2dest = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_src2d != NULL && func_dest2d != NULL && func_d2dest != NULL ) {
	size_t len_src, len_dest, ndim;
	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */
	size_t f_idx;
	if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_minus_equal(obj, ndim-1, len_src, len_dest, 0, 0,
			    func_src2d, func_dest2d, func_d2dest, f_idx);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}
#endif


#if 0
/* ���δؿ��ϺƵ�Ū�˸ƤФ�� */
mdarray &mdarray::r_multiply_equal(const mdarray &obj, size_t dim_idx,
			       size_t len_src, size_t len_dest,
			       size_t pos_src, size_t pos_dest,
			       void (*func_src2d)(const void *,void *,void *),
			       void (*func_dest2d)(const void *,void *,void *),
			       void (*func_d2dest)(const void *,void *,void *),
			       size_t f_idx)
{
    size_t len;
    len = this->length(dim_idx);
    if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_multiply_equal(obj, dim_idx-1, len_src, len_dest,
				pos_src + len_src * i, pos_dest + len_dest * i,
			        func_src2d, func_dest2d, func_d2dest, f_idx);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	double tmp0, tmp1;
	size_t i;
	for ( i=0 ; i < len ; i++ ) {
	    (*func_src2d)((const void *)src,  (void *)&tmp0, (void *)&f_idx);
	    (*func_dest2d)((const void *)dest, (void *)&tmp1, (void *)&f_idx);
	    tmp1 *= tmp0;
	    (*func_d2dest)((const void *)&tmp1, (void *)dest, (void *)&f_idx);
	    src += src_bytes;
	    dest += dest_bytes;
	}
    }
    return *this;
}

mdarray &mdarray::ope_star_equal(const mdarray &obj)
{
    void (*func_src2d)(const void *,void *,void *);
    void (*func_dest2d)(const void *,void *,void *);
    void (*func_d2dest)(const void *,void *,void *);

    if ( this->auto_resize_rec == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->dim_length() == 0 ) return *this;
    if ( this->data_ptr_cs() == NULL ) return *this;
    if ( obj.data_ptr_cs() == NULL ) return *this;

    func_src2d = NULL;
    func_dest2d = NULL;
    func_d2dest = NULL;

    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(double �� ���Ȥη�) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->sz_type_rec == new_sz_type ) { \
        func_d2dest = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_src2d != NULL && func_dest2d != NULL && func_d2dest != NULL ) {
	size_t len_src, len_dest, ndim;
	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */
	size_t f_idx;
	if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_multiply_equal(obj, ndim-1, len_src, len_dest, 0, 0,
			       func_src2d, func_dest2d, func_d2dest, f_idx);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}
#endif


#if 0
/* ���δؿ��ϺƵ�Ū�˸ƤФ�� */
mdarray &mdarray::r_divide_equal(const mdarray &obj, size_t dim_idx,
			       size_t len_src, size_t len_dest,
			       size_t pos_src, size_t pos_dest,
			       void (*func_src2d)(const void *,void *,void *),
			       void (*func_dest2d)(const void *,void *,void *),
			       void (*func_d2dest)(const void *,void *,void *),
			       size_t f_idx)
{
    size_t len;
    len = this->length(dim_idx);
    if ( obj.length(dim_idx) < len ) len = obj.length(dim_idx);
    if ( 0 < dim_idx ) {
	size_t i;
	len_src /= obj.length(dim_idx);
	len_dest /= this->length(dim_idx);
	for ( i=0 ; i < len ; i++ ) {
	    this->r_divide_equal(obj, dim_idx-1, len_src, len_dest,
				pos_src + len_src * i, pos_dest + len_dest * i,
				func_src2d, func_dest2d, func_d2dest, f_idx);
	}
    }
    else {	/* ����=0 �Ǥν��� */
	const size_t src_bytes = obj.bytes();
	const size_t dest_bytes = this->bytes();
	char *dest = (char *)(this->data_ptr()) + dest_bytes * pos_dest;
	const char *src =
	    (const char *)(obj.data_ptr_cs()) + src_bytes * pos_src;
	double tmp0, tmp1;
	size_t i;
	for ( i=0 ; i < len ; i++ ) {
	    (*func_src2d)((const void *)src,  (void *)&tmp0, (void *)&f_idx);
	    (*func_dest2d)((const void *)dest, (void *)&tmp1, (void *)&f_idx);
	    tmp1 /= tmp0;
	    (*func_d2dest)((const void *)&tmp1, (void *)dest, (void *)&f_idx);
	    src += src_bytes;
	    dest += dest_bytes;
	}
    }
    return *this;
}

mdarray &mdarray::ope_slash_equal(const mdarray &obj)
{
    void (*func_src2d)(const void *,void *,void *);
    void (*func_dest2d)(const void *,void *,void *);
    void (*func_d2dest)(const void *,void *,void *);

    if ( this->auto_resize_rec == true ) {
	size_t i;
	for ( i=0 ; i < obj.dim_length() ; i++ ) {
	    if ( this->length(i) < obj.length(i) ) {
		this->resize(i, obj.length(i));
	    }
	}
    }

    if ( this->dim_length() == 0 ) return *this;
    if ( this->data_ptr_cs() == NULL ) return *this;
    if ( obj.data_ptr_cs() == NULL ) return *this;

    func_src2d = NULL;
    func_dest2d = NULL;
    func_d2dest = NULL;

    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( obj.sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_src2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(����obj�η���double) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( this->sz_type_rec == org_sz_type && DOUBLE_ZT == new_sz_type ) { \
        func_dest2d = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC
    /* ���Ѵ��Τ���δؿ�������(double �� ���Ȥη�) */
#define SEL_FUNC(fncname,org_sz_type,org_type,new_sz_type,new_type,fnc) \
    if ( DOUBLE_ZT == org_sz_type && this->sz_type_rec == new_sz_type ) { \
        func_d2dest = &cnv::fncname; \
    }
    SLI__MDARRAY__DO_OPERATION_2TYPES_ALL(SEL_FUNC,,,,,,,,,,,,,,,,,,,,,,,,,,,,else);
#undef SEL_FUNC

    /* �ؿ��������������Ԥʤ� */
    if ( func_src2d != NULL && func_dest2d != NULL && func_d2dest != NULL ) {
	size_t len_src, len_dest, ndim;
	/* ��ư�����������������Ѵ�������ˤɤ����뤫 */
	size_t f_idx;
	if ( this->rounding_rec == true ) f_idx = 1;	/* �ͼθ������Ѵ� */
	else f_idx = 0;					/* �ڤ�ΤƤ��Ѵ� */

	/* �礭������Ȥ� */
	ndim = this->dim_length();
	if ( obj.dim_length() < ndim ) ndim = obj.dim_length();

	/* �ǽ�ϡ���������������ԥ�������� len_xxx �˥��åȤ��� */
	len_src = obj.length();
	len_dest = this->length();
	
	this->r_divide_equal(obj, ndim-1, len_src, len_dest, 0, 0,
			     func_src2d, func_dest2d, func_d2dest, f_idx);
    }
    else {
	err_report(__FUNCTION__,"WARNING","invalid size of type; ignored");
    }

    return *this;
}
#endif


#if 0
/* ����ϸŤ��С�����󡥥إå��ե�����˲����Ǥ����롥 */
/** 
 * ���ꤵ�줿���Ǥΰ��֤��֤�<br>
 * ������ϰϳ������Ǥ���ꤷ�����ꥵ������Ԥ�
 * @param ix0 1�����ܤ������ֹ�
 * @param ix1 2�����ܤ������ֹ�
 * @param ix2 3�����ܤ������ֹ�
 * @return �����������ϰ����ֹ�<br>
 *             �������������ʤ��Ȥ���-1
 * @throw �����Хåե��γ��ݤ˼��Ԥ������
 * @note ���Υ��дؿ���protected�Ǥ�
 */
ssize_t mdarray::get_idx_3d( ssize_t ix0, ssize_t ix1, ssize_t ix2 )
{
    ssize_t idx;
    if ( this->auto_resize_rec == true ) {
	/* auto resize ON */
	if ( this->dim_size_rec == 0 ) {
	    this->increase_dim();
	}
	if ( ix0 < 0 ) return -1;
	else {
	    size_t zix0 = abs_sz2z(ix0);
	    if ( ix1 == MDARRAY_INDEF ) {
		if ( this->cached_length_rec <= zix0 ) {
		    /* �ǹ⼡���μ���ꥵ�������� */
		    size_t i, blk = 1, nn, nlen = zix0;
		    size_t ndim = this->dim_size_rec;
		    for ( i=0 ; i+1 < ndim ; i++ ) blk *= this->size_rec[i];
		    nn = (nlen / blk) + 1;
		    this->resize(ndim-1, nn);
		}
	    }
	    else {
		if ( this->size_rec[0] <= zix0 ) this->resize(0,ix0+1);
	    }
	}
	idx = ix0;
	if ( ix1 == MDARRAY_INDEF ) goto quit;
	if ( ix1 < 0 ) return -1;
	else {
	    size_t zix1 = abs_sz2z(ix1);
	    if ( this->dim_size_rec == 1 ) {
		this->increase_dim();
	    }
	    if ( ix2 == MDARRAY_INDEF ) {
		if ( this->cached_layer_length_rec * this->size_rec[1] <= zix1 ) {
		    /* �ǹ⼡���μ���ꥵ�������� */
		    size_t i, blk = 1, nn, nlen = zix1;
		    size_t ndim = this->dim_size_rec;
		    for ( i=1 ; i+1 < ndim ; i++ ) blk *= this->size_rec[i];
		    nn = (nlen / blk) + 1;
		    this->resize(ndim-1, nn);
		}
	    }
	    else {
		if ( this->size_rec[1] <= zix1 ) this->resize(1,ix1+1);
	    }
	    idx += this->size_rec[0] * ix1;
	}
	if ( ix2 == MDARRAY_INDEF ) goto quit;
	if ( ix2 < 0 ) return -1;
	else {
	    size_t zix2 = abs_sz2z(ix2);
	    if ( this->dim_size_rec == 2 ) {
		this->increase_dim();
	    }
	    if ( this->cached_layer_length_rec <= zix2 ) {
		//this->resize(2,ix2+1);
		/* �ǹ⼡���μ���ꥵ�������� */
		size_t i, blk = 1, nn, nlen = zix2;
		size_t ndim = this->dim_size_rec;
		for ( i=2 ; i+1 < ndim ; i++ ) blk *= this->size_rec[i];
		nn = (nlen / blk) + 1;
		this->resize(ndim-1, nn);
	    }
	    idx += this->size_rec[0] * this->size_rec[1] * ix2;
	}
    quit:
	if ( this->arr_rec == NULL ) return -1;
	return idx;
    }
    else {					/* auto resize OFF */
	return this->get_idx_3d_cs(ix0,ix1,ix2);
    }
}
#endif
