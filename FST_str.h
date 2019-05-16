#ifndef FASTTALK_FST_STR_H
#define FASTTALK_FST_STR_H

#include "FST_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FST_Str {
    FST_StrDef val;
    FST_UintDef len;
} FST_Str;

FST_Str FST_MkStr2(FST_StrDef str, FST_UintDef len);
FST_Str FST_MkStr(FST_StrDef str);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_STR_H
