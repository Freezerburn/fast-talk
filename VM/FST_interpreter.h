#ifndef FASTTALK_FST_INTERPRETER_H
#define FASTTALK_FST_INTERPRETER_H

#include "FST_env.h"
#include "FST_array.h"
#include "FST_str.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Ft_Cls;

typedef struct Ft_Interp {
    Ft_Env globalEnv;
    Ft_Arr clazzes;
    struct Ft_Obj *nilObj;
} Ft_Interp;

Ft_Interp* FST_MkInterp();
void FST_DelInterp(Ft_Interp *i);
Ft_Interp* FST_CpInterp(Ft_Interp *i);

void FST_InterpAddCls(Ft_Interp *interp, struct Ft_Cls *cls);
struct Ft_Cls* FST_InterpFindCls(Ft_Interp *interp, Ft_Str name);

#define Ft_RETURN_NIL(interp) do { _FtObj_INCREF(interp->nilObj); return interp->nilObj; } while(0)

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_INTERPRETER_H
