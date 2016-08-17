/* Copyright (c) 2007-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
/  These coded instructions, statements, and computer programs are the
/  copyrighted works and confidential proprietary information of Tensilica Inc.
/  They may not be modified, copied, reproduced, distributed, or disclosed to
/  third parties in any manner, medium, or form, in whole or in part, without
/  the prior written consent of Tensilica Inc.
*/

/*  xtav110.h   -  Xtensa Avnet LX110 (XT-AV110) board specific definitions  */

#ifndef _INC_XTAV110_H_
#define _INC_XTAV110_H_

#include <xtensa/config/core.h>
#include <xtensa/config/system.h>

#define XTBOARD_NAME		"XT-AV110"


/*
 *  Default assignment of XTAV110 devices to external interrupts.
 */

/*  Ethernet interrupt:  */
#ifdef XCHAL_EXTINT1_NUM
#define ETHERNET_INTNUM         XCHAL_EXTINT1_NUM
#define ETHERNET_INTLEVEL       XCHAL_EXTINT1_LEVEL
#define ETHERNET_INTMASK        XCHAL_EXTINT1_MASK
#else
#define ETHERNET_INTMASK        0
#endif

/*  UART interrupt: */
#ifdef XCHAL_EXTINT0_NUM
#define UART16550_INTNUM        XCHAL_EXTINT0_NUM
#define UART16550_INTLEVEL      XCHAL_EXTINT0_LEVEL
#define UART16550_INTMASK       XCHAL_EXTINT0_MASK
#else
#define UART16550_INTMASK       0
#endif

/* Audio output interrupt (I2S transmitter FIFO): */
#ifdef XCHAL_EXTINT2_NUM
#define AUDIO_I2S_OUT_INTNUM    XCHAL_EXTINT2_NUM
#define AUDIO_I2S_OUT_INTLEVEL  XCHAL_EXTINT2_LEVEL
#define AUDIO_I2S_OUT_INTMASK   XCHAL_EXTINT2_MASK
#else
#define AUDIO_I2S_OUT_INTMASK   0
#endif

/* Audio input interrupt (I2S receiver FIFO): */
#ifdef XCHAL_EXTINT3_NUM
#define AUDIO_I2S_IN_INTNUM     XCHAL_EXTINT3_NUM
#define AUDIO_I2S_IN_INTLEVEL   XCHAL_EXTINT3_LEVEL
#define AUDIO_I2S_IN_INTMASK    XCHAL_EXTINT3_MASK
#else
#define AUDIO_I2S_IN_INTMASK    0
#endif

/* I2C interrupt */
#ifdef XCHAL_EXTINT4_NUM
#define I2C_INTNUM              XCHAL_EXTINT4_NUM
#define I2C_INTLEVEL            XCHAL_EXTINT4_LEVEL
#define I2C_INTMASK             XCHAL_EXTINT4_MASK
#else
#define I2C_INTMASK             0
#endif

/* USB interrupt */
#ifdef XCHAL_EXTINT5_NUM
#define USB_INTNUM              XCHAL_EXTINT5_NUM
#define USB_INTLEVEL            XCHAL_EXTINT5_LEVEL
#define USB_INTMASK             XCHAL_EXTINT5_MASK
#else
#define USB_INTMASK             0
#endif

/*
 *  Device addresses.
 *
 *  Note:  for endianness-independence, use 32-bit loads and stores for all
 *  register accesses to Ethernet, UART and LED devices.  Undefined bits
 *  may need to be masked out if needed when reading if the actual register
 *  size is smaller than 32 bits.
 *
 *  Note:  XTAV110 bus byte lanes are defined in terms of msbyte and lsbyte
 *  relative to the processor.  So 32-bit registers are accessed consistently
 *  from both big and little endian processors.  However, this means byte
 *  sequences are not consistent between big and little endian processors.
 *  This is fine for RAM, and for ROM if ROM is created for a specific
 *  processor (and thus has correct byte sequences).  However this may be
 *  unexpected for Flash, which might contain a file-system that one wants
 *  to use for multiple processor configurations (eg. the Flash might contain
 *  the Ethernet card's address, endianness-independent application data, etc).
 *  That is, byte sequences written in Flash by a core of a given endianness
 *  will be byte-swapped when seen by a core of the other endianness.
 *  Someone implementing an endianness-independent Flash file system will
 *  likely handle this byte-swapping issue in the Flash driver software.
 */

#define XTBOARD_FLASH_MAXSIZE   0x1000000       /* 16 MB */

#ifdef XSHAL_IOBLOCK_BYPASS_PADDR

/*  Flash Memory: */
# define XTBOARD_FLASH_PADDR        (XSHAL_IOBLOCK_BYPASS_PADDR+0x08000000)

/*  FPGA registers:  */
# define XTBOARD_FPGAREGS_PADDR     (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D020000)

/*  Ethernet controller/transceiver SONIC SN83934:  */
# define ETHERNET_PADDR             (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D030000)


/*  UART National-Semi PC16550D:  */
# define UART16550_PADDR            (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D050000)

/*  I2S transmitter */
# define AUDIO_I2S_OUT_PADDR        (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D080000)

/*  I2S receiver */
# define AUDIO_I2S_IN_PADDR         (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D088000)

/*  I2C master */
# define I2C_PADDR                  (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D090000)

/*  SPI controller */
# define SPI_PADDR                  (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D0A0000)

/*  Display controller Sunplus SPLC780D, 4bit mode, 
 *  LCD Display MYTech MOC-16216B-B: */
# define SPLC780D_4BIT_PADDR	    (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D0C0000)

/*  USB Controller */
# define USB_PADDR                  (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D0D0000)

/*  Ethernet buffer:  */
# define ETHERNET_BUFFER_PADDR      (XSHAL_IOBLOCK_BYPASS_PADDR+0x0D800000)

#endif /* XSHAL_IOBLOCK_BYPASS_PADDR  */

/*  These devices might be accessed cached:  */
#ifdef XSHAL_IOBLOCK_CACHED_PADDR
# define XTBOARD_FLASH_CACHED_PADDR   (XSHAL_IOBLOCK_CACHED_PADDR+0x08000000)
# define ETHERNET_BUFFER_CACHED_PADDR (XSHAL_IOBLOCK_CACHED_PADDR+0x0D800000)
#endif /* XSHAL_IOBLOCK_CACHED_PADDR */


/***  Same thing over again, this time with virtual addresses:  ***/

#ifdef XSHAL_IOBLOCK_BYPASS_VADDR

/*  Flash Memory: */
# define XTBOARD_FLASH_VADDR        (XSHAL_IOBLOCK_BYPASS_VADDR+0x08000000)

/*  FPGA registers:  */
# define XTBOARD_FPGAREGS_VADDR     (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D020000)

/*  Ethernet controller/transceiver SONIC SN83934:  */
# define ETHERNET_VADDR             (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D030000)


/*  UART National-Semi PC16550D:  */
# define UART16550_VADDR            (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D050000)

/*  I2S transmitter */
# define AUDIO_I2S_OUT_VADDR        (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D080000)

/*  I2S receiver */
# define AUDIO_I2S_IN_VADDR         (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D088000)

/*  I2C master */
# define I2C_VADDR                  (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D090000)

/*  SPI controller */
# define SPI_VADDR                  (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D0A0000)

/*  Display controller Sunplus SPLC780D, 4bit mode, 
 *  LCD Display MYTech MOC-16216B-B: */
# define SPLC780D_4BIT_VADDR	    (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D0C0000)

/*  USB Controller */
# define USB_VADDR                  (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D0D0000)

/*  Ethernet buffer:  */
# define ETHERNET_BUFFER_VADDR      (XSHAL_IOBLOCK_BYPASS_VADDR+0x0D800000)

#endif /* XSHAL_IOBLOCK_BYPASS_VADDR */

/*  These devices might be accessed cached:  */
#ifdef XSHAL_IOBLOCK_CACHED_VADDR
# define XTBOARD_FLASH_CACHED_VADDR   (XSHAL_IOBLOCK_CACHED_VADDR+0x08000000)
# define ETHERNET_BUFFER_CACHED_VADDR (XSHAL_IOBLOCK_CACHED_VADDR+0x0D800000)
#endif /* XSHAL_IOBLOCK_CACHED_VADDR */


/*  System ROM:  */
#define XTBOARD_ROM_SIZE        XSHAL_ROM_SIZE
#ifdef XSHAL_ROM_VADDR
#define XTBOARD_ROM_VADDR       XSHAL_ROM_VADDR
#endif
#ifdef XSHAL_ROM_PADDR
#define XTBOARD_ROM_PADDR       XSHAL_ROM_PADDR
#endif

/*  System RAM:  */
#define XTBOARD_RAM_SIZE        XSHAL_RAM_SIZE
#ifdef XSHAL_RAM_VADDR
#define XTBOARD_RAM_VADDR       XSHAL_RAM_VADDR
#endif
#ifdef XSHAL_RAM_PADDR
#define XTBOARD_RAM_PADDR       XSHAL_RAM_PADDR
#endif
#define XTBOARD_RAM_BYPASS_VADDR    XSHAL_RAM_BYPASS_VADDR
#define XTBOARD_RAM_BYPASS_PADDR    XSHAL_RAM_BYPASS_PADDR



/*
 *  Things that depend on device addresses.
 */


#define XTBOARD_CACHEATTR_WRITEBACK XSHAL_XT2000_CACHEATTR_WRITEBACK
#define XTBOARD_CACHEATTR_WRITEALLOC    XSHAL_XT2000_CACHEATTR_WRITEALLOC
#define XTBOARD_CACHEATTR_WRITETHRU XSHAL_XT2000_CACHEATTR_WRITETHRU
#define XTBOARD_CACHEATTR_BYPASS    XSHAL_XT2000_CACHEATTR_BYPASS
#define XTBOARD_CACHEATTR_DEFAULT   XSHAL_XT2000_CACHEATTR_DEFAULT

#define XTBOARD_BUSINT_PIPE_REGIONS XSHAL_XT2000_PIPE_REGIONS
#define XTBOARD_BUSINT_SDRAM_REGIONS    XSHAL_XT2000_SDRAM_REGIONS


/*
 *  FPGA registers.
 *  All these registers are normally accessed using 32-bit loads/stores.
 */

/*  Register offsets:  */
#define XTBOARD_DATECD_OFS  0x00    /* date code (read-only) */
#define XTBOARD_CLKFRQ_OFS  0x04    /* clock frequency Hz (read-only) */
#define XTBOARD_SYSLED_OFS  0x08    /* LEDs */
#define XTBOARD_DIPSW_OFS   0x0C    /* DIP switch bits (read-only) */
#define XTBOARD_SWRST_OFS   0x10    /* software reset */

/*  Physical register addresses:  */
#ifdef XTBOARD_FPGAREGS_PADDR
#define XTBOARD_DATECD_PADDR    (XTBOARD_FPGAREGS_PADDR+XTBOARD_DATECD_OFS)
#define XTBOARD_CLKFRQ_PADDR    (XTBOARD_FPGAREGS_PADDR+XTBOARD_CLKFRQ_OFS)
#define XTBOARD_SYSLED_PADDR    (XTBOARD_FPGAREGS_PADDR+XTBOARD_SYSLED_OFS)
#define XTBOARD_DIPSW_PADDR (XTBOARD_FPGAREGS_PADDR+XTBOARD_DIPSW_OFS)
#define XTBOARD_SWRST_PADDR (XTBOARD_FPGAREGS_PADDR+XTBOARD_SWRST_OFS)
#endif

/*  Virtual register addresses:  */
#ifdef XTBOARD_FPGAREGS_VADDR
#define XTBOARD_DATECD_VADDR    (XTBOARD_FPGAREGS_VADDR+XTBOARD_DATECD_OFS)
#define XTBOARD_CLKFRQ_VADDR    (XTBOARD_FPGAREGS_VADDR+XTBOARD_CLKFRQ_OFS)
#define XTBOARD_SYSLED_VADDR    (XTBOARD_FPGAREGS_VADDR+XTBOARD_SYSLED_OFS)
#define XTBOARD_DIPSW_VADDR (XTBOARD_FPGAREGS_VADDR+XTBOARD_DIPSW_OFS)
#define XTBOARD_SWRST_VADDR (XTBOARD_FPGAREGS_VADDR+XTBOARD_SWRST_OFS)
/*  Register access (for C code):  */
#define XTBOARD_DATECD_REG  (*(volatile unsigned*) XTBOARD_DATECD_VADDR)
#define XTBOARD_CLKFRQ_REG  (*(volatile unsigned*) XTBOARD_CLKFRQ_VADDR)
#define XTBOARD_SYSLED_REG  (*(volatile unsigned*) XTBOARD_SYSLED_VADDR)
#define XTBOARD_DIPSW_REG   (*(volatile unsigned*) XTBOARD_DIPSW_VADDR)
#define XTBOARD_SWRST_REG   (*(volatile unsigned*) XTBOARD_SWRST_VADDR)
#endif

/*  DATECD (date code; when core was built) bit fields:  */
/*  BCD-coded month (01..12):  */
#define XTBOARD_DATECD_MONTH_SHIFT  24
#define XTBOARD_DATECD_MONTH_BITS   8
#define XTBOARD_DATECD_MONTH_MASK   0xFF000000
/*  BCD-coded day (01..31):  */
#define XTBOARD_DATECD_DAY_SHIFT    16
#define XTBOARD_DATECD_DAY_BITS     8
#define XTBOARD_DATECD_DAY_MASK     0x00FF0000
/*  BCD-coded year (2001..9999):  */
#define XTBOARD_DATECD_YEAR_SHIFT   0
#define XTBOARD_DATECD_YEAR_BITS    16
#define XTBOARD_DATECD_YEAR_MASK    0x0000FFFF

/*  SYSLED (system LED) bit fields:  */

/*  LED control bits (off=0, on=1):  */
#define XTBOARD_SYSLED_USER_SHIFT   0
#define XTBOARD_SYSLED_USER_BITS    2
#define XTBOARD_SYSLED_USER_MASK    0x00000003

/*  DIP Switch SW5 (left=sw1=lsb=bit0, right=sw4=msb=bit3; off=0, on=1): */
/*  DIP switch bit fields (bit2/sw3 is reserved and presently unused):  */
#define XTBOARD_DIPSW_USER_SHIFT    0       /* labeled 1-2 (1=lsb) */
#define XTBOARD_DIPSW_USER_BITS     2
#define XTBOARD_DIPSW_USER_MASK     0x00000003
#define XTBOARD_DIPSW_BOOT_SHIFT    3       /* labeled 8 (msb) */
#define XTBOARD_DIPSW_BOOT_BITS     1
#define XTBOARD_DIPSW_BOOT_MASK     0x00000008
/*  Boot settings: bit3/sw4, off=0, on=1 (this switch controls hardware): */
#define XTBOARD_DIPSW_BOOT_RAM      (0<<XTBOARD_DIPSW_BOOT_SHIFT) /* off */
#define XTBOARD_DIPSW_BOOT_FLASH    (1<<XTBOARD_DIPSW_BOOT_SHIFT) /* on */

/*  SWRST (software reset; allows s/w to generate power-on equivalent reset): */
/*  Software reset bits:  */
#define XTBOARD_SWRST_SWR_SHIFT     0
#define XTBOARD_SWRST_SWR_BITS      16
#define XTBOARD_SWRST_SWR_MASK      0x0000FFFF
/*  Software reset value -- writing this value resets the board:  */
#define XTBOARD_SWRST_RESETVALUE    0x0000DEAD


#endif /*_INC_XTAV110_H_*/

