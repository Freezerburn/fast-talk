#ifndef FASTTALK_FT_CLASS_H
#define FASTTALK_FT_CLASS_H

#include "Ft_str.h"
#include "Ft_array.h"
#include "Ft_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Ft_Constructor)(struct Ft_Cls *, struct Ft_Obj *, struct Ft_Obj **, Ft_Uint);
typedef Ft_Ptr (*Ft_ClassAlloc)(struct Ft_Cls *);

typedef struct Ft_Cls {
    struct Ft_Cls *super;
    Ft_Str name;
    Ft_Arr handlers;
    Ft_Constructor constructor;
    Ft_Uint baseSize;
    short native;
    struct Ft_MemPool* pool;
} Ft_Cls;

Ft_Cls *FtCls_Alloc(Ft_Interp *interp, Ft_Str name, Ft_Uint native);
void FtCls_Init(Ft_Interp* interp, Ft_Cls* clazz);
void FtCls_AddMsgHandler(Ft_Cls *cls, Ft_Str name, Ft_MsgCallback fn);
Ft_MsgHandler FtCls_FindMsgHandler(Ft_Cls *cls, Ft_Str name);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FT_CLASS_H
