#ifndef FASTTALK_FT_CLASS_H
#define FASTTALK_FT_CLASS_H

#include <stdatomic.h>

#include "Ft_str.h"
#include "Ft_array.h"
#include "Ft_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Ft_Constructor)(struct Ft_Cls *, struct Ft_Obj *, struct Ft_Obj **, Ft_Uint);
typedef Ft_Ptr (*Ft_ClassAlloc)(struct Ft_Cls *);

typedef struct Ft_ClsMsgCacheEntry {
    Ft_MsgName* name;
    Ft_MsgHandler handler;
} Ft_ClsMsgCacheEntry;
typedef struct Ft_ClsMsgCache {
    Ft_Uint mask;
    Ft_Uint occupied;
    atomic_uint_fast32_t cacheAccessors;
    Ft_ClsMsgCacheEntry buckets[0];
} Ft_ClsMsgCache;
typedef struct Ft_Cls {
    struct Ft_Cls *super;
    _Atomic(Ft_ClsMsgCache*) cache;
    Ft_Str name;
    Ft_Arr handlers;
    Ft_Constructor constructor;
    Ft_Uint baseSize;
    short native;
    struct Ft_MemPool* pool;
} Ft_Cls;

Ft_Cls *FtCls_Alloc(Ft_Interp *interp, Ft_Str name, Ft_Uint native);
void FtCls_Init(Ft_Interp* interp, Ft_Cls* clazz);
void FtCls_AddMsgHandler(Ft_Cls *cls, Ft_MsgName* name, Ft_MsgCallback fn);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FT_CLASS_H
