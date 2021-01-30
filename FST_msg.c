#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "FST_msg.h"
#include "FST_object.h"

Ft_Msg *FtMsg_Init(Ft_Str name, ...) {
    va_list args;
    va_start(args, name);

    size_t cap = 10;
    Ft_Msg *ret = Ft_Alloc(sizeof(Ft_Msg) + sizeof(Ft_Obj *) * cap);
    ret->name = name;
    ret->len = 0;
    memset(ret->args, 0, sizeof(Ft_Obj *) * cap);

    Ft_Obj *arg;
    while ((arg = va_arg(args, Ft_Obj*)) != NULL) {
        // Do the check at the start so that if we're at the end of the args after a new value is appended, we don't
        // expand the size of the message when there are no more values to place in the expanded message.
        // Unfortunately I don't believe there is any way to get any information about the arg list other than the
        // next arg so this is an unfortunate side effect of the limited API.
        // Slightly less performant for the first arg, but the CPU should be able to predict the branch will not be
        // taken in most cases since >10 args is usually rare.
        if (ret->len == cap) {
            size_t newCap = cap * 2;
            Ft_Msg *newRet = Ft_Alloc(sizeof(Ft_Msg) + sizeof(Ft_Obj *) * newCap);
            memcpy(newRet, ret, sizeof(Ft_Msg) + sizeof(Ft_Obj *) * cap);
            Ft_Free(ret);
            ret = newRet;
            cap = newCap;
        }
        ret->args[ret->len++] = arg;
    }

    va_end(args);
    return ret;
}

Ft_StaticMsg FtStaticMsg_Init(Ft_Str name, ...) {
    va_list args;
    va_start(args, name);

    Ft_Uint len = 0;
    Ft_Obj *arg;
    Ft_StaticMsg ret;
    len = 0;
    while ((arg = va_arg(args, Ft_Obj*)) != NULL) {
        if (len == 10) {
            printf("[CRITICAL] Attempted to allocate a static msg with more than 10 arguments.");
            exit(1);
        }
        ret.args[len++] = arg;
    }
    ret.len = len;
    ret.name = name;
    va_end(args);
    return ret;
}

Ft_Msg *FtStaticMsg_CastMsg(Ft_StaticMsg *msg) {
    return (Ft_Msg *) msg;
}

void FtMsg_Del(Ft_Msg *msg) {
    Ft_Free(msg);
}

Ft_MsgHandler FtMsgHandler_Init(Ft_Str name, Ft_MSGCALLBACK(fn)) {
    Ft_MsgHandler ret;
    ret.name = name;
    ret.fn = fn;
    return ret;
}

Ft_MsgHandler FtMsgHandler_InitNull() {
    return FtMsgHandler_Init(FST_MkStr2("", 0), NULL);
}

Ft_Byte FST_IsMsgHandlerNull(Ft_MsgHandler handler) {
    return handler.fn == NULL;
}

Ft_Byte FST_IsMsgHandlerNullP(Ft_MsgHandler *handler) {
    return handler->fn == NULL;
}
