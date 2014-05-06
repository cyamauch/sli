static bool is_continue_record( const fits_header_record &ref )
{
    if ( ref.status() == FITS__DESCRIPTION_RECORD &&
	 ref.keyword_cs().strcmp("CONTINUE") == 0 &&
	 1 <= ref.value_cs().length() ) return true;
    else return false;
}

static int expand_continue_records( fits_header *target_header )
{
    long j;
    //fprintf(stderr,"debug: expanding CONTINUE\n");

    /*
     * SFITSIO 'CONTINUE' convension:
     *
     * - Basic Convension:
     *
     *     FOO      = 'aaaaaaaaaaaa & '
     *     CONTINUE   'bbbbbbbbbbb&' / this is 
     *     CONTINUE   '' / a looooooooooooooooong
     *     CONTINUE   / comment.
     *
     *  - NOTE
     *
     *   Most easy way to generate header records with long string + long
     *   comment is that we reserve byte 79-80 for "&'" string in a 
     *   header record.  We show an example:
     *
     *                                    70        80
     *   12345678901234567890 ...         01234567890
     *   KEYWORD = '......... ... .................&'
     *   CONTINUE  '......... ... .................&'
     *   CONTINUE  '......... ... .................&'
     *   CONTINUE  / this is a comment.
     *
     *   Note that "''" should not be put on 78th column:
     *
     *   CONTINUE  '......... ... ................'&'       <= No good
     *   CONTINUE  ''........ ... .................&'       <= No good
     *
     */

    for ( j=1 ; j < target_header->length() ; j++ ) {

	if ( is_continue_record(target_header->record(j)) ) {

	    const long j0 = j - 1;			/* master record */
	    long n_erase;
	    bool prev_value_amp_exists = false;
	    bool prev_comment_amp_exists = false;
	    tstring long_value;
	    tstring long_comment;
	    const tstring &v0_ref = target_header->record(j0).value_cs();
	    /* 右側の空白は除去されている */
	    const tstring &c0_ref = target_header->record(j0).comment_cs();
	    size_t v0_len = v0_ref.length();
	    size_t c0_len = c0_ref.length();

	    //err_report1(__FUNCTION__,"DEBUG","key = [%s]",
	    //	    target_header->record(j0).keyword());
	    //err_report1(__FUNCTION__,"DEBUG","val = [%s]", v0_ref.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","com = [%s]", c0_ref.cstr());

	    /* まずは，v0 を処理(最後の "'" をつけないで代入) */
	    if ( target_header->record(j0).type() == FITS::STRING_T ) {
		size_t s_pos;
		/*
		 * 値
		 */
		/* 2文字以下の場合は，1文字だけ採用するだけ */
		if ( v0_len <= 2 ) {
		    long_value.append(v0_ref,0,1);
		}
		/* 3文字以上の場合は，最後の "'"," ","&" の処理が必要 */
		else {
		    size_t r_spn;
		    s_pos = v0_len - 1;
		    /* 最後の '&' をチェック(これは認めるべきではないが) */
		    if ( v0_ref.cchr(s_pos) == '&' ) {
			s_pos --;
		    }
		    /* スペースをチェック */
		    r_spn = v0_ref.strrspn(s_pos,' ');
		    s_pos -= r_spn;
		    /* 最後のクォーテーションをチェック */
		    if ( v0_ref.cchr(s_pos) == '\'' ) {
			s_pos --;
		    }
		    /* さらに内側のスペースもチェック */
		    r_spn = v0_ref.strrspn(s_pos,' ');
		    s_pos -= r_spn;
		    /* 最後に & をチェック */
		    if ( v0_ref.cchr(s_pos) == '&' ) {
			prev_value_amp_exists = true;
			s_pos --;
		    }
		    long_value.append(v0_ref, 0, s_pos + 1);
		}
		/*
		 * コメント
		 */
		/* 最後の '&' をチェック */
		s_pos = c0_len - 1;
		//if ( c0_ref.cchr(s_pos) == '&' ) {
		//    s_pos --;
		//    prev_comment_amp_exists = true;
		//}
		long_comment.append(c0_ref, 0, s_pos + 1);
	    }
	    else {
		size_t s_pos;
		long_value.append(v0_ref);
		/* 最後の '&' をチェック */
		s_pos = c0_len - 1;
		//if ( c0_ref.cchr(s_pos) == '&' ) {
		//    s_pos --;
		//    prev_comment_amp_exists = true;
		//}
		long_comment.append(c0_ref, 0, s_pos + 1);
	    }

	    //err_report1(__FUNCTION__,"DEBUG","l_v = [%s]",long_value.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","l_c = [%s]",long_comment.cstr());

	    /* 続く CONTINUE 文を全部やってしまう */
	    /* (処理済みの CONTINUE レコードは削除する) */
	    n_erase = 0;
	    while ( j < target_header->length() &&
		    is_continue_record(target_header->record(j)) == true ) {
		bool curr_comment_amp_exists = false;
		/* ナマの値．ここでは左右の空白はそのまま */
		const tstring &vref = target_header->record(j).value_cs();
		size_t l_spn, r_spn;
		bool next_is_continue;
		/* 次も CONTINUE かどうか */
		next_is_continue = ( j+1 < target_header->length() &&
				     is_continue_record(target_header->record(j+1)) == true );
		/* 左右のスペースをチェック */
		l_spn = vref.strspn(' ');
		if ( vref.length() == l_spn ) r_spn = 0;
		else {
		    r_spn = vref.strrspn(' ');
		    /* 次も CONTINUE の場合は '&' をチェック */
		    //if ( next_is_continue ) {
		    //	if (vref.cchr(vref.length() - r_spn - 1) == '&') {
		    //	    r_spn ++;
		    //	    curr_comment_amp_exists = true;
		    //	}
		    //}
		}
		
		if (target_header->record(j0).type() == FITS::STRING_T) {
		    size_t s_pos = l_spn;
		    /* 前から順に "'" を探していく */
		    if ( prev_value_amp_exists == true &&
			 vref.cchr(s_pos) == '\'' ) {
			size_t v_begin;	/* 文字列値の開始点 */
			s_pos ++;
			v_begin = s_pos;
			while ( 1 ) {
			    ssize_t f_pos;
			    if ( (f_pos=vref.find(s_pos,'\'')) < 0 ) {
				size_t v_len;
				/* 終端の "'" が無い場合は，全部値にする */
				v_len = vref.length() - v_begin;
				/* 最後のスペースの消去 */
				if ( r_spn <= v_len ) v_len -= r_spn;
				/* 最後に "&" があったらそのままにしとく */
				if ( curr_comment_amp_exists == true ) v_len ++;
				long_value.append(vref, v_begin, v_len);
				prev_value_amp_exists = false;
				break;
			    }
			    if ( vref.cchr(f_pos+1) == '\'' ) {
				/* 「''」のための処理 */
				s_pos = f_pos+2;
				/* while() へ戻る */
			    }
			    else {
				/* comment 文字列を探す */
				/* "/" or '&' を探して，ダメなら再度
				   while() に戻る */
				s_pos = f_pos + 1;
				/* '&' も探すが，本来は来てはいけない*/
				s_pos += vref.strspn(s_pos,' ');
				if ( s_pos == vref.length() ||
				     vref.cchr(s_pos) == '/' ||
				     vref.cchr(s_pos) == '&' ) {
				    /* ようやく合格 */
				    size_t s_spn, v_len, c_len;
				    if ( s_pos < vref.length() ) {
					s_pos++;
					s_pos += vref.strspn(s_pos,' ');
				    }
				    /* register comment */
				    c_len = vref.length() - s_pos;
				    /* 最後のスペースと '&' の消去 */
				    if ( r_spn <= c_len ) c_len -= r_spn;
				    if ( 0 < c_len ) {
					if ( 0 < long_comment.length() &&
					     prev_comment_amp_exists == false ) {
					    long_comment.append(" ");
					}
					long_comment.append(vref, 
							    s_pos, c_len);
				    }
				    /* register value */
				    s_spn = vref.strrspn(f_pos-1,' ');
				    /* 次も CONTINUE の場合は '&' をチェック */
				    prev_value_amp_exists = false;  /* reset */
				    if ( next_is_continue ) {
					if ( vref.cchr(f_pos-s_spn-1) == '&') {
					    prev_value_amp_exists = true;
					    s_spn++;
					}
				    }
				    v_len = f_pos - s_spn - v_begin;
				    long_value.append(vref, v_begin,v_len);
				    break;
				}
				s_pos = f_pos+1;
				/* while() へ戻る */
			    }
			}
		    }
		    else {
			size_t c_len;
			/* 文字列と判定できない場合はコメントとして処理 */
			c_len = vref.length() - l_spn - r_spn;
			if ( 0 < c_len ) {
			    if ( 0 < long_comment.length() &&
			    	 prev_comment_amp_exists == false ) {
			    	long_comment.append(" ");
			    }
			    /* 先頭に「/」があれば除去 */
			    if ( 2 <= c_len && vref.cchr(l_spn) == '/' && 
				 vref.cchr(l_spn + 1) == ' ' ) {
				long_comment.append(vref, l_spn+2, c_len-2);
			    }
			    else if ( 1 <= c_len && vref.cchr(l_spn) == '/' ) {
				long_comment.append(vref, l_spn+1, c_len-1);
			    }
			    else {
				long_comment.append(vref, l_spn, c_len);
			    }
			}
		    }
		}
		else {
		    size_t c_len;
		    /* 文字列型以外の場合は単にコメントとして処理 */
		    c_len = vref.length() - l_spn - r_spn;
		    if ( 0 < c_len ) {
			if ( 0 < long_comment.length() &&
			     prev_comment_amp_exists == false ) {
			    long_comment.append(" ");
			}
			/* 先頭に「/」があれば除去 */
			if ( 2 <= c_len && vref.cchr(l_spn) == '/' && 
			     vref.cchr(l_spn + 1) == ' ' ) {
			    long_comment.append(vref, l_spn+2, c_len-2);
			}
			else if ( 1 <= c_len && vref.cchr(l_spn) == '/' ) {
			    long_comment.append(vref, l_spn+1, c_len-1);
			}
			else {
			    long_comment.append(vref, l_spn, c_len);
			}
		    }
		}

		/* count CONTINUE records to erase */
		n_erase ++;
		j ++ ;

		prev_comment_amp_exists = curr_comment_amp_exists;

	    }	/* while ( ) ... */

	    /* 文字列値の場合は，最後の "'" を追加 */
	    if ( target_header->record(j0).type() == FITS::STRING_T ) {
		long_value.append("'");
	    }
	    
	    //err_report1(__FUNCTION__,"DEBUG","length of long_value = [%d]",
	    //			(int)(long_value.length()));
	    //err_report1(__FUNCTION__,"DEBUG","long_value = [%s]",
	    //			    long_value.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","long_comment = [%s]",
	    //			    long_comment.cstr());
	    //err_report1(__FUNCTION__,"DEBUG","%s","========");
	    
	    /* register ! */
	    target_header->assignf_comment(j0, "%s",long_comment.cstr());
	    target_header->assignf_value(j0, "%s", long_value.cstr());

	    /* erase CONTINUE records */
	    target_header->erase_records(j0 + 1, n_erase);

	    /* important! */
	    j = j0;
	    
	}	/* if ( is_continue_record(...) ) ... */
    }
    
    return 0;
}
