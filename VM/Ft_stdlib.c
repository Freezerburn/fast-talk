#include <stdlib.h>
#include <stdio.h>

#include "Ft_stdlib.h"

static Ft_Err currentErrorCode = 0;

Ft_Ptr Ft_Alloc(Ft_Uint b) {
    Ft_Ptr ret = malloc(b);

#if PARANOID_ERRORS
    if (ret == NULL) {
        printf("[ERROR] [Ft_Alloc] malloc failed to return a non-NULL pointer.\n");
        FtErr_Set(FT_ERR_ALLOC);
    }
#endif

    return ret;
}

void Ft_Free(Ft_Ptr p) {
    free(p);
}

Ft_Err Ft_ClearError() {
    Ft_Err last = currentErrorCode;
    currentErrorCode = FT_NO_ERR;
    return last;
}

Ft_Err FtErr_Set(Ft_Err e) {
    printf("err set: %d\n", e);
    Ft_Err last = currentErrorCode;
    currentErrorCode = e;
    return last;
}

Ft_Err Ft_GetError() {
    return currentErrorCode;
}
