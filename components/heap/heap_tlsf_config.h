/*
** Two Level Segregated Fit memory allocator, version 3.1.
** Written by Matthew Conte
**	http://tlsf.baisoku.org
**
** Based on the original documentation by Miguel Masmano:
**	http://www.gii.upv.es/tlsf/main/docs
**
** This implementation was written to the specification
** of the document, therefore no GPL restrictions apply.
**
** Copyright (c) 2006-2016, Matthew Conte
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the copyright holder nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL MATTHEW CONTE BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifdef ESP_PLATFORM

#include "soc/soc.h"

#if !CONFIG_SPIRAM
#define TLSF_MAX_POOL_SIZE (SOC_DIRAM_DRAM_HIGH - SOC_DIRAM_DRAM_LOW)
#else
#define TLSF_MAX_POOL_SIZE SOC_EXTRAM_DATA_SIZE
#endif

enum tlsf_config
{
	/* log2 of number of linear subdivisions of block sizes. Larger
	** values require more memory in the control structure. Values of
	** 4 or 5 are typical.
	*/
	SL_INDEX_COUNT_LOG2  = 5,

	/* All allocation sizes and addresses are aligned to 4 bytes. */
	ALIGN_SIZE_LOG2 = 2,
	ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

	/*
	** We support allocations of sizes up to (1 << FL_INDEX_MAX) bits.
	** However, because we linearly subdivide the second-level lists, and
	** our minimum size granularity is 4 bytes, it doesn't make sense to
	** create first-level lists for sizes smaller than SL_INDEX_COUNT * 4,
	** or (1 << (SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
	** trying to split size ranges into more slots than we have available.
	** Instead, we calculate the minimum threshold size, and place all
	** blocks below that size into the 0th first-level list.
	*/

	/* Tunning the first level, we can reduce TLSF pool overhead
	 * in exchange of manage a pool smaller than 4GB
	 */
	#if (TLSF_MAX_POOL_SIZE <= (256 * 1024))
	FL_INDEX_MAX = 18, //Each pool can have up 256KB
	#elif (TLSF_MAX_POOL_SIZE <= (512 * 1024))
	FL_INDEX_MAX = 19, //Each pool can have up 512KB
	#elif (TLSF_MAX_POOL_SIZE <= (1 * 1024 * 1024))
	FL_INDEX_MAX = 20, //Each pool can have up 1MB
	#elif (TLSF_MAX_POOL_SIZE <= (2 * 1024 * 1024))
	FL_INDEX_MAX = 21, //Each pool can have up 2MB
	#elif (TLSF_MAX_POOL_SIZE <= (4 * 1024 * 1024))
	FL_INDEX_MAX = 22, //Each pool can have up 4MB
	#elif (TLSF_MAX_POOL_SIZE <= (8 * 1024 * 1024))
	FL_INDEX_MAX = 23, //Each pool can have up 8MB
	#elif (TLSF_MAX_POOL_SIZE <= (16 * 1024 * 1024))
	FL_INDEX_MAX = 24, //Each pool can have up 16MB
	#else
	#error "Higher TLSF pool sizes should be added for this new config"
	#endif

	SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
	FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
	FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

	SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
};
#else
enum tlsf_config
{
	//Specific configuration for host test.

	/* log2 of number of linear subdivisions of block sizes. Larger
	** values require more memory in the control structure. Values of
	** 4 or 5 are typical.
	*/
	SL_INDEX_COUNT_LOG2  = 5,

	/* All allocation sizes and addresses are aligned to 4 bytes. */
	ALIGN_SIZE_LOG2 = 2,
	ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

	/* Tunning the first level, we can reduce TLSF pool overhead
	 * in exchange of manage a pool smaller than 4GB
	 */
	FL_INDEX_MAX = 30,

	SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
	FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
	FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

	SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
};
#endif
