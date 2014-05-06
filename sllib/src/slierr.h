#ifndef _SLI__SLIERR_H
#define _SLI__SLIERR_H 1

/**
 * @file   slierr.h
 * @brief  ��¤�� err_rec ����� (�㳰ȯ�����˻���)
 */

namespace sli
{
    /**
     * @brief  �㳰ȯ�����˾����Ͽ����뤿��˻Ȥ���
     */
    typedef struct {
	char class_name[64];
	char func_name[64];
	char level[64];
	char message[128];
    } err_rec;
}

#endif  /* _SLI__SLIERR_H */
