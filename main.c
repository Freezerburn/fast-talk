#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FST_StrDef char*
#define FST_PtrDef void*
#define FST_UintDef uint32_t
#define FST_FloatDef float
#define FST_EnvDefaultSize 32

enum FST_Type {
    FST_TypeInt,
    FST_TypeFloat,
    FST_TypeStr,
    FST_TypeFn
};

typedef struct _FST_Str {
    FST_StrDef val;
    FST_UintDef len;
} FST_Str;

typedef struct _FST_EnvVal {
    FST_Str name;
    enum FST_Type typ;
    FST_PtrDef ptr;
} FST_EnvVal;

typedef struct _FST_Env {
    FST_EnvVal *arr;
    size_t *sizes;
    FST_UintDef len;
    FST_UintDef lenBytes;
    FST_UintDef cap;
} FST_Env;

typedef struct _FST_Interp {
    struct _FST_Interp *parent;
    FST_Env env;
} FST_Interp;

size_t FST_ValLenBytes(const enum FST_Type t) {
    switch (t) {
        case FST_TypeInt:
            return sizeof(FST_UintDef);
        case FST_TypeFloat:
            return sizeof(FST_FloatDef);
        case FST_TypeStr:
            return sizeof(FST_StrDef);
        case FST_TypeFn:
            return sizeof(int*);
        default:
            printf("Unhandled FST_Type case: %d\n", t);
    }
}

FST_EnvVal* Fst_MkEnvVal(FST_Str name, FST_PtrDef v, enum FST_Type t) {
    size_t typeSize = FST_ValLenBytes(t);
    FST_EnvVal *val = malloc(sizeof(FST_EnvVal) + typeSize);
    val->ptr = ((uint8_t*) val) + sizeof(FST_EnvVal);
    memcpy(val->ptr, v, typeSize);
    val->name = name;
    val->typ = t;
    return val;
}

FST_Str FST_MkStr2(FST_StrDef str, FST_UintDef len) {
    FST_Str ret;
    ret.val = str;
    ret.len = len;
    return ret;
}

FST_Str FST_MkStr(FST_StrDef str) {
    return FST_MkStr2(str, strlen(str));
}

void FST_DelEnvVal(FST_EnvVal *e) {
    free(e);
}

void FST_InitEnv(FST_Env *env, FST_UintDef size) {
    if (size == 0) {
        size = FST_EnvDefaultSize;
    }
    env->arr = malloc(sizeof(FST_EnvVal) * size);
    memset(env->arr, 0, sizeof(FST_EnvVal) * size);
    env->sizes = malloc(sizeof(FST_UintDef) * size);
    memset(env->sizes, 0, sizeof(FST_UintDef) * size);
    env->len = 0;
    env->lenBytes = 0;
    env->cap = size;
}

FST_Env* FST_MkEnv1(FST_UintDef size) {
    FST_Env *ret;
    if (size == 0) {
        size = FST_EnvDefaultSize;
    }
    ret = malloc(sizeof(FST_Env));
    FST_InitEnv(ret, size);
    return ret;
}

FST_Env* FST_MkEnv() {
    return FST_MkEnv1(FST_EnvDefaultSize);
}

void FST_EnvResizeBigger(FST_Env *env) {
    FST_UintDef newCap = sizeof(FST_EnvVal) * env->cap * 2;
    FST_EnvVal *newArr = malloc(newCap);
    memcpy(newArr, env->arr, env->lenBytes);
    free(env->arr);
    env->arr = newArr;
    env->cap = newCap;
}

void FST_EnvAppend(FST_Env *env, FST_EnvVal *val) {
    size_t typBytes = FST_ValLenBytes(val->typ);
    size_t totalBytes = sizeof(FST_EnvVal) + typBytes;

    while (env->lenBytes + totalBytes >= env->cap) {
        FST_EnvResizeBigger(env);
    }

    uint8_t *arrByte = (uint8_t*) env->arr;
    memcpy(arrByte + env->lenBytes, val, totalBytes);
    env->sizes[env->len++] = totalBytes;
    env->lenBytes += totalBytes;
}

FST_Interp* FST_MkInterp() {
    FST_Interp *interp = (FST_Interp*) malloc(sizeof(FST_Interp));
    interp->parent = NULL;
    FST_InitEnv(&interp->env, FST_EnvDefaultSize);
    return interp;
}

FST_EnvVal* FST_InterpFindValByName(FST_Interp *interp, FST_Str name) {
    for (FST_UintDef i = 0, ib = 0; i < interp->env.len; i++, ib += interp->env.sizes[i]) {
        uint8_t *arrBytes = (uint8_t*) interp->env.arr;
        FST_EnvVal* val = (FST_EnvVal*) (arrBytes + ib);
        if (val->name.len != name.len) {
            continue;
        } else if (strncmp(name.val, val->name.val, name.len) == 0) {
            return val;
        }
    }
    return NULL;
}

void FST_PrnVal(FST_EnvVal *e) {
    switch (e->typ) {
        case FST_TypeInt:
            printf("INT(%d)\n", *(FST_UintDef*)e->ptr);
            break;
        case FST_TypeFloat:
            printf("FLOAT(%f)\n", *(FST_FloatDef*)e->ptr);
            break;
        case FST_TypeStr:
            printf("STR(%s)\n", (FST_StrDef)e->ptr);
            break;
        case FST_TypeFn:
            printf("FUNC(%x)\n", *(unsigned int*)e->ptr);
            break;
    }
}

FST_Interp* FST_CpInterp(FST_Interp *i) {
    FST_Interp *in = malloc(sizeof(FST_Interp));
    memcpy(in, i, sizeof(FST_Interp));
    return in;
}

void FST_DelInterp(FST_Interp *i) {
    free(i);
}

int main() {
    FST_Interp *i = FST_MkInterp();
    FST_UintDef v = 10;
    FST_EnvVal *e = Fst_MkEnvVal(FST_MkStr("test"), &v, FST_TypeInt);
    FST_EnvAppend(&i->env, e);
    FST_DelEnvVal(e);

    e = FST_InterpFindValByName(i, FST_MkStr("test"));
    if (e == NULL) {
        printf("FAIL: Did not find\n");
        exit(1);
    }
    FST_PrnVal(e);

    return 0;
}