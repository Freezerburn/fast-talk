#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ft_interpreter.h"
#include "Ft_class.h"
#include "Ft_object.h"
#include "Ft_msg.h"

static Ft_Cls* nilCls = NULL;

static Ft_Obj* handle_print(Ft_Interp* interp, Ft_Obj* self, Ft_Msg *msg) {
    printf("Nil\n");
    Ft_RETURN_NIL(interp);
}

Ft_Interp* FtInterp_Init() {
    Ft_Interp *interp = (Ft_Interp*) Ft_Alloc(sizeof(Ft_Interp));
    interp->globalEnv = FtEnv_Init(NULL, Ft_InvalidSize);
    interp->clazzes = FtArr_Init(sizeof(Ft_Cls), 10);

    nilCls = FtCls_Alloc(interp, FtStr_Init("Nil"), 0);
    FtCls_Init(interp, nilCls);
    FtCls_AddMsgHandler(nilCls, FtMsgName_Find(FtStr_Init("print")), handle_print);
    interp->nilObj = FtObj_Init(nilCls);

    return interp;
}

void FtInterp_Del(Ft_Interp *i) {
    FtArr_Delete(&i->clazzes);
    Ft_Free(i);
}

Ft_Interp* FtInterp_Copy(Ft_Interp *i) {
    Ft_Interp *in = Ft_Alloc(sizeof(Ft_Interp));
    in->globalEnv = FtEnv_Init(NULL, Ft_InvalidSize);
    // TODO: Implement array copy
    return in;
}

void FtInterp_AddCls(Ft_Interp *interp, Ft_Cls *cls) {
    FtArr_Append(&interp->clazzes, cls);
}

Ft_Cls *FtInterp_FindCls(Ft_Interp *interp, Ft_Str name) {
    for (Ft_Uint i = 0; i < interp->clazzes.len; i++) {
        Ft_Cls *cls = FtArr_Get(&interp->clazzes, i);
        if (name.len == cls->name.len && strncmp(name.val, cls->name.val, name.len) == 0) {
            return cls;
        }
    }

    return NULL;
}
