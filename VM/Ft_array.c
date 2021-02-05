#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "Ft_array.h"
#include "Ft_stdlib.h"

#define DEFAULT_VALUE_SIZE 32
#define DEFAULT_CAP 32
#define DEFAULT_GROWTH_FACTOR 2.0f

Ft_Arr FtArr_Init(Ft_Uint valueSize, Ft_Uint initialCap) {
    if (valueSize == 0) {
        FtErr_Set(FT_ERR_ARR_BAD_SIZE);
        Ft_Arr ret;
        memset(&ret, 0, sizeof(Ft_Arr));
        return ret;
    }
    if (initialCap == Ft_InvalidSize) {
        initialCap = DEFAULT_CAP;
    }
    Ft_Arr ret;
    ret.compactOnRemove = 1;
    ret.valueSize = valueSize;
    ret.len = 0;
    ret.cap = initialCap;
    ret.growthFactor = DEFAULT_GROWTH_FACTOR;
    if (initialCap == 0) {
        ret.ptr = NULL;
        return ret;
    }
    ret.ptr = Ft_ZeroAlloc(valueSize * initialCap);
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

    return ret;
}

void FtArr_Delete(Ft_Arr *arr) {
    Ft_Free(arr->ptr);
    arr->valueSize = 0;
    arr->len = 0;
    arr->cap = 0;
    arr->growthFactor = 0;
    arr->ptr = NULL;
}

// TODO: Add hybrid mode to resize where the data from the previous memory is only partially copied to the new memory.
//       Will prevent a large latency spike if the array is large and it needs to copy and memset a ton of memory.
void FtArr_Resize(Ft_Arr *arr, Ft_Uint newCap) {
    if (newCap < arr->len) {
        FtErr_Set(FT_ERR_ARR_TOO_SMALL);
        return;
    }

    Ft_Uint curBytes = arr->len * arr->valueSize;
    Ft_Uint newBytes = newCap * arr->valueSize;
    uint8_t *newArr = Ft_ZeroAlloc(newBytes);
    memcpy(newArr, arr->ptr, curBytes);
    Ft_Free(arr->ptr);
    arr->ptr = newArr;
    arr->cap = newCap;
}

void FtArr_Append(Ft_Arr *arr, void *value) {
    if (arr->len == arr->cap) {
        FtArr_Resize(arr, (Ft_Uint) (arr->len * arr->growthFactor));
    }
    if (arr->len == 0) {
        uint8_t *arrPtr = arr->ptr;
        memcpy(arrPtr, value, arr->valueSize);
        arr->len++;
        return;
    }

    Ft_Uint totalBytes = arr->len * arr->valueSize;
    uint8_t *arrPtr = arr->ptr;
    memcpy(arrPtr + totalBytes, value, arr->valueSize);
    arr->len++;
}

Ft_Ptr FtArr_AppendZeroed(Ft_Arr *arr) {
    if (arr->len == arr->cap) {
        FtArr_Resize(arr, (Ft_Uint) (arr->len * arr->growthFactor));
    }
    Ft_Ptr ret = arr->ptr + (arr->len * arr->valueSize);
    arr->len++;
    return ret;
}

void FtArr_Pop(Ft_Arr *arr, Ft_Ptr dest) {
    arr->len--;
    if (dest != NULL) {
        memcpy(dest, arr->ptr + (arr->len * arr->valueSize), arr->valueSize);
    }
    if (arr->compactOnRemove && arr->len / 4 >= arr->cap) {
        FtArr_Resize(arr, arr->len / 2);
    }
}

Ft_Ptr FtArr_Get(Ft_Arr *arr, Ft_Uint idx) {
    if (idx >= arr->len) {
        FtErr_Set(FT_ERR_ARR_IDX_OUT_OF_RANGE);
        return NULL;
    }

    uint8_t *arrPtr = arr->ptr;
    return (Ft_Ptr) (arrPtr + idx * arr->valueSize);
}

void FtArr_Set(Ft_Arr *arr, Ft_Uint idx, void *value) {
    if (idx >= arr->len) {
        FtErr_Set(FT_ERR_ARR_IDX_OUT_OF_RANGE);
        return;
    }

    Ft_Uint totalBytes = idx * arr->valueSize;
    uint8_t *arrPtr = arr->ptr;
    memcpy(arrPtr + totalBytes, value, arr->valueSize);
}
