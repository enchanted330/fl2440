#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H


#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

/* bsd */
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

/* sysv */
typedef unsigned char unchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */
typedef __signed__ char __s8;
typedef unsigned char __u8;
typedef __signed__ short __s16;
typedef unsigned short __u16;
typedef __signed__ int __s32;
typedef unsigned int __u32;
#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64; 

typedef unsigned int __kernel_size_t;
typedef int __kernel_ssize_t;
typedef int __kernel_ptrdiff_t;
typedef long __kernel_off_t;
typedef long long __kernel_loff_t;
typedef unsigned short __kernel_dev_t;
typedef unsigned short __kernel_mode_t;


typedef __kernel_dev_t dev_t;
typedef __kernel_mode_t mode_t;
typedef __kernel_off_t off_t;
typedef __kernel_loff_t loff_t;

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef __kernel_ptrdiff_t ptrdiff_t;
#endif 

#ifndef _SIZE_T
#define _SIZE_T
typedef __kernel_size_t size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef __kernel_ssize_t ssize_t;
#endif

#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__

typedef __u8 u_int8_t;
typedef __s8 int8_t;
typedef __u16 u_int16_t;
typedef __s16 int16_t;
typedef __u32 u_int32_t;
typedef __s32 int32_t;

#endif                          /* !(__BIT_TYPES_DEFINED__) */

typedef __u8 uint8_t;
typedef __u16 uint16_t;
typedef __u32 uint32_t;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __u64 uint64_t;
typedef __u64 u_int64_t;
typedef __s64 int64_t;
#endif

#endif                          /* _LINUX_TYPES_H */
