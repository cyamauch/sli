size_t header_format_a_record( const fits_header_record &src_record,
			       tstring *result )
{
    size_t size_all = 0;

    if ( result != NULL ) *result = NULL;

    //if ( index < 0 && target_header->length() <= index ) goto quit;

    //if ( c_strlen(src_record.keyword()) == 0 ) goto quit;

    if ( src_record.status() == FITS::NORMAL_RECORD ) {
	tstring tmp_fmt;
	tstring tmp_str, tmp_str_mark;
	long width_val;
	int record_type;
	size_t value_begin, comment_begin, keyvalue_length;
	bool is_comment_exists;
	tstring str_continue;
	tstring tmp_buf(FITS::HEADER_RECORD_UNIT);	/* fixed-length mode */
	size_t i,j;
	
	//err_report1(__FUNCTION__,"DEBUG","keyword: [%s]",
	//	    src_record.keyword());
	//err_report1(__FUNCTION__,"DEBUG","value: [%s]",
	//	    src_record.value());

	/* tmp_str = "KEYWORDX= " */
	tmp_fmt.printf("%%-%ds= ",FITS::HEADER_KEYWORD_ALIGNED_LENGTH);
	tmp_str.assignf(tmp_fmt.cstr(),src_record.keyword());
	if ( tmp_str.length() < 
	     FITS::HEADER_KEYWORD_AND_VALUE_ALIGNED_LENGTH ) {
	    width_val = FITS::HEADER_KEYWORD_AND_VALUE_ALIGNED_LENGTH 
		- tmp_str.length();
	}
	else width_val = 0;

	record_type = src_record.type();

	value_begin = tmp_str.length();

	if ( record_type == FITS::STRING_T ) {
	    const char *tmp_val_p;
	    /* ���ΤޤޤȤäƤ��� */
	    tmp_val_p = src_record.value();

	    if ( width_val == 0 ) tmp_fmt.assign("%s");
	    else tmp_fmt.printf("%%-%lds",width_val);

	    /* tmp_str = "KEYWORDX= 'xxxx    '" */
	    tmp_str.appendf(tmp_fmt.cstr(), tmp_val_p);
	}
	else {
	    if ( width_val == 0 ) tmp_fmt.assign("%s");
	    else tmp_fmt.printf("%%%lds",width_val);
	    /* tmp_str = "KEYWORDX=         123" */
	    if ( record_type == FITS::ANY_T ) {
	        tmp_str.appendf(tmp_fmt.cstr(),"");
	    }
	    else {
	        tmp_str.appendf(tmp_fmt.cstr(),
				src_record.svalue());
	    }
	}

	if ( FITS::HEADER_KEYWORD_ALIGNED_LENGTH < 
	     src_record.keyword_cs().length() ) 
	    str_continue = "CONTINUE";
	else 
	    str_continue = "CONTINUE ";

	/* 
	 * �������顤CONTINUE �ν����򤹤�
	 * 
	 * ʸ���󡦥�����ʸ�� CONTINUE �Ǥ���³�򥵥ݡ��Ȥ���
	 * �����ޤ��֤���ñ��ñ�̤ǹԤʤ��褦�ˤ��Ƥ��뤿�ᡤ
	 * ���ʣ�������Ƥ��롥
	 *
	 */
	comment_begin = 0;
	is_comment_exists = ( src_record.comment() != NULL &&
			      0 < src_record.comment_cs().length() );
	if ( is_comment_exists ) {
	    if ( tmp_str.length() < FITS::HEADER_KEYWORD_AND_VALUE_ALIGNED_LENGTH ) {
		tmp_str.append(' ',FITS::HEADER_KEYWORD_AND_VALUE_ALIGNED_LENGTH - tmp_str.length());
	    }
	    comment_begin = tmp_str.length();
	    tmp_str.appendf(" / %s", src_record.comment());
	}

	if ( 0 < comment_begin ) keyvalue_length = comment_begin;
	else keyvalue_length = tmp_str.length();

	/* �����餸�� '' �ΰ��֤�ޡ������Ƥ��� */
	if ( record_type == FITS::STRING_T ) {
	    tmp_str_mark.resize(tmp_str.length());
	    for ( i=0 ; i < tmp_str.length() ; i++ ) {
		if ( value_begin < i && i+2 < keyvalue_length &&
		     tmp_str.cchr(i) == '\'' && tmp_str.cchr(i+1) == '\'' ) {
		    tmp_str_mark[i] = 'Q';	/* 'Q' �ǥޡ����� */
		    i++;
		}
	    }
	}

	j=0;
	for ( i=0 ; i < tmp_str.length() ; i++ ) {
	    bool in_comment;	/* i ��������ʸ������ˤ��뤫�ɤ��� */
	    if ( 0 < comment_begin && comment_begin <= i ) in_comment = true;
	    else in_comment = false;
	    /*
	     * ʸ����ξ��ǡ������ȤǤϤʤ���ʬ
	     */
	    if ( record_type == FITS::STRING_T && in_comment == false ) {
		/* �ͤ��ϤޤäƤ����("'"�Ͻ���) */
		if ( value_begin < i ) {
		    /* ��ť������ơ������ʸ�����ɤ�����Ƚ�� */
		    bool is_qo = (tmp_str_mark.cchr(i) == 'Q');
		    /* ����β��Ԥ��ɤ�����Ƚ�� */
		    bool is_cr = ( tmp_str.cchr(i) == '\\' && 
				   tmp_str.cchr(i+1) == 'n' &&
				  /* �Ǹ�β���ʸ���ˤĤ��Ƥ����̰������ʤ� */
				   i+3 < keyvalue_length );
		    /* ��ɶ��ڤ꤫�ɤ���Ƚ�� */
		    size_t next_blk_len = 0, next_word_pos = 0;
		    bool is_nw = ( tmp_str.cchr(i) == ' ' ||
				   tmp_str.cchr(i) == ',' ||
				   tmp_str.cchr(i) == ';' );
		    /* ��ɶ��ڤ���ä���硤ɬ�פ�ʸ�����Ĺ������� */
		    /* next_blk_len ���������ʸ����+����ʸ���� ��Ĺ�� */
		    if ( is_nw == true ) {
			bool found_last_quot = false;
			size_t next_word_len = tmp_str.strcspn(i+1," ,;\\");
			/* ,' */
			if ( keyvalue_length <= i+1 + next_word_len ) {
			    next_word_len = keyvalue_length - (i+1);
			    if ( 0 < next_word_len ) {
				next_word_len --;	/* for "'" */
				found_last_quot = true;
			    }
			}
			is_nw = ( 0 < next_word_len );
			if ( is_nw == true && found_last_quot != true ) {
			    next_blk_len =
				next_word_len + 
				tmp_str.strspn(i+1+next_word_len," ,;");
			    if ( keyvalue_length <= i+1 + next_blk_len ) {
				next_blk_len = keyvalue_length - (i+1);
				if ( 0 < next_blk_len ) next_blk_len --;
			    }
			    next_word_pos = i + 1+next_blk_len;
			    //err_report1(__FUNCTION__,"DEBUG",
			    //		"next_blk_len: [%zd]", next_blk_len);
			}
		    }
		    /* ü�ä��˲��Ԥ��褿���ν��� */
		    /*        ...&' */
		    /*        ..\&' ���ʲ��ǤϤ����ػ� */
		    /*        .\n&' */
		    if ( j+3 == FITS::HEADER_RECORD_UNIT && is_cr == true ) {
			tmp_buf.put(j, "&' ");
			j += 3;
			i--;
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			tmp_buf.assign(str_continue).append(" '");
			j = tmp_buf.length();
		    }
		    /* �̾�� '&' �ν��� */
		    else if ( j+2 == FITS::HEADER_RECORD_UNIT ) {
			bool flg_finished = false;
			if ( i+2 == tmp_str.length() ) {	/* finish */
			    tmp_buf[j] = tmp_str.cchr(i);
			    j++;  i++;
			    tmp_buf[j] = tmp_str.cchr(i);
			    j++;
			    flg_finished = true;
			}
			else if ( i+1 == tmp_str.length() ) {	/* finish */
			    tmp_buf[j] = tmp_str.cchr(i);
			    j++;
			    tmp_buf[j] = ' ';
			    j++;
			    flg_finished = true;
			}
			else {
			    tmp_buf.put(j, "&'");
			    j += 2;
			    i--;
			}
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			j = 0;
			if ( flg_finished == false ) {
			    /* ��Ƭ�� CONTINUE ��� */
			    tmp_buf.assign(str_continue).append(" '");
			    j = tmp_buf.length();
			}
		    }
		    /*
		     * "''" �ν���
		     *                                               654321
		     * ����Ͻ񤱤ʤ��Τ��ޤ��֤���                  XXX''
		     *                                               XX''&' 
		     */
		    else if ( is_qo == true && 
			      j+3 == FITS::HEADER_RECORD_UNIT ) {
			tmp_buf.put(j, "&' ");
			j += 3;
			i--;
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			j = 0;
			/* ��Ƭ�� CONTINUE ��� */
			tmp_buf.assign(str_continue).append(" '");
			j = tmp_buf.length();
		    }
		    /*
		     * ����ʸ���ξ��ν���
		     */
		    else if ( is_cr == true ) {
			tmp_buf.put(j, "\\n&'");
			j += 4;
			/* ���򥦥ᥦ�� */
			for ( ; j < FITS::HEADER_RECORD_UNIT ; j++ ) 
			    tmp_buf[j] = ' ';
			i++;
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			tmp_buf.assign(str_continue).append(" '");
			j = tmp_buf.length();
		    }
		    /* 
		     * ʸ����ν�ü�ˤ����ơ��Ǥ��������������ʬ�� CONTINUE
		     * ��Ȥ�ʤ��褦�ˤ������
		     */
		    else if ( 0 < comment_begin && i+1 == comment_begin &&
			      tmp_str[i] == '\'' &&
			      j+2 <= FITS::HEADER_RECORD_UNIT &&
			      FITS::HEADER_RECORD_UNIT - j <= 
			      tmp_str.length() - comment_begin &&
		       tmp_str.length() - comment_begin <= 
		       FITS::HEADER_RECORD_UNIT - str_continue.length() - 3 ) {
			tmp_buf.put(j, "&'");
			j += 2;
			/* ���򥦥ᥦ�� */
			for ( ; j < FITS::HEADER_RECORD_UNIT ; j++ ) 
			    tmp_buf[j] = ' ';
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			/* ��Ƭ�� CONTINUE ��ʸ����ν�ü��� */
			tmp_buf.assign(str_continue).append(" ''");
			j = tmp_buf.length();
		    }
		    /* 
		     * �����Ȥ�¸�ߤ����硤
		     *   KEYWORD   = '..................................&'
		     *   CONTINUE    '' / long comment 
		     * �Ȥ����ѥ�����ˤ��롥
		     */
		    else if ( 0 < comment_begin && i+1 == comment_begin &&
			      tmp_str[i] == '\'' &&
			      ( j+2 == FITS::HEADER_RECORD_UNIT ||
				j+3 == FITS::HEADER_RECORD_UNIT ||
				j+4 == FITS::HEADER_RECORD_UNIT ) ) {
			tmp_buf.put(j, "&'");
			j += 2;
			/* ���򥦥ᥦ�� */
			for ( ; j < FITS::HEADER_RECORD_UNIT ; j++ ) 
			    tmp_buf[j] = ' ';
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			/* ��Ƭ�� CONTINUE ��ʸ����ν�ü��� */
			tmp_buf.assign(str_continue).append(" ''");
			j = tmp_buf.length();
		    }
		    /*
		     * ��ɤǶ��ڤ����
		     */
		    else if ( is_nw == true && 
			     FITS::HEADER_RECORD_UNIT <= j + 1+next_blk_len + 1
			    ) {
			tmp_buf[j] = tmp_str.cchr(i);
			j++;
			tmp_buf.put(j, "&'");
			j += 2;
			/* ���򥦥ᥦ�� */
			for ( ; j < FITS::HEADER_RECORD_UNIT ; j++ ) 
			    tmp_buf[j] = ' ';
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			/* ��Ƭ�� CONTINUE ��� */
			tmp_buf.assign(str_continue).append(" '");
			j = tmp_buf.length();
		    }
		    /* ü�ä��ǤϤʤ���� */
		    else {
			tmp_buf[j] = tmp_str.cchr(i);
			j++;
		    }
		}
		/* �ͤ��ϤޤäƤ��ʤ���(��Ƭ����"'"�ޤ�) */
		else {
		    tmp_buf[j] = tmp_str.cchr(i);
		    j++;
		}
	    }
	    /*
	     * ʸ����ʳ��ξ�礫��������ʬ
	     */
	    else {
		/* �ͤ��ϤޤäƤ����("'"�Ͻ���) */
		if ( value_begin <= i ) {
		    size_t ch_spn = 0, sp_spn = 0, sp_off = 0;
		    /*
		     * ������ʸ����a very   long comment�פΤȤ�
		     *   KEYWORD   = ................................ / a very
		     *   CONTINUE  /   long comment 
		     * �Τ褦�ˤʤ�ʤ��褦�ˤ��뤿��ν���:
		     * ���Τ褦����¸����褦�ˤ��롥
		     *   KEYWORD   = ................................ / a
		     *   CONTINUE  / very   long comment 
		     */
		    if ( 0 < i && tmp_str.cchr(i-1) == ' ' ) {
			ch_spn = tmp_str.strcspn(i,' ');
			if ( 0 < ch_spn ) sp_spn = tmp_str.strspn(i + ch_spn,' ');
			if ( 0 < sp_spn ) sp_off = 1;
		    }
		    if ( 0 < ch_spn &&
			 FITS::HEADER_RECORD_UNIT < j + ch_spn + sp_spn ) {
			/* ���򥦥ᥦ�� */
			for ( ; j < FITS::HEADER_RECORD_UNIT ; j++ ) 
			    tmp_buf[j] = ' ';
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			/* ��Ƭ�� CONTINUE ��� */
			tmp_buf.assign(str_continue)
			       .append(" / ").append(tmp_str.cchr(i),1);
			j = tmp_buf.length();
		    }
		    /* ü�ä��ν��� */
		    else if ( j+1 == FITS::HEADER_RECORD_UNIT ) {
			bool flg_finished = false;
			if ( i+1 == tmp_str.length() ) {	/* finish */
			    tmp_buf[j] = tmp_str.cchr(i);
			    j++;
			    flg_finished = true;
			}
			else {
			    tmp_buf[j] = tmp_str.cchr(i);
			    j++;
			}
			if ( result != NULL ) result->append(tmp_buf);
			size_all += FITS::HEADER_RECORD_UNIT;
			j = 0;
			if ( flg_finished == false ) {
			    /* ��Ƭ�� CONTINUE ��� */
			    tmp_buf.assign(str_continue).append(" / ");
			    j = tmp_buf.length();
			}
		    }
		    /* ü�ä��ǤϤʤ���� */
		    else {
			tmp_buf[j] = tmp_str.cchr(i);
			j++;
		    }
		}
		/* �ͤ��ϤޤäƤ��ʤ��� */
		else {
		    tmp_buf[j] = tmp_str.cchr(i);
		    j++;
		}
	    }
	}	/* for ( i=0 ; i < tmp_str.length() ; i++ ) ... */

	if ( 0 < j ) {
	    /* ���򥦥ᥦ�� */
	    for ( ; j < FITS::HEADER_RECORD_UNIT ; j++ ) tmp_buf[j] = ' ';
	    if ( result != NULL ) result->append(tmp_buf);
	    size_all += FITS::HEADER_RECORD_UNIT;
	}

    }
    else if ( src_record.status() == FITS::DESCRIPTION_RECORD ) {
	/* description �����ξ�� */
	/* description ������Ĺ�����ϡ����ڡ����Τ���Ȥ���Ƕ��ڤä� */
	/* ʣ���Ԥˤ櫓�� record ��������� */
	tstring tmp_str;
	tmp_str.assignf("%s %s",src_record.keyword(),
			src_record.value());
	while ( true ) {
	    char tmp_buf[FITS::HEADER_RECORD_UNIT + 1];
	    tmp_buf[FITS::HEADER_RECORD_UNIT] = '\0';
	    /* 80ʸ������ʤ鴰λ */
	    if ( tmp_str.cchr(FITS::HEADER_RECORD_UNIT) <= 0 ) {	
		size_t i;
		const char *tmp_str_ptr = tmp_str.cstr();
		size_t tmp_str_len = tmp_str.length();
		for ( i=0 ; i < tmp_str_len ; i++ ) 
		    tmp_buf[i] = tmp_str_ptr[i];
		for ( ; i < FITS::HEADER_RECORD_UNIT ; i++ ) 
		    tmp_buf[i] = ' ';
		if ( result != NULL ) {
		    result->append(tmp_buf);
		}
		size_all += FITS::HEADER_RECORD_UNIT;
		break;
	    }
	    /* 80ʸ����ۤ���Ȥ���˲�������С�ʬ�䤹��ɬ�פ����� */
	    else {
		size_t next_pos;
		if ( tmp_str.cchr(FITS::HEADER_RECORD_UNIT) != ' ' ) {
		    size_t i = FITS::HEADER_RECORD_UNIT - 1;
		    size_t len_keyword = 
			src_record.keyword_cs().length();
		    const char *tmp_str_ptr = tmp_str.cstr();
		    /* �������� ' ' ������Ȥ��򸫤Ĥ��� */
		    for ( ; len_keyword + 1 < i ; i-- ) {
			if ( tmp_str_ptr[i] == ' ' ) break;
		    }
		    if ( len_keyword + 1 == i ) {
			/* ���Ĥ���ʤ����� */
			next_pos = FITS::HEADER_RECORD_UNIT;
		    }
		    else {
			next_pos = i + 1;
		    }
		    for ( i=0 ; i < next_pos ; i++ ) 
			tmp_buf[i] = tmp_str_ptr[i];
		    for ( ; i < FITS::HEADER_RECORD_UNIT ; i++ )
			tmp_buf[i] = ' ';
		    if ( result != NULL ) {
		        result->append(tmp_buf);
		    }
		    size_all += FITS::HEADER_RECORD_UNIT;
		}
		else {	/* == ' ' */
		    const char *tmp_str_ptr = tmp_str.cstr();
		    size_t tmp_str_len = tmp_str.length();
		    size_t i = FITS::HEADER_RECORD_UNIT;
		    for ( ; i < tmp_str_len ; i++ ) {
			if ( tmp_str_ptr[i] != ' ' ) break;
		    }
		    if ( result != NULL ) {
		        result->append(tmp_str_ptr,
				       FITS::HEADER_RECORD_UNIT);
		    }
		    size_all += FITS::HEADER_RECORD_UNIT;
		    if ( i == tmp_str_len ) break;
		    next_pos = i;
		}
		tmp_str.assignf("%s %s",
				src_record.keyword(),
				tmp_str.cstr() + next_pos);
	    }
	}
    }
    else {	/* NULL RECORD �ξ�� */
	if ( result != NULL ) {
	    result->append(' ',FITS::HEADER_RECORD_UNIT);
	}
	size_all += FITS::HEADER_RECORD_UNIT;
    }

    return size_all;
}
