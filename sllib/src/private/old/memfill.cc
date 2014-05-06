
/*
 * VERSION 1
 */

inline static void memfill( const void *src_pattern, size_t sz_bytes,
			    size_t len, void *buf )
{
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    register unsigned char *d_ptr = (unsigned char *)buf;
    size_t len_all = len;
    register size_t i, j;

    /*
     * ロジックを単純なコードで表現した場合:
     *
     *	    for ( i=0 ; i < len_all ; i++ ) {
     *		for ( j=0 ; j < sz_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += sz_bytes;
     *	    }
     */

    /* この 128, 512 は memcpy() のオーバヘッドの大きさでエイヤで決定 */
    if ( sz_bytes <= 128 ) {		/* sz_bytes is small */
	const size_t n_unit = 512 / sz_bytes;
	if ( len_all < n_unit ) {	/* data length is small */
	    for ( i=0 ; i < len_all ; i++ ) {
		for ( j=0 ; j < sz_bytes ; j++ ) {
		    d_ptr[j] = src_pat[j];
		}
		d_ptr += sz_bytes;
	    }
	}
	else {				/* data length is large */
	    const unsigned char *d_ptr0 = d_ptr;
	    for ( i=0 ; i < n_unit ; i++ ) {
		for ( j=0 ; j < sz_bytes ; j++ ) {
		    d_ptr[j] = src_pat[j];
		}
		d_ptr += sz_bytes;
	    }
	    len_all -= n_unit;
	    while ( n_unit <= len_all ) {
		memcpy(d_ptr, d_ptr0, sz_bytes * n_unit);
		d_ptr += sz_bytes * n_unit;
		len_all -= n_unit;
	    }
	    memcpy(d_ptr, d_ptr0, sz_bytes * len_all);
	}
    }
    else {				/* sz_bytes is large */
	const unsigned char *d_ptr0 = d_ptr;
	i = 0;
	if ( i < len_all ) {
	    for ( j=0 ; j < sz_bytes ; j++ ) {
		d_ptr[j] = src_pat[j];
	    }
	    d_ptr += sz_bytes;
	    i++;
	}
	for ( ; i < len_all ; i++ ) {
	    memcpy(d_ptr, d_ptr0, sz_bytes);
	    d_ptr += sz_bytes;
	}
    }

    return;
}

/*
 * VERSION 2
 */

inline static void memfill( const void *src_pattern, size_t sz_bytes,
			    size_t len, void *buf )
{
    const size_t ma_bytes = sizeof(size_t);
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    register unsigned char *d_ptr = (unsigned char *)buf;
    size_t len_all = len;
    register size_t i, j;

    /*
     * ロジックを単純なコードで表現した場合:
     *
     *	    for ( i=0 ; i < len_all ; i++ ) {
     *		for ( j=0 ; j < sz_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += sz_bytes;
     *	    }
     */

    if ( sz_bytes <= 128 ) {				/* sz_bytes is small */
	const size_t n_unit = 1024 / sz_bytes;
	const size_t len_i = ((len_all < n_unit) ? len_all : n_unit);
	/* 先頭の部分を埋める(len_allが小さい場合はここだけで終わり) */
	if ( sz_bytes < ma_bytes ) {
	    for ( i=0 ; i < len_i ; i++ ) {
		for ( j=0 ; j < sz_bytes ; j++ ) d_ptr[j] = src_pat[j];
		d_ptr += sz_bytes;
	    }
	}
	else {
	    for ( i=0 ; i < len_i ; i++ ) {
		const unsigned char *s_ptr = src_pat;
		j = sz_bytes;
		while ( ma_bytes <= j ) {
		    j -= ma_bytes;
		    memcpy(d_ptr, s_ptr, ma_bytes);
		    s_ptr += ma_bytes;  d_ptr += ma_bytes;
		}
		while ( 0 < j ) {
		    j--;
		    *d_ptr = *s_ptr;
		    s_ptr ++;  d_ptr ++;
		}
	    }
	}
	len_all -= len_i;
	/* 続く部分を n_unit 個ずつ高速memcpy()で埋める */
	while ( n_unit <= len_all ) {
	    memcpy(d_ptr, buf, sz_bytes * n_unit);
	    d_ptr += sz_bytes * n_unit;
	    len_all -= n_unit;
	}
	/* 残りの部分を埋める */
	memcpy(d_ptr, buf, sz_bytes * len_all);
    }
    else {						/* sz_bytes is large */
	for ( i=0 ; i < len_all ; i++ ) {
	    const unsigned char *s_ptr = src_pat;
	    j = sz_bytes;
	    while ( ma_bytes <= j ) {
		j -= ma_bytes;
		memcpy(d_ptr, s_ptr, ma_bytes);
		s_ptr += ma_bytes;  d_ptr += ma_bytes;
	    }
	    while ( 0 < j ) {
		j--;
		*d_ptr = *s_ptr;
		s_ptr ++;  d_ptr ++;
	    }
	}
    }

    return;
}

