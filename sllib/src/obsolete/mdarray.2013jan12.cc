
/**
 * �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ���ڤ�Ф������ꤵ�줿
 * ���֥�������dest_obj �إ��ԡ����ޤ���<br>
 * ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°���ʤ�
 * ���٤Ƥ򥳥ԡ����ޤ�������(���Ф���) ������Ĺ���Ѥ�餺����2 �����ʹߤǻ�
 * �ꤵ�줿�ΰ�����Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 * dest_obj �˼��Ȥޤ���NULL��Ϳ������硤clean()��¹Ԥ�������Ʊ����̤ˤʤ�
 * �ޤ���
 *
 * @param     dest_obj ���ԡ���Υ��֥�������
 * @param     col_idx ���ԡ����������
 * @param     col_len ���ԡ������󥵥���
 * @param     row_idx ���ԡ����ι԰���
 * @param     row_len ���ԡ����ιԥ�����
 * @param     layer_idx ���ԡ����Υ쥤�����
 * @param     layer_len ���ԡ����Υ쥤�䥵����
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ������
 * @throw     �����˲��򵯤��������
 */
mdarray &mdarray::cut( mdarray *dest_obj,
		       ssize_t col_index, size_t col_size, 
		       ssize_t row_index, size_t row_size,
		       ssize_t layer_idx, size_t layer_size )
{
    debug_report("( mdarray *dest_obj, ssize_t, ... )");
    this->image_copy_and_cut( dest_obj, col_index, col_size,
			      row_index, row_size, layer_idx, layer_size );
    return *this;
}

/**
 * �����ǡ��������Υ��дؿ��ǡ����Ȥ���������Ƥΰ���ʬ���ڤ�Ф������ꤵ�줿
 * ���֥�������dest_obj �إ��ԡ����ޤ���<br>
 * ���ԡ���ؤϡ�����������ͤȥ��ԡ����η����ǥե�����͡��ݤ�ե饰��°���ʤ�
 * ���٤Ƥ򥳥ԡ����ޤ�������(���Ф���) ������Ĺ���Ѥ�餺����2 �����ʹߤǻ�
 * �ꤵ�줿�ΰ�����Ǥ��ͤϥǥե�����ͤ������ޤ���<br>
 * dest_obj �˼��Ȥ�Ϳ������硤clean()��¹Ԥ�������Ʊ����̤ˤʤ�ޤ���
 *
 * @param     dest_obj ���ԡ���Υ��֥�������
 * @param     col_idx ���ԡ����������
 * @param     col_len ���ԡ������󥵥���
 * @param     row_idx ���ԡ����ι԰���
 * @param     row_len ���ԡ����ιԥ�����
 * @param     layer_idx ���ԡ����Υ쥤�����
 * @param     layer_len ���ԡ����Υ쥤�䥵����
 * @return    ���Ȥλ���
 * @throw     �Хåե��γ��ݤ˼��Ԥ������
 * @throw     �����˲��򵯤��������
 */
mdarray &mdarray::cut( mdarray &dest_obj,
		       ssize_t col_index, size_t col_size, 
		       ssize_t row_index, size_t row_size,
		       ssize_t layer_idx, size_t layer_size )
{
    debug_report("( mdarray &dest_obj, ssize_t, ... )");
    this->image_copy_and_cut( &dest_obj, col_index, col_size,
			      row_index, row_size, layer_idx, layer_size );
    return *this;
}
