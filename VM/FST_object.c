#include <string.h>

#include "FST_object.h"
#include "FST_class.h"
#include "Ft_MemPool.h"

Ft_Obj *FtObj_Init(Ft_Cls *clazz) {
    Ft_Uint poolIdx;
    Ft_Obj *ret = FtMemPool_Alloc(clazz->pool, &poolIdx);
    // Instead of using the standard Init function for the Array, we're going to manually set just a couple
    // necessary values on it to reduce the amount of data being changed. As a bonus we can re-use one of the
    // values we set as the "boolean" for whether or not it's been initialized.
    // Thankfully we zero-initialize pretty much everything, so the values inside the array struct are predictable.
    if (!ret->handlers.valueSize) {
        ret->handlers.valueSize = sizeof(Ft_MsgHandler);
        ret->handlers.growthFactor = 2;
    }
    ret->clazz = clazz;
    atomic_init(&ret->refCnt, 1);
    ret->poolIdx = poolIdx;
    if (clazz->constructor != NULL) {
        clazz->constructor(clazz, ret, NULL, 0);
    }
    return ret;
}

void _FtObj_INCREF(Ft_Obj *o) {
    atomic_fetch_add(&o->refCnt, 1);
}

void _FtObj_DECREF(Ft_Obj *o) {
    // TODO: Validate that the atomic operations here are actually going to accomplish what's desired.
    // Since atomics are tricky and I'm too tired right now to validate that the memory operations in the docs
    // about how these atomic operations work are going to keep things consistent without using a mutex.
    atomic_fetch_sub(&o->refCnt, 1);
    Ft_Uint cnt = atomic_load(&o->refCnt);
    if (cnt == 0) {
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
