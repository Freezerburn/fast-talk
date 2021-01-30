#include <string.h>

#include "FST_object.h"
#include "FST_class.h"

Ft_Obj* FtObj_Init(Ft_Cls *clazz) {
    Ft_Obj *ret = clazz->alloc(clazz);
    // Lazy init the handlers array only when someone actually adds a handler specifically for this object and isn't
    // just using the inherited messages from the class.
    ret->handlers.cap = 0;
    ret->handlers.len = 0;
    ret->clazz = clazz;
    if (clazz->constructor != NULL) {
        clazz->constructor(clazz, ret, NULL, 0);
    }
    return ret;
}

void FtObj_Del(Ft_Obj *obj) {
    if (obj->handlers.cap > 0) {
        FtArr_Delete(&obj->handlers);
    }
    Ft_Free(obj);
}

Ft_Ptr FtObj_DefaultAlloc(struct Ft_Cls *clazz) {
    return Ft_Alloc(sizeof(Ft_Obj));
}

void FtObj_AddHandler(Ft_Obj *obj, Ft_Str name, Ft_MSGCALLBACK(fn)) {
    if (obj->handlers.cap == 0) {
        obj->handlers = FtArr_Init(sizeof(Ft_MsgHandler), 0);
    }

    Ft_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FtArr_Append(&obj->handlers, 0, &handler);
}

Ft_MsgHandler FtObj_FindHandler(Ft_Obj *obj, Ft_Str name) {
    for (Ft_Uint i = 0; i < obj->handlers.len; i++) {
        Ft_MsgHandler *handler = FtArr_Get(&obj->handlers, i);
        if (FtStr_Eq(name, handler->name)) {
            return *handler;
        }
    }

    Ft_MsgHandler clsHandler = FtCls_FindMsgHandler(obj->clazz, name);
    if (!FST_IsMsgHandlerNullP(&clsHandler)) {
        return clsHandler;
    }

    return FtMsgHandler_InitNull();
}

Ft_Obj* FtObj_Handle(Ft_Interp *interp, Ft_Obj *target, Ft_Msg *msg) {
    Ft_MsgHandler handler = FtObj_FindHandler(target, msg->name);
    if (handler.fn == NULL) {
        return NULL;
    }

    return handler.fn(interp, target, msg);
}
