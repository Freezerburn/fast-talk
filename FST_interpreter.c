#include <stdlib.h>
#include <string.h>

#include "FST_interpreter.h"
#include "FST_class.h"

FST_Interp* FST_MkInterp() {
    FST_Interp *interp = (FST_Interp*) FST_Alloc(sizeof(FST_Interp));
    FST_InitEnv(&interp->globalEnv, NULL, FST_EnvDefaultLen);
    interp->clazzes = FST_MkArray1(sizeof(FST_Class));
    return interp;
}

FST_Interp* FST_CpInterp(FST_Interp *i) {
    FST_Interp *in = FST_Alloc(sizeof(FST_Interp));
    FST_InitEnv(&in->globalEnv, NULL, FST_EnvDefaultLen);
    // TODO: Implement array copy
    return in;
}

void FST_DelInterp(FST_Interp *i) {
    FST_DelArray(&i->clazzes);
    FST_Dealloc(i);
}

void FST_InterpAddCls(FST_Interp *interp, FST_Class *cls) {
    FST_ArrPush(&interp->clazzes, cls);
}

FST_Class *FST_InterpFindCls(FST_Interp *interp, FST_Str name) {
    for (FST_UintDef i = 0; i < interp->clazzes.len; i++) {
        FST_Class *cls = FST_ArrGet(&interp->clazzes, i);
        if (name.len == cls->name.len && strncmp(name.val, cls->name.val, name.len) == 0) {
            return cls;
        }
    }

    return NULL;
}
