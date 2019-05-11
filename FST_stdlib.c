#include <stdlib.h>

#include "FST_stdlib.h"

FST_PtrDef FST_Alloc(FST_UintDef bytes) {
    return malloc(bytes);
}

void FST_Dealloc(FST_PtrDef ptr) {
    free(ptr);
}
