#include <stdlib.h>
#include <stdio.h>

#include "FST_stdlib.h"

static FST_ErrDef currentErrorCode = 0;

FST_PtrDef FST_Alloc(FST_UintDef bytes) {
    FST_PtrDef ret = malloc(bytes);

#if PARANOID_ERRORS
    if (ret == NULL) {
        printf("[ERROR] [FST_Alloc] malloc failed to return a non-NULL pointer.\n");
        FST_SetErr(FST_ERR_ALLOC);
    }
#endif

    return ret;
}

void FST_Dealloc(FST_PtrDef ptr) {
    free(ptr);
}

FST_ErrDef FST_ResetErr() {
    FST_ErrDef last = currentErrorCode;
    currentErrorCode = FST_NO_ERR;
    return last;
}

FST_ErrDef FST_SetErr(FST_ErrDef errValue) {
    FST_ErrDef last = currentErrorCode;
    currentErrorCode = errValue;
    return last;
}

FST_ErrDef FST_GetErr() {
    return currentErrorCode;
}
