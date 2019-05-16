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

#include "FST.h"

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