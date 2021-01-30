//
// Created by Vincent K on 1/29/21.
//

#ifndef FASTTALK_FT_INTOBJ_H
#define FASTTALK_FT_INTOBJ_H

#include "../FST_stdlib.h"
#include "../FST_interpreter.h"
#include "../FST_array.h"
#include "../FST_object.h"

struct Ft_IntObj {
    FtObj_HEAD
    Ft_Uint value;
};
typedef struct Ft_IntObj Ft_IntObj;

void FtModuleInit_IntObj(Ft_Interp *interp);

struct Ft_Obj* FtIntObj_Init(Ft_Uint);

#endif //FASTTALK_FT_INTOBJ_H
