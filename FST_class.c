#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FST_class.h"

FST_Class* FST_MkClass(FST_Interp *interp, FST_Str name) {
    FST_Class *existing = FST_InterpFindCls(interp, name);
    if (existing != NULL) {
        return existing;
    }

    FST_Class *ret = FST_Alloc(sizeof(FST_Class));
#if PARANOID_ERRORS
    FST_ErrDef err = FST_GetErr();
    if (err) {
        switch (err) {
            case FST_ERR_ALLOC:
                printf("[ERROR] [FST_MkClass] Alloc failed due to malloc returning null.\n");
                break;
            default:
                printf("[ERROR] [FST_MkClass] Alloc failed for unknown reasons.\n");
                break;
        }
        return NULL;
    }
#endif

    ret->name = name;
    ret->handlers = FST_MkArray1(sizeof(FST_MsgHandler));
    FST_InterpAddCls(interp, ret);
    return ret;
}

void FST_ClsAddMsgHandler(FST_Class *cls, FST_Str name, FST_MsgCallbackDef(fn)) {
    FST_MsgHandler handler;
    handler.name = name;
    handler.fn = fn;
    FST_ArrPush(&cls->handlers, &handler);

#if PARANOID_ERRORS
    FST_ErrDef err = FST_GetErr();
    if (err) {
        switch (err) {
            case FST_ERR_ARR_DEFAULT_SIZE:
                printf("[ERROR] [FST_ClsAddMsgHandler] Unable to push handler into array: no default size set.\n");
                break;
            default:
                printf("[ERROR] [FST_ClsAddMsgHandler] Unable to push handler into array for unknown reasons.\n");
                break;
        }
    }
#endif
}

FST_MsgHandler FST_ClsFindMsgHandler(FST_Class *cls, FST_Str name) {
    for (FST_UintDef i = 0; i < cls->handlers.len; i++) {
        FST_MsgHandler *handler = FST_ArrGet(&cls->handlers, i);

#if PARANOID_ERRORS
        FST_ErrDef err = FST_GetErr();
        if (err) {
            switch (err) {
                case FST_ERR_ARR_DEFAULT_SIZE:
                    printf("[ERROR] [FST_ClsAddMsgHandler] Unable to iterator over message handler array: index out of range.\n");
                    break;
                default:
                    printf("[ERROR] [FST_ClsAddMsgHandler] Unable to iterator over message handler array for unknown reasons.\n");
                    break;
            }
            return FST_MkNullMsgHandler();
        }
#endif

        if (name.len == handler->name.len && strncmp(name.val, handler->name.val, name.len) == 0) {
            return *handler;
        }
    }

    return FST_MkNullMsgHandler();
}
