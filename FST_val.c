#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FST_val.h"
#include "FST_object.h"

FST_Val* FST_MkVal(enum FST_Type typ, FST_PtrDef v) {
    FST_Val *ret = FST_Alloc(sizeof(FST_Val) + FST_ValLenBytes(typ));
    FST_InitVal(ret, typ, v);
    return ret;
}


void FST_InitVal(FST_Val *val, enum FST_Type typ, FST_PtrDef ptr) {
    size_t typeSize = FST_ValLenBytes(typ);
    val->typ = typ;
    memcpy(val->ptr, ptr, typeSize);
}

void FST_DelVal(FST_Val *val) {
    FST_Dealloc(val);
}

size_t FST_ValLenBytes(const enum FST_Type t) {
    switch (t) {
        case FST_TypeUint:
            return sizeof(FST_UintDef);
        case FST_TypeFloat:
            return sizeof(FST_FloatDef);
        case FST_TypeStr:
            return sizeof(FST_StrDef);
        case FST_TypeFn:
            return sizeof(int*);
        case FST_TypeObject:
            return sizeof(FST_Object);
        default:
            printf("Unhandled FST_Type case: %d\n", t);
            exit(1);
    }
}
