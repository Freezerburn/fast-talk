#ifndef FASTTALK_FT_MSG_H
#define FASTTALK_FT_MSG_H

#include "Ft_stdlib.h"
#include "Ft_str.h"
#include "Ft_interpreter.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Ft_Msg;

typedef struct Ft_Obj *(*Ft_MsgCallback)(struct Ft_Interp *, struct Ft_Obj *, struct Ft_Msg *);

typedef struct Ft_MsgName {
    Ft_Str name;
} Ft_MsgName;
struct Ft_Msg {
    Ft_MsgName* name;
    Ft_Uint len;
    struct Ft_Obj *args[0];
};
struct Ft_StaticMsg {
    Ft_MsgName* name;
    Ft_Uint len;
    struct Ft_Obj *args[10];
};
struct Ft_MsgHandler {
    Ft_MsgName* name;
    Ft_MsgCallback fn;
};

typedef struct Ft_Msg Ft_Msg;
typedef struct Ft_StaticMsg Ft_StaticMsg;
typedef struct Ft_MsgHandler Ft_MsgHandler;

Ft_MsgName* FtMsgName_Find(Ft_Str name);

Ft_Msg* FtMsg_InitStr(Ft_Str strName, ...);
Ft_Msg* FtMsg_Init(Ft_MsgName* name, ...);
void FtMsg_Del(Ft_Msg *msg);
Ft_StaticMsg FtStaticMsg_InitStr(Ft_Str name, ...);
Ft_StaticMsg FtStaticMsg_Init(Ft_MsgName* name, ...);

Ft_Msg* FtStaticMsg_CastMsg(Ft_StaticMsg *msg);

Ft_MsgHandler FtMsgHandler_Init(Ft_MsgName* name, Ft_MsgCallback fn);
Ft_MsgHandler FtMsgHandler_InitNull();

Ft_Byte FST_IsMsgHandlerNull(Ft_MsgHandler handler);
Ft_Byte FST_IsMsgHandlerNullP(Ft_MsgHandler *handler);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FT_MSG_H
