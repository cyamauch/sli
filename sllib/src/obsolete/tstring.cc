
#if 0
/* 正規表現の ".*" とか "[...]*" とかの位置，継続区間を保存するためのクラス */

typedef struct {
    ssize_t p0_next;	/* '*' の開始点の次の位置 */
    size_t p1;		/* p1 の開始点 */
    size_t p1_max;	/* p1 はどこまで探して良いか */
} _tstring__stars_def;

class _tstring__stars_rec
{
public:
    _tstring__stars_rec() {
	_tstring__stars_def dval = {-1,0,0};
	try {
	    this->rec = new tarray_plain(sizeof(_tstring__stars_def),
					 (void **)&this->ptr);
	    this->rec->assign_default(&dval);
	}
	catch (...) {
	    err_throw("_tstring__stars_rec","FATAL","new failed");
	}
    }
    virtual ~_tstring__stars_rec() {
	delete this->rec;
    }
    virtual _tstring__stars_def &at( size_t idx ) {
	if ( this->length() <= idx ) this->resize(idx + 1);
	return (this->ptr)[idx];
    }
    virtual _tstring__stars_rec &resize( size_t sz ) {
	this->rec->resize(sz);
	return *this;
    }
    virtual size_t length() const {
	return this->rec->length();
    }
private:
    tarray_plain *rec;
    _tstring__stars_def *ptr;
};
#endif	/* 0 */

#if 0
/* 未使用：とりあえずとっておく */
/** 
 * 自身の文字列で引数で与えられた文字列パターンでパターンマッチを行います.
 * 
 * @param pos パターンマッチの開始位置
 * @param pat 検索する文字列パターン
 * @param ret_span マッチした文字列の長さ
 * @return 成功時:マッチした文字列の位置
 *           失敗時負の値<br>           
 */
ssize_t tstring::str_match( size_t pos, const char *pat, 
			    size_t *ret_span ) const
{
    bool complete = true;	/* 完全マッチか，部分マッチか */
    ssize_t ret_p = -1;
    ssize_t ret_p_cand = -1;
    size_t ret_p1_cand = 0;
    size_t p0 = 0;		/* パターンのポインタ */
    size_t p1 = pos;		/* 文字列のポインタ */
    /* */
    size_t stars_cnt = 0;
    _tstring__stars_rec stars_rec;
    /* */
    char prev_ch = '\0';	/* 1つ前のパターンキャラ */

    if ( ret_span != NULL ) complete = false;

    /* */
    if ( ret_span != NULL ) *ret_span = 0;
    if ( this->_str_rec == NULL || pat == NULL ) {
	return -1;
    }
    if ( this->length() < pos ) return -1;
    if ( pat[p0] == '\0' && 
	 (this->_str_rec[p1] == '\n' || this->_str_rec[p1] == '\0') ) {
	return 0;
    }
    /* */
    while ( 1 ) {
	if ( 0 < p0 ) prev_ch = pat[p0-1];
	//err_report1(__FUNCTION__,"DEBUG","top [p0]  '%c'",pat[p0]);
	//err_report1(__FUNCTION__,"DEBUG","top [p1] '%c'",this->_str_rec[p1]);
	/* エスケープキャラ */
	if ( prev_ch != '\\' && pat[p0] == '\\' ) {
	    p0++;
	}
	else if ( pat[p0] == '\0' ) {
	    if ( complete == false ) {
		if ( 0 <= ret_p ) {
		    if ( ret_p_cand == ret_p ) {
			if ( ret_p1_cand < p1 ) ret_p1_cand = p1;
		    }
		    else {
			ret_p_cand = ret_p;
			ret_p1_cand = p1;
		    }
		}
	    }
	    if ( this->_str_rec[p1] != '\n' && this->_str_rec[p1] != '\0' ) {
		bool retry_by_stars = false;
		/* できるだけ長い区間でマッチさせようとする */
		while ( 1 ) {
		    if ( 0 < stars_rec.at(stars_cnt).p0_next &&
			 stars_rec.at(stars_cnt).p1 < stars_rec.at(stars_cnt).p1_max ) {
			p0 = stars_rec.at(stars_cnt).p0_next;   /* retry */
			stars_rec.at(stars_cnt).p1++;
			p1 = stars_rec.at(stars_cnt).p1;
			retry_by_stars = true;
			break;
		    }
		    if ( stars_cnt <= 0 ) break;
		    stars_cnt--;
		};
		if ( retry_by_stars == false ) {
		    ret_p = -1;
		    goto quit;
		}
	    }
	    else {
		goto quit;
	    }
	}
	/* '?' パターンの場合 */
	else if ( prev_ch != '\\' && pat[p0] == '?' ) {
	    if ( this->_str_rec[p1] == '\n' || this->_str_rec[p1] == '\0' ) {
		ret_p = -1;
		goto quit;
	    }
	    if ( ret_p < 0 ) ret_p = p1;	/* mark */
	    p0++;
	    p1++;
	}
	/* '*' パターンの場合 */
	else if ( prev_ch != '\\' && pat[p0] == '*' ) {
	    size_t n_p0 = p0;
	    size_t p1_tmp;
	    n_p0++;
	    if ( ret_p < 0 ) ret_p = p1;	/* mark */
	    if ( pat[n_p0] == '\0' ) {
		p0 = n_p0;
		/* finish */
		while ( this->_str_rec[p1] != '\n' &&
			this->_str_rec[p1] != '\0' ) p1++;
		goto quit;
	    }
	    /* ?* が連続してない場合 */
	    if (stars_rec.at(stars_cnt).p0_next != (ssize_t)p0) {
		stars_cnt++;	/* 上書きでなく新規にする */
	    }
	    p0 = n_p0;
	    stars_rec.at(stars_cnt).p0_next = p0;
	    stars_rec.at(stars_cnt).p1 = p1;
	    p1_tmp = p1;
	    while ( this->_str_rec[p1_tmp] != '\n' &&
		    this->_str_rec[p1_tmp] != '\0' )
		p1_tmp++;
	    stars_rec.at(stars_cnt).p1_max = p1_tmp;
	}
	/* '[...]' パターンの場合 */
	else if ( prev_ch != '\\' && pat[p0] == '[' ) {
	    tstring accepts;
	    bool bl;
	    int flags;
	    size_t n_p0 = p0;
	    n_p0 += make_accepts_regexp(pat+n_p0, true, accepts, &flags, &bl);
	    if ( n_p0 == p0 ) {	/* パターンがおかしい */
		goto normal_chars;
	    }
	    /* */
	    if ( 1 ) {
		char tst[2] = {this->_str_rec[p1],'\0'};
		if ( this->_str_rec[p1] == '\n' || this->_str_rec[p1] == '\0' ) {
		    ret_p = -1;
		    goto quit;
		}
		if ( pattern_length(tst,1,accepts.cstr(),flags,bl) == 1 ) {
		    if ( ret_p < 0 ) ret_p = p1;	/* mark */
		    p0 = n_p0;
		    p1++;
		}
		else {
		    bool retry_by_stars = false;
		    while ( 1 ) {
			if ( 0 < stars_rec.at(stars_cnt).p0_next &&
			     stars_rec.at(stars_cnt).p1 < stars_rec.at(stars_cnt).p1_max ) {
			    p0 = stars_rec.at(stars_cnt).p0_next;   /* retry */
			    stars_rec.at(stars_cnt).p1++;
			    p1 = stars_rec.at(stars_cnt).p1;
			    retry_by_stars = true;
			    break;
			}
			if ( stars_cnt <= 0 ) break;
			stars_cnt--;
		    };
		    if ( retry_by_stars == false ) {
			if ( complete == true ) {
			    ret_p = -1;
			    goto quit;
			}
			if ( 0 <= ret_p ) {
			    p1 = ret_p + 1;
			    p0 = 0;	/* retry */
			    ret_p = -1;
			}
			else p1++;
		    }
		}
	    }
	}
	else {
	normal_chars:
	    if ( this->_str_rec[p1] == '\n' || this->_str_rec[p1] == '\0' ) {
		ret_p = -1;
		goto quit;
	    }
	    if ( pat[p0] != this->_str_rec[p1] ) {	/* matchしない */
		bool retry_by_stars = false;
		while ( 1 ) {
		    if ( 0 < stars_rec.at(stars_cnt).p0_next &&
			 stars_rec.at(stars_cnt).p1 < stars_rec.at(stars_cnt).p1_max ) {
			p0 = stars_rec.at(stars_cnt).p0_next;   /* retry */
			stars_rec.at(stars_cnt).p1++;
			p1 = stars_rec.at(stars_cnt).p1;
			retry_by_stars = true;
			break;
		    }
		    if ( stars_cnt <= 0 ) break;
		    stars_cnt--;
		};
		if ( retry_by_stars == false ) {
		    if ( complete == true ) {
			ret_p = -1;
			goto quit;
		    }
		    if ( 0 <= ret_p ) {
			p1 = ret_p + 1;
			p0 = 0;	/* retry */
			ret_p = -1;
		    }
		    else p1++;
		}
	    }
	    else {					/* match する */
		if ( ret_p < 0 ) ret_p = p1;		/* mark */
		p0++;
		p1++;
	    }
	}
    }

 quit:
    if ( ret_p < 0 ) {
	ret_p = ret_p_cand;
	p1 = ret_p1_cand;
    }
    if ( ret_span != NULL ) {
	if ( 0 <= ret_p ) {
	    *ret_span = p1 - ret_p;
	}
    }
    return ret_p;
}
#endif	/* 0 */
