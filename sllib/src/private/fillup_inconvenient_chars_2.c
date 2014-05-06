/* -*- Mode: C ; Coding: euc-japan -*- */
/* Time-stamp: <2008-09-13 03:47:23 cyamauch> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * parse に都合の悪いスペースやバックスラシュによるエスケープシー
 * ケンスなどを，_ 等で塗り潰す．
 * 例としては，
 * <foo name="bar \"hoge\" <xyz>">  のようなものを
 * <foo name="bar___hoge____xyz_">  のように書き換えたものを返す
 * ↑の例では，
 * target_string → "<foo name="bar \"hoge\" <xyz>">"
 * inconvenient  → "<> \t"
 * quotations    → "\'"
 * escape        → '\\'
 * substitute    → '_'
 * と設定する．
 */
#define FUNC_MSG "fillup_inconvenient_chars_2(): "
static int fillup_inconvenient_chars_2( const char *target_string, 
					ssize_t len_targetstring,
					const char *inconvenient, 
					const char *quotations , 
					int escape, 
					int substitute0, int substitute1,
					char **return_filled )
{
    ssize_t i;
    int qcount;
    int status=-1;
    char *filled;

    if ( len_targetstring < 0 )
	len_targetstring = strlen(target_string);

    filled=(char *)realloc(*return_filled,len_targetstring+1);
    if ( filled == NULL ) {
	fprintf(stderr,FUNC_MSG "[FATAL ERROR] realloc() failed\n");
	goto quit;
    }
    *return_filled=filled;

    memcpy(filled,target_string,len_targetstring);
    filled[len_targetstring] = '\0';

    /*「バックスラッシュ(escape)+何か」の場合は __(substitute0) に置換 */
    for ( i=0 ; i<len_targetstring ; i++ ) {
	if ( ((unsigned char *)filled)[i] == escape ) {
	    filled[i] = substitute0;
	    i++;
	    filled[i] = substitute0;
	}
    }
    /* 括弧(例えば "") の中身については，inconvenient にあるもの
       (例えば <，> ，スペース，タブ) は substitute1 (例えば _) に置き換え */
    qcount=0;
    if ( quotations != NULL && inconvenient != NULL ) {
	char prev_quotation = '\0';
	size_t len_quotations,len_inconvenient;
	len_quotations=strlen(quotations);
	len_inconvenient=strlen(inconvenient);
	for ( i=0 ; i<len_targetstring ; i++ ) {
	    size_t j;
	    for ( j=0 ; j<len_quotations ; j++ ) {
		if ( prev_quotation == '\0' ) {	/* 括弧の外にいる */
		    if ( filled[i] == quotations[j] ) {
			prev_quotation = quotations[j];
			break;
		    }
		}
		else {				/* 括弧の中にいる */
		    if ( filled[i] == prev_quotation ) {
			prev_quotation = '\0';
			break;
		    }
		}
	    }
	    if ( j < len_quotations ) {
		qcount++;
	    }
	    else {
		if ( qcount % 2 != 0 ) {
		    if ( ((unsigned char *)filled)[i] == substitute0 )
			filled[i] = substitute1;
		    else {
			for ( j=0 ; j<len_inconvenient ; j++ ) {
			    if ( filled[i] == inconvenient[j] ) break;
			}
			if ( j<len_inconvenient ) {
			    filled[i] = substitute1;	/* 例えば _ に置き換え */
			}
		    }
		}
	    }
	}
    }
    status=0;
    
 quit:
    return status;
}
#undef FUNC_MSG

