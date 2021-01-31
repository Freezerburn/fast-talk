#include <string.h>

#include "FST_object.h"
#include "FST_class.h"
#include "Ft_MemPool.h"

Ft_Obj *FtObj_Init(Ft_Cls *clazz) {
    Ft_Uint poolIdx;
    Ft_Obj *ret = FtMemPool_Alloc(clazz->pool, &poolIdx);
    // Init the handlers array, but don't allocate any capacity until a handler for this specific object is actually
    // created. It's probably more uncommon than not to have object-specific handler overrides.
    if (ret->handlers.ptr == NULL) {
        ret->handlers = FtArr_Init(sizeof(Ft_MsgHandler), 0);
    }
    ret->clazz = clazz;
    ret->refCnt = 1;
    ret->poolIdx = poolIdx;
    if (clazz->constructor != NULL) {
        clazz->constructor(clazz, ret, NULL, 0);
    }
    return ret;
}

void _FtObj_INCREF(Ft_Obj *o) {
    o->refCnt++;
}

void _FtObj_DECREF(Ft_Obj *o) {
    o->refCnt--;
    if (o->refCnt == 0) {
        o->handlers.len = 0;
        FtMemPool_Free(o->clazz->pool, o, o->poolIdx);
    }
}

void FtObj_AddHandler(Ft_Obj *obj, Ft_Str name, Ft_MsgCallback fn) {
    Ft_MsgHandler* handler = FtArr_AppendZeroed(&obj->handlers);
    handler->name = name;
    handler->fn = fn;
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

Ft_Obj *FtObj_Handle(Ft_Interp *interp, Ft_Obj *target, Ft_Msg *msg) {
    Ft_MsgHandler handler = FtObj_FindHandler(target, msg->name);
    if (handler.fn == NULL) {
        return NULL;
    }

    return handler.fn(interp, target, msg);
}
