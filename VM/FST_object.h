#ifndef FASTTALK_FST_OBJECT_H
#define FASTTALK_FST_OBJECT_H

#include "FST_array.h"
#include "FST_env.h"
#include "FST_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Ft_Obj {
    struct Ft_Cls *clazz;
    Ft_Arr handlers;
    Ft_Uint refCnt;
};
typedef struct Ft_Obj Ft_Obj;

Ft_Obj* FtObj_Init(struct Ft_Cls *clazz);
void FtObj_Del(Ft_Obj *obj);

Ft_Ptr FtObj_DefaultAlloc(struct Ft_Cls *clazz);

void FtObj_AddHandler(Ft_Obj *obj, Ft_Str name, Ft_MsgCallback fn);
Ft_MsgHandler FtObj_FindHandler(Ft_Obj *obj, Ft_Str name);
Ft_Obj* FtObj_Handle(Ft_Interp *interp, Ft_Obj *target, Ft_Msg *msg);

void _FtObj_INCREF(Ft_Obj* o);
void _FtObj_DECREF(Ft_Obj* o);

#define FtObj_INCREF(o) _FtObj_INCREF(o)
#define FtObj_DECREF(o) _FtObj_DECREF(o)

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_OBJECT_H
