static int parse_a_header_record( const tstring &rec_str, 
				  bool keyword_igncase,
				  fits_header_record *ret_header )
{
    int ret_status = -1;

    ssize_t eq_idx = -1;	/* '=' の出現位置 */
    ssize_t sp_idx = -1;	/* ' ' の出現位置 */
    ssize_t sl_idx = -1;	/* '/' の出現位置 */
    ssize_t amp_idx = -1;	/* '&' の出現位置 */

    /* スペースが出現しない or =の後にスペースがあれば確定 */
    /* HOGE=123 */
    /* HOGE= 123 */
    /* = の前にスペースがある場合は，スペースの連続の直後に = があれば確定 */
    /* HOGE = 123 */
    /* HOGE =123 */
    /* HOGE  = 123 */
    /* HOGE  =123 */
    /*  */
    /* HOGE ABC=XYZ これは description */
    /* HOGE ABC =XYZ これは description */
    /* HOGE ABC= XYZ これは description */
    /* HOGE ABC = XYZ これは description */

    eq_idx = rec_str.strchr('=');
    sp_idx = rec_str.strchr(' ');

    if ( 0 <= sp_idx && 0 <= eq_idx ) {		/* 両方が出現する場合 */
	if ( sp_idx < eq_idx ) {
	    ssize_t span_spc;
	    /* スペースの長さ */
	    span_spc = rec_str.strspn(sp_idx," ");
	    /* space が連続して = につながってない場合は無効 */
	    if ( sp_idx + span_spc != eq_idx ) eq_idx = -1;
	    else {
		/* COMMENT, HISTORY, CONTINUE だけは例外とする */
		if ( rec_str.strncmp("COMMENT ",8)==0 ) eq_idx = -1;
		if ( rec_str.strncmp("HISTORY ",8)==0 ) eq_idx = -1;
		if ( rec_str.strncmp("CONTINUE ",9)==0 ) eq_idx = -1;
		if ( keyword_igncase == true ) {
		    if ( rec_str.strncasecmp("COMMENT ",8)==0 ) eq_idx = -1;
		    if ( rec_str.strncasecmp("HISTORY ",8)==0 ) eq_idx = -1;
		    if ( rec_str.strncasecmp("CONTINUE ",9)==0 ) eq_idx = -1;
		}
	    }
	}
    }

    /* '/' の位置を調べる */
    if ( 0 <= eq_idx ) {
	/* まず，keyword が文字列かどうかを調べる */
	ssize_t qt_idx = -1;	/* "'" の開始位置 */
	if ( rec_str.cchr(eq_idx + 1) == '\'' ) qt_idx = eq_idx + 1;
	else if ( rec_str.cchr(eq_idx + 1) == ' ' ) {
	    size_t span_spc;
	    span_spc = rec_str.strspn(eq_idx + 1," ");
	    if ( rec_str.cchr(eq_idx+(1+span_spc)) == '\'' ) {
		qt_idx = eq_idx + 1 + span_spc;
	    }
	}
	if ( qt_idx < 0 ) {	/* まず，文字列ではない場合 */
	    /* これは単純: 文字列でない場合 & は最初に現れる事はない */
	    sl_idx = rec_str.strchr(eq_idx + 1,'/');
	}
	else {			/* 文字列の場合 */
	    /* "'/", "' /", "'    /"  というパターンを探す */
	    ssize_t idx1 = qt_idx + 1;
	    ssize_t idx0;
	    while ( 0 <= (idx0=rec_str.strchr(idx1,'\'')) ) {
		idx0 ++;
		/* "''" の処理 */
		if ( rec_str.cchr(idx0) == '\'' ) {
		    idx0 ++;
		}
		else if ( rec_str.cchr(idx0) == '/' ) {
		    sl_idx = idx0;
		    break;
		}
		/* '&' も探すが，本来は来てはいけない*/
		else if ( rec_str.cchr(idx0) == '&' &&
			  idx0 + 1 + rec_str.strspn(idx0+1,' ') ==
			  rec_str.length() ) {
		    amp_idx = idx0;
		    break;
		}
		else if ( rec_str.cchr(idx0) == ' ' ) {
		    size_t span_spc;
		    span_spc = rec_str.strspn(idx0," ");
		    idx0 += span_spc;
		    if ( rec_str.cchr(idx0) == '/' ) {
			sl_idx = idx0;
			break;
		    }
		    /* '&' も探すが，本来は来てはいけない*/
		    else if ( rec_str.cchr(idx0) == '&' &&
			      idx0 + 1 + rec_str.strspn(idx0+1,' ') ==
			      rec_str.length() ) {
			amp_idx = idx0;
			break;
		    }
		}
		idx1 = idx0;
	    }
	}
    }

    if ( 0 <= eq_idx ) {
	/* HOGE = 123 のような場合 */
	tstring keyword_str;
	tstring value_str;
	fits::header_def a_def = {NULL,NULL,NULL};

	keyword_str.assign(rec_str.cstr(),eq_idx).trim(" ");
	if ( keyword_igncase == true ) keyword_str.toupper();

	if ( 0 <= sl_idx ) {
	    value_str.assign(rec_str.cstr() + eq_idx + 1,
			     sl_idx - eq_idx - 1);
	    a_def.keyword = keyword_str.cstr();
	    a_def.value = value_str.cstr();
	    a_def.comment = rec_str.cstr() + sl_idx + 1;
	    ret_header->assign( a_def );
	}
	/* '&' も面倒をみるが，本来は来てはいけない */
	else if ( 0 <= amp_idx ) {
	    value_str.assign(rec_str.cstr() + eq_idx + 1,
			     amp_idx - eq_idx - 1);
	    a_def.keyword = keyword_str.cstr();
	    a_def.value = value_str.cstr();
	    a_def.comment = rec_str.cstr() + amp_idx;
	    ret_header->assign( a_def );
	}
	else {
	    value_str.assign(rec_str.cstr() + eq_idx + 1);
	    a_def.keyword = keyword_str.cstr();
	    a_def.value = value_str.cstr();
	    a_def.comment = "";
	    ret_header->assign( a_def );
	}
    }
    else if ( 0 <= sp_idx ) {
	tstring keyword_str;
	fits::header_def a_def = {NULL,NULL,NULL};
	/* HISTORY などの description の場合 */
	keyword_str.assign(rec_str.cstr(),sp_idx);
	if ( keyword_igncase == true ) keyword_str.toupper();
	a_def.keyword = keyword_str.cstr();
	a_def.value   = rec_str.cstr() + sp_idx + 1;
	ret_header->assign( a_def );
    }
    else {
	/* ゴミデータの場合 */
	goto quit;
    }

    ret_status = 0;
 quit:
    return ret_status;
}
