#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "FST_array.h"
#include "FST_stdlib.h"

#define DEFAULT_VALUE_SIZE 32
#define DEFAULT_CAP 32
#define DEFAULT_GROWTH_FACTOR 2.0f

Ft_Arr FtArr_Init(Ft_Uint valueSize, Ft_Uint initialCap) {
    if (valueSize == 0) {
        valueSize = DEFAULT_VALUE_SIZE;
    }
    if (initialCap == 0) {
        initialCap = DEFAULT_CAP;
    }
    Ft_Arr ret;
    ret.len = 0;
    ret.cap = initialCap;
    ret.growthFactor = DEFAULT_GROWTH_FACTOR;
    ret.defaultValueSize = valueSize;
    Ft_Ptr allArr;
    ret.capBytes = valueSize * initialCap;
    allArr = Ft_Alloc(sizeof(Ft_Uint) * (initialCap + 1) + ret.capBytes);
#if PARANOID_ERRORS
    Ft_Err err = Ft_GetError();
    if (err) {
        switch (err) {
            case FT_ERR_ALLOC:
                printf("[ERROR] [FtArr_Init] Alloc failed due to malloc returning null.\n");
                break;
            default:
                printf("[ERROR] [FtArr_Init] Alloc failed for unknown reasons.\n");
                break;
        }
        memset(&ret, 0, sizeof(Ft_Arr));
        return ret;
    }
#endif

    ret.byteIdxs = allArr;
    ret.ptr = (uint8_t *) (allArr) + sizeof(Ft_Uint) * (initialCap + 1);
    memset(allArr, 0, sizeof(Ft_Uint) * (initialCap + 1) + ret.capBytes);
    return ret;
}

void FtArr_Delete(Ft_Arr *arr) {
    Ft_Free(arr->byteIdxs);
    arr->len = 0;
    arr->cap = 0;
    arr->capBytes = 0;
    arr->growthFactor = 0;
    arr->defaultValueSize = 0;
    arr->byteIdxs = NULL;
    arr->ptr = NULL;
}

void FtArr_Resize(Ft_Arr *arr, Ft_Uint bytes) {
    Ft_Uint totalStoredBytes = arr->byteIdxs[arr->len];
    if (bytes < totalStoredBytes) {
        FtErr_Set(FT_ERR_ARR_TOO_SMALL);
        return;
    }

    Ft_Uint newSizesCap;
    if (arr->defaultValueSize == 0) {
        // TODO: Calculate average size of values stored to use to get number of sizes to store
        newSizesCap = bytes / DEFAULT_VALUE_SIZE;
    } else {
        newSizesCap = bytes / arr->defaultValueSize;
    }

    Ft_Uint sizesBytes = sizeof(Ft_Uint) * (arr->cap + 1);
    Ft_Uint totalBytes = sizesBytes + totalStoredBytes;
    Ft_Uint newSizesBytes = sizeof(Ft_Uint) * (newSizesCap + 1);
    Ft_Uint newTotalBytes = newSizesBytes + bytes;

    uint8_t *newArr = Ft_Alloc(newTotalBytes);
    memcpy(newArr, arr->byteIdxs, sizesBytes);
    memcpy(newArr + newSizesBytes, arr->ptr, totalBytes);
    Ft_Free(arr->byteIdxs);
    arr->byteIdxs = (Ft_Uint *) newArr;
    arr->ptr = newArr + newSizesBytes;
    arr->cap = newSizesCap;
    arr->capBytes = bytes;
}

void FtArr_Append(Ft_Arr *arr, Ft_Uint valueSize, void *value) {
    if (valueSize == 0) {
        if (arr->defaultValueSize == 0) {
            FtErr_Set(FT_ERR_ARR_DEFAULT_SIZE);
            return;
        }
        valueSize = arr->defaultValueSize;
    }
    if (arr->len == 0) {
        uint8_t *arrPtr = arr->ptr;
        memcpy(arrPtr, value, valueSize);
        arr->byteIdxs[0] = 0;
        arr->byteIdxs[1] = valueSize;
        arr->len++;
        return;
    }

    Ft_Uint totalBytes = arr->byteIdxs[arr->len];
    if (arr->len == arr->cap || totalBytes + valueSize > arr->capBytes) {
        FtArr_Resize(arr, (Ft_Uint) (arr->capBytes * arr->growthFactor));
    }

    uint8_t *arrPtr = arr->ptr;
    memcpy(arrPtr + totalBytes, value, valueSize);
    arr->len++;
    arr->byteIdxs[arr->len] = totalBytes + valueSize;
}

Ft_Ptr FtArr_Pop(Ft_Arr *arr) {
    arr->len--;
    uint8_t *arrPtr = arr->ptr;
    Ft_Ptr ret = arrPtr + arr->byteIdxs[arr->len];
    if (arr->len / 4 >= arr->cap) {
        FtArr_Resize(arr, arr->capBytes / 2);
    }
    return ret;
}

Ft_Ptr FtArr_Get(Ft_Arr *arr, Ft_Uint idx) {
    if (idx >= arr->len) {
        FtErr_Set(FT_ERR_ARR_IDX_OUT_OF_RANGE);
        return NULL;
    }

    uint8_t *arrPtr = arr->ptr;
    return (Ft_Ptr) (arrPtr + arr->byteIdxs[idx]);
}

void FtArr_Set(Ft_Arr *arr, Ft_Uint idx, Ft_Uint valueSize, void *value) {
    if (valueSize == 0) {
        if (arr->defaultValueSize == 0) {
            FtErr_Set(FT_ERR_ARR_DEFAULT_SIZE);
            return;
        }
        valueSize = arr->defaultValueSize;
    }
    if (idx >= arr->len) {
        FtErr_Set(FT_ERR_ARR_IDX_OUT_OF_RANGE);
        return;
    }

    Ft_Uint totalBytes = arr->byteIdxs[idx];
    Ft_Uint nextOffset;
    if (idx + 1 < arr->cap) {
        nextOffset = arr->byteIdxs[idx + 1];
    } else {
        nextOffset = arr->capBytes;
    }
    if (nextOffset - totalBytes != valueSize) {
        FtErr_Set(FT_ERR_ARR_SET_SIZE_MISMATCH);
        return;
    }
    uint8_t *arrPtr = arr->ptr;
    memcpy(arrPtr + totalBytes, value, valueSize);
}
