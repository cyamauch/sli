#include <string.h>

/**  
 * Ǥ�դΥХ���Ĺ�ΥХ��ȥѥ�����(���饤���ȵ���ʤ�)�η����֤��ǥХåե��ΰ�
 * ���ɤ��٤��ޤ���
 *
 * @param      dest ������ΥХåե�����Ƭ���ɥ쥹
 * @param      src_pattern �Х��ȥѥ�����
 * @param      word_bytes �Х��ȥѥ�����ΥХ���Ĺ
 * @param      len_elements �ɤ��٤��٤��Х��ȥѥ�����θĿ�
 * @note       �ѥ����󤪤�ӽ�����Хåե����礭���˱����Ƥ��������ɤ���ǽ��
 *             �Ф�褦��Ĵ�����Ƥ���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåס�<br>
 *             ɸ��饤�֥��� wmemset() �⻲�ȡ�<br>
 *             ���: ������Ȼ����̡�32-bit�Ķ��Ǥ� n = 8�Х��ȸ����
 *                   memcpy()�Υ롼�פ���®�Ȥ��������ˤʤä���
 */
inline static void memfill( void *dest, 
	      const void *src_pattern, size_t word_bytes, size_t len_elements )
{
    const size_t op_bytes = 2 * sizeof(size_t);	/* good bytes for memcpy() */
    const unsigned char *src_pat = (const unsigned char *)src_pattern;
    unsigned char *d_ptr = (unsigned char *)dest;
    size_t i, j;

    /*
     * ���å���ñ��ʥ����ɤ�ɽ���������:
     *
     *	    for ( i=0 ; i < len_elements ; i++ ) {
     *		for ( j=0 ; j < word_bytes ; j++ ) {
     *		    d_ptr[j] = src_pat[j];
     *		}
     *		d_ptr += word_bytes;
     *	    }
     */

    /* BLOCK-A �� BLOCK-B �Ȥ�ʬ����� */
    const size_t len_blk = 4 * op_bytes;	/* �� ���� 4 �ϼ�¬��Ĵ�� */

    /*                                    */
    /* BLOCK-A : when word_bytes is small */
    /*                                    */
    if ( word_bytes < len_blk ) {
        /* factor_nn ������Ϥ��ʤ��񤷤������ʤ��Ȥ�2�ʾ�����ꤷ�ʤ��ȡ� */
        /* �㤨�� word_bytes = 3 �ξ��˥ѥե����ޥ󥹤��Фʤ��ʤ�        */
        const size_t factor_nn = 2;
	const size_t nn = factor_nn * (len_blk / word_bytes);
	const unsigned char *s_ptr0 = src_pat;		/* ���ԡ������ɥ쥹 */
	if ( nn < len_elements ) {
	    s_ptr0 = d_ptr;	  /* ���ԡ������ɥ쥹���ѹ�(��Ƭ���Ϥ�Ͽ) */
	    /* �ޤ���Ƭ�� nn �Ĥ����� */
	    len_elements -= nn;
	    for ( i=0 ; i < nn ; i++ ) {
		for ( j=0 ; j < word_bytes ; j++ ) d_ptr[j] = src_pat[j];
		d_ptr += word_bytes;
	    }
	    /* ³����ʬ�� nn �Ĥ��Ĺ�®�ʥ֥�å�ž�������� */
	    while ( nn <= len_elements ) {
		const unsigned char *s_ptr = s_ptr0;
		len_elements -= nn;
		j = nn * word_bytes;
		while ( op_bytes <= j ) {
		    j -= op_bytes;
		    memcpy(d_ptr, s_ptr, op_bytes);
		    s_ptr += op_bytes;  d_ptr += op_bytes;
		}
		while ( 0 < j ) {
		    j--;
		    *d_ptr = *s_ptr;
		    s_ptr ++;  d_ptr ++;
		}
	    }
	}
	/* �Ĥ����ʬ������ */
	while ( 0 < len_elements ) {
	    len_elements --;
	    for ( j=0 ; j < word_bytes ; j++ ) d_ptr[j] = s_ptr0[j];
	    d_ptr += word_bytes;
	}
    }
    /*                                    */
    /* BLOCK-B : when word_bytes is large */
    /*                                    */
    else {
	for ( i=0 ; i < len_elements ; i++ ) {
	    const unsigned char *s_ptr = src_pat;
	    j = word_bytes;
	    while ( op_bytes <= j ) {
		j -= op_bytes;
		memcpy(d_ptr, s_ptr, op_bytes);
		s_ptr += op_bytes;  d_ptr += op_bytes;
	    }
	    while ( 0 < j ) {
		j--;
		*d_ptr = *s_ptr;
		s_ptr ++;  d_ptr ++;
	    }
	}
    }

    return;
}
