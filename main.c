#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef __MACH__
#include <mach/mach_time.h> /* mach_absolute_time */
#include <mach/mach.h>      /* host_get_clock_service, mach_... */
#include <mach/clock.h>     /* clock_get_time */
#endif

#include "FST_stdlib.h"
#include "FST_array.h"

enum FST_Type {
    FST_TypeUint,
    FST_TypeFloat,
    FST_TypeStr,
    FST_TypeFn,
    FST_TypeObject
};

struct _FST_Object;
struct _FST_Class;
struct _FST_Msg;
struct _FST_Interp;

typedef struct _FST_Str {
    FST_StrDef val;
    FST_UintDef len;
} FST_Str;

void FST_InterpAddCls(struct _FST_Interp *interp, struct _FST_Class *cls);
struct _FST_Class* FST_InterpFindCls(struct _FST_Interp *interp, struct _FST_Str name);

typedef struct _FST_Val {
    enum FST_Type typ;
    FST_ValDef ptr[0];
} FST_Val;

typedef struct _FST_Msg {
    FST_Str name;
    FST_UintDef len;
    FST_Val *args[0];
} FST_Msg;

typedef struct _FST_StaticMsg {
    FST_Str name;
    FST_UintDef len;
    FST_Val *args[10];
} FST_StaticMsg;

typedef struct _FST_MsgHandler {
    FST_Str name;
    FST_MsgCallbackDef(fn);
} FST_MsgHandler;

typedef struct _FST_EnvVal {
    FST_Str name;
    FST_Val val;
} FST_EnvVal;

typedef struct _FST_Env {
    struct _FST_Env *parent;
    FST_Array values;
} FST_Env;

typedef struct _FST_Class {
    FST_Str name;
    FST_Array handlers;
} FST_Class;

typedef struct _FST_Object {
    enum FST_Type typ;
    FST_Env *env;
    FST_Array handlers;
    FST_Class *clazz;
} FST_Object;

typedef struct _FST_Interp {
    FST_Env globalEnv;
    FST_Array clazzes;
} FST_Interp;

size_t FST_ValLenBytes(const enum FST_Type t) {
    switch (t) {
        case FST_TypeUint:
            return sizeof(FST_UintDef);
        case FST_TypeFloat:
            return sizeof(FST_FloatDef);
        case FST_TypeStr:
            return sizeof(FST_StrDef);
        case FST_TypeFn:
            return sizeof(int*);
        case FST_TypeObject:
            return sizeof(FST_Object);
        default:
            printf("Unhandled FST_Type case: %d\n", t);
            exit(1);
    }
}

void FST_InitVal(FST_Val *val, enum FST_Type typ, FST_PtrDef ptr) {
    size_t typeSize = FST_ValLenBytes(typ);
    val->typ = typ;
    memcpy(val->ptr, ptr, typeSize);
}

FST_Val* FST_MkVal(enum FST_Type typ, FST_PtrDef v) {
    FST_Val *ret = FST_Alloc(sizeof(FST_Val) + FST_ValLenBytes(typ));
    FST_InitVal(ret, typ, v);
    return ret;
}

void FST_DelVal(FST_Val *val) {
    FST_Dealloc(val);
}

FST_Msg* FST_MkMsg(FST_Str name, ...) {
    va_list args;
    va_start(args, name);

    size_t cap = 10;
    FST_Msg *ret = FST_Alloc(sizeof(FST_Msg) + sizeof(FST_Val*) * cap);
    ret->name = name;
    ret->len = 0;
    memset(ret->args, 0, sizeof(FST_Val*) * cap);

    FST_Val *arg;
    while ((arg = va_arg(args, FST_Val*)) != NULL) {
        // Do the check at the start so that if we're at the end of the args after a new value is appended, we don't
        // expand the size of the message when there are no more values to place in the expanded message.
        // Unfortunately I don't believe there is any way to get any information about the arg list other than the
        // next arg so this is an unfortunate side effect of the limited API.
        // Slightly less performant for the first arg, but the CPU should be able to predict the branch will not be
        // taken in most cases since >10 args is usually rare.
        if (ret->len == cap) {
            size_t newCap = cap * 2;
            FST_Msg *newRet = FST_Alloc(sizeof(FST_Msg) + sizeof(FST_Val*) * newCap);
            memcpy(newRet, ret, sizeof(FST_Msg) + sizeof(FST_Val*) * cap);
            FST_Dealloc(ret);
            ret = newRet;
            cap = newCap;
        }
        ret->args[ret->len++] = arg;
    }

    va_end(args);
    return ret;
}

FST_StaticMsg FST_MkMsgNonAlloc(FST_Str name, ...) {
    va_list args;
    va_start(args, name);

    FST_UintDef len = 0;
    FST_Val *arg;
    FST_StaticMsg ret;
    len = 0;
    while ((arg = va_arg(args, FST_Val*)) != NULL) {
        if (len == 10) {
            printf("[CRITICAL] Attempted to allocate a static msg with more than 10 arguments.");
            exit(1);
        }
        ret.args[len++] = arg;
    }
    ret.len = len;
    ret.name = name;
    va_end(args);
    return ret;
}

FST_Msg* FST_CastStaticMsgToMsg(FST_StaticMsg *msg) {
    return (FST_Msg*) msg;
}

void FST_DelMsg(FST_Msg *msg) {
    FST_Dealloc(msg);
}

void FST_InitEnvVal(FST_EnvVal *val, FST_Str name, FST_PtrDef v, enum FST_Type typ) {
    size_t valSize = FST_ValLenBytes(typ);
    val->name = name;
    val->val.typ = typ;
    memcpy(val->val.ptr, v, valSize);
}

FST_EnvVal* FST_MkEnvVal(FST_Str name, FST_PtrDef v, enum FST_Type typ) {
    size_t valSize = FST_ValLenBytes(typ);
    FST_EnvVal *val = FST_Alloc(sizeof(FST_EnvVal) + valSize);
    val->name = name;
    val->val.typ = typ;
    memcpy(val->val.ptr, v, valSize);
    return val;
}

FST_Str FST_MkStr2(FST_StrDef str, FST_UintDef len) {
    FST_Str ret;
    ret.val = str;
    ret.len = len;
    return ret;
}

FST_Str FST_MkStr(FST_StrDef str) {
    return FST_MkStr2(str, strlen(str));
}

void FST_DelEnvVal(FST_EnvVal *e) {
    FST_Dealloc(e);
}

void FST_InitEnv(FST_Env *env, FST_Env *parent, FST_UintDef len) {
    if (len == 0) {
        len = FST_EnvDefaultLen;
    }
    env->parent = parent;
    env->values = FST_MkArray2(sizeof(FST_EnvVal) + 8, len);
}

FST_Env* FST_MkEnv2(FST_Env *parent, FST_UintDef len) {
    if (len == 0) {
        len = FST_EnvDefaultLen;
    }
    FST_Env *ret = FST_Alloc(sizeof(FST_Env));
    FST_InitEnv(ret, parent, len);
    return ret;
}

FST_Env* FST_MkEnv1(FST_UintDef len) {
    if (len == 0) {
        len = FST_EnvDefaultLen;
    }
    FST_Env *ret = FST_Alloc(sizeof(FST_Env));
    FST_InitEnv(ret, NULL, len);
    return ret;
}

FST_Env* FST_MkEnv() {
    return FST_MkEnv1(FST_EnvDefaultLen);
}

void FST_DelEnv(FST_Env *env) {
    FST_DelArray(&env->values);
    FST_Dealloc(env);
}

FST_Env* FST_EnvChild(FST_Env *parent) {
    FST_Env *child = FST_MkEnv2(parent, FST_EnvDefaultLen);
    child->parent = parent;
    return child;
}

void FST_EnvAppend3(FST_Env *env, FST_Str name, FST_PtrDef v, enum FST_Type typ) {
    size_t typBytes = FST_ValLenBytes(typ);
    size_t totalBytes = sizeof(FST_EnvVal) + typBytes;
    uint8_t envVarBytes[totalBytes];
    FST_EnvVal *envVal = (FST_EnvVal*) envVarBytes;
    FST_InitEnvVal(envVal, name, v, typ);
    FST_ArrPush2(&env->values, totalBytes, envVal);
}

void FST_EnvAppend(FST_Env *env, FST_EnvVal *val) {
    size_t typBytes = FST_ValLenBytes(val->val.typ);
    size_t totalBytes = sizeof(FST_EnvVal) + typBytes;
    FST_ArrPush2(&env->values, totalBytes, val);
}

FST_EnvVal* FST_EnvFindValByName(FST_Env *env, FST_Str name) {
    FST_Env *cur = env;
    while (cur != NULL) {
        for (FST_UintDef i = 0; i < env->values.len; i++) {
            FST_EnvVal *val = FST_ArrGet(&env->values, i);
            if (name.len == val->name.len && strncmp(name.val, val->name.val, name.len) == 0) {
                return val;
            }
        }
        cur = cur->parent;
    }
    return NULL;
}


FST_MsgHandler FST_MkMsgHandler(FST_Str name, FST_MsgCallbackDef(fn)) {
    FST_MsgHandler ret;
    ret.name = name;
    ret.fn = fn;
    return ret;
}

FST_MsgHandler FST_MkNullMsgHandler() {
    return FST_MkMsgHandler(FST_MkStr2("", 0), NULL);
}

FST_BoolDef FST_IsMsgHandlerNull(FST_MsgHandler handler) {
    return handler.fn == NULL;
}

FST_BoolDef FST_IsMsgHandlerNullP(FST_MsgHandler *handler) {
    return handler->fn == NULL;
}

FST_Class* FST_MkClass(FST_Interp *interp, FST_Str name) {
    FST_Class *existing = FST_InterpFindCls(interp, name);
    if (existing != NULL) {
        return existing;
    }

    FST_Class *ret = FST_Alloc(sizeof(FST_Class));
    ret->name = name;
    ret->handlers = FST_MkArray1(sizeof(FST_MsgHandler));
    FST_InterpAddCls(interp, ret);
    return ret;
}

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

void FST_ClsAddMsgHandler(FST_Class *cls, FST_Str name, FST_MsgCallbackDef(fn)) {
    FST_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FST_ArrPush(&cls->handlers, &handler);
}

FST_MsgHandler FST_ClsFindMsgHandler(FST_Class *cls, FST_Str name) {
    for (FST_UintDef i = 0; i < cls->handlers.len; i++) {
        FST_MsgHandler *handler = FST_ArrGet(&cls->handlers, i);
        if (name.len == handler->name.len && strncmp(name.val, handler->name.val, name.len) == 0) {
            return *handler;
        }
    }

    return FST_MkNullMsgHandler();
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

void FST_PrnVal(FST_Interp *interp, FST_Val *val) {
    FST_Object *obj;
    switch (val->typ) {
        case FST_TypeUint:
            printf("INT(%d)\n", *(FST_UintDef*)val->ptr);
            break;
        case FST_TypeFloat:
            printf("FLOAT(%f)\n", *(FST_FloatDef*)val->ptr);
            break;
        case FST_TypeStr:
            printf("STR(%s)\n", (FST_StrDef)val->ptr);
            break;
        case FST_TypeFn:
            printf("FUNC(%x)\n", *(unsigned int*)val->ptr);
            break;
        case FST_TypeObject:
            obj = FST_CastValToObj(val);
            FST_MsgHandler handler = FST_ObjFindMsgHandler(obj, FST_MkStr("prn"));
            if (handler.fn != NULL) {
                FST_StaticMsg prnMsg = FST_MkMsgNonAlloc(FST_MkStr("prn"), NULL);
                if (FST_ObjHandleMsg(interp, obj, FST_CastStaticMsgToMsg(&prnMsg)) != NULL) {
                    printf("OBJ(%s) cannot be printed.\n", obj->clazz->name.val);
                }
            }
            break;
    }
}

void FST_PrnEnvVal(FST_Interp *interp, FST_EnvVal *e) {
    FST_PrnVal(interp, &e->val);
}

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

FST_Object* intPrintCallback(FST_Interp *interp, FST_Object *target, FST_Msg *msg) {
    FST_EnvVal *v = FST_EnvFindValByName(target->env, FST_MkStr("intVal"));
    printf("UINT(%d)\n", *(FST_UintDef*)v->val.ptr);
    return NULL;
}

FST_Object* intPlusCallback(FST_Interp *interp, FST_Object *target, FST_Msg *msg) {
    if (msg->len != 1) {
        printf("UINT('+') message must be called with one parameter.\n");
        // TODO: Set error
        return NULL;
    }

    FST_EnvVal *intVal = FST_EnvFindValByName(target->env, FST_MkStr("intVal"));
    FST_UintDef thisInt = *(FST_UintDef*) intVal->val.ptr;

    FST_UintDef otherUint;
    FST_Val *other = msg->args[0];
    FST_EnvVal *e;
    FST_Object *ret;
    switch (other->typ) {
        case FST_TypeUint:
            otherUint = *(FST_UintDef*) other->ptr;
            otherUint += thisInt;

            ret = FST_MkObject(target->clazz);
            FST_EnvAppend3(ret->env, FST_MkStr("intVal"), &otherUint, FST_TypeUint);
            return ret;
        case FST_TypeFloat:
            printf("UINT('+') message does not support floats yet.");
            // TODO: Support float add with uint plus
            return NULL;
        case FST_TypeStr:
            printf("UINT('+') message does not support strings yet.");
            // TODO: Support string add with uint plus
            return NULL;
        case FST_TypeObject:
            printf("UINT('+') message does not support objects yet.");
            // TODO: Support object add with uint plus
            return NULL;
        case FST_TypeFn:
            printf("UINT('+') message cannot be used with a function argument.");
            // TODO: Set error
            return NULL;
    }
}

int main() {
#ifdef __MACH__
    mach_timebase_info_data_t timebase = {0, 0};
    mach_timebase_info(&timebase);
    uint64_t timebaseRatio = (uint64_t)timebase.numer / (uint64_t)timebase.denom;
    uint64_t initclock = mach_absolute_time();
#endif

    FST_Interp *interp = FST_MkInterp();

    FST_Class *intCls = FST_MkClass(interp, FST_MkStr("int"));
    FST_ClsAddMsgHandler(intCls, FST_MkStr("prn"), &intPrintCallback);
    FST_ClsAddMsgHandler(intCls, FST_MkStr("+"), &intPlusCallback);

    FST_Object *testInt = FST_MkObject(intCls);

    FST_UintDef v = 10;
    FST_EnvVal *e = FST_MkEnvVal(FST_MkStr("intVal"), &v, FST_TypeUint);
    FST_EnvAppend(testInt->env, e);
    FST_DelEnvVal(e);

#ifdef __MACH__
    uint64_t total = 0;
#endif

    int runs = 10000000;
    for (int i = 0; i < runs; i++) {
#ifdef __MACH__
        uint64_t clock = mach_absolute_time() - initclock;
        uint64_t nanoBefore = clock * timebaseRatio;
#endif
        FST_Val *testPlusInt = FST_MkVal(FST_TypeUint, &v);
        FST_StaticMsg plusMsg = FST_MkMsgNonAlloc(FST_MkStr("+"), testPlusInt, NULL);
        FST_Object *result = FST_ObjHandleMsg(interp, testInt, FST_CastStaticMsgToMsg(&plusMsg));
        FST_DelObject(result);
        FST_DelVal(testPlusInt);

#ifdef __MACH__
        clock = mach_absolute_time() - initclock;
        uint64_t nanoAfter = clock * timebaseRatio;
        total += (nanoAfter - nanoBefore);
#endif
    }

#ifdef __MACH__
    printf("%llu nanos\n", total / runs);
#endif

    return 0;
}