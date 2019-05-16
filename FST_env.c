//
// Created by Vincent K on 2019-05-16.
//

#include <stdlib.h>
#include <string.h>

#include "FST_env.h"

void FST_InitEnvVal(FST_EnvVal *val, FST_Str name, FST_PtrDef v, enum FST_Type typ) {
    size_t valSize = FST_ValLenBytes(typ);
    val->name = name;
    val->val.typ = typ;
    memcpy(val->val.ptr, v, valSize);
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

FST_EnvVal* FST_MkEnvVal(FST_Str name, FST_PtrDef v, enum FST_Type typ) {
    size_t valSize = FST_ValLenBytes(typ);
    FST_EnvVal *val = FST_Alloc(sizeof(FST_EnvVal) + valSize);
    val->name = name;
    val->val.typ = typ;
    memcpy(val->val.ptr, v, valSize);
    return val;
}

void FST_DelEnvVal(FST_EnvVal *e) {
    FST_Dealloc(e);
}
