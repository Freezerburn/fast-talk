#ifndef FASTTALK_FST_CLASS_H
#define FASTTALK_FST_CLASS_H

#include "FST_str.h"
#include "FST_array.h"
#include "FST_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FST_Class {
    FST_Str name;
    FST_Array handlers;
} FST_Class;

FST_Class* FST_MkClass(struct FST_Interp *interp, FST_Str name);
void FST_ClsAddMsgHandler(FST_Class *cls, FST_Str name, FST_MsgCallbackDef(fn));
FST_MsgHandler FST_ClsFindMsgHandler(FST_Class *cls, FST_Str name);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_CLASS_H
