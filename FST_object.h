#ifndef FASTTALK_FST_OBJECT_H
#define FASTTALK_FST_OBJECT_H

#include "FST_array.h"
#include "FST_env.h"
#include "FST_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FST_Object {
    enum FST_Type typ;
    FST_Env *env;
    FST_Array handlers;
    struct FST_Class *clazz;
} FST_Object;

FST_Object* FST_MkObject(struct FST_Class *clazz);
void FST_DelObject(FST_Object *obj);

void FST_ObjAddMsgHandler(FST_Object *obj, FST_Str name, FST_MsgCallbackDef(fn));
FST_MsgHandler FST_ObjFindMsgHandler(FST_Object *obj, FST_Str name);
FST_Object* FST_ObjHandleMsg(FST_Interp *interp, FST_Object *target, FST_Msg *msg);

struct FST_Val* FST_CastObjToVal(FST_Object *obj);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_OBJECT_H
