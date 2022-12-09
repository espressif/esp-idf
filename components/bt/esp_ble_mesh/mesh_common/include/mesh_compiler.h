/*
 * Copyright (c) 2010-2014,2017 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_COMPILER_H_
#define _BLE_MESH_COMPILER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ___in_section(a, b, c)

#define __in_section(a, b, c) ___in_section(a, b, c)

#define __in_section_unique(seg) ___in_section(seg, __FILE__, __COUNTER__)

#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(x)    __attribute__((__aligned__(x)))
#endif

#ifndef __used
#define __used          __attribute__((__used__))
#endif

#ifndef ARG_UNUSED
#define ARG_UNUSED(x)   (void)(x)
#endif

#ifndef popcount
#define popcount(x)     __builtin_popcount(x)
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__((always_inline))
#endif


/*
 * This is meant to be used in conjunction with __in_section() and similar
 * where scattered structure instances are concatened together by the linker
 * and walked by the code at run time just like a contiguous array of such
 * structures.
 *
 * Assemblers and linkers may insert alignment padding by default whose
 * size is larger than the natural alignment for those structures when
 * gathering various section segments together, messing up the array walk.
 * To prevent this, we need to provide an explicit alignment not to rely
 * on the default that might just work by luck.
 *
 * Alignment statements in  linker scripts are not sufficient as
 * the assembler may add padding by itself to each segment when switching
 * between sections within the same file even if it merges many such segments
 * into a single section in the end.
 */
#ifndef Z_DECL_ALIGN
#define Z_DECL_ALIGN(type) __aligned(__alignof(type)) type
#endif

/*
 * Convenience helper combining __in_section() and Z_DECL_ALIGN().
 * The section name is the struct type prepended with an underscore.
 * The subsection is "static" and the subsubsection is the variable name.
 */
#ifndef Z_STRUCT_SECTION_ITERABLE
#define Z_STRUCT_SECTION_ITERABLE(struct_type, name) \
        Z_DECL_ALIGN(struct struct_type) name
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_COMPILER_H_ */
