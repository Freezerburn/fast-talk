#include <stdlib.h>
#include <string.h>

#include "FST_class.h"

FST_Class* FST_MkClass(FST_Interp *interp, FST_Str name) {
    FST_Class *existing = FST_InterpFindCls(interp, name);
    if (existing != NULL) {
        return existing;
    }

    FST_Class *ret = FST_Alloc(sizeof(FST_Class));
    ret->name = name;
    ret->handlers = FST_MkArray1(sizeof(FST_MsgHandler));
    FST_InterpAddCls(interp, ret);
    return ret;
}

void FST_ClsAddMsgHandler(FST_Class *cls, FST_Str name, FST_MsgCallbackDef(fn)) {
    FST_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FST_ArrPush(&cls->handlers, &handler);
}

FST_MsgHandler FST_ClsFindMsgHandler(FST_Class *cls, FST_Str name) {
    for (FST_UintDef i = 0; i < cls->handlers.len; i++) {
        FST_MsgHandler *handler = FST_ArrGet(&cls->handlers, i);
        if (name.len == handler->name.len && strncmp(name.val, handler->name.val, name.len) == 0) {
            return *handler;
        }
    }

    return FST_MkNullMsgHandler();
}
