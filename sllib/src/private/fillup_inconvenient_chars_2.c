/* -*- Mode: C ; Coding: euc-japan -*- */
/* Time-stamp: <2008-09-13 03:47:23 cyamauch> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * parse ���Թ�ΰ������ڡ�����Хå����饷��ˤ�륨�������ץ���
 * ���󥹤ʤɤ�_ �����ɤ��٤���
 * ��Ȥ��Ƥϡ�
 * <foo name="bar \"hoge\" <xyz>">  �Τ褦�ʤ�Τ�
 * <foo name="bar___hoge____xyz_">  �Τ褦�˽񤭴�������Τ��֤�
 * ������Ǥϡ�
 * target_string �� "<foo name="bar \"hoge\" <xyz>">"
 * inconvenient  �� "<> \t"
 * quotations    �� "\'"
 * escape        �� '\\'
 * substitute    �� '_'
 * �����ꤹ�롥
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

    /*�֥Хå�����å���(escape)+�����פξ��� __(substitute0) ���ִ� */
    for ( i=0 ; i<len_targetstring ; i++ ) {
	if ( ((unsigned char *)filled)[i] == escape ) {
	    filled[i] = substitute0;
	    i++;
	    filled[i] = substitute0;
	}
    }
    /* ���(�㤨�� "") ����ȤˤĤ��Ƥϡ�inconvenient �ˤ�����
       (�㤨�� <��> �����ڡ���������) �� substitute1 (�㤨�� _) ���֤����� */
    qcount=0;
    if ( quotations != NULL && inconvenient != NULL ) {
	char prev_quotation = '\0';
	size_t len_quotations,len_inconvenient;
	len_quotations=strlen(quotations);
	len_inconvenient=strlen(inconvenient);
	for ( i=0 ; i<len_targetstring ; i++ ) {
	    size_t j;
	    for ( j=0 ; j<len_quotations ; j++ ) {
		if ( prev_quotation == '\0' ) {	/* ��̤γ��ˤ��� */
		    if ( filled[i] == quotations[j] ) {
			prev_quotation = quotations[j];
			break;
		    }
		}
		else {				/* ��̤���ˤ��� */
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
			    filled[i] = substitute1;	/* �㤨�� _ ���֤����� */
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

