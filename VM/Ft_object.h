#ifndef FASTTALK_FT_OBJECT_H
#define FASTTALK_FT_OBJECT_H

#include <stdatomic.h>

#include "Ft_array.h"
#include "Ft_env.h"
#include "Ft_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Ft_Obj {
    struct Ft_Cls *clazz;
    Ft_Arr handlers;
    atomic_uint_fast32_t refCnt;
    Ft_Uint poolIdx;
};
typedef struct Ft_Obj Ft_Obj;

Ft_Obj* FtObj_Init(struct Ft_Cls *clazz);

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

#endif //FASTTALK_FT_OBJECT_H
