
#if 0
/* ����ɽ���� ".*" �Ȥ� "[...]*" �Ȥ��ΰ��֡���³��֤���¸���뤿��Υ��饹 */

typedef struct {
    ssize_t p0_next;	/* '*' �γ������μ��ΰ��� */
    size_t p1;		/* p1 �γ����� */
    size_t p1_max;	/* p1 �Ϥɤ��ޤ�õ�����ɤ��� */
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
/* ̤���ѡ��Ȥꤢ�����ȤäƤ��� */
/** 
 * ���Ȥ�ʸ����ǰ�����Ϳ����줿ʸ����ѥ�����ǥѥ�����ޥå���Ԥ��ޤ�.
 * 
 * @param pos �ѥ�����ޥå��γ��ϰ���
 * @param pat ��������ʸ����ѥ�����
 * @param ret_span �ޥå�����ʸ�����Ĺ��
 * @return ������:�ޥå�����ʸ����ΰ���
 *           ���Ի������<br>           
 */
ssize_t tstring::str_match( size_t pos, const char *pat, 
			    size_t *ret_span ) const
{
    bool complete = true;	/* �����ޥå�������ʬ�ޥå��� */
    ssize_t ret_p = -1;
    ssize_t ret_p_cand = -1;
    size_t ret_p1_cand = 0;
    size_t p0 = 0;		/* �ѥ�����Υݥ��� */
    size_t p1 = pos;		/* ʸ����Υݥ��� */
    /* */
    size_t stars_cnt = 0;
    _tstring__stars_rec stars_rec;
    /* */
    char prev_ch = '\0';	/* 1�����Υѥ����󥭥�� */

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
	/* ���������ץ���� */
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
		/* �Ǥ������Ĺ����֤ǥޥå������褦�Ȥ��� */
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
	/* '?' �ѥ�����ξ�� */
	else if ( prev_ch != '\\' && pat[p0] == '?' ) {
	    if ( this->_str_rec[p1] == '\n' || this->_str_rec[p1] == '\0' ) {
		ret_p = -1;
		goto quit;
	    }
	    if ( ret_p < 0 ) ret_p = p1;	/* mark */
	    p0++;
	    p1++;
	}
	/* '*' �ѥ�����ξ�� */
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
	    /* ?* ��Ϣ³���Ƥʤ���� */
	    if (stars_rec.at(stars_cnt).p0_next != (ssize_t)p0) {
		stars_cnt++;	/* ��񤭤Ǥʤ������ˤ��� */
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
	/* '[...]' �ѥ�����ξ�� */
	else if ( prev_ch != '\\' && pat[p0] == '[' ) {
	    tstring accepts;
	    bool bl;
	    int flags;
	    size_t n_p0 = p0;
	    n_p0 += make_accepts_regexp(pat+n_p0, true, accepts, &flags, &bl);
	    if ( n_p0 == p0 ) {	/* �ѥ����󤬤������� */
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
	    if ( pat[p0] != this->_str_rec[p1] ) {	/* match���ʤ� */
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
	    else {					/* match ���� */
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
