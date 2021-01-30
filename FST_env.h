#ifndef FASTTALK_FST_ENV_H
#define FASTTALK_FST_ENV_H

#include "FST_array.h"
#include "FST_str.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FST_Env {
    struct FST_Env *parent;
    Ft_Arr values;
} FST_Env;

typedef struct Ft_EnvVal {
    Ft_Str name;
    struct Ft_Obj *val;
} Ft_EnvVal;

FST_Env FtEnv_Init(FST_Env *parent, Ft_Uint len);
FST_Env FtEnv_InitChild(FST_Env *parent);
void FtEnv_Del(FST_Env *env);

void FtEnv_Set(FST_Env *env, struct Ft_Obj *obj, Ft_Str name);

Ft_EnvVal* FtEnv_Find(FST_Env *env, Ft_Str name);

#ifdef __cplusplus
}
#endif


#endif //FASTTALK_FST_ENV_H
