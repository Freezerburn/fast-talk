#ifndef FASTTALK_FT_STR_H
#define FASTTALK_FT_STR_H

#include "Ft_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FST_Str {
    Ft_CStr val;
    Ft_Uint len;
} Ft_Str;

Ft_Str FtStr_InitLen(Ft_CStr str, Ft_Uint len);
Ft_Str FtStr_Init(Ft_CStr str);
Ft_Byte FtStr_Eq(Ft_Str l, Ft_Str r);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FT_STR_H
