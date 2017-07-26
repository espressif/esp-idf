/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016 Espressif Systems (Shanghai) PTE LTD
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _SDMMC_TYPES_H_
#define _SDMMC_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

/**
 * Decoded values from SD card Card Specific Data register
 */
typedef struct {
    int csd_ver;                /*!< CSD structure format */
    int mmc_ver;                /*!< MMC version (for CID format) */
    int capacity;               /*!< total number of sectors */
    int sector_size;            /*!< sector size in bytes */
    int read_block_len;         /*!< block length for reads */
    int card_command_class;     /*!< Card Command Class for SD */
    int tr_speed;               /*!< Max transfer speed */
} sdmmc_csd_t;

/**
 * Decoded values from SD card Card IDentification register
 */
typedef struct {
    int mfg_id;     /*!< manufacturer identification number */
    int oem_id;     /*!< OEM/product identification number */
    char name[8];   /*!< product name (MMC v1 has the longest) */
    int revision;   /*!< product revision */
    int serial;     /*!< product serial number */
    int date;       /*!< manufacturing date */
} sdmmc_cid_t;

/**
 * Decoded values from SD Configuration Register
 */
typedef struct {
    int sd_spec;    /*!< SD Physical layer specification version, reported by card */
    int bus_width;  /*!< bus widths supported by card: BIT(0) — 1-bit bus, BIT(2) — 4-bit bus */
} sdmmc_scr_t;

/**
 * SD/MMC command response buffer
 */
typedef uint32_t sdmmc_response_t[4];

/**
 * SD/MMC command information
 */
typedef struct {
        uint32_t opcode;            /*!< SD or MMC command index */
        uint32_t arg;               /*!< SD/MMC command argument */
        sdmmc_response_t response;  /*!< response buffer */
        void* data;                 /*!< buffer to send or read into */
        size_t datalen;             /*!< length of data buffer */
        size_t blklen;              /*!< block length */
        int flags;                  /*!< see below */
#define SCF_ITSDONE      0x0001     /*!< command is complete */
#define SCF_CMD(flags)   ((flags) & 0x00f0)
#define SCF_CMD_AC       0x0000
#define SCF_CMD_ADTC     0x0010
#define SCF_CMD_BC       0x0020
#define SCF_CMD_BCR      0x0030
#define SCF_CMD_READ     0x0040     /*!< read command (data expected) */
#define SCF_RSP_BSY      0x0100
#define SCF_RSP_136      0x0200
#define SCF_RSP_CRC      0x0400
#define SCF_RSP_IDX      0x0800
#define SCF_RSP_PRESENT  0x1000
/* response types */
#define SCF_RSP_R0       0 /*!< none */
#define SCF_RSP_R1       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
#define SCF_RSP_R1B      (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX|SCF_RSP_BSY)
#define SCF_RSP_R2       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_136)
#define SCF_RSP_R3       (SCF_RSP_PRESENT)
#define SCF_RSP_R4       (SCF_RSP_PRESENT)
#define SCF_RSP_R5       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
#define SCF_RSP_R5B      (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX|SCF_RSP_BSY)
#define SCF_RSP_R6       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
#define SCF_RSP_R7       (SCF_RSP_PRESENT|SCF_RSP_CRC|SCF_RSP_IDX)
        esp_err_t error;            /*!< error returned from transfer */
} sdmmc_command_t;

/**
 * SD/MMC Host description
 *
 * This structure defines properties of SD/MMC host and functions
 * of SD/MMC host which can be used by upper layers.
 */
typedef struct {
    uint32_t flags;             /*!< flags defining host properties */
#define SDMMC_HOST_FLAG_1BIT    BIT(0)      /*!< host supports 1-line SD and MMC protocol */
#define SDMMC_HOST_FLAG_4BIT    BIT(1)      /*!< host supports 4-line SD and MMC protocol */
#define SDMMC_HOST_FLAG_8BIT    BIT(2)      /*!< host supports 8-line MMC protocol */
#define SDMMC_HOST_FLAG_SPI     BIT(3)      /*!< host supports SPI protocol */
    int slot;                   /*!< slot number, to be passed to host functions */
    int max_freq_khz;           /*!< max frequency supported by the host */
#define SDMMC_FREQ_DEFAULT      20000       /*!< SD/MMC Default speed (limited by clock divider) */
#define SDMMC_FREQ_HIGHSPEED    40000       /*!< SD High speed (limited by clock divider) */
#define SDMMC_FREQ_PROBING      400         /*!< SD/MMC probing speed */
    float io_voltage;           /*!< I/O voltage used by the controller (voltage switching is not supported) */
    esp_err_t (*init)(void);    /*!< Host function to initialize the driver */
    esp_err_t (*set_bus_width)(int slot, size_t width);    /*!< host function to set bus width */
    esp_err_t (*set_card_clk)(int slot, uint32_t freq_khz); /*!< host function to set card clock frequency */
    esp_err_t (*do_transaction)(int slot, sdmmc_command_t* cmdinfo);    /*!< host function to do a transaction */
    esp_err_t (*deinit)(void);  /*!< host function to deinitialize the driver */
} sdmmc_host_t;

/**
 * SD/MMC card information structure
 */
typedef struct {
    sdmmc_host_t host;          /*!< Host with which the card is associated */
    uint32_t ocr;               /*!< OCR (Operation Conditions Register) value */
    sdmmc_cid_t cid;            /*!< decoded CID (Card IDentification) register value */
    sdmmc_csd_t csd;            /*!< decoded CSD (Card-Specific Data) register value */
    sdmmc_scr_t scr;            /*!< decoded SCR (SD card Configuration Register) value */
    uint16_t rca;               /*!< RCA (Relative Card Address) */
} sdmmc_card_t;




#endif // _SDMMC_TYPES_H_
