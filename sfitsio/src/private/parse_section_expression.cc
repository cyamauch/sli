
typedef struct _section_exp_info {
    mdarray_long begin;		/* start position                           */
    mdarray_long length;	/* length or FITS::ALL                      */
    mdarray_bool flip_flag;	/* flag for flip image                      */
    /* next 3 values are only for ASCII/binary table */
    tarray_tstring sel_cols;	/* columns to be selected or deleted        */
    mdarray_bool digit_flag;	/* true for elem that expresses col number  */
    mdarray_bool del_flag;	/* true for columns to be deleted           */
    bool zero_indexed;
} section_exp_info;

static const tregex Pat_iraf_section_ex0("^[-]?[0-9]+[ ]*[:][ ]*[-]?[0-9]+$");
static const tregex Pat_iraf_section_ex1("^[-]?[0-9]+$");
static const tregex Pat_iraf_section_ex2("^[-][ ]*[*]$");
static const tregex Pat_iraf_section_ex3("^[-]?[0-9]+[ ]*[:][ ]*[*]?$");

/*
 * [1:100, ...]
 * (0:99, ...)
 * [*, ...]  [-*, ...]  [1, ...]
 * [col1;col2, ...]  [col1;-col2, ...] 
 *
 */
inline static int parse_section_expression( const char *section_expression,
					    section_exp_info *result )
{
    int ret_value = -1;
    const tregex &rex0 = Pat_iraf_section_ex0;
    const tregex &rex1 = Pat_iraf_section_ex1;
    const tregex &rex2 = Pat_iraf_section_ex2;
    const tregex &rex3 = Pat_iraf_section_ex3;
    tstring exp_str;
    tarray_tstring exp_arr, tmp_arr1;
    size_t i;

    result->begin.init();
    result->length.init();
    result->flip_flag.init();
    result->sel_cols.init();
    result->digit_flag.init();
    result->del_flag.init();
    result->zero_indexed = false;

    if ( section_expression == NULL ) goto quit;
    exp_str.assign(section_expression);
    if ( exp_str.length() <= 2 ) goto quit;

    //exp_str.dprint();

    /* check 0-indexed */
    if ( exp_str.at(0) == '(' ) result->zero_indexed = true;
    exp_str.resizeby(-1).erase(0,1);			/* erase [] or () */
    
    exp_arr.split(exp_str.cstr(), ",", true, "'\"", '\\', false).trim();

    /* check each dim */
    for ( i=0 ; i < exp_arr.length() ; i++ ) {
	const tstring &exp_i = exp_arr[i];
	size_t junk;
	/* どのパターンか判定する */
	if ( exp_i.length() == 0 || exp_i.strcmp("*") == 0 ) {
	    result->begin[i] = 0;
	    result->length[i] = FITS::ALL;
	    result->flip_flag[i] = false;
	}
	else if ( exp_i.regmatch(rex2,&junk) == 0 ) {
	    result->begin[i] = 0;
	    result->length[i] = FITS::ALL;
	    result->flip_flag[i] = true;
	}
	else if ( exp_i.regmatch(rex1,&junk) == 0 ) {
	    result->begin[i] = exp_i.atol();
	    result->length[i] = 1;
	    if ( result->zero_indexed == false ) result->begin[i] --;
	    result->flip_flag[i] = false;
	}
	else if ( exp_i.regmatch(rex0,&junk) == 0 ||
		  exp_i.regmatch(rex3,&junk) == 0 ) {
	    long p0,p1;
	    tmp_arr1.split(exp_i.cstr(), ":", false).trim();
	    p0 = tmp_arr1[0].atol();
	    if ( tmp_arr1[1].length() == 0 || tmp_arr1[1].at(0) == '*' ) {
		/* 後ろが指定なしor「*」の場合は最後まで */
		result->begin[i] = p0;
		result->length[i] = FITS::ALL;
		result->flip_flag[i] = false;
	    }
	    else {
		p1 = tmp_arr1[1].atol();
		if ( p1 < p0 ) {
		    result->begin[i] = p1;
		    result->length[i] = 1 + p0 - p1;
		    result->flip_flag[i] = true;
		}
		else {
		    result->begin[i] = p0;
		    result->length[i] = 1 + p1 - p0;
		    result->flip_flag[i] = false;
		}
	    }
	    if ( result->zero_indexed == false ) result->begin[i] --;
	}
	else {
	    if ( i == 0 ) {
		size_t j;
		tmp_arr1.split(exp_i.cstr(), ";", false, "'\"", '\\', false)
		        .trim();
		for ( j=0 ; j < tmp_arr1.length() ; j++ ) {
		    tstring &exp_j = tmp_arr1[j];
		    if ( exp_j.cchr(0) == '-' ) {
			result->del_flag[j] = true;
			exp_j.erase(0,1).ltrim();
		    }
		    else {
			result->del_flag[j] = false;
		    }
		    /* 数字の場合はカラム番号とみなす */
		    if ( exp_j.strspn("0123456789") == exp_j.length() ) {
			/* 登録 */
			result->sel_cols[j] = exp_j;
			result->digit_flag[j] = true;
		    }
		    else {
			size_t len;
			/* クォーテーションを消去する                 */
			/* (strmatch() で使うために escape文字は残す) */
			exp_j.erase_quotes(0, "'\"", '\\', false, &len, true);
			/* 登録 */
			result->sel_cols[j] = exp_j;
			result->digit_flag[j] = false;
		    }
		}
	    }
	    else {
		/* error */
		goto quit;
	    }
	}
    }

    //result->begin.dprint();
    //result->length.dprint();
    //result->flip_flag.dprint();
    //result->sel_cols.dprint();
    //result->digit_flag.dprint();
    //result->del_flag.dprint();

    ret_value = 0;

 quit:
    return ret_value;
}
