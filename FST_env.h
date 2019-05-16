#ifndef FASTTALK_FST_ENV_H
#define FASTTALK_FST_ENV_H

#include "FST_array.h"
#include "FST_str.h"
#include "FST_val.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FST_Env {
    struct _FST_Env *parent;
    FST_Array values;
} FST_Env;

typedef struct _FST_EnvVal {
    FST_Str name;
    FST_Val val;
} FST_EnvVal;

FST_Env* FST_MkEnv();
FST_Env* FST_MkEnv1(FST_UintDef len);
FST_Env* FST_MkEnv2(FST_Env *parent, FST_UintDef len);
void FST_InitEnv(FST_Env *env, FST_Env *parent, FST_UintDef len);
void FST_DelEnv(FST_Env *env);

FST_Env* FST_EnvChild(FST_Env *parent);

void FST_EnvAppend(FST_Env *env, FST_EnvVal *val);
void FST_EnvAppend3(FST_Env *env, FST_Str name, FST_PtrDef v, enum FST_Type typ);

FST_EnvVal* FST_EnvFindValByName(FST_Env *env, FST_Str name);

FST_EnvVal* FST_MkEnvVal(FST_Str name, FST_PtrDef v, enum FST_Type typ);
void FST_InitEnvVal(FST_EnvVal *val, FST_Str name, FST_PtrDef v, enum FST_Type typ);
void FST_DelEnvVal(FST_EnvVal *e);

#ifdef __cplusplus
}
#endif


#endif //FASTTALK_FST_ENV_H
