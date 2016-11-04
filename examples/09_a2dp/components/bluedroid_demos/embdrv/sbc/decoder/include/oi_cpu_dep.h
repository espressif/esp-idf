/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
 *  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#ifndef _OI_CPU_DEP_H
#define _OI_CPU_DEP_H
/**
 * @file
 * This file contains definitions for characteristics of the target CPU and
 * compiler, including primitive data types and endianness.
 *
 * This file defines the byte order and primitive data types for various
 * CPU families. The preprocessor symbol 'CPU' must be defined to be an
 * appropriate value or this header will generate a compile-time error.
 *
 * @note The documentation for this header file uses the x86 family of processors
 * as an illustrative example for CPU/compiler-dependent data type definitions.
 * Go to the source code of this header file to see the details of primitive type
 * definitions for each platform.
 *
 * Additional information is available in the @ref data_types_docpage section.
 */

/**********************************************************************************
  $Revision: #1 $
***********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

/** @name Definitions indicating family of target OI_CPU_TYPE
 *  @{
 */

#define OI_CPU_X86         1 /**< x86 processor family */
#define OI_CPU_ARM         2 /**< ARM processor family.
                                  @deprecated Use #OI_CPU_ARM7_LEND or
                                  #OI_CPU_ARM7_BEND. */
#define OI_CPU_ARC         3 /**< ARC processor family.
                                  @deprecated Use #OI_CPU_ARC_LEND or
                                  #OI_CPU_ARC_BEND. */
#define OI_CPU_SH3         4 /**< Hitachi SH-3 processor family */
#define OI_CPU_H8          5 /**< Hitachi H8 processor family */
#define OI_CPU_MIPS        6 /**< MIPS processor family */
#define OI_CPU_SPARC       7 /**< SPARC processor family */
#define OI_CPU_M68000      8 /**< Motorola M68000 processor family */
#define OI_CPU_PPC         9 /**< PowerPC (PPC) processor family */
#define OI_CPU_SH4_7750   10 /**< Hitachi SH7750 series in SH-4 processor family */
#define OI_CPU_SH2        11 /**< Hitachi SH-2 processor family */
#define OI_CPU_ARM7_LEND  12 /**< ARM7, little-endian */
#define OI_CPU_ARM7_BEND  13 /**< ARM7, big-endian */
#define OI_CPU_GDM1202    14 /**< GCT GDM1202 */
#define OI_CPU_ARC_LEND   15 /**< ARC processor family, little-endian */
#define OI_CPU_ARC_BEND   16 /**< ARC processor family, big-endian */
#define OI_CPU_M30833F    17 /**< Mitsubishi M308 processor family */
#define OI_CPU_CR16C      18 /**< National Semiconductor 16 bit processor family */
#define OI_CPU_M64111     19 /**< Renesas M64111 processor (M32R family) */
#define OI_CPU_ARMV5_LEND 20 //*< ARM5, little-endian */

#define OI_CPU_TYPE 12

#ifndef OI_CPU_TYPE
    #error "OI_CPU_TYPE type not defined"
#endif

/**@}*/


/** @name Definitions indicating byte-wise endianness of target CPU
 *  @{
 */

#define OI_BIG_ENDIAN_BYTE_ORDER    0  /**< Multiple-byte values are stored in memory beginning with the most significant byte at the lowest address.  */
#define OI_LITTLE_ENDIAN_BYTE_ORDER 1  /**< Multiple-byte values are stored in memory beginning with the least significant byte at the lowest address. */

/**@}*/


/** @name  CPU/compiler-independent primitive data type definitions
 *  @{
 */

typedef int             OI_BOOL;  /**< Boolean values use native integer data type for target CPU. */
typedef int             OI_INT;   /**< Integer values use native integer data type for target CPU. */
typedef unsigned int    OI_UINT;  /**< Unsigned integer values use native unsigned integer data type for target CPU. */
typedef unsigned char   OI_BYTE;  /**< Raw bytes type uses native character data type for target CPU. */

/**@}*/



/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_X86

#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER  /**< x86 platform byte ordering is little-endian */

/** @name CPU/compiler-dependent primitive data type definitions for x86 processor family
 *  @{
 */
typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for x86 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for x86 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for x86 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for x86 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for x86 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for x86 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARM
/* This CPU type is deprecated (removed from use). Instead, use OI_CPU_ARM7_LEND or OI_CPU_ARM7_BEND for
   little-endian or big-endian configurations of the ARM7, respectively. */
#error OI_CPU_ARM is deprecated
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARC
/* This CPU type is deprecated (removed from use). Instead, use OI_CPU_ARC_LEND or OI_CPU_ARC_BEND for
   little-endian or big-endian configurations of the ARC, respectively. */
#error OI_CPU_ARC is deprecated
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SH3
/* The Hitachi SH C compiler defines _LIT or _BIG, depending on the endianness
    specified to the compiler on the command line. */
#if defined(_LIT)
    #define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER  /**< If _LIT is defined, SH-3 platform byte ordering is little-endian. */
#elif defined(_BIG)
    #define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER     /**< If _BIG is defined, SH-3 platform byte ordering is big-endian. */
#else
    #error SH compiler endianness undefined
#endif

/** @name CPU/compiler-dependent primitive data type definitions for SH-3 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for SH-3 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for SH-3 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for SH-3 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for SH-3 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for SH-3 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for SH-3 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif
/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SH2

#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER /**< SH-2 platform byte ordering is big-endian. */

/** @name  CPU/compiler-dependent primitive data type definitions for SH-2 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for SH-2 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for SH-2 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for SH-2 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for SH-2 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for SH-2 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for SH-2 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif
/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_H8
#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER
#error basic types not defined
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_MIPS
#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER
/** @name  CPU/compiler-dependent primitive data type definitions for MIPS processor family
 *  @{
 */
typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARM7 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARM7 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARM7 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARM7 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARM7 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARM7 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SPARC
#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER
#error basic types not defined
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_M68000
#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER  /**< M68000 platform byte ordering is big-endian. */

/** @name  CPU/compiler-dependent primitive data type definitions for M68000 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for M68000 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for M68000 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for M68000 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for M68000 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for M68000 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for M68000 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_PPC
#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER


/** @name  CPU/compiler-dependent primitive data type definitions for PPC 8XX processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for PPC8XX processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for PPC8XX processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for PPC8XX processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for PPC8XX processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for PPC8XX processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for PPC8XX processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SH4_7750
#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER  /**< SH7750 platform byte ordering is big-endian. */

/** @name   CPU/compiler-dependent primitive data type definitions for SH7750 processor series of the SH-4 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for SH7750 SH-4 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for SH7750 SH-4 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for SH7750 SH-4 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for SH7750 SH-4 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for SH7750 SH-4 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for SH7750 SH-4 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARM7_LEND
#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** @name   little-endian CPU/compiler-dependent primitive data type definitions for the ARM7 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARM7 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARM7 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARM7 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARM7 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARM7 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARM7 processor. */

typedef void * OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARM7_BEND
#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER
/** @name   big-endian CPU/compiler-dependent primitive data type definitions for the ARM7 processor family
 *  @{
 */
typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARM7 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARM7 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARM7 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARM7 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARM7 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARM7 processor. */

typedef void * OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_GDM1202
#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER

typedef signed char     OI_INT8;   /**< 8-bit signed integer. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARC_LEND

#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** @name CPU/compiler-dependent primitive data type definitions for ARC processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARC processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARC processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARC processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARC processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARC processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARC processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARC_BEND

#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER

/** @name CPU/compiler-dependent primitive data type definitions for ARC processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARC processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARC processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARC processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARC processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARC processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARC processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_M30833F

#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** @name CPU/compiler-dependent primitive data type definitions for Mitsubishi M308 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for M308 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for M308 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for M308 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for M308 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for M308 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for M308 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_CR16C

#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** @name CPU/compiler-dependent primitive data type definitions for National Semicnductor processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for CR16C processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for CR16C processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for CR16C processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for CR16C processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for CR16C processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for CR16C processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_M64111

#define OI_CPU_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER

/** @name CPU/compiler-dependent primitive data type definitions for Renesas M32R processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for M64111 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for M64111 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for M64111 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for M64111 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for M64111 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for M64111 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARMV5_LEND
#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** @name   little-endian CPU/compiler-dependent primitive data type definitions for the ARM7 processor family
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARM7 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARM7 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARM7 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARM7 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARM7 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARM7 processor. */

typedef OI_UINT32 OI_ELEMENT_UNION; /**< Type for first element of a union to support all data types up to pointer width. */

/**@}*/

#endif

/*********************************************************************************/


#ifndef OI_CPU_BYTE_ORDER
    #error "Byte order (endian-ness) not defined"
#endif


/**@}*/

#ifdef __cplusplus
}
#endif

/*********************************************************************************/
#endif /* _OI_CPU_DEP_H */
