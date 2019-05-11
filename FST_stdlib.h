//
// Created by Vincent K on 2019-05-11.
//

#ifndef FASTTALK_FST_STDLIB_H
#define FASTTALK_FST_STDLIB_H

#define FST_StrDef char*
#define FST_PtrDef void*
#define FST_ValDef uint8_t
#define FST_UintDef uint32_t
#define FST_FloatDef float
#define FST_BoolDef uint8_t
#define FST_MsgCallbackDef(name) struct _FST_Object* (*name)(struct _FST_Interp*, struct _FST_Object*, struct _FST_Msg*)
#define FST_EnvDefaultLen 32

FST_PtrDef FST_Alloc(FST_UintDef bytes);
void FST_Dealloc(FST_PtrDef ptr);

#endif //FASTTALK_FST_STDLIB_H
