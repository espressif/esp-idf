/*
 * xtensa/config/system.h -- HAL definitions that are dependent on SYSTEM configuration
 *
 *  NOTE: The location and contents of this file are highly subject to change.
 *
 *  Source for configuration-independent binaries (which link in a
 *  configuration-specific HAL library) must NEVER include this file.
 *  The HAL itself has historically included this file in some instances,
 *  but this is not appropriate either, because the HAL is meant to be
 *  core-specific but system independent.
 */

/* Customer ID=11657; Build=0x5fe96; Copyright (c) 2000-2010 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */


#ifndef XTENSA_CONFIG_SYSTEM_H
#define XTENSA_CONFIG_SYSTEM_H

/*#include <xtensa/hal.h>*/



/*----------------------------------------------------------------------
				CONFIGURED SOFTWARE OPTIONS
  ----------------------------------------------------------------------*/

#define XSHAL_USE_ABSOLUTE_LITERALS	0	/* (sw-only option, whether software uses absolute literals) */
#define XSHAL_HAVE_TEXT_SECTION_LITERALS 1 /* Set if there is some memory that allows both code and literals.  */

#define XSHAL_ABI			XTHAL_ABI_WINDOWED	/* (sw-only option, selected ABI) */
/*  The above maps to one of the following constants:  */
#define XTHAL_ABI_WINDOWED		0
#define XTHAL_ABI_CALL0			1
/*  Alternatives:  */
/*#define XSHAL_WINDOWED_ABI		1*/	/* set if windowed ABI selected */
/*#define XSHAL_CALL0_ABI		0*/	/* set if call0 ABI selected */

#define XSHAL_CLIB			XTHAL_CLIB_NEWLIB	/* (sw-only option, selected C library) */
/*  The above maps to one of the following constants:  */
#define XTHAL_CLIB_NEWLIB		0
#define XTHAL_CLIB_UCLIBC		1
#define XTHAL_CLIB_XCLIB		2
/*  Alternatives:  */
/*#define XSHAL_NEWLIB			1*/	/* set if newlib C library selected */
/*#define XSHAL_UCLIBC			0*/	/* set if uCLibC C library selected */
/*#define XSHAL_XCLIB			0*/	/* set if Xtensa C library selected */

#define XSHAL_USE_FLOATING_POINT	1

#define XSHAL_FLOATING_POINT_ABI	0

/*  SW workarounds enabled for HW errata:  */

/*----------------------------------------------------------------------
				DEVICE ADDRESSES
  ----------------------------------------------------------------------*/

/*
 *  Strange place to find these, but the configuration GUI
 *  allows moving these around to account for various core
 *  configurations.  Specific boards (and their BSP software)
 *  will have specific meanings for these components.
 */

/*  I/O Block areas:  */
#define XSHAL_IOBLOCK_CACHED_VADDR	0x70000000
#define XSHAL_IOBLOCK_CACHED_PADDR	0x70000000
#define XSHAL_IOBLOCK_CACHED_SIZE	0x0E000000

#define XSHAL_IOBLOCK_BYPASS_VADDR	0x90000000
#define XSHAL_IOBLOCK_BYPASS_PADDR	0x90000000
#define XSHAL_IOBLOCK_BYPASS_SIZE	0x0E000000

/*  System ROM:  */
#define XSHAL_ROM_VADDR		0x50000000
#define XSHAL_ROM_PADDR		0x50000000
#define XSHAL_ROM_SIZE		0x01000000
/*  Largest available area (free of vectors):  */
#define XSHAL_ROM_AVAIL_VADDR	0x50000000
#define XSHAL_ROM_AVAIL_VSIZE	0x01000000

/*  System RAM:  */
#define XSHAL_RAM_VADDR		0x60000000
#define XSHAL_RAM_PADDR		0x60000000
#define XSHAL_RAM_VSIZE		0x20000000
#define XSHAL_RAM_PSIZE		0x20000000
#define XSHAL_RAM_SIZE		XSHAL_RAM_PSIZE
/*  Largest available area (free of vectors):  */
#define XSHAL_RAM_AVAIL_VADDR	0x60000000
#define XSHAL_RAM_AVAIL_VSIZE	0x20000000

/*
 *  Shadow system RAM (same device as system RAM, at different address).
 *  (Emulation boards need this for the SONIC Ethernet driver
 *   when data caches are configured for writeback mode.)
 *  NOTE: on full MMU configs, this points to the BYPASS virtual address
 *  of system RAM, ie. is the same as XSHAL_RAM_* except that virtual
 *  addresses are viewed through the BYPASS static map rather than
 *  the CACHED static map.
 */
#define XSHAL_RAM_BYPASS_VADDR		0xA0000000
#define XSHAL_RAM_BYPASS_PADDR		0xA0000000
#define XSHAL_RAM_BYPASS_PSIZE		0x20000000

/*  Alternate system RAM (different device than system RAM):  */
/*#define XSHAL_ALTRAM_[VP]ADDR		...not configured...*/
/*#define XSHAL_ALTRAM_SIZE		...not configured...*/

/*  Some available location in which to place devices in a simulation (eg. XTMP):  */
#define XSHAL_SIMIO_CACHED_VADDR	0xC0000000
#define XSHAL_SIMIO_BYPASS_VADDR	0xC0000000
#define XSHAL_SIMIO_PADDR		0xC0000000
#define XSHAL_SIMIO_SIZE		0x20000000


/*----------------------------------------------------------------------
 *  For use by reference testbench exit and diagnostic routines.
 */
#define XSHAL_MAGIC_EXIT		0x0

/*----------------------------------------------------------------------
 *			DEVICE-ADDRESS DEPENDENT...
 *
 *  Values written to CACHEATTR special register (or its equivalent)
 *  to enable and disable caches in various modes.
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
			BACKWARD COMPATIBILITY ...
  ----------------------------------------------------------------------*/

/*
 *  NOTE:  the following two macros are DEPRECATED.  Use the latter
 *  board-specific macros instead, which are specially tuned for the
 *  particular target environments' memory maps.
 */
#define XSHAL_CACHEATTR_BYPASS		XSHAL_XT2000_CACHEATTR_BYPASS	/* disable caches in bypass mode */
#define XSHAL_CACHEATTR_DEFAULT		XSHAL_XT2000_CACHEATTR_DEFAULT	/* default setting to enable caches (no writeback!) */

/*----------------------------------------------------------------------
				GENERIC
  ----------------------------------------------------------------------*/

/*  For the following, a 512MB region is used if it contains a system (PIF) RAM,
 *  system (PIF) ROM, local memory, or XLMI.  */

/*  These set any unused 512MB region to cache-BYPASS attribute:  */
#define XSHAL_ALLVALID_CACHEATTR_WRITEBACK	0x22221112	/* enable caches in write-back mode */
#define XSHAL_ALLVALID_CACHEATTR_WRITEALLOC	0x22221112	/* enable caches in write-allocate mode */
#define XSHAL_ALLVALID_CACHEATTR_WRITETHRU	0x22221112	/* enable caches in write-through mode */
#define XSHAL_ALLVALID_CACHEATTR_BYPASS		0x22222222	/* disable caches in bypass mode */
#define XSHAL_ALLVALID_CACHEATTR_DEFAULT	XSHAL_ALLVALID_CACHEATTR_WRITEBACK	/* default setting to enable caches */

/*  These set any unused 512MB region to ILLEGAL attribute:  */
#define XSHAL_STRICT_CACHEATTR_WRITEBACK	0xFFFF111F	/* enable caches in write-back mode */
#define XSHAL_STRICT_CACHEATTR_WRITEALLOC	0xFFFF111F	/* enable caches in write-allocate mode */
#define XSHAL_STRICT_CACHEATTR_WRITETHRU	0xFFFF111F	/* enable caches in write-through mode */
#define XSHAL_STRICT_CACHEATTR_BYPASS		0xFFFF222F	/* disable caches in bypass mode */
#define XSHAL_STRICT_CACHEATTR_DEFAULT		XSHAL_STRICT_CACHEATTR_WRITEBACK	/* default setting to enable caches */

/*  These set the first 512MB, if unused, to ILLEGAL attribute to help catch
 *  NULL-pointer dereference bugs; all other unused 512MB regions are set
 *  to cache-BYPASS attribute:  */
#define XSHAL_TRAPNULL_CACHEATTR_WRITEBACK	0x2222111F	/* enable caches in write-back mode */
#define XSHAL_TRAPNULL_CACHEATTR_WRITEALLOC	0x2222111F	/* enable caches in write-allocate mode */
#define XSHAL_TRAPNULL_CACHEATTR_WRITETHRU	0x2222111F	/* enable caches in write-through mode */
#define XSHAL_TRAPNULL_CACHEATTR_BYPASS		0x2222222F	/* disable caches in bypass mode */
#define XSHAL_TRAPNULL_CACHEATTR_DEFAULT	XSHAL_TRAPNULL_CACHEATTR_WRITEBACK	/* default setting to enable caches */

/*----------------------------------------------------------------------
			ISS (Instruction Set Simulator) SPECIFIC ...
  ----------------------------------------------------------------------*/

/*  For now, ISS defaults to the TRAPNULL settings:  */
#define XSHAL_ISS_CACHEATTR_WRITEBACK	XSHAL_TRAPNULL_CACHEATTR_WRITEBACK
#define XSHAL_ISS_CACHEATTR_WRITEALLOC	XSHAL_TRAPNULL_CACHEATTR_WRITEALLOC
#define XSHAL_ISS_CACHEATTR_WRITETHRU	XSHAL_TRAPNULL_CACHEATTR_WRITETHRU
#define XSHAL_ISS_CACHEATTR_BYPASS	XSHAL_TRAPNULL_CACHEATTR_BYPASS
#define XSHAL_ISS_CACHEATTR_DEFAULT	XSHAL_TRAPNULL_CACHEATTR_WRITEBACK

#define XSHAL_ISS_PIPE_REGIONS	0
#define XSHAL_ISS_SDRAM_REGIONS	0


/*----------------------------------------------------------------------
			XT2000 BOARD SPECIFIC ...
  ----------------------------------------------------------------------*/

/*  For the following, a 512MB region is used if it contains any system RAM,
 *  system ROM, local memory, XLMI, or other XT2000 board device or memory.
 *  Regions containing devices are forced to cache-BYPASS mode regardless
 *  of whether the macro is _WRITEBACK vs. _BYPASS etc.  */

/*  These set any 512MB region unused on the XT2000 to ILLEGAL attribute:  */
#define XSHAL_XT2000_CACHEATTR_WRITEBACK	0xFF22111F	/* enable caches in write-back mode */
#define XSHAL_XT2000_CACHEATTR_WRITEALLOC	0xFF22111F	/* enable caches in write-allocate mode */
#define XSHAL_XT2000_CACHEATTR_WRITETHRU	0xFF22111F	/* enable caches in write-through mode */
#define XSHAL_XT2000_CACHEATTR_BYPASS		0xFF22222F	/* disable caches in bypass mode */
#define XSHAL_XT2000_CACHEATTR_DEFAULT		XSHAL_XT2000_CACHEATTR_WRITEBACK	/* default setting to enable caches */

#define XSHAL_XT2000_PIPE_REGIONS	0x00000000	/* BusInt pipeline regions */
#define XSHAL_XT2000_SDRAM_REGIONS	0x00000440	/* BusInt SDRAM regions */


/*----------------------------------------------------------------------
				VECTOR INFO AND SIZES
  ----------------------------------------------------------------------*/

#define XSHAL_VECTORS_PACKED		0
#define XSHAL_STATIC_VECTOR_SELECT	1
#define XSHAL_RESET_VECTOR_VADDR	0x40000400
#define XSHAL_RESET_VECTOR_PADDR	0x40000400

/*
 *  Sizes allocated to vectors by the system (memory map) configuration.
 *  These sizes are constrained by core configuration (eg. one vector's
 *  code cannot overflow into another vector) but are dependent on the
 *  system or board (or LSP) memory map configuration.
 *
 *  Whether or not each vector happens to be in a system ROM is also
 *  a system configuration matter, sometimes useful, included here also:
 */
#define XSHAL_RESET_VECTOR_SIZE	0x00000300
#define XSHAL_RESET_VECTOR_ISROM	0
#define XSHAL_USER_VECTOR_SIZE	0x00000038
#define XSHAL_USER_VECTOR_ISROM	0
#define XSHAL_PROGRAMEXC_VECTOR_SIZE	XSHAL_USER_VECTOR_SIZE	/* for backward compatibility */
#define XSHAL_USEREXC_VECTOR_SIZE	XSHAL_USER_VECTOR_SIZE	/* for backward compatibility */
#define XSHAL_KERNEL_VECTOR_SIZE	0x00000038
#define XSHAL_KERNEL_VECTOR_ISROM	0
#define XSHAL_STACKEDEXC_VECTOR_SIZE	XSHAL_KERNEL_VECTOR_SIZE	/* for backward compatibility */
#define XSHAL_KERNELEXC_VECTOR_SIZE	XSHAL_KERNEL_VECTOR_SIZE	/* for backward compatibility */
#define XSHAL_DOUBLEEXC_VECTOR_SIZE	0x00000040
#define XSHAL_DOUBLEEXC_VECTOR_ISROM	0
#define XSHAL_WINDOW_VECTORS_SIZE	0x00000178
#define XSHAL_WINDOW_VECTORS_ISROM	0
#define XSHAL_INTLEVEL2_VECTOR_SIZE	0x00000038
#define XSHAL_INTLEVEL2_VECTOR_ISROM	0
#define XSHAL_INTLEVEL3_VECTOR_SIZE	0x00000038
#define XSHAL_INTLEVEL3_VECTOR_ISROM	0
#define XSHAL_INTLEVEL4_VECTOR_SIZE	0x00000038
#define XSHAL_INTLEVEL4_VECTOR_ISROM	0
#define XSHAL_INTLEVEL5_VECTOR_SIZE	0x00000038
#define XSHAL_INTLEVEL5_VECTOR_ISROM	0
#define XSHAL_INTLEVEL6_VECTOR_SIZE	0x00000038
#define XSHAL_INTLEVEL6_VECTOR_ISROM	0
#define XSHAL_DEBUG_VECTOR_SIZE		XSHAL_INTLEVEL6_VECTOR_SIZE
#define XSHAL_DEBUG_VECTOR_ISROM	XSHAL_INTLEVEL6_VECTOR_ISROM
#define XSHAL_NMI_VECTOR_SIZE	0x00000038
#define XSHAL_NMI_VECTOR_ISROM	0
#define XSHAL_INTLEVEL7_VECTOR_SIZE	XSHAL_NMI_VECTOR_SIZE


#endif /*XTENSA_CONFIG_SYSTEM_H*/
