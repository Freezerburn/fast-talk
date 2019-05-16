#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "FST_msg.h"

FST_Msg* FST_MkMsg(FST_Str name, ...) {
    va_list args;
    va_start(args, name);

    size_t cap = 10;
    FST_Msg *ret = FST_Alloc(sizeof(FST_Msg) + sizeof(FST_Val*) * cap);
    ret->name = name;
    ret->len = 0;
    memset(ret->args, 0, sizeof(FST_Val*) * cap);

    FST_Val *arg;
    while ((arg = va_arg(args, FST_Val*)) != NULL) {
        // Do the check at the start so that if we're at the end of the args after a new value is appended, we don't
        // expand the size of the message when there are no more values to place in the expanded message.
        // Unfortunately I don't believe there is any way to get any information about the arg list other than the
        // next arg so this is an unfortunate side effect of the limited API.
        // Slightly less performant for the first arg, but the CPU should be able to predict the branch will not be
        // taken in most cases since >10 args is usually rare.
        if (ret->len == cap) {
            size_t newCap = cap * 2;
            FST_Msg *newRet = FST_Alloc(sizeof(FST_Msg) + sizeof(FST_Val*) * newCap);
            memcpy(newRet, ret, sizeof(FST_Msg) + sizeof(FST_Val*) * cap);
            FST_Dealloc(ret);
            ret = newRet;
            cap = newCap;
        }
        ret->args[ret->len++] = arg;
    }

    va_end(args);
    return ret;
}

FST_StaticMsg FST_MkMsgNonAlloc(FST_Str name, ...) {
    va_list args;
    va_start(args, name);

    FST_UintDef len = 0;
    FST_Val *arg;
    FST_StaticMsg ret;
    len = 0;
    while ((arg = va_arg(args, FST_Val*)) != NULL) {
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

FST_Msg* FST_CastStaticMsgToMsg(FST_StaticMsg *msg) {
    return (FST_Msg*) msg;
}

void FST_DelMsg(FST_Msg *msg) {
    FST_Dealloc(msg);
}

FST_MsgHandler FST_MkMsgHandler(FST_Str name, FST_MsgCallbackDef(fn)) {
    FST_MsgHandler ret;
    ret.name = name;
    ret.fn = fn;
    return ret;
}

FST_MsgHandler FST_MkNullMsgHandler() {
    return FST_MkMsgHandler(FST_MkStr2("", 0), NULL);
}

FST_BoolDef FST_IsMsgHandlerNull(FST_MsgHandler handler) {
    return handler.fn == NULL;
}

FST_BoolDef FST_IsMsgHandlerNullP(FST_MsgHandler *handler) {
    return handler->fn == NULL;
}
