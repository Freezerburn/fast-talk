#include <stdlib.h>
#include <string.h>

#include "FST_interpreter.h"
#include "FST_class.h"

Ft_Interp* FST_MkInterp() {
    Ft_Interp *interp = (Ft_Interp*) Ft_Alloc(sizeof(Ft_Interp));
    interp->globalEnv = FtEnv_Init(NULL, Ft_InvalidSize);
    interp->clazzes = FtArr_Init(sizeof(Ft_Cls), 0);
    return interp;
}

Ft_Interp* FST_CpInterp(Ft_Interp *i) {
    Ft_Interp *in = Ft_Alloc(sizeof(Ft_Interp));
    in->globalEnv = FtEnv_Init(NULL, Ft_InvalidSize);
    // TODO: Implement array copy
    return in;
}

void FST_DelInterp(Ft_Interp *i) {
    FtArr_Delete(&i->clazzes);
    Ft_Free(i);
}

void FST_InterpAddCls(Ft_Interp *interp, Ft_Cls *cls) {
    FtArr_Append(&interp->clazzes, 0, cls);
}

Ft_Cls *FST_InterpFindCls(Ft_Interp *interp, Ft_Str name) {
    for (Ft_Uint i = 0; i < interp->clazzes.len; i++) {
        Ft_Cls *cls = FtArr_Get(&interp->clazzes, i);
        if (name.len == cls->name.len && strncmp(name.val, cls->name.val, name.len) == 0) {
            return cls;
        }
    }

    return NULL;
}
