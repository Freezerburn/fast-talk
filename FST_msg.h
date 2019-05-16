#ifndef FASTTALK_FST_MSG_H
#define FASTTALK_FST_MSG_H

#include "FST_stdlib.h"
#include "FST_str.h"
#include "FST_val.h"
#include "FST_interpreter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FST_Msg {
    FST_Str name;
    FST_UintDef len;
    FST_Val *args[0];
} FST_Msg;

typedef struct FST_StaticMsg {
    FST_Str name;
    FST_UintDef len;
    FST_Val *args[10];
} FST_StaticMsg;

typedef struct FST_MsgHandler {
    FST_Str name;
    FST_MsgCallbackDef(fn);
} FST_MsgHandler;

FST_Msg* FST_MkMsg(FST_Str name, ...);
void FST_DelMsg(FST_Msg *msg);
FST_StaticMsg FST_MkMsgNonAlloc(FST_Str name, ...);

FST_Msg* FST_CastStaticMsgToMsg(FST_StaticMsg *msg);

FST_MsgHandler FST_MkMsgHandler(FST_Str name, FST_MsgCallbackDef(fn));
FST_MsgHandler FST_MkNullMsgHandler();

FST_BoolDef FST_IsMsgHandlerNull(FST_MsgHandler handler);
FST_BoolDef FST_IsMsgHandlerNullP(FST_MsgHandler *handler);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_MSG_H
