/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * @brief Enum to define JPEG marker codes
 */
typedef enum {
    JPEG_M_SOF0 = 0xFFC0,       ///< Start of Frame 0
    JPEG_M_SOF1 = 0xFFC1,       ///< Start of Frame 1
    JPEG_M_SOF2 = 0xFFC2,       ///< Start of Frame 2
    JPEG_M_SOF3 = 0xFFC3,       ///< Start of Frame 3
    JPEG_M_SOF5 = 0xFFC5,       ///< Start of Frame 5
    JPEG_M_SOF6 = 0xFFC6,       ///< Start of Frame 6
    JPEG_M_SOF7 = 0xFFC7,       ///< Start of Frame 7
    JPEG_M_JPG = 0xFFC8,        ///< JPEG Extension
    JPEG_M_SOF9 = 0xFFC9,       ///< Start of Frame 9
    JPEG_M_SOF10 = 0xFFCA,      ///< Start of Frame 10
    JPEG_M_SOF11 = 0xFFCB,      ///< Start of Frame 11
    JPEG_M_SOF13 = 0xFFCD,      ///< Start of Frame 13
    JPEG_M_SOF14 = 0xFFCE,      ///< Start of Frame 14
    JPEG_M_SOF15 = 0xFFCF,      ///< Start of Frame 15
    JPEG_M_DHT = 0xFFC4,        ///< Define Huffman Table
    JPEG_M_DAC = 0xFFCC,        ///< Define Arithmetic Coding Conditioning
    JPEG_M_RST0 = 0xFFD0,       ///< Restart with modulo 8 count 0
    JPEG_M_RST1 = 0xFFD1,       ///< Restart with modulo 8 count 1
    JPEG_M_RST2 = 0xFFD2,       ///< Restart with modulo 8 count 2
    JPEG_M_RST3 = 0xFFD3,       ///< Restart with modulo 8 count 3
    JPEG_M_RST4 = 0xFFD4,       ///< Restart with modulo 8 count 4
    JPEG_M_RST5 = 0xFFD5,       ///< Restart with modulo 8 count 5
    JPEG_M_RST6 = 0xFFD6,       ///< Restart with modulo 8 count 6
    JPEG_M_RST7 = 0xFFD7,       ///< Restart with modulo 8 count 7
    JPEG_M_SOI = 0xFFD8,        ///< Start of Image
    JPEG_M_EOI = 0xFFD9,        ///< End of Image
    JPEG_M_SOS = 0xFFDA,        ///< Start of Scan
    JPEG_M_DQT = 0xFFDB,        ///< Define Quantization Table
    JPEG_M_DNL = 0xFFDC,        ///< Define Number of Lines
    JPEG_M_DRI = 0xFFDD,        ///< Define Restart Interval
    JPEG_M_DHP = 0xFFDE,        ///< Define Hierarchical Progression
    JPEG_M_EXP = 0xFFDF,        ///< Expand Reference Component(s)
    JPEG_M_APP0 = 0xFFE0,       ///< Application Segment 0
    JPEG_M_APP1 = 0xFFE1,       ///< Application Segment 1
    JPEG_M_APP2 = 0xFFE2,       ///< Application Segment 2
    JPEG_M_APP3 = 0xFFE3,       ///< Application Segment 3
    JPEG_M_APP4 = 0xFFE4,       ///< Application Segment 4
    JPEG_M_APP5 = 0xFFE5,       ///< Application Segment 5
    JPEG_M_APP6 = 0xFFE6,       ///< Application Segment 6
    JPEG_M_APP7 = 0xFFE7,       ///< Application Segment 7
    JPEG_M_APP8 = 0xFFE8,       ///< Application Segment 8
    JPEG_M_APP9 = 0xFFE9,       ///< Application Segment 9
    JPEG_M_APP10 = 0xFFEA,      ///< Application Segment 10
    JPEG_M_APP11 = 0xFFEB,      ///< Application Segment 11
    JPEG_M_APP12 = 0xFFEC,      ///< Application Segment 12
    JPEG_M_APP13 = 0xFFED,      ///< Application Segment 13
    JPEG_M_APP14 = 0xFFEE,      ///< Application Segment 14
    JPEG_M_APP15 = 0xFFEF,      ///< Application Segment 15
    JPEG_M_JPG0 = 0xFFF0,       ///< Reserved for JPEG extension
    JPEG_M_JPG13 = 0xFFFD,      ///< Reserved for JPEG extension
    JPEG_M_COM = 0xFFFE,        ///< Comment
    JPEG_M_TEM = 0xFF01,        ///< Temporary use in arithmetic coding
    JPEG_M_INV = 0xFFFF,        ///< Invalid marker
} __attribute__((packed)) jpeg_marker_code_t;


#ifdef __cplusplus
}
#endif
