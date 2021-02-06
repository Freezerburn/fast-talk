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

static void once_InitClassModule() {
    defaultPool = FtMemPool_Init(sizeof(Ft_Obj));
    pools = FtArr_Init(sizeof(struct ObjPool), Ft_InvalidSize);
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
    ret->cache = Ft_Alloc(sizeof(Ft_ClsMsgCache) + sizeof(Ft_ClsMsgCacheEntry) * 50);
    ret->cache->occupied = 0;
    ret->cache->mask = 50 * 3 / 4;
    atomic_init(&ret->cache->cacheAccessors, 0);
    ret->name = name;
    ret->handlers = FtArr_Init(sizeof(Ft_MsgHandler), 0);
    ret->constructor = NULL;
    ret->native = native;
    return ret;
}

void FtCls_Init(Ft_Interp *interp, Ft_Cls *clazz) {
    call_once(&defaultPoolInit, once_InitClassModule);

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

void FtCls_AddMsgHandler(Ft_Cls *cls, Ft_MsgName *name, Ft_MsgCallback fn) {
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
