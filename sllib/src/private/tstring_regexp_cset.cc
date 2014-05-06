
static const int TSTRING__LOWER  = 0x0001;
static const int TSTRING__UPPER  = 0x0002;
static const int TSTRING__ALPHA  = 0x0004;
static const int TSTRING__DIGIT  = 0x0008;
static const int TSTRING__XDIGIT = 0x0010;
static const int TSTRING__SPACE  = 0x0020;
static const int TSTRING__PUNCT  = 0x0040;
static const int TSTRING__GRAPH  = 0x0080;
static const int TSTRING__PRINT  = 0x0100;
static const int TSTRING__CNTRL  = 0x0200;

static const int TSTRING__ALNUM  = 0x000c;

/* 正規表現の [...] を展開する */
static size_t make_accepts_regexp( const char *pat, bool exclaim_ok,
				   tstring &accepts, int *flags, bool *bl )
{
    const unsigned char *upat = (const unsigned char *)pat;
    size_t p0;

    accepts.assign("");
    *bl = true;
    *flags = 0;

    p0 = 0;
    if ( pat[p0] != '[' ) return 0;
    p0++;
    if ( pat[p0] == '^' ) {
	*bl = false;
	p0++;
    }
    else {
	if ( exclaim_ok == true ) {
	    if ( pat[p0] == '!' ) {
		*bl = false;
		p0++;
	    }
	}
    }

    if ( pat[p0] == ']' ) {
	accepts.append(upat[p0],1);
	p0++;
    }

    while ( pat[p0] != ']' ) {
	if ( pat[p0] == '\0' ) {
	    accepts.erase();
	    p0 = 0;
	    break;
	}
	if ( pat[p0] == '[' ) {
	    /*                     123456789 */
	    if ( c_strncmp(pat+p0,"[:lower:]",9) == 0 ) {
		*flags |= TSTRING__LOWER;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:upper:]",9) == 0 ) {
		*flags |= TSTRING__UPPER;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:alpha:]",9) == 0 ) {
		*flags |= TSTRING__ALPHA;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:digit:]",9) == 0 ) {
		*flags |= TSTRING__DIGIT;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:xdigit:]",10) == 0 ) {
		*flags |= TSTRING__XDIGIT;  p0 += 10;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:alnum:]",9) == 0 ) {
		*flags |= TSTRING__ALNUM;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:space:]",9) == 0 ) {
		*flags |= TSTRING__SPACE;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:punct:]",9) == 0 ) {
		*flags |= TSTRING__PUNCT;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:graph:]",9) == 0 ) {
		*flags |= TSTRING__GRAPH;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:print:]",9) == 0 ) {
		*flags |= TSTRING__PRINT;  p0 += 9;  continue;
	    }
	    else if ( c_strncmp(pat+p0,"[:cntrl:]",9) == 0 ) {
		*flags |= TSTRING__CNTRL;  p0 += 9;  continue;
	    }
	}
	if ( pat[p0] == '\\' ) p0++;
	if ( pat[p0] == '\0' ) {
	    accepts.erase();
	    p0 = 0;
	    break;
	}
	/* */
	if ( pat[p0 + 1] == '-' && pat[p0 + 2] != ']' ) {
	    int begin = upat[p0];
	    int last = 0;
	    size_t off = 2;
	    if ( pat[p0 + off] == '\\' ) off++;
	    if ( pat[p0 + off] == '\0' ) {
		accepts.erase();
		p0 = 0;
		break;
	    }
	    /* A-Z とかになってた場合 */
	    if ( upat[p0] <= upat[p0 + off] ) {
		last = upat[p0 + off];
		p0 += off + 1;
		{
		    size_t i;
		    size_t len = last - begin + 1;
		    size_t idx0 = accepts.length();
		    accepts.resize(accepts.length()+len);
		    for ( i=0 ; i < len ; i++ ) {
			accepts.put(idx0 + i,begin + i,1);
		    }
		}
	    }
	    else {
		accepts.erase();
		p0 = 0;
		break;
	    }
	}
	else {
	    accepts.append(upat[p0],1);
	    p0++;
	}
    }
    if ( pat[p0] == ']' ) {
	p0++;
    }
    else {
	p0 = 0;
    }
    return p0;
}

inline static bool is_found( int ch, const char *accept_in, int flags )
{
    const unsigned char *accept = (const unsigned char *)accept_in;
    bool found = false;
    size_t j;
    if ( accept != NULL ) {
	for ( j=0 ; accept[j] != '\0' ; j++ ) {
	    if ( ch == accept[j] ) {
		found = true;
		break;
	    }
	}
    }
    if ( found == false ) {
     if ( (flags & TSTRING__LOWER) != 0 ) found = c_islower(ch);
     if ( found == false ) {
      if ( (flags & TSTRING__UPPER) != 0 ) found = c_isupper(ch);
      if ( found == false ) {
       if ( (flags & TSTRING__ALPHA) != 0 ) found = c_isalpha(ch);
       if ( found == false ) {
        if ( (flags & TSTRING__DIGIT) != 0 ) found = c_isdigit(ch);
        if ( found == false ) {
         if ( (flags & TSTRING__XDIGIT) != 0 ) found = c_isxdigit(ch);
         if ( found == false ) {
          if ( (flags & TSTRING__SPACE) != 0 ) found = c_isspace(ch);
          if ( found == false ) {
           if ( (flags & TSTRING__PUNCT) != 0 ) found = c_ispunct(ch);
           if ( found == false ) {
	    if ( (flags & TSTRING__GRAPH) != 0 ) found = c_isgraph(ch);
	    if ( found == false ) {
	     if ( (flags & TSTRING__PRINT) != 0 ) found = c_isprint(ch);
	     if ( found == false ) {
	      if ( (flags & TSTRING__CNTRL) != 0 ) found = c_iscntrl(ch);
	     }
	    }
	   }
	  }
	 }
	}
       }
      }
     }
    }
    return found;
}
