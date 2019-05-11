#ifndef FASTTALK_FST_ARRAY_H
#define FASTTALK_FST_ARRAY_H

#include <stdint.h>

#include "FST_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct _FST_Array {
        FST_UintDef defaultValueSize;
        float growthFactor;
        FST_UintDef len;
        FST_UintDef cap;
        FST_UintDef capBytes;
        FST_UintDef *byteIdxs;
        void *ptr;
    } FST_Array;

    FST_Array FST_MkArray3(FST_UintDef valueSize, FST_UintDef initialCap, FST_FloatDef growthFactor);
    FST_Array FST_MkArray2(FST_UintDef valueSize, FST_UintDef initialCap);
    FST_Array FST_MkArray1(FST_UintDef valueSize);
    FST_Array FST_MkArray();
    void FST_DelArray(FST_Array *arr);

    void FST_ArrResize(FST_Array *arr, FST_UintDef bytes);

    void FST_ArrPush2(FST_Array *arr, FST_UintDef valueSize, FST_PtrDef value);
    void FST_ArrPush(FST_Array *arr, FST_PtrDef value);
    FST_PtrDef FST_ArrPop(FST_Array *arr);

    FST_PtrDef FST_ArrGet(FST_Array *arr, FST_UintDef idx);

#ifdef __cplusplus
}
#endif

#endif //FASTTALK_FST_ARRAY_H
