#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../tinycthread/tinycthread.h"

#include "Ft_class.h"
#include "Ft_object.h"
#include "Ft_MemPool.h"

static once_flag defaultPoolInit = ONCE_FLAG_INIT;
static Ft_MemPool defaultPool;
static Ft_Arr pools;

typedef struct ObjPool {
    Ft_Cls *clazz;
    Ft_MemPool objects;
} ObjPool;

static void once_InitDefaultPool() {
    defaultPool = FtMemPool_Init(sizeof(Ft_Obj));
    pools = FtArr_Init(sizeof(struct ObjPool), 10);
}

Ft_Cls *FtCls_Alloc(Ft_Interp *interp, Ft_Str name, Ft_Uint native) {
    Ft_Cls *ret = Ft_Alloc(sizeof(Ft_Cls));
#if PARANOID_ERRORS
    Ft_Err err = Ft_GetError();
    if (err) {
        switch (err) {
            case FT_ERR_ALLOC:
                printf("[ERROR] [FtCls_Alloc] Alloc failed due to malloc returning null.\n");
                break;
            default:
                printf("[ERROR] [FtCls_Alloc] Alloc failed for unknown reasons.\n");
                break;
        }
        return NULL;
    }
#endif

    ret->super = NULL;
    ret->name = name;
    ret->handlers = FtArr_Init(sizeof(Ft_MsgHandler), 0);
    ret->constructor = NULL;
    ret->native = native;
    return ret;
}

void FtCls_Init(Ft_Interp* interp, Ft_Cls* clazz) {
    call_once(&defaultPoolInit, once_InitDefaultPool);

    Ft_Cls *existing = FtInterp_FindCls(interp, clazz->name);
    if (existing != NULL && clazz != existing) {
        FtErr_Set(FT_ERR_REDEFINED_CLASS);
        return;
    }

    if (!clazz->native) {
        clazz->pool = &defaultPool;
    } else {
        ObjPool *pool = (ObjPool *) FtArr_AppendZeroed(&pools);
        pool->clazz = clazz;
        pool->objects = FtMemPool_Init(clazz->baseSize);
        clazz->pool = &(pool->objects);
    }
    FtInterp_AddCls(interp, clazz);
}

void FtCls_AddMsgHandler(Ft_Cls *cls, Ft_MsgName* name, Ft_MsgCallback fn) {
    Ft_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FtArr_Append(&cls->handlers, &handler);

#if PARANOID_ERRORS
    Ft_Err err = Ft_GetError();
    if (err) {
        switch (err) {
            case FT_ERR_ARR_DEFAULT_SIZE:
                printf("[ERROR] [FtCls_AddMsgHandler] Unable to push handler into array: no default size set.\n");
                break;
            default:
                printf("[ERROR] [FtCls_AddMsgHandler] Unable to push handler into array for unknown reasons.\n");
                break;
        }
    }
#endif
}

Ft_MsgHandler FtCls_FindMsgHandler(Ft_Cls *cls, Ft_MsgName* name) {
    for (Ft_Uint i = 0; i < cls->handlers.len; i++) {
        Ft_MsgHandler *handler = FtArr_Get(&cls->handlers, i);

#if PARANOID_ERRORS
        Ft_Err err = Ft_GetError();
        if (err) {
            switch (err) {
                case FT_ERR_ARR_DEFAULT_SIZE:
                    printf("[ERROR] [FtCls_AddMsgHandler] Unable to iterate over message handler array: index out of range.\n");
                    break;
                case FT_ERR_ARR_IDX_OUT_OF_RANGE:
                    printf("[ERROR] [FtCls_AddMsgHandler] Idx %d out of range of handlers len %d.\n", i,
                           cls->handlers.len);
                    break;
                default:
                    printf("[ERROR] [FtCls_AddMsgHandler] Unable to iterate over message handler array for unknown reason: %d.\n",
                           err);
                    break;
            }
            return FtMsgHandler_InitNull();
        }
#endif

//        if (name.len == handler->name.len && strncmp(name.val, handler->name.val, name.len) == 0) {
        if (name == handler->name) {
            return *handler;
        }
    }

    return FtMsgHandler_InitNull();
}
