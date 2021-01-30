#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FST_class.h"

Ft_Cls* FtCls_Init(Ft_Interp *interp, Ft_Str name) {
    Ft_Cls *existing = FST_InterpFindCls(interp, name);
    if (existing != NULL) {
        return existing;
    }

    Ft_Cls *ret = Ft_Alloc(sizeof(Ft_Cls));
#if PARANOID_ERRORS
    Ft_Err err = Ft_GetError();
    if (err) {
        switch (err) {
            case FT_ERR_ALLOC:
                printf("[ERROR] [FtCls_Init] Alloc failed due to malloc returning null.\n");
                break;
            default:
                printf("[ERROR] [FtCls_Init] Alloc failed for unknown reasons.\n");
                break;
        }
        return NULL;
    }
#endif

    ret->name = name;
    ret->handlers = FtArr_Init(sizeof(Ft_MsgHandler), 0);
    ret->constructor = NULL;
    FST_InterpAddCls(interp, ret);
    return ret;
}

void FtCls_AddMsgHandler(Ft_Cls *cls, Ft_Str name, Ft_MsgCallback fn) {
    Ft_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FtArr_Append(&cls->handlers, &handler);

#if PARANOID_ERRORS
    Ft_Err err = Ft_GetError();
    if (err) {
        switch (err) {
            case FT_ERR_ARR_DEFAULT_SIZE:
                printf("[ERROR] [FtCls_AddMsgHandler] Unable to push handler into array: no default size set.\n");
                break;
            default:
                printf("[ERROR] [FtCls_AddMsgHandler] Unable to push handler into array for unknown reasons.\n");
                break;
        }
    }
#endif
}

Ft_MsgHandler FtCls_FindMsgHandler(Ft_Cls *cls, Ft_Str name) {
    for (Ft_Uint i = 0; i < cls->handlers.len; i++) {
        Ft_MsgHandler *handler = FtArr_Get(&cls->handlers, i);

#if PARANOID_ERRORS
        Ft_Err err = Ft_GetError();
        if (err) {
            switch (err) {
                case FT_ERR_ARR_DEFAULT_SIZE:
                    printf("[ERROR] [FtCls_AddMsgHandler] Unable to iterate over message handler array: index out of range.\n");
                    break;
                case FT_ERR_ARR_IDX_OUT_OF_RANGE:
                    printf("[ERROR] [FtCls_AddMsgHandler] Idx %d out of range of handlers len %d.\n", i, cls->handlers.len);
                    break;
                default:
                    printf("[ERROR] [FtCls_AddMsgHandler] Unable to iterate over message handler array for unknown reason: %d.\n", err);
                    break;
            }
            return FtMsgHandler_InitNull();
        }
#endif

        if (name.len == handler->name.len && strncmp(name.val, handler->name.val, name.len) == 0) {
            return *handler;
        }
    }

    return FtMsgHandler_InitNull();
}
