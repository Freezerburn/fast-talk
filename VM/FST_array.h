#ifndef FASTTALK_FST_ARRAY_H
#define FASTTALK_FST_ARRAY_H

#include <stdint.h>

#include "FST_stdlib.h"

ft_c_open
typedef struct {
    Ft_Float growthFactor;
    uint8_t compactOnRemove;
    Ft_Uint valueSize;
    Ft_Uint len;
    Ft_Uint cap;
    Ft_Ptr ptr;
} Ft_Arr;

Ft_Arr FtArr_Init(Ft_Uint valueSize, Ft_Uint initialCap);
void FtArr_Delete(Ft_Arr *arr);

void FtArr_Resize(Ft_Arr *arr, Ft_Uint newCap);

void FtArr_Append(Ft_Arr *arr, void *value);
Ft_Ptr FtArr_AppendZeroed(Ft_Arr *arr);
void FtArr_Pop(Ft_Arr *arr, Ft_Ptr dest);

Ft_Ptr FtArr_Get(Ft_Arr *arr, Ft_Uint idx);
void FtArr_Set(Ft_Arr *arr, Ft_Uint idx, void *value);
ft_c_open

#endif //FASTTALK_FST_ARRAY_H
