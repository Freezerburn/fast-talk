#ifndef FASTTALK_FST_INTERPRETER_H
#define FASTTALK_FST_INTERPRETER_H

#include "FST_env.h"
#include "FST_array.h"
#include "FST_str.h"

#ifdef __cplusplus
extern "C" {
#endif

struct FST_Class;

typedef struct FST_Interp {
    FST_Env globalEnv;
    FST_Array clazzes;
} FST_Interp;

FST_Interp* FST_MkInterp();
void FST_DelInterp(FST_Interp *i);
FST_Interp* FST_CpInterp(FST_Interp *i);

void FST_InterpAddCls(FST_Interp *interp, struct FST_Class *cls);
struct FST_Class* FST_InterpFindCls(FST_Interp *interp, FST_Str name);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_INTERPRETER_H
