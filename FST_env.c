//
// Created by Vincent K on 2019-05-16.
//

#include <stdlib.h>
#include <string.h>

#include "FST_env.h"
#include "FST_object.h"

FST_Env FtEnv_Init(FST_Env *parent, Ft_Uint len) {
    if (len == Ft_InvalidSize) {
        len = Ft_EnvDefaultLen;
    }
    FST_Env ret;
    ret.parent = parent;
    ret.values = FtArr_Init(sizeof(Ft_EnvVal) + 8, len);
    return ret;
}

void FtEnv_Del(FST_Env *env) {
    FtArr_Delete(&env->values);
}

FST_Env FtEnv_InitChild(FST_Env *parent) {
    FST_Env child = FtEnv_Init(parent, Ft_EnvDefaultLen);
    child.parent = parent;
    return child;
}

void FtEnv_Set(FST_Env *env, struct Ft_Obj *obj, Ft_Str name) {
    Ft_EnvVal newVal = {
            .name = name,
            .val = obj,
    };
    for (Ft_Uint i = 0; i < env->values.len; i++) {
        Ft_EnvVal *existing = FtArr_Get(&env->values, i);
        if (FtStr_Eq(existing->name, name)) {
            FtArr_Set(&env->values, i, sizeof(Ft_EnvVal), &newVal);
            return;
        }
    }
    FtArr_Append(&env->values, sizeof(Ft_EnvVal), &newVal);
}

Ft_EnvVal* FtEnv_Find(FST_Env *env, Ft_Str name) {
    FST_Env *cur = env;
    while (cur != NULL) {
        for (Ft_Uint i = 0; i < env->values.len; i++) {
            Ft_EnvVal *val = FtArr_Get(&env->values, i);
            if (FtStr_Eq(name, val->name)) {
                return val;
            }
        }
        cur = cur->parent;
    }
    return NULL;
}
