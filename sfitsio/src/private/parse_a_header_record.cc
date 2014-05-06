static int parse_a_header_record( const tstring &rec_str, 
				  bool keyword_igncase,
				  fits_header_record *ret_header )
{
    int ret_status = -1;

    ssize_t eq_idx = -1;	/* '=' �νи����� */
    ssize_t sp_idx = -1;	/* ' ' �νи����� */
    ssize_t sl_idx = -1;	/* '/' �νи����� */
    ssize_t amp_idx = -1;	/* '&' �νи����� */

    /* ���ڡ������и����ʤ� or =�θ�˥��ڡ���������г��� */
    /* HOGE=123 */
    /* HOGE= 123 */
    /* = �����˥��ڡ�����������ϡ����ڡ�����Ϣ³��ľ��� = ������г��� */
    /* HOGE = 123 */
    /* HOGE =123 */
    /* HOGE  = 123 */
    /* HOGE  =123 */
    /*  */
    /* HOGE ABC=XYZ ����� description */
    /* HOGE ABC =XYZ ����� description */
    /* HOGE ABC= XYZ ����� description */
    /* HOGE ABC = XYZ ����� description */

    eq_idx = rec_str.strchr('=');
    sp_idx = rec_str.strchr(' ');

    if ( 0 <= sp_idx && 0 <= eq_idx ) {		/* ξ�����и������� */
	if ( sp_idx < eq_idx ) {
	    ssize_t span_spc;
	    /* ���ڡ�����Ĺ�� */
	    span_spc = rec_str.strspn(sp_idx," ");
	    /* space ��Ϣ³���� = �ˤĤʤ��äƤʤ�����̵�� */
	    if ( sp_idx + span_spc != eq_idx ) eq_idx = -1;
	    else {
		/* COMMENT, HISTORY, CONTINUE �������㳰�Ȥ��� */
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

    /* '/' �ΰ��֤�Ĵ�٤� */
    if ( 0 <= eq_idx ) {
	/* �ޤ���keyword ��ʸ���󤫤ɤ�����Ĵ�٤� */
	ssize_t qt_idx = -1;	/* "'" �γ��ϰ��� */
	if ( rec_str.cchr(eq_idx + 1) == '\'' ) qt_idx = eq_idx + 1;
	else if ( rec_str.cchr(eq_idx + 1) == ' ' ) {
	    size_t span_spc;
	    span_spc = rec_str.strspn(eq_idx + 1," ");
	    if ( rec_str.cchr(eq_idx+(1+span_spc)) == '\'' ) {
		qt_idx = eq_idx + 1 + span_spc;
	    }
	}
	if ( qt_idx < 0 ) {	/* �ޤ���ʸ����ǤϤʤ���� */
	    /* �����ñ��: ʸ����Ǥʤ���� & �Ϻǽ�˸������Ϥʤ� */
	    sl_idx = rec_str.strchr(eq_idx + 1,'/');
	}
	else {			/* ʸ����ξ�� */
	    /* "'/", "' /", "'    /"  �Ȥ����ѥ������õ�� */
	    ssize_t idx1 = qt_idx + 1;
	    ssize_t idx0;
	    while ( 0 <= (idx0=rec_str.strchr(idx1,'\'')) ) {
		idx0 ++;
		/* "''" �ν��� */
		if ( rec_str.cchr(idx0) == '\'' ) {
		    idx0 ++;
		}
		else if ( rec_str.cchr(idx0) == '/' ) {
		    sl_idx = idx0;
		    break;
		}
		/* '&' ��õ�������������ƤϤ����ʤ�*/
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
		    /* '&' ��õ�������������ƤϤ����ʤ�*/
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
	/* HOGE = 123 �Τ褦�ʾ�� */
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
	/* '&' �����ݤ�ߤ뤬���������ƤϤ����ʤ� */
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
	/* HISTORY �ʤɤ� description �ξ�� */
	keyword_str.assign(rec_str.cstr(),sp_idx);
	if ( keyword_igncase == true ) keyword_str.toupper();
	a_def.keyword = keyword_str.cstr();
	a_def.value   = rec_str.cstr() + sp_idx + 1;
	ret_header->assign( a_def );
    }
    else {
	/* ���ߥǡ����ξ�� */
	goto quit;
    }

    ret_status = 0;
 quit:
    return ret_status;
}
