//
// Created by Vincent K on 1/30/21.
//

#include <memory.h>
#include "Ft_MemPool.h"

// The type here is tuned for speed. All uint types up to 64 have been tested and this seems to squeek out ahead
// by a few nanos in general.
#define BITSET_TYPE uint16_t
#define BITSET_INIT 0xFFFF
static const Ft_Uint BITSET_NUM_BITS = sizeof(BITSET_TYPE) * 8;

Ft_MemPool FtMemPool_Init(Ft_Uint valueSize) {
    Ft_MemPool ret;
    ret.valueSize = valueSize;
    ret.pool = FtArr_Init(valueSize, Ft_InvalidSize);
    ret.freeValues = FtArr_Init(sizeof(BITSET_TYPE), Ft_InvalidSize);
    ret.valuesWithOpenSlots = FtArr_Init(sizeof(Ft_Uint), Ft_InvalidSize);
    return ret;
}

Ft_Ptr FtMemPool_Alloc(Ft_MemPool *pool, Ft_Uint *idxOut) {
    Ft_Uint freeSlot = Ft_InvalidSize;
    if (pool->valuesWithOpenSlots.len > 0) {
        freeSlot = *(Ft_Uint *) FtArr_Get(&pool->valuesWithOpenSlots, pool->valuesWithOpenSlots.len - 1);
        BITSET_TYPE slotInfo = *(BITSET_TYPE *) FtArr_Get(&pool->freeValues, freeSlot);
        // Library function to find the first set bit. Return value starts at 1, not 0, so we have to offset it.
        freeSlot += ffs(slotInfo) - 1;
    }
    if (freeSlot == Ft_InvalidSize) {
        FtArr_Append(&pool->valuesWithOpenSlots, &pool->freeValues.len);
        Ft_Uint bitLoc = pool->pool.len % BITSET_NUM_BITS;
        BITSET_TYPE *slotInfo = FtArr_AppendZeroed(&pool->freeValues);
        *slotInfo = BITSET_INIT;
        *slotInfo = (*slotInfo) & (BITSET_INIT ^ (1 << bitLoc));
        *idxOut = pool->pool.len;
        return FtArr_AppendZeroed(&pool->pool);
    }

    Ft_Uint slot = freeSlot / BITSET_NUM_BITS;
    Ft_Uint bitLoc = freeSlot % BITSET_NUM_BITS;
    BITSET_TYPE *slotInfo = FtArr_Get(&pool->freeValues, slot);
    *slotInfo = (*slotInfo) & (BITSET_INIT ^ (1 << bitLoc));
    *idxOut = freeSlot;
    if (*slotInfo == 0) {
        FtArr_Pop(&pool->valuesWithOpenSlots, NULL);
    }
    if (freeSlot >= pool->pool.len) {
        return FtArr_AppendZeroed(&pool->pool);
    }
    return FtArr_Get(&pool->pool, freeSlot);
}

void FtMemPool_Free(Ft_MemPool *pool, Ft_Ptr p, Ft_Uint idx) {
    Ft_Uint index = idx / BITSET_NUM_BITS;
    Ft_Uint bitLoc = idx % BITSET_NUM_BITS;
    BITSET_TYPE *slotInfo = FtArr_Get(&pool->freeValues, index);
    if (*slotInfo == 0) {
        FtArr_Append(&pool->valuesWithOpenSlots, &index);
    }
    *slotInfo |= (1 << bitLoc);
}
