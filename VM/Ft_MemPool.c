//
// Created by Vincent K on 1/30/21.
//

#include "Ft_MemPool.h"

Ft_MemPool FtMemPool_Init(Ft_Uint valueSize) {
    Ft_MemPool ret;
    ret.valueSize = valueSize;
    ret.pool = FtArr_Init(valueSize, Ft_InvalidSize);
    ret.freeValues = FtArr_Init(sizeof(uint8_t), Ft_InvalidSize);
    ret.valuesWithOpenSlots = FtArr_Init(sizeof(Ft_Uint), Ft_InvalidSize);
    return ret;
}

Ft_Ptr FtMemPool_Alloc(Ft_MemPool *pool, Ft_Uint *idxOut) {
    Ft_Uint freeSlot = Ft_InvalidSize;
    if (pool->valuesWithOpenSlots.len > 0) {
        freeSlot = *(Ft_Uint *) FtArr_Get(&pool->valuesWithOpenSlots, pool->valuesWithOpenSlots.len - 1);
        uint8_t slotInfo = *(uint8_t *) FtArr_Get(&pool->freeValues, freeSlot);
        for (uint8_t i = 0; i < 8; i++) {
            Ft_Uint shifted = 1 << i;
            if ((slotInfo & shifted) == shifted) {
                freeSlot += i;
                break;
            }
        }
    }
    if (freeSlot == Ft_InvalidSize) {
        FtArr_Append(&pool->valuesWithOpenSlots, &pool->freeValues.len);
        Ft_Uint bitLoc = pool->pool.len % 8;
        uint8_t *slotInfo = FtArr_AppendZeroed(&pool->freeValues);
        *slotInfo = 0xFF;
        *slotInfo = (*slotInfo) & (0xFF ^ (1 << bitLoc));
        *idxOut = pool->pool.len;
        return FtArr_AppendZeroed(&pool->pool);
    }

    Ft_Uint slot = freeSlot / 8;
    Ft_Uint bitLoc = freeSlot % 8;
    uint8_t *slotInfo = FtArr_Get(&pool->freeValues, slot);
    *slotInfo = (*slotInfo) & (0xFF ^ (1 << bitLoc));
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
    Ft_Uint index = idx / 8;
    Ft_Uint bitLoc = idx % 8;
    uint8_t *slotInfo = FtArr_Get(&pool->freeValues, index);
    if (*slotInfo == 0) {
        FtArr_Append(&pool->valuesWithOpenSlots, &index);
    }
    *slotInfo |= (1 << bitLoc);
}
