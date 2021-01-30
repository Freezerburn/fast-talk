//
// Created by Vincent K on 1/29/21.
//

#include "Ft_IntObj.h"
#include "../FST_class.h"
#include "../FST_object.h"
#include "../FST_msg.h"

static Ft_Cls *intObjCls = NULL;

static Ft_Ptr intobj_alloc(Ft_Cls *clazz) {
    return Ft_Alloc(sizeof(Ft_IntObj));
}

static void intobj_constructor(struct Ft_Cls *clazz, struct Ft_IntObj *self, struct Ft_Obj **args, Ft_Uint len) {
    self->value = 0;
}

static Ft_Obj *handle_plus(Ft_Interp *interp, Ft_IntObj *self, Ft_Msg *msg) {
    if (msg->len == 0) {
        FtErr_Set(FT_ERR_BAD_MSG);
        return NULL;
    }
    Ft_Obj *other = msg->args[0];
    if (other->clazz != intObjCls) {
        FtErr_Set(FT_ERR_BAD_MSG);
        return NULL;
    }
    Ft_IntObj *otherInt = (Ft_IntObj *) other;
    return FtIntObj_Init(self->value + otherInt->value);
}

void FtModuleInit_IntObj(Ft_Interp *interp) {
    intObjCls = FtCls_Init(interp, FtStr_Init("Uint"));
    intObjCls->alloc = intobj_alloc;
    intObjCls->constructor = (Ft_Constructor) intobj_constructor;
    FtCls_AddMsgHandler(intObjCls, FtStr_Init("+"), (Ft_MsgCallback) handle_plus);
}

Ft_Obj *FtIntObj_Init(Ft_Uint value) {
    Ft_IntObj *ret = (Ft_IntObj *) FtObj_Init(intObjCls);
    ret->value = value;
    return (Ft_Obj *) ret;
}
