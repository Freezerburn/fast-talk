//
// Created by Vincent K on 1/30/21.
//

#include "Ft_MemPool.h"

Ft_MemPool FtMemPool_Init(Ft_Uint valueSize) {
    Ft_MemPool ret;
    ret.valueSize = valueSize;
    ret.pool = FtArr_Init(valueSize, Ft_InvalidSize);
    ret.freeValues = FtArr_Init(sizeof(uint8_t), Ft_InvalidSize);
    return ret;
}

Ft_Ptr FtMemPool_Alloc(Ft_MemPool* pool, Ft_Uint *idxOut) {
    Ft_Uint freeSlot = Ft_InvalidSize;
    for (Ft_Uint i = 0; i < pool->freeValues.len; i++) {
        uint8_t *freeInfo = FtArr_Get(&pool->freeValues, i);
        if (((*freeInfo) & 1) == 1) {
            freeSlot = i * 8;
            break;
        } else if (((*freeInfo) & 1 << 1) == 1 << 1) {
            freeSlot = i * 8 + 1;
            break;
        } else if (((*freeInfo) & 1 << 2) == 1 << 2) {
            freeSlot = i * 8 + 2;
            break;
        } else if (((*freeInfo) & 1 << 3) == 1 << 3) {
            freeSlot = i * 8 + 3;
            break;
        } else if (((*freeInfo) & 1 << 4) == 1 << 4) {
            freeSlot = i * 8 + 4;
            break;
        } else if (((*freeInfo) & 1 << 5) == 1 << 5) {
            freeSlot = i * 8 + 5;
            break;
        } else if (((*freeInfo) & 1 << 6) == 1 << 6) {
            freeSlot = i * 8 + 6;
            break;
        } else if (((*freeInfo) & 1 << 7) == 1 << 7) {
            freeSlot = i * 8 + 7;
            break;
        }
    }
    if (freeSlot == Ft_InvalidSize) {
        Ft_Uint slot = pool->pool.len / 8;
        Ft_Uint bitLoc = pool->pool.len % 8;
        uint8_t* slotInfo;
        if (slot >= pool->freeValues.len) {
            slotInfo = FtArr_AppendZeroed(&pool->freeValues);
            *slotInfo = 0xFF;
        } else {
            slotInfo = FtArr_Get(&pool->freeValues, slot);
        }
        *slotInfo = (*slotInfo) & (0xFF ^ (1 << bitLoc));
        *idxOut = pool->pool.len;
        return FtArr_AppendZeroed(&pool->pool);
    }

    Ft_Uint slot = freeSlot / 8;
    Ft_Uint bitLoc = freeSlot % 8;
    uint8_t* slotInfo = FtArr_Get(&pool->freeValues, slot);
    *slotInfo = (*slotInfo) & (0xFF ^ (1 << bitLoc));
    *idxOut = freeSlot;
    if (freeSlot >= pool->pool.len) {
        return FtArr_AppendZeroed(&pool->pool);
    }
    return FtArr_Get(&pool->pool, freeSlot);
}

void FtMemPool_Free(Ft_MemPool* pool, Ft_Ptr p, Ft_Uint idx) {
    Ft_Uint index = idx / 8;
    Ft_Uint bitLoc = idx % 8;
    uint8_t* slotInfo = FtArr_Get(&pool->freeValues, index);
    *slotInfo |= (1 << bitLoc);
}
