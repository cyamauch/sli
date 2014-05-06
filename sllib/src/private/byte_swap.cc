/**  
 * Ǥ�դΥХ���Ĺ������(���饤���ȵ���ʤ�)�ΥХ��ȥ��������Ѵ����ޤ���
 *
 * @param      buf �������Ƭ���ɥ쥹
 * @param      sz_bytes 1���ǤΥХ���Ĺ
 * @param      len �����Ĺ��
 * @note       �Х���Ĺ��2,4,8�Х��Ȥξ������̤ʥ����ɤ�ư����ä˹�®
 *             �Ǥ�������¾�ΥХ���Ĺ�Ǥ����ѥ����ɤ�ư��ޤ�����8�Х��Ȥ�
 *             �ۤ�����ǤϤ�������®��ư���褦��Ĵ�����Ƥ���ޤ���<br>
 *             �ƥ��ȴĶ��� GCC 3.3.2 + PentiumM�Τ��Ť���åץȥåס�<br>
 *             ɸ��饤�֥��� htonl(), htons(), ntohl(), ntohs(), swab()
 *             �⻲�ȤΤ��ȡ�
 */

#define USE_UNLOOPED_CODE__BYTE_SWAP 1

inline static void byte_swap( void *buf, size_t sz_bytes, size_t len )
{
    /*
     * ���å���ñ��ʥ����ɤǤ�ɽ��:
     *	const size_t len_all = len;
     *	const size_t len_each = sz_bytes / 2;
     *	unsigned char *d_ptr1 = d_ptr + sz_bytes - 1;
     *	for ( i=0 ; i < len_all ; i++ ) {
     *	    for ( j=0 ; j < len_each ; j++ ) {
     *		tmp = d_ptr[j];
     *		d_ptr[j] = d_ptr1[-j];
     *		d_ptr1[-j] = tmp;
     *	    }
     *	    d_ptr += sz_bytes;
     *	    d_ptr1 += sz_bytes;
     *	}
     */
    register unsigned char *d_ptr = (unsigned char *)buf;
    register unsigned char tmp;
    register size_t i;
    /* fast vesion for 2,4 and 8 bytes */
    if ( sz_bytes == 2 ) {
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	/* 256 �� loop_cnt �η׻��� 8-bit ���եȤǤ���������饨����Ƿ��� */
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
		for ( i=0 ; i < 16 ; i++ ) {
		 /*
		  * �����ϼ��Υ����ɤ򥺥饺��Ƚ���ˡ�⤢�뤬���ۤȤ��
		  * �ѥե����ޥ󥹤ϲ������ʤ���
		  * tmp = *d_ptr; *d_ptr = d_ptr[1]; *(++d_ptr) = tmp; d_ptr++;
		  * �����Υ����ɤǤϥ롼��Ÿ�����Ƥ�����ϼ㴳�ɤ��ʤ뤬��
		  * �롼��Ÿ�����ʤ����ϵդ��٤��ʤ롥
		  */
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[1];  d_ptr[1] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[6];  d_ptr[6] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[9];  d_ptr[9] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[14];  d_ptr[14] = d_ptr[15];  d_ptr[15] = tmp;
		    d_ptr += 2 * 8;
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
	    tmp = d_ptr[0];
	    d_ptr[0] = d_ptr[1];
	    d_ptr[1] = tmp;
	    d_ptr += 2;
	}
    }
    else if ( sz_bytes == 4 ) {
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
		for ( i=0 ; i < 32 ; i++ ) {
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
		    tmp = d_ptr[4];  d_ptr[4] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[5];  d_ptr[5] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[11];  d_ptr[11] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[10];  d_ptr[10] = tmp;
		    tmp = d_ptr[12];  d_ptr[12] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[13];  d_ptr[13] = d_ptr[14];  d_ptr[14] = tmp;
		    d_ptr += 4 * 4;
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
	    tmp = d_ptr[0];  d_ptr[0] = d_ptr[3];  d_ptr[3] = tmp;
	    tmp = d_ptr[1];  d_ptr[1] = d_ptr[2];  d_ptr[2] = tmp;
	    d_ptr += 4;
	}
    }
    else if ( sz_bytes == 8 ) {
	size_t len_all = len;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	if ( 256 <= len_all ) {
	    const size_t loop_cnt = len_all >> 8;
	    size_t j;
	    for ( j=0 ; j < loop_cnt ; j++ ) {
		/* 256 elements */
		for ( i=0 ; i < 64 ; i++ ) {
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[14];  d_ptr[14] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[11];  d_ptr[11] = d_ptr[12];  d_ptr[12] = tmp;
		    d_ptr += 8 * 2;
		    tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
		    tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
		    tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
		    tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
		    tmp = d_ptr[8];  d_ptr[8] = d_ptr[15];  d_ptr[15] = tmp;
		    tmp = d_ptr[9];  d_ptr[9] = d_ptr[14];  d_ptr[14] = tmp;
		    tmp = d_ptr[10];  d_ptr[10] = d_ptr[13];  d_ptr[13] = tmp;
		    tmp = d_ptr[11];  d_ptr[11] = d_ptr[12];  d_ptr[12] = tmp;
		    d_ptr += 8 * 2;
		}
	    }
	    len_all &= 0x00ff;
	}
#endif
	for ( i=0 ; i < len_all ; i++ ) {
	    tmp = d_ptr[0];  d_ptr[0] = d_ptr[7];  d_ptr[7] = tmp;
	    tmp = d_ptr[1];  d_ptr[1] = d_ptr[6];  d_ptr[6] = tmp;
	    tmp = d_ptr[2];  d_ptr[2] = d_ptr[5];  d_ptr[5] = tmp;
	    tmp = d_ptr[3];  d_ptr[3] = d_ptr[4];  d_ptr[4] = tmp;
	    d_ptr += 8;
	}
    }
    /* fallback                                                     */
    /* �¸��Ǥ� sz_bytes=4 �λ����٤��ʤ뤬��¾�ξ��Ϥ�������®�� */
    else {
	const size_t len_all = len;
	const size_t len_each = sz_bytes / 2;
	register unsigned char *d_ptr1 = d_ptr;
	register unsigned char *d_ptr_x;
	register size_t j;
	for ( i=0 ; i < len_all ; i++ ) {
	    d_ptr1 += sz_bytes;
	    d_ptr_x = d_ptr1;
	    j = len_each;
#ifdef USE_UNLOOPED_CODE__BYTE_SWAP
	    while ( 4 <= j ) {
		d_ptr_x -= 4;
		tmp = d_ptr[0];  d_ptr[0] = d_ptr_x[3];  d_ptr_x[3] = tmp;
		tmp = d_ptr[1];  d_ptr[1] = d_ptr_x[2];  d_ptr_x[2] = tmp;
		tmp = d_ptr[2];  d_ptr[2] = d_ptr_x[1];  d_ptr_x[1] = tmp;
		tmp = d_ptr[3];  d_ptr[3] = d_ptr_x[0];  d_ptr_x[0] = tmp;
		d_ptr += 4;
		j -= 4;
	    }
#endif
	    d_ptr_x -= j;
	    while ( 0 < j ) {
		j--;
		tmp = *d_ptr;  *d_ptr = d_ptr_x[j];  d_ptr_x[j] = tmp;
		d_ptr++;
	    }
	    d_ptr = d_ptr1;
	}
    }
    return;
}
