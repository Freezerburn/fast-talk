//
// Created by Vincent K on 1/30/21.
//

#ifndef FASTTALK_FT_MEMPOOL_H
#define FASTTALK_FT_MEMPOOL_H

#include "Ft_stdlib.h"
#include "Ft_array.h"

ft_c_open
typedef struct Ft_MemPool {
    Ft_Uint valueSize;
    Ft_Arr pool;
    Ft_Arr freeValues;
    Ft_Arr valuesWithOpenSlots;
} Ft_MemPool;

Ft_MemPool FtMemPool_Init(Ft_Uint);
Ft_Ptr FtMemPool_Alloc(Ft_MemPool*, Ft_Uint*);
void FtMemPool_Free(Ft_MemPool*, Ft_Ptr, Ft_Uint);
ft_c_close

#endif //FASTTALK_FT_MEMPOOL_H
