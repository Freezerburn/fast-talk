#ifndef FASTTALK_FST_CLASS_H
#define FASTTALK_FST_CLASS_H

#include "FST_str.h"
#include "FST_array.h"
#include "FST_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*Ft_Constructor)(struct Ft_Cls *, struct Ft_Obj *, struct Ft_Obj **, Ft_Uint);
typedef Ft_Ptr (*Ft_ClassAlloc)(struct Ft_Cls *);

typedef struct Ft_Cls {
    struct Ft_Cls *super;
    Ft_Str name;
    Ft_Arr handlers;
    Ft_ClassAlloc alloc;
    Ft_Constructor constructor;
} Ft_Cls;

Ft_Cls *FtCls_Init(struct Ft_Interp *interp, Ft_Str name);
void FtCls_AddMsgHandler(Ft_Cls *cls, Ft_Str name, Ft_MsgCallback fn);
Ft_MsgHandler FtCls_FindMsgHandler(Ft_Cls *cls, Ft_Str name);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_CLASS_H
