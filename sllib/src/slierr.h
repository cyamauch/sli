#ifndef _SLI__SLIERR_H
#define _SLI__SLIERR_H 1

/**
 * @file   slierr.h
 * @brief  構造体 err_rec を定義 (例外発生時に使用)
 */

namespace sli
{
    /**
     * @brief  例外発生時に情報を記録されるために使われる
     */
    typedef struct {
	char class_name[64];
	char func_name[64];
	char level[64];
	char message[128];
    } err_rec;
}

#endif  /* _SLI__SLIERR_H */
