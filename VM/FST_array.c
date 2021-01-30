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
        FtErr_Set(FT_ERR_ARR_BAD_SIZE);
        Ft_Arr ret;
        memset(&ret, 0, sizeof(Ft_Arr));
        return ret;
    }
    if (initialCap == 0) {
        initialCap = DEFAULT_CAP;
    }
    Ft_Arr ret;
    ret.valueSize = valueSize;
    ret.len = 0;
    ret.cap = initialCap;
    ret.growthFactor = DEFAULT_GROWTH_FACTOR;
    ret.ptr = Ft_Alloc(valueSize * initialCap);
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

    memset(ret.ptr, 0, valueSize * initialCap);
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

void FtArr_Resize(Ft_Arr *arr, Ft_Uint newCap) {
    if (newCap < arr->len) {
        FtErr_Set(FT_ERR_ARR_TOO_SMALL);
        return;
    }

    Ft_Uint newBytes = newCap * arr->valueSize;
    uint8_t *newArr = Ft_Alloc(newBytes);
    memcpy(newArr, arr->ptr, newBytes);
    Ft_Free(arr->ptr);
    arr->ptr = newArr;
    arr->cap = newCap;
}

void FtArr_Append(Ft_Arr *arr, void *value) {
    if (arr->len == 0) {
        uint8_t *arrPtr = arr->ptr;
        memcpy(arrPtr, value, arr->valueSize);
        arr->len++;
        return;
    }

    Ft_Uint totalBytes = arr->len * arr->valueSize;
    if (arr->len == arr->cap) {
        FtArr_Resize(arr, (Ft_Uint) (arr->len * arr->growthFactor));
    }

    uint8_t *arrPtr = arr->ptr;
    memcpy(arrPtr + totalBytes, value, arr->valueSize);
    arr->len++;
}

Ft_Ptr FtArr_Pop(Ft_Arr *arr) {
    arr->len--;
    uint8_t *arrPtr = arr->ptr;
    Ft_Ptr ret = arrPtr + arr->len * arr->valueSize;
    if (arr->len / 4 >= arr->cap) {
        FtArr_Resize(arr, arr->len / 2);
    }
    return ret;
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
