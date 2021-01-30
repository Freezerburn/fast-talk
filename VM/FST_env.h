#ifndef FASTTALK_FST_ENV_H
#define FASTTALK_FST_ENV_H

#include "FST_array.h"
#include "FST_str.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Ft_Env {
    struct Ft_Env *parent;
    Ft_Arr values;
} Ft_Env;

typedef struct Ft_EnvVal {
    Ft_Str name;
    struct Ft_Obj *val;
} Ft_EnvVal;

Ft_Env FtEnv_Init(Ft_Env *parent, Ft_Uint len);
Ft_Env FtEnv_InitChild(Ft_Env *parent);
void FtEnv_Del(Ft_Env *env);

void FtEnv_Set(Ft_Env *env, struct Ft_Obj *obj, Ft_Str name);

Ft_EnvVal* FtEnv_Find(Ft_Env *env, Ft_Str name);

#ifdef __cplusplus
}
#endif


#endif //FASTTALK_FST_ENV_H
