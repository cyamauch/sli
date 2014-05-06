
/**
 * 画像データ向きのメンバ関数で，自身の配列の内容の一部分を切り出し，指定された
 * オブジェクトdest_obj へコピーします．<br>
 * コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性など
 * すべてをコピーします．自身(取り出し元) の配列長は変わらず，第2 引数以降で指
 * 定された領域の要素の値はデフォルト値で埋められます．<br>
 * dest_obj に自身またはNULLを与えた場合，clean()を実行した場合と同じ結果になり
 * ます．
 *
 * @param     dest_obj コピー先のオブジェクト
 * @param     col_idx コピー元の列位置
 * @param     col_len コピー元の列サイズ
 * @param     row_idx コピー元の行位置
 * @param     row_len コピー元の行サイズ
 * @param     layer_idx コピー元のレイヤ位置
 * @param     layer_len コピー元のレイヤサイズ
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合
 * @throw     メモリ破壊を起こした場合
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
 * 画像データ向きのメンバ関数で，自身の配列の内容の一部分を切り出し，指定された
 * オブジェクトdest_obj へコピーします．<br>
 * コピー先へは，配列の要素値とコピー元の型，デフォルト値，丸めフラグの属性など
 * すべてをコピーします．自身(取り出し元) の配列長は変わらず，第2 引数以降で指
 * 定された領域の要素の値はデフォルト値で埋められます．<br>
 * dest_obj に自身を与えた場合，clean()を実行した場合と同じ結果になります．
 *
 * @param     dest_obj コピー先のオブジェクト
 * @param     col_idx コピー元の列位置
 * @param     col_len コピー元の列サイズ
 * @param     row_idx コピー元の行位置
 * @param     row_len コピー元の行サイズ
 * @param     layer_idx コピー元のレイヤ位置
 * @param     layer_len コピー元のレイヤサイズ
 * @return    自身の参照
 * @throw     バッファの確保に失敗した場合
 * @throw     メモリ破壊を起こした場合
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
