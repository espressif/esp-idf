/*
 * _default_types implementation for xtensa lx106 arch
 *
 * Simplified version of generic _default_types.h, ignores gcc
 * built-in standard types.
 */

#ifndef _MACHINE__DEFAULT_TYPES_H
#define _MACHINE__DEFAULT_TYPES_H

#include <sys/features.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char __int8_t ;
typedef unsigned char __uint8_t ;
#define ___int8_t_defined 1

typedef signed short __int16_t;
typedef unsigned short __uint16_t;
#define ___int16_t_defined 1

typedef signed int __int32_t;
typedef unsigned int __uint32_t;
#define ___int32_t_defined 1

typedef signed long long __int64_t;
typedef unsigned long long __uint64_t;
#define ___int64_t_defined 1

typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
#define ___int_least8_t_defined

typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
#define ___int_least16_t_defined

typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
#define ___int_least32_t_defined

typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;
#define ___int_least64_t_defined

typedef __INTPTR_TYPE__ __intptr_t;
typedef __UINTPTR_TYPE__ __uintptr_t;

#ifdef __cplusplus
}
#endif

#endif /* _MACHINE__DEFAULT_TYPES_H */
