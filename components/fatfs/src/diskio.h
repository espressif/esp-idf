/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2014          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */


/* Redefine names of disk IO functions to prevent name collisions */
#define disk_initialize     ff_disk_initialize
#define disk_status         ff_disk_status
#define disk_read           ff_disk_read
#define disk_write          ff_disk_write
#define disk_ioctl          ff_disk_ioctl


DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

/**
 * Structure of pointers to disk IO driver functions.
 *
 * See FatFs documentation for details about these functions
 */
typedef struct {
    DSTATUS (*init) (BYTE pdrv);    /*!< disk initialization function */
    DSTATUS (*status) (BYTE pdrv);  /*!< disk status check function */
    DRESULT (*read) (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);  /*!< sector read function */
    DRESULT (*write) (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);   /*!< sector write function */
    DRESULT (*ioctl) (BYTE pdrv, BYTE cmd, void* buff); /*!< function to get info about disk and do some misc operations */
} ff_diskio_impl_t;

/**
 * Register or unregister diskio driver for given drive number.
 *
 * When FATFS library calls one of disk_xxx functions for driver number pdrv,
 * corresponding function in discio_impl for given pdrv will be called.
 *
 * @param pdrv drive number
 * @param discio_impl   pointer to ff_diskio_impl_t structure with diskio functions
 *                      or NULL to unregister and free previously registered drive
 */
void ff_diskio_register(BYTE pdrv, const ff_diskio_impl_t* discio_impl);

#define ff_diskio_unregister(pdrv_) ff_diskio_register(pdrv_, NULL)

/**
 * Register SD/MMC diskio driver
 *
 * @param pdrv  drive number
 * @param card  pointer to sdmmc_card_t structure describing a card; card should be initialized before calling f_mount.
 */
void ff_diskio_register_sdmmc(BYTE pdrv, sdmmc_card_t* card);

/**
 * Get next available drive number
 *
 * @param   out_pdrv            pointer to the byte to set if successful
 *
 * @return  ESP_OK              on success
 *          ESP_ERR_NOT_FOUND   if all drives are attached
 */
esp_err_t ff_diskio_get_drive(BYTE* out_pdrv);

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at _FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at _MAX_SS != _MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at _USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

#ifdef __cplusplus
}
#endif

#endif
