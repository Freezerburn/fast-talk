//
// Created by Vincent K on 2019-05-11.
//

#ifndef FASTTALK_FST_STDLIB_H
#define FASTTALK_FST_STDLIB_H

#include <stdint.h>

#define FST_StrDef char*
#define FST_PtrDef void*
#define FST_ValDef uint8_t
#define FST_UintDef uint32_t
#define FST_FloatDef float
#define FST_BoolDef uint8_t
#define FST_MsgCallbackDef(name) struct FST_Object* (*name)(struct FST_Interp*, struct FST_Object*, struct FST_Msg*)
#define FST_EnvDefaultLen 32

#ifdef __cplusplus
extern "C" {
#endif

enum FST_Type {
    FST_TypeUint,
    FST_TypeFloat,
    FST_TypeStr,
    FST_TypeFn,
    FST_TypeObject
};

FST_PtrDef FST_Alloc(FST_UintDef bytes);
void FST_Dealloc(FST_PtrDef ptr);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_STDLIB_H
