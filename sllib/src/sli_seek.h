#ifndef _SLI__SLI_SEEK_H
#define _SLI__SLI_SEEK_H 1

/**
 * @file   sli_seek.h
 * @brief  SEEK_SET，SEEK_CUR，SEEK_ENDを定義
 */

#include "sli_funcs.h"

#ifndef SEEK_SET
#define SEEK_SET (sli__seek_set())
#endif
#ifndef SEEK_CUR
#define SEEK_CUR (sli__seek_cur())
#endif
#ifndef SEEK_END
#define SEEK_END (sli__seek_end())
#endif

#endif  /* _SLI__SLI_SEEK_H */
