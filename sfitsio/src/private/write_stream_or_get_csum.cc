/* buf の内容をストリームに書き出すか，内容についてのチェックサムを計算する */
/* sptr が NULL の場合，返り値は必ず len_buf になる */
static ssize_t write_stream_or_get_csum( const void *buf, size_t len_buf,
				    cstreamio *sptr, fitsio_csum *ret_suminfo )
{
    ssize_t return_value = len_buf;

    /* check 1st and 2nd args */
    if ( buf == NULL ) {
	if ( 0 < len_buf ) return -1;
	else return 0;
    }

    /* write buffer to stream, if sptr is set */
    if ( sptr != NULL ) {
	return_value = sptr->write(buf, len_buf);
    }

    /* obtain check sum */
    if ( ret_suminfo != NULL ) {
	const unsigned char *in_p = (const unsigned char *)buf;
	size_t len_dat = len_buf;
	unsigned long lo, hi;
	size_t n_fill;

	hi = ret_suminfo->hi;
	lo = ret_suminfo->lo;

	n_fill = FITS::FILE_RECORD_UNIT 
	       - (ret_suminfo->cnt % FITS::FILE_RECORD_UNIT);

	//err_report1(__FUNCTION__,"DEBUG","\nn_fill = %ld",(long)n_fill);

	while ( 0 < len_dat ) {
	    size_t nn, i;
	    bool flg;
	    if ( n_fill <= len_dat ) {
		nn = n_fill;
		flg = true;
	    }
	    else {
		nn = len_dat;
		flg = false;
	    }
	    n_fill = FITS::FILE_RECORD_UNIT;

	    //err_report1(__FUNCTION__,"DEBUG","nn = %ld",(long)nn);
	    if ( nn == FITS::FILE_RECORD_UNIT ) {
		const size_t mm = FITS::FILE_RECORD_UNIT / 32;
		for ( i=0 ; i < mm ; i++ ) {
		    hi += ((unsigned long)(in_p[0])) << 8;
		    hi += ((unsigned long)(in_p[1]));
		    lo += ((unsigned long)(in_p[2])) << 8;
		    lo += ((unsigned long)(in_p[3]));
		    hi += ((unsigned long)(in_p[4])) << 8;
		    hi += ((unsigned long)(in_p[5]));
		    lo += ((unsigned long)(in_p[6])) << 8;
		    lo += ((unsigned long)(in_p[7]));
		    hi += ((unsigned long)(in_p[8])) << 8;
		    hi += ((unsigned long)(in_p[9]));
		    lo += ((unsigned long)(in_p[10])) << 8;
		    lo += ((unsigned long)(in_p[11]));
		    hi += ((unsigned long)(in_p[12])) << 8;
		    hi += ((unsigned long)(in_p[13]));
		    lo += ((unsigned long)(in_p[14])) << 8;
		    lo += ((unsigned long)(in_p[15]));
		    in_p += 16;
		    hi += ((unsigned long)(in_p[0])) << 8;
		    hi += ((unsigned long)(in_p[1]));
		    lo += ((unsigned long)(in_p[2])) << 8;
		    lo += ((unsigned long)(in_p[3]));
		    hi += ((unsigned long)(in_p[4])) << 8;
		    hi += ((unsigned long)(in_p[5]));
		    lo += ((unsigned long)(in_p[6])) << 8;
		    lo += ((unsigned long)(in_p[7]));
		    hi += ((unsigned long)(in_p[8])) << 8;
		    hi += ((unsigned long)(in_p[9]));
		    lo += ((unsigned long)(in_p[10])) << 8;
		    lo += ((unsigned long)(in_p[11]));
		    hi += ((unsigned long)(in_p[12])) << 8;
		    hi += ((unsigned long)(in_p[13]));
		    lo += ((unsigned long)(in_p[14])) << 8;
		    lo += ((unsigned long)(in_p[15]));
		    in_p += 16;
		}
		ret_suminfo->cnt += 32 * mm;
		len_dat          -= 32 * mm;
		nn               -= 32 * mm;
	    }
	    for ( i=0 ; i < nn ; i++ ) {
		const size_t sw = (ret_suminfo->cnt % 4);
		unsigned long ch;
		ch = (unsigned long)(in_p[i]);
		/* big-endian rule */
		if ( sw == 0 )      hi += ch << 8;
		else if ( sw == 1 ) hi += ch;
		else if ( sw == 2 ) lo += ch << 8;
		else                lo += ch;
		/* count up */
		ret_suminfo->cnt ++;
	    }
	    in_p += nn;
	    len_dat -= nn;

	    //err_report1(__FUNCTION__,"DEBUG","rem = %d",
	    //		(int)(ret_suminfo->cnt % FITS::FILE_RECORD_UNIT));
	    //err_report1(__FUNCTION__,"DEBUG","flg = %d",(int)flg);

	    /* when (2880 * n == cnt), get csum */
	    if ( flg ) {
		unsigned long lo_carry, hi_carry, sum;

		/* fold carry-bit in */
		hi_carry = hi >> 16;
		lo_carry = lo >> 16;

		while ( hi_carry | lo_carry ) {
		    hi = (hi & 0xffff) + lo_carry;
		    lo = (lo & 0xffff) + hi_carry;
		    hi_carry = hi >> 16;
		    lo_carry = lo >> 16;
		}

		sum = (hi << 16) + lo;
		ret_suminfo->sum = sum;
	    }
	}

	/* push hi and lo back */
	ret_suminfo->hi = hi;
	ret_suminfo->lo = lo;
    }

    return return_value;
}
