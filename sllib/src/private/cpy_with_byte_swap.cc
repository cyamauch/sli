/**  
 * Ǥ�դΥХ���Ĺ������(���饤���ȵ���ʤ�)�ΥХ��ȥ��������Ѵ����ʤ����̤�
 * �Хåե�������򥳥ԡ����ޤ���
 *
 * @param      buf_in �ɤ߼���������Ƭ���ɥ쥹
 * @param      sz_bytes 1���ǤΥХ���Ĺ
 * @param      len �����Ĺ��
 * @param      buf_out ������ΥХåե�����Ƭ���ɥ쥹
 * @note       �Х���Ĺ��2,4,8�Х��Ȥξ������̤ʥ����ɤ�ư����ä˹�®
 *             �Ǥ�������¾�ΥХ���Ĺ�Ǥ����ѥ����ɤ�ư��ޤ�����8�Х��Ȥ�
 *             �ۤ�����ǤϤ�������®��ư���褦��Ĵ�����Ƥ���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåס�<br>
 *             ɸ��饤�֥��� htonl(), htons(), ntohl(), ntohs(), swab()
 *             �⻲�ȤΤ��ȡ�
 */
#include <string.h>
#include <unistd.h>

#define USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP 1

inline static void cpy_with_byte_swap( const void *buf_in, 
				       size_t sz_bytes, size_t len,
				       void *buf_out )
{
    /*
     * ���å���ñ��ʥ����ɤǤ�ɽ��:
     *	 const unsigned char *s_ptr = (const unsigned char *)buf_in;
     *	 unsigned char *d_ptr = (unsigned char *)buf_out;
     *   for ( i=0 ; i < len_all ; i++ ) {
     *       for ( j=0 ; j < sz_bytes ; j++ ) {
     *	         d_ptr[j] = s_ptr[sz_bytes-1-j];
     *       }
     *       d_ptr += sz_bytes;
     *       s_ptr += sz_bytes;
     *   }
     */

    /* fast vesion for 2,4 and 8 bytes */
    if ( sz_bytes == 2 ) {
	/* ���饤���Ȥ�OK�ʾ��� swab() ��Ȥ�                          */
	/* swab() �� htons()���ϥХ��ȶ�����·�äƤ��ʤ����٤��ʤ�������� */
	if ( ((size_t)(buf_in) & 1) == 0 && ((size_t)(buf_out) & 1) == 0 ) {
	    /* Linux �� swab() �Ϥ��ä���®�� */
	    swab(buf_in, buf_out, 2 * len);
	    /*
	     * �¸��Ǥ� swab() ��꼡�Υ����ɤ�®���ä������ѥե����ޥ󥹤�
	     * htons() �μ����˰�¸����ΤǺ��Ѥϸ����ꡥ
	     *  
	     *  uint16_t *d_ptr_s = (uint16_t *)d_ptr;
	     *  const uint16_t *s_ptr_s = (const uint16_t *)s_ptr;
	     *  for ( i=0 ; i < len_all ; i++ ) {
	     *	    *d_ptr_s = htons(*s_ptr_s);
	     *	    d_ptr_s ++;  s_ptr_s ++;
	     *  }
	     */
	}
	else {
	    /* swab() ���1�䤯�餤�٤������饤���ȴط��ʤ���ư�������� */
	    /* (�����󡤥��饤���Ȥ����פ��Ƥʤ����Ϥ�ä��٤��ʤ�) */
	    register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	    register unsigned char *d_ptr = (unsigned char *)buf_out;
	    register size_t i;
	    size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP
	    /* 256 �� loop_cnt �η׻��� 8-bit ���եȤǤ������������� */
	    if ( 256 <= len_all ) {
		const size_t loop_cnt = len_all >> 8;
		register unsigned char tmp;
		size_t j;
		for ( j=0 ; j < loop_cnt ; j++ ) {
		  /* 256 elements */
		  for ( i=0 ; i < 32 ; i++ ) {
		    /* �֥�å�Ĺ�� 8,16,32 �Ȼ������16���Ǥ��ɤ��褦�� */
		    memcpy(d_ptr, s_ptr, 2 * 8);  /* memcpy() �ΰ��Ϥ�ڤ�� */
		    s_ptr += 2 * 8;
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[6];  d_ptr[6] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[9];  d_ptr[9] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[14];  d_ptr[14] = d_ptr[15];  d_ptr[15] = tmp;
		    d_ptr += 2 * 8;
		  }
		}
		len_all &= 0x00ff;
	    }
#endif
	    for ( i=0 ; i < len_all ; i++ ) {
		d_ptr[0] = s_ptr[1];  d_ptr[1] = s_ptr[0];
		d_ptr += 2;  s_ptr += 2;
	    }
	}
    }
    else if ( sz_bytes == 4 ) {
       /*
        * ���Υ֥�å��Υ����ɤϡ�htonl() ��Ȥä����Υ�����
        * {
        *     uint32_t *d_ptr_l = (uint32_t *)d_ptr;
        *     const uint32_t *s_ptr_l = (const uint32_t *)s_ptr;
        *     for ( i=0 ; i < len_all ; i++ ) {
        *         *d_ptr_l = htonl(*s_ptr_l);
        *         d_ptr_l ++;  s_ptr_l ++;
        *     }
        * }
        * ��Ʊ����ǽ���Ф롥
        */
	register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	register unsigned char *d_ptr = (unsigned char *)buf_out;
	register size_t i;
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    register unsigned char tmp;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
                for ( i=0 ; i < 64 ; i++ ) {
		    memcpy(d_ptr, s_ptr, 4 * 4);
		    s_ptr += 4 * 4;
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[5];  d_ptr[5] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[10];  d_ptr[10] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[13];  d_ptr[13] = d_ptr[14];  d_ptr[14] = tmp;
		    d_ptr += 4 * 4;
                }
	    }
	    len_all &= 0x00ff;
	}
#endif
	for ( i=0 ; i < len_all ; i++ ) {
	    d_ptr[0] = s_ptr[3];
	    d_ptr[1] = s_ptr[2];
	    d_ptr[2] = s_ptr[1];
	    d_ptr[3] = s_ptr[0];
	    d_ptr += 4;  s_ptr += 4;
	}
    }
    else if ( sz_bytes == 8 ) {
	const size_t len_all = len;
	register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	register unsigned char *d_ptr = (unsigned char *)buf_out;
	register size_t i;
	/* 8�Х��Ȥξ��� memcpy() ��Ȥ���ꤳ�������㴳®�� */
	/* memcpy() ��Ȥ����ɤ����ζ����� 8�Х��Ȥˤ���餷�� */
	/* �ʤ���*d_ptr++ �ϻȤ��Ȥ���٤��ʤ�                 */
	for ( i=0 ; i < len_all ; i++ ) {
	    d_ptr[0] = s_ptr[7];
	    d_ptr[1] = s_ptr[6];
	    d_ptr[2] = s_ptr[5];
	    d_ptr[3] = s_ptr[4];
	    d_ptr[4] = s_ptr[3];
	    d_ptr[5] = s_ptr[2];
	    d_ptr[6] = s_ptr[1];
	    d_ptr[7] = s_ptr[0];
	    d_ptr += 8;  s_ptr += 8;
	}
    }
    /* fallback                                                  */
    /* sz_bytes�������������٤�����8�Х��Ȥ�ۤ������ǽ���Ф� */
    else {
	const size_t len_all = len;
	register const unsigned char *s_ptr = (const unsigned char *)buf_in;
	register unsigned char *d_ptr = (unsigned char *)buf_out;
	register const unsigned char *s_ptr_x;
	register size_t i, j;
	for ( i=0 ; i < len_all ; i++ ) {
	    s_ptr += sz_bytes;
	    s_ptr_x = s_ptr;
	    j = sz_bytes;
#ifdef USE_UNLOOPED_CODE__CPY_WITH_BYTE_SWAP
	    while ( 8 <= j ) {
		s_ptr_x -= 8;
		d_ptr[0] = s_ptr_x[7];
		d_ptr[1] = s_ptr_x[6];
		d_ptr[2] = s_ptr_x[5];
		d_ptr[3] = s_ptr_x[4];
		d_ptr[4] = s_ptr_x[3];
		d_ptr[5] = s_ptr_x[2];
		d_ptr[6] = s_ptr_x[1];
		d_ptr[7] = s_ptr_x[0];
		d_ptr += 8;
		j -= 8;
	    }
#endif
	    s_ptr_x -= j;
	    while ( 0 < j ) {
		j--;
		*d_ptr = s_ptr_x[j];
		d_ptr++;
	    }
	}
    }
    return;
}
