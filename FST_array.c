#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "FST_array.h"
#include "FST_stdlib.h"

#define DEFAULT_VALUE_SIZE 32
#define DEFAULT_CAP 32
#define DEFAULT_GROWTH_FACTOR 2.0f

FST_Array FST_MkArray3(FST_UintDef valueSize, FST_UintDef initialCap, FST_FloatDef growthFactor) {
    FST_Array ret;
    ret.len = 0;
    ret.cap = initialCap;
    ret.growthFactor = growthFactor;
    ret.defaultValueSize = valueSize;
    FST_PtrDef allArr;
    if (valueSize == 0) {
        ret.capBytes = DEFAULT_VALUE_SIZE * initialCap;
    } else {
        ret.capBytes = valueSize * initialCap;
    }
    allArr = FST_Alloc(sizeof(FST_UintDef) * (initialCap + 1) + ret.capBytes);
    ret.byteIdxs = allArr;
    ret.ptr = (uint8_t*)(allArr) + sizeof(FST_UintDef) * (initialCap + 1);
    memset(ret.ptr, 0, sizeof(FST_UintDef) * (initialCap + 1) + ret.capBytes);
    return ret;
}

FST_Array FST_MkArray2(FST_UintDef valueSize, FST_UintDef initialCap) {
    return FST_MkArray3(valueSize, initialCap, DEFAULT_GROWTH_FACTOR);
}

FST_Array FST_MkArray1(FST_UintDef valueSize) {
    return FST_MkArray3(valueSize, DEFAULT_CAP, DEFAULT_GROWTH_FACTOR);
}

FST_Array FST_MkArray() {
    return FST_MkArray3(0, DEFAULT_CAP, DEFAULT_GROWTH_FACTOR);
}

void FST_DelArray(FST_Array *arr) {
    FST_Dealloc(arr->byteIdxs);
    FST_Dealloc(arr->ptr);
}

void FST_ArrResize(FST_Array *arr, FST_UintDef bytes) {
    FST_UintDef totalStoredBytes = arr->byteIdxs[arr->len];
    if (bytes < totalStoredBytes) {
        // TODO: Set error
        exit(1);
    }

    FST_UintDef newSizesCap;
    if (arr->defaultValueSize == 0) {
        // TODO: Calculate average size of values stored to use to get number of sizes to store
        newSizesCap = bytes / DEFAULT_VALUE_SIZE;
    } else {
        newSizesCap = bytes / arr->defaultValueSize;
    }

    FST_UintDef sizesBytes = sizeof(FST_UintDef) * (arr->cap + 1);
    FST_UintDef totalBytes = sizesBytes + totalStoredBytes;
    FST_UintDef newSizesBytes = sizeof(FST_UintDef) * (newSizesCap + 1);
    FST_UintDef newTotalBytes = newSizesBytes + bytes;

    uint8_t *newArr = FST_Alloc(newTotalBytes);
    memcpy(newArr, arr->byteIdxs, sizesBytes);
    memcpy(newArr + newSizesBytes, arr->ptr, totalBytes);
    FST_Dealloc(arr->byteIdxs);
    arr->byteIdxs = (FST_UintDef*) newArr;
    arr->ptr = newArr + newSizesBytes;
    arr->cap = newSizesCap;
    arr->capBytes = bytes;
}

void FST_ArrPush2(FST_Array *arr, FST_UintDef valueSize, FST_PtrDef value) {
    if (arr->len == 0) {
        uint8_t *arrPtr = arr->ptr;
        memcpy(arrPtr, value, valueSize);
        arr->byteIdxs[0] = 0;
        arr->byteIdxs[1] = valueSize;
        arr->len++;
        return;
    }

    FST_UintDef totalBytes = arr->byteIdxs[arr->len];
    if (arr->len == arr->cap || totalBytes + valueSize > arr->capBytes) {
        FST_ArrResize(arr, (FST_UintDef)(arr->capBytes * arr->growthFactor));
    }

    uint8_t *arrPtr = arr->ptr;
    memcpy(arrPtr + totalBytes, value, valueSize);
    arr->len++;
    arr->byteIdxs[arr->len] = totalBytes + valueSize;
}

void FST_ArrPush(FST_Array *arr, FST_PtrDef value) {
    if (arr->defaultValueSize == 0) {
        // TODO: Set error
        exit(1);
    }
    FST_ArrPush2(arr, arr->defaultValueSize, value);
}

FST_PtrDef FST_ArrPop(FST_Array *arr) {
    arr->len--;
    uint8_t *arrPtr = arr->ptr;
    FST_PtrDef ret = arrPtr + arr->byteIdxs[arr->len];
    if (arr->len / 4 >= arr->cap) {
        FST_ArrResize(arr, arr->capBytes / 2);
    }
    return ret;
}

FST_PtrDef FST_ArrGet(FST_Array *arr, FST_UintDef idx) {
    if (idx >= arr->len) {
        // TODO: Set error
        return NULL;
    }

    uint8_t *arrPtr = arr->ptr;
    return (FST_PtrDef) (arrPtr + idx * arr->defaultValueSize);
}
