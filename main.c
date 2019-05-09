#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FST_StrDef char*
#define FST_PtrDef void*
#define FST_UintDef uint32_t
#define FST_FloatDef float
#define FST_EnvDefaultLen 32

enum FST_Type {
    FST_TypeUint,
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
    struct _FST_Env *parent;
    uint8_t *arr;
    size_t *sizes;
    FST_UintDef len;
    FST_UintDef lenBytes;
    FST_UintDef cap;
} FST_Env;

typedef struct _FST_Interp {
    FST_Env env;
} FST_Interp;

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
        default:
            printf("Unhandled FST_Type case: %d\n", t);
            exit(1);
    }
}

FST_EnvVal* FST_MkEnvVal(FST_Str name, FST_PtrDef v, enum FST_Type t) {
    size_t typeSize = FST_ValLenBytes(t);
    size_t totalSize = sizeof(FST_EnvVal) + typeSize;
    FST_EnvVal *val = (FST_EnvVal*) malloc(totalSize);

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

void FST_InitEnv(FST_Env *env, FST_Env *parent, FST_UintDef len) {
    if (len == 0) {
        len = FST_EnvDefaultLen;
    }
    FST_UintDef size = sizeof(FST_EnvVal) * len;
    env->parent = parent;
    env->arr = malloc(size);
    memset(env->arr, 0, size);
    env->sizes = malloc(sizeof(FST_UintDef) * len);
    memset(env->sizes, 0, sizeof(FST_UintDef) * len);
    env->len = 0;
    env->lenBytes = 0;
    env->cap = size;
}

FST_Env* FST_MkEnv2(FST_Env *parent, FST_UintDef len) {
    if (len == 0) {
        len = FST_EnvDefaultLen;
    }
    FST_Env *ret = malloc(sizeof(FST_Env));
    FST_InitEnv(ret, parent, len);
    return ret;
}

FST_Env* FST_MkEnv1(FST_UintDef len) {
    if (len == 0) {
        len = FST_EnvDefaultLen;
    }
    FST_Env *ret = malloc(sizeof(FST_Env));
    FST_InitEnv(ret, NULL, len);
    return ret;
}

FST_Env* FST_MkEnv() {
    return FST_MkEnv1(FST_EnvDefaultLen);
}

FST_Env* FST_EnvChild(FST_Env *parent) {
    FST_Env *child = FST_MkEnv2(parent, FST_EnvDefaultLen);
    child->parent = parent;
    return child;
}

void FST_EnvResizeBigger(FST_Env *env) {
    FST_UintDef newCap = sizeof(FST_EnvVal) * env->cap * 2;
    FST_EnvVal *newArr = malloc(newCap);
    memcpy(newArr, env->arr, env->lenBytes);
    free(env->arr);
    env->arr = newArr;
    env->cap = newCap;
}

void FST_EnvAppend3(FST_Env *env, FST_Str name, FST_PtrDef v, enum FST_Type typ) {
    // TODO: Append values to env without having to separately allocate a val.
}

void FST_EnvAppend(FST_Env *env, FST_EnvVal *val) {
    size_t typBytes = FST_ValLenBytes(val->typ);
    size_t totalBytes = sizeof(FST_EnvVal) + typBytes;

    while (env->lenBytes + totalBytes >= env->cap) {
        FST_EnvResizeBigger(env);
    }

    uint8_t *arrByte = (uint8_t*) env->arr;
    memcpy(arrByte + env->lenBytes, val, totalBytes);
    FST_EnvVal *newVal = (FST_EnvVal*)(arrByte + env->lenBytes);
    newVal->ptr = ((uint8_t*) newVal) + sizeof(FST_EnvVal);
    env->sizes[env->len++] = totalBytes;
    env->lenBytes += totalBytes;
}

FST_EnvVal* FST_EnvFindValByName(FST_Env *env, FST_Str name) {
    FST_Env *cur = env;
    while (cur != NULL) {
        uint8_t *arrBytes = (uint8_t*) cur->arr;
        for (FST_UintDef i = 0, ib = 0; i < cur->len; i++, ib += cur->sizes[i]) {
            FST_EnvVal* val = (FST_EnvVal*) (arrBytes + ib);
            if (val->name.len != name.len) {
                continue;
            } else if (strncmp(name.val, val->name.val, name.len) == 0) {
                return val;
            }
        }
        cur = cur->parent;
    }
    return NULL;
}

FST_Interp* FST_MkInterp() {
    FST_Interp *interp = (FST_Interp*) malloc(sizeof(FST_Interp));
    FST_InitEnv(&interp->env, NULL, FST_EnvDefaultLen);
    return interp;
}

void FST_PrnVal(FST_EnvVal *e) {
    switch (e->typ) {
        case FST_TypeUint:
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
    FST_Str test = FST_MkStr("test");
    FST_EnvVal *e = FST_MkEnvVal(test, &v, FST_TypeUint);
    FST_EnvAppend(&i->env, e);
    FST_DelEnvVal(e);

    FST_EnvVal *e2 = FST_EnvFindValByName(&i->env, FST_MkStr("test"));
    if (e2 == NULL) {
        printf("FAIL 2: Did not find\n");
        exit(1);
    }
    FST_PrnVal(e2);

    FST_Env *childTest = FST_EnvChild(&i->env);
    FST_PrnVal(e2);

    e2 = FST_EnvFindValByName(childTest, test);
    if (e2 == NULL) {
        printf("FAIL: Did not find\n");
        exit(1);
    }
    FST_PrnVal(e2);

    return 0;
}