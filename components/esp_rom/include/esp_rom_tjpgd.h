/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*-----------------------------------------------------------------------------/
/ TJpgDec - Tiny JPEG Decompressor R0.01b                     (C)ChaN, 2012
/-----------------------------------------------------------------------------/
/ The TJpgDec is a generic JPEG decompressor module for tiny embedded systems.
/ This is a free software that opened for education, research and commercial
/  developments under license policy of following terms.
/
/  Copyright (C) 2012, ChaN, all right reserved.
/
/ * The TJpgDec module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error code */
typedef enum {
    JDR_OK = 0, /* 0: Succeeded */
    JDR_INTR,   /* 1: Interrupted by output function */
    JDR_INP,    /* 2: Device error or wrong termination of input stream */
    JDR_MEM1,   /* 3: Insufficient memory pool for the image */
    JDR_MEM2,   /* 4: Insufficient stream input buffer */
    JDR_PAR,    /* 5: Parameter error */
    JDR_FMT1,   /* 6: Data format error (may be damaged data) */
    JDR_FMT2,   /* 7: Right format but not supported */
    JDR_FMT3    /* 8: Not supported JPEG standard */
} esp_rom_tjpgd_result_t;

/* Rectangular structure */
typedef struct {
    uint16_t left;  /* Left end */
    uint16_t right; /* Right end */
    uint16_t top;   /* Top end */
    uint16_t bottom;/* Bottom end */
} esp_rom_tjpgd_rect_t;

typedef struct JDEC_s esp_rom_tjpgd_dec_t;

/**
 * @brief Type of user defined input function to read data from input stream
 * @param dec Specifies the decompression object of the decompression session
 * @param buffer Specifies the pointer to the read buffer to store the read data. A NULL specifies to remove the data from input stream
 * @param ndata Specifies number of bytes to read/remove from the input stream
 *
 * @return number of bytes read/removed. When a zero is returned, the esp_rom_tjpgd_prepare and esp_rom_tjpgd_decomp function aborts with JDR_INP
 */
typedef uint32_t (*esp_rom_tjpgd_input_function_t)(esp_rom_tjpgd_dec_t *dec, uint8_t *buffer, uint32_t ndata);

/**
 * @brief User defined output function to write decompressed pixels to the output device
 *
 * This function is the data output interface of the TJpgDec module.
 * The corresponding decompression session can be identified by the pointer to the device identifier jdec->device passed to the 5th argument of jd_prepare function.
 * The bitmap is sent to the frame buffer or display device in this function.
 * The first pixel in the bitmap is the left-top of the rectangular, the second one is next right and last pixel is the bottom-right of the rectangular.
 * The size of rectangular varies from 1x1 to 16x16 depends on clipping, scaling and sampling factor of the image.
 * If the rectangular is out of the frame buffer, it should be clipped in this function.
 *
 * The pixel format is currently configured to RGB888
 *
 * @param dec Specifies the decompression object of the decompression session
 * @param bitmap Specifies the RGB bitmap to be output
 * @param rect Specifies rectangular region in the image to output the RGB bitmap
 *
 * @return Normally returns 1. It lets TJpgDec to continue the decompressing process.
 *         When a 0 is returned, the esp_rom_tjpgd_decomp function aborts with JDR_INTR.
 *         This is useful to interrupt the decompression process
 */
typedef uint32_t (*esp_rom_tjpgd_output_function_t)(esp_rom_tjpgd_dec_t *dec, void *bitmap, esp_rom_tjpgd_rect_t *rect);

struct JDEC_s {
    uint32_t dctr;              /* Number of bytes available in the input buffer */
    uint8_t *dptr;              /* Current data read ptr */
    uint8_t *inbuf;             /* Bit stream input buffer */
    uint8_t dmsk;               /* Current bit in the current read byte */
    uint8_t scale;              /* Output scaling ratio */
    uint8_t msx, msy;           /* MCU size in unit of block (width, height) */
    uint8_t qtid[3];            /* Quantization table ID of each component */
    int16_t dcv[3];             /* Previous DC element of each component */
    uint16_t nrst;              /* Restart inverval */
    uint32_t width, height;     /* Size of the input image (pixel) */
    uint8_t *huffbits[2][2];    /* Huffman bit distribution tables [id][dcac] */
    uint16_t *huffcode[2][2];   /* Huffman code word tables [id][dcac] */
    uint8_t *huffdata[2][2];    /* Huffman decoded data tables [id][dcac] */
    int32_t *qttbl[4];          /* Dequaitizer tables [id] */
    void *workbuf;              /* Working buffer for IDCT and RGB output */
    uint8_t *mcubuf;            /* Working buffer for the MCU */
    void *pool;                 /* Pointer to available memory pool */
    uint32_t sz_pool;           /* Size of momory pool (bytes available) */
    esp_rom_tjpgd_input_function_t infunc; /* Pointer to jpeg stream input function */
    void *device;               /* Pointer to I/O device identifiler for the session */
};

/* TJpgDec API functions */

/**
 * @brief Analyzes the JPEG data and create a decompression object for subsequent decompression process.
 * @param dec Specifies the decompression object to be initialized. The decompression object is used for subsequent decompression process.
 * @param infunc Specifies the user defined data input function.
 * @param work Specifies pointer to the work area for this session. It should be aligned to word boundary or it can result an exception.
 * @param sz_work Specifies size of the work area in unit of byte.
 *                TJpgDec requires upto 3092 bytes of work area depends on the built-in parameter tables of the JPEG image.
 *                Thus 3092 bytes of work area is sufficient for most case.
 * @param dev Specifies pointer to the user defined device identifier for this session.
 *            It is stored to the member device in the decompression object. It can be referred by I/O functions to identify the current session.
 *            When I/O device is fixed in the project or this feature is not needed, set NULL and do not care about this.
 *
 * @return
 *      - JDR_OK Function succeeded and decompression object is valid.
 *      - JDR_INP An error occurred in input function due to hard error or wrong stream termination.
 *      - JDR_MEM1 Insufficient work area for this JPEG image.
 *      - JDR_MEM2 Insufficient input buffer for this JPEG image. JD_SZBUF may be too small.
 *      - JDR_PAR Parameter error. Given pointer to the work area is NULL.
 *      - JDR_FMT1 Data format error. The JPEG data can be collapsed.
 *      - JDR_FMT2 Right format but not supported. May be a grayscale image.
 *      - JDR_FMT3 Not supported JPEG standard. May be a progressive JPEG image.
 */
esp_rom_tjpgd_result_t esp_rom_tjpgd_prepare(esp_rom_tjpgd_dec_t *dec, esp_rom_tjpgd_input_function_t infunc, void *work, uint32_t sz_work, void *dev);

/**
 * @brief Decompress the JPEG image and output it as RGB data.
 * @param dec Specifies the valid decompressor object.
 * @param outfunc Specifies the user defined data output function. The esp_rom_tjpgd_decomp function calls this function to output the decompressed JPEG image in RGB form.
 * @param scale Specifies scaling factor N for output. The output image is descaled to 1 / 2 ^ N (N = 0 to 3).
 *
 * @return
 *      - JDR_OK Function succeeded.
 *      - JDR_INTR The decompression process is interrupted by output function.
 *      - JDR_INP An error occured in input function due to hard error or wrong stream termination.
 *      - JDR_PAR Parameter error. Given scale factor is invalid.
 *      - JDR_FMT1 Data format error. The JPEG data can be collapted.
 */
esp_rom_tjpgd_result_t esp_rom_tjpgd_decomp(esp_rom_tjpgd_dec_t *dec, esp_rom_tjpgd_output_function_t outfunc, uint8_t scale);

#ifdef __cplusplus
}
#endif
