//
// Created by Vincent K on 2019-05-11.
//

#ifndef FASTTALK_FST_STDLIB_H
#define FASTTALK_FST_STDLIB_H

#include <stdint.h>
#include <stddef.h>

#define PARANOID_ERRORS 1

#define Ft_CStr char*
#define Ft_Ptr void*
#define FT_Int int64_t
#define Ft_Uint uint32_t
#define Ft_Float float
#define Ft_Byte uint8_t
#define Ft_Err uint32_t
#define Ft_Size size_t

#define Ft_InvalidSize UINT32_MAX
#define Ft_EnvDefaultLen 32

#define FT_NO_ERR 0
#define FT_ERR_ARR_TOO_SMALL 1
#define FT_ERR_ARR_DEFAULT_SIZE 2
#define FT_ERR_ARR_IDX_OUT_OF_RANGE 3
#define FT_ERR_ALLOC 4
#define FT_ERR_ARR_SET_SIZE_MISMATCH 5
#define FT_ERR_BAD_MSG 6
#define FT_ERR_ARR_BAD_SIZE 7
#define FT_ERR_REDEFINED_CLASS 8

#define FtObj_HEAD Ft_Obj obj_base;

#ifdef __cplusplus
#define ft_c_open extern "C" {}
#define ft_c_close }
#else
#define ft_c_open
#define ft_c_close
#endif

ft_c_open
typedef enum {
    FtTy_Uint,
    FtTy_Float,
    FtTy_String,
    FtTy_Fn,
    FtTy_Object
} Ft_Type;

Ft_Ptr Ft_Alloc(Ft_Uint b);
void Ft_Free(Ft_Ptr p);

Ft_Err Ft_ClearError();
Ft_Err FtErr_Set(Ft_Err e);
Ft_Err Ft_GetError();
ft_c_close

#endif //FASTTALK_FST_STDLIB_H
