#ifndef FASTTALK_FT_INTERPRETER_H
#define FASTTALK_FT_INTERPRETER_H

#include "Ft_env.h"
#include "Ft_array.h"
#include "Ft_str.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Ft_Cls;

typedef struct Ft_Interp {
    Ft_Env globalEnv;
    Ft_Arr clazzes;
    struct Ft_Obj *nilObj;
} Ft_Interp;

Ft_Interp* FtInterp_Init();
void FtInterp_Del(Ft_Interp *i);
Ft_Interp* FtInterp_Copy(Ft_Interp *i);

void FtInterp_AddCls(Ft_Interp *interp, struct Ft_Cls *cls);
struct Ft_Cls* FtInterp_FindCls(Ft_Interp *interp, Ft_Str name);

#define Ft_RETURN_NIL(interp) do { _FtObj_INCREF(interp->nilObj); return interp->nilObj; } while(0)

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FT_INTERPRETER_H
