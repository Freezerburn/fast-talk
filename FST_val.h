#ifndef FASTTALK_FST_VAL_H
#define FASTTALK_FST_VAL_H

#include "FST_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FST_Val {
    enum FST_Type typ;
    FST_ValDef ptr[0];
} FST_Val;

FST_Val* FST_MkVal(enum FST_Type typ, FST_PtrDef v);
void FST_InitVal(FST_Val *val, enum FST_Type typ, FST_PtrDef ptr);
void FST_DelVal(FST_Val *val);

size_t FST_ValLenBytes(enum FST_Type t);
struct FST_Object* FST_CastValToObj(FST_Val *val);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_VAL_H
