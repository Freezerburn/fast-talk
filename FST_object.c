#include <string.h>

#include "FST_object.h"
#include "FST_class.h"

FST_Object* FST_MkObject(FST_Class *clazz) {
    FST_Object *ret = FST_Alloc(sizeof(FST_Object));
    ret->typ = FST_TypeObject;
    ret->env = FST_MkEnv();
    // Lazy init the handlers array only when someone actually adds a handler specifically for this object and isn't
    // just using the inherited messages from the class.
    ret->handlers.cap = 0;
    ret->handlers.len = 0;
    ret->clazz = clazz;
    return ret;
}

void FST_DelObject(FST_Object *obj) {
    FST_DelEnv(obj->env);
    if (obj->handlers.cap > 0) {
        FST_DelArray(&obj->handlers);
    }
    FST_Dealloc(obj);
}

void FST_ObjAddMsgHandler(FST_Object *obj, FST_Str name, FST_MsgCallbackDef(fn)) {
    if (obj->handlers.cap == 0) {
        obj->handlers = FST_MkArray1(sizeof(FST_MsgHandler));
    }

    FST_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FST_ArrPush(&obj->handlers, &handler);
}

FST_MsgHandler FST_ObjFindMsgHandler(FST_Object *obj, FST_Str name) {
    for (FST_UintDef i = 0; i < obj->handlers.len; i++) {
        FST_MsgHandler *handler = FST_ArrGet(&obj->handlers, i);
        if (name.len == handler->name.len && strncmp(name.val, handler->name.val, name.len) == 0) {
            return *handler;
        }
    }

    FST_MsgHandler clsHandler = FST_ClsFindMsgHandler(obj->clazz, name);
    if (!FST_IsMsgHandlerNullP(&clsHandler)) {
        return clsHandler;
    }

    return FST_MkNullMsgHandler();
}

FST_Object* FST_ObjHandleMsg(FST_Interp *interp, FST_Object *target, FST_Msg *msg) {
    FST_MsgHandler handler = FST_ObjFindMsgHandler(target, msg->name);
    if (handler.fn == NULL) {
        return NULL;
    }

    return handler.fn(interp, target, msg);
}

FST_Object* FST_CastValToObj(FST_Val *val) {
    return (FST_Object*) val;
}

FST_Val* FST_CastObjToVal(FST_Object *obj) {
    return (FST_Val*) obj;
}
