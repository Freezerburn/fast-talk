#include <string.h>
#include <printf.h>

#include "Ft_object.h"
#include "Ft_class.h"
#include "Ft_MemPool.h"
#include "../tinycthread/tinycthread.h"

static once_flag cacheInit = ONCE_FLAG_INIT;
static thrd_t cleanupThread;
static cnd_t cleanupCondition;
static mtx_t cleanupConditionMutex;
static Ft_Arr cachesToFree;
static mtx_t cachesToFreeMutex;

_Noreturn static int thread_CacheCleanup(void *ignored) {
    for (;;) {
        cnd_wait(&cleanupCondition, &cleanupConditionMutex);
        mtx_lock(&cachesToFreeMutex);
        // TODO: Add actual Arr remove so we can remove stuff from the middle of the Arr.
        // Currently there's a possibility that new stuff will be continuously added to the end fo the caches
        // to free Arr that is never seen as able to be freed yet. Therefore no caches will actually be freed,
        // effectively creating a memory leak. We need to be able to scan other parts of the Arr than just the
        // last item to find caches that are ready to be freed so we don't hit that possibility no matter what.
        Ft_Uint i = 100;
        do {
            Ft_ClsMsgCache *cache = *(Ft_ClsMsgCache **) FtArr_Get(&cachesToFree, cachesToFree.len - 1);
            if (atomic_load(&cache->cacheAccessors) != 0) {
                break;
            }
            FtArr_Pop(&cachesToFree, NULL);
            i--;
        } while (i > 0);
        mtx_unlock(&cachesToFreeMutex);
    }
}

static void once_InitObjectModule() {
    cachesToFree = FtArr_Init(sizeof(Ft_Ptr), Ft_InvalidSize);
    cnd_init(&cleanupCondition);
    mtx_init(&cleanupConditionMutex, mtx_plain);
    mtx_init(&cachesToFreeMutex, mtx_plain);
    thrd_create(&cleanupThread, thread_CacheCleanup, NULL);
}

Ft_Obj *FtObj_Init(Ft_Cls *clazz) {
    call_once(&cacheInit, once_InitObjectModule);

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

void FtObj_AddHandler(Ft_Obj *obj, Ft_MsgName* name, Ft_MsgCallback fn) {
    Ft_MsgHandler* handler = FtArr_AppendZeroed(&obj->handlers);
    handler->name = name;
    handler->fn = fn;
}

Ft_Obj *FtObj_SendMsg(Ft_Interp *interp, Ft_Obj *target, Ft_Msg *msg) {
    for (Ft_Uint i = 0; i < target->handlers.len; i++) {
        Ft_MsgHandler *handler = FtArr_Get(&target->handlers, i);
        if (msg->name == handler->name) {
            return handler->fn(interp, target, msg);
        }
    }

    Ft_MsgName *name = msg->name;
    Ft_Cls *cls = target->clazz;
    atomic_fetch_add(&cls->cache->cacheAccessors, 1);
    Ft_ClsMsgCacheEntry *entry;
    Ft_ClsMsgCache *cache = cls->cache;
    Ft_Uint mask = cache->mask;
    Ft_Uint cacheIdx = ((uintptr_t) name) & mask;
    do {
        entry = cache->buckets + cacheIdx;
        if (entry->name == NULL) {
            break;
        }
        cacheIdx = (cacheIdx + 1) & mask;
    } while (entry->name != name);
    if (entry->name != NULL) {
        Ft_MsgCallback fn = entry->handler.fn;
        atomic_fetch_sub(&cls->cache->cacheAccessors, 1);
        return fn(interp, target, msg);
    }
    atomic_fetch_sub(&cls->cache->cacheAccessors, 1);

    Ft_Cls *cur = cls;
    do {
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
                Ft_RETURN_NIL(interp);
            }
#endif

            if (name == handler->name) {
                Ft_Uint cacheCap = mask + 1;
                if (cache->occupied == cacheCap) {
                    Ft_Uint newCap = cacheCap * 2;
                    Ft_ClsMsgCache *newCache = Ft_Alloc(sizeof(Ft_ClsMsgCache) + sizeof(Ft_ClsMsgCacheEntry) * newCap);
                    newCache->occupied = 0;
                    newCache->mask = newCap * 3 / 4;
                    atomic_init(&newCache->cacheAccessors, 1);
                    cls->cache = newCache;

                    if (atomic_load(&cache->cacheAccessors) == 0) {
                        Ft_Free(cache);
                    } else {
                        mtx_lock(&cachesToFreeMutex);
                        FtArr_Append(&cachesToFree, &cache);
                        mtx_unlock(&cachesToFreeMutex);
                        cnd_signal(&cleanupCondition);
                    }
                    cache = newCache;
                    mask = newCache->mask;
                }

                Ft_Uint begin = ((uintptr_t) name) & mask;
                cacheIdx = begin;
                entry = cache->buckets + cacheIdx;
                do {
                    if (entry->name == NULL) {
                        entry->name = name;
                        entry->handler = *handler;
                        cache->occupied++;
                        break;
                    }
                    cacheIdx = (cacheIdx + 1) & mask;
                    entry = cache->buckets + cacheIdx;
                } while (cacheIdx != begin);

                Ft_MsgCallback fn = handler->fn;
                atomic_fetch_sub(&cache->cacheAccessors, 1);
                return fn(interp, target, msg);
            }
        }
        cur = cur->super;
    } while (cur != NULL);

    Ft_RETURN_NIL(interp);
}
