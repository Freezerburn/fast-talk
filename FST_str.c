//
// Created by Vincent K on 2019-05-16.
//

#include <string.h>

#include "FST_str.h"

Ft_Str FST_MkStr2(Ft_CStr str, Ft_Uint len) {
    Ft_Str ret;
    ret.val = str;
    ret.len = len;
    return ret;
}

Ft_Str FtStr_Init(Ft_CStr str) {
    return FST_MkStr2(str, strlen(str));
}

Ft_Byte FtStr_Eq(Ft_Str l, Ft_Str r) {
    if (l.val == r.val) {
        return 1;
    }
    return l.len == r.len && strncmp(l.val, r.val, l.len) == 0;
}
