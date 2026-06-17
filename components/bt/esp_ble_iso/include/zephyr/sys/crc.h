/*
 * SPDX-FileCopyrightText: 2018 Workaround GmbH.
 * SPDX-FileCopyrightText: 2017 Intel Corporation.
 * SPDX-FileCopyrightText: 2017 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015 Runtime Inc
 * SPDX-FileCopyrightText: 2018 Google LLC.
 * SPDX-FileCopyrightText: 2022 Meta
 * SPDX-FileCopyrightText: 2024 Intercreate, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** @file
 * @brief CRC computation function
 */

#ifndef ZEPHYR_INCLUDE_SYS_CRC_H_
#define ZEPHYR_INCLUDE_SYS_CRC_H_

#include <zephyr/types.h>
#include <stdbool.h>
#include <stddef.h>

#include <zephyr/sys/__assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initial value expected to be used at the beginning of the crc8_ccitt
 * computation.
 */
#define CRC8_CCITT_INITIAL_VALUE 0xFF
#define CRC8_ROHC_INITIAL_VALUE  0xFF

/* Initial value expected to be used at the beginning of the OpenPGP CRC-24 computation. */
#define CRC24_PGP_INITIAL_VALUE 0x00B704CEU
/*
 * The CRC-24 value is stored on a 32-bit value, only the 3 least significant bytes
 * are meaningful. Use the following mask to only keep the CRC-24 value.
 */
#define CRC24_FINAL_VALUE_MASK 0x00FFFFFFU

/**
 * @defgroup checksum Checksum
 * @ingroup os_services
 */

/**
 * @defgroup crc CRC
 * @ingroup checksum
 * @{
 */

/**
 * @brief CRC polynomial definitions
 * @anchor CRC_POLYNOMIAL
 *
 * @{
 */

/** CRC4 polynomial */
#define CRC4_POLY 0x3

/** CRC4_TI polynomial */
#define CRC4_REFLECT_POLY 0xC

/** CRC7_BE polynomial */
#define CRC7_BE_POLY 0x09

/** CRC8 polynomial */
#define CRC8_POLY 0x07

/** CRC8_CCITT polynomial */
#define CRC8_REFLECT_POLY 0xE0

/** CRC8_ROHC polynomial */
#define CRC16_POLY 0x8005

/** CRC16_ANSI polynomial */
#define CRC16_REFLECT_POLY 0xA001

/** CRC16_CCITT polynomial */
#define CRC16_CCITT_POLY 0x1021

/** CRC16_ITU_T polynomial */
#define CRC24_PGP_POLY 0x01864CFBU

/** CRC32_C polynomial */
#define CRC32_IEEE_POLY 0x04C11DB7U

/** CRC32C polynomial */
#define CRC32C_POLY 0x1EDC6F41U

/** CRC32_K_4_2 polynomial */
#define CRC32K_4_2_POLY 0x93A409EBU

/** @} */

/**
 * @brief CRC algorithm enumeration
 *
 * These values should be used with the @ref crc dispatch function.
 */
enum crc_type {
    CRC4,        /**< Use @ref crc4 */
    CRC4_TI,     /**< Use @ref crc4_ti */
    CRC7_BE,     /**< Use @ref crc7_be */
    CRC8,        /**< Use @ref crc8 */
    CRC8_CCITT,  /**< Use @ref crc8_ccitt */
    CRC8_ROHC,   /**< Use @ref crc8_rohc */
    CRC16,       /**< Use @ref crc16 */
    CRC16_ANSI,  /**< Use @ref crc16_ansi */
    CRC16_CCITT, /**< Use @ref crc16_ccitt */
    CRC16_ITU_T, /**< Use @ref crc16_itu_t */
    CRC24_PGP,   /**< Use @ref crc24_pgp */
    CRC32_C,     /**< Use @ref crc32_c */
    CRC32_IEEE,  /**< Use @ref crc32_ieee */
    CRC32_K_4_2, /**< Use @ref crc32_k_4_2_update */
};

/**
 * @brief Generic function for computing a CRC-16 without input or output
 *        reflection.
 *
 * Compute CRC-16 by passing in the address of the input, the input length
 * and polynomial used in addition to the initial value. This is O(n*8) where n
 * is the length of the buffer provided. No reflection is performed.
 *
 * @note If you are planning to use a CRC based on poly 0x1012 the functions
 * crc16_itu_t() is faster and thus recommended over this one.
 *
 * @param poly The polynomial to use omitting the leading x^16
 *             coefficient
 * @param seed Initial value for the CRC computation
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC16 value (without any XOR applied to it)
 */
uint16_t crc16(uint16_t poly, uint16_t seed, const uint8_t *src, size_t len);

/**
 * @brief Generic function for computing a CRC-16 with input and output
 *        reflection.
 *
 * Compute CRC-16 by passing in the address of the input, the input length
 * and polynomial used in addition to the initial value. This is O(n*8) where n
 * is the length of the buffer provided. Both input and output are reflected.
 *
 * @note If you are planning to use a CRC based on poly 0x1012 the function
 * crc16_ccitt() is faster and thus recommended over this one.
 *
 * The following checksums can, among others, be calculated by this function,
 * depending on the value provided for the initial seed and the value the final
 * calculated CRC is XORed with:
 *
 * - CRC-16/ANSI, CRC-16/MODBUS, CRC-16/USB, CRC-16/IBM
 *   https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-modbus
 *   poly: 0x8005 (0xA001) initial seed: 0xffff, xor output: 0x0000
 *
 * @param poly The polynomial to use omitting the leading x^16
 *             coefficient. Important: please reflect the poly. For example,
 *             use 0xA001 instead of 0x8005 for CRC-16-MODBUS.
 * @param seed Initial value for the CRC computation
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC16 value (without any XOR applied to it)
 */
uint16_t crc16_reflect(uint16_t poly, uint16_t seed, const uint8_t *src, size_t len);
/**
 * @brief Generic function for computing CRC 8
 *
 * Compute CRC 8 by passing in the address of the input, the input length
 * and polynomial used in addition to the initial value.
 *
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 * @param polynomial The polynomial to use omitting the leading x^8
 *        coefficient
 * @param initial_value Initial value for the CRC computation
 * @param reversed Should we use reflected/reversed values or not
 *
 * @return The computed CRC8 value
 */
uint8_t crc8(const uint8_t *src, size_t len, uint8_t polynomial, uint8_t initial_value,
             bool reversed);

/**
 * @brief Compute the checksum of a buffer with polynomial 0x1021, reflecting
 *        input and output.
 *
 * This function is able to calculate any CRC that uses 0x1021 as it polynomial
 * and requires reflecting both the input and the output. It is a fast variant
 * that runs in O(n) time, where n is the length of the input buffer.
 *
 * The following checksums can, among others, be calculated by this function,
 * depending on the value provided for the initial seed and the value the final
 * calculated CRC is XORed with:
 *
 * - CRC-16/CCITT, CRC-16/CCITT-TRUE, CRC-16/KERMIT
 *   https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-kermit
 *   initial seed: 0x0000, xor output: 0x0000
 *
 * - CRC-16/X-25, CRC-16/IBM-SDLC, CRC-16/ISO-HDLC
 *   https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-ibm-sdlc
 *   initial seed: 0xffff, xor output: 0xffff
 *
 * @note To calculate the CRC across non-contiguous blocks use the return
 *       value from block N-1 as the seed for block N.
 *
 * See ITU-T Recommendation V.41 (November 1988).
 *
 * @param seed Value to seed the CRC with
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC16 value (without any XOR applied to it)
 */
uint16_t crc16_ccitt(uint16_t seed, const uint8_t *src, size_t len);

/**
 * @brief Compute the checksum of a buffer with polynomial 0x1021, no
 *        reflection of input or output.
 *
 * This function is able to calculate any CRC that uses 0x1021 as it polynomial
 * and requires no reflection on  both the input and the output. It is a fast
 * variant that runs in O(n) time, where n is the length of the input buffer.
 *
 * The following checksums can, among others, be calculated by this function,
 * depending on the value provided for the initial seed and the value the final
 * calculated CRC is XORed with:
 *
 * - CRC-16/XMODEM, CRC-16/ACORN, CRC-16/LTE
 *   https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-xmodem
 *   initial seed: 0x0000, xor output: 0x0000
 *
 * - CRC16/CCITT-FALSE, CRC-16/IBM-3740, CRC-16/AUTOSAR
 *   https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-ibm-3740
 *   initial seed: 0xffff, xor output: 0x0000
 *
 * - CRC-16/GSM
 *   https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-gsm
 *   initial seed: 0x0000, xor output: 0xffff
 *
 * @note To calculate the CRC across non-contiguous blocks use the return
 *       value from block N-1 as the seed for block N.
 *
 * See ITU-T Recommendation V.41 (November 1988) (MSB first).
 *
 * @param seed Value to seed the CRC with
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC16 value (without any XOR applied to it)
 */
uint16_t crc16_itu_t(uint16_t seed, const uint8_t *src, size_t len);

/**
 * @brief Compute the ANSI (or Modbus) variant of CRC-16
 *
 * The ANSI variant of CRC-16 uses 0x8005 (0xA001 reflected) as its polynomial
 * with the initial * value set to 0xffff.
 *
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC16 value
 */
static inline uint16_t crc16_ansi(const uint8_t *src, size_t len)
{
    return crc16_reflect(0xA001, 0xffff, src, len);
}

/**
 * @brief Generate IEEE conform CRC32 checksum.
 *
 * @param  data         Pointer to data on which the CRC should be calculated.
 * @param  len          Data length.
 *
 * @return CRC32 value.
 *
 */
uint32_t crc32_ieee(const uint8_t *data, size_t len);

/**
 * @brief Update an IEEE conforming CRC32 checksum.
 *
 * @param crc   CRC32 checksum that needs to be updated.
 * @param data  Pointer to data on which the CRC should be calculated.
 * @param len   Data length.
 *
 * @return CRC32 value.
 *
 */
uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len);

/**
 * @brief Calculate CRC32C (Castagnoli) checksum.
 *
 * @param crc       CRC32C checksum that needs to be updated.
 * @param data      Pointer to data on which the CRC should be calculated.
 * @param len       Data length.
 * @param first_pkt Whether this is the first packet in the stream.
 * @param last_pkt  Whether this is the last packet in the stream.
 *
 * @return CRC32 value.
 *
 */
uint32_t crc32_c(uint32_t crc, const uint8_t *data,
                 size_t len, bool first_pkt, bool last_pkt);

/**
 * @brief Update a CRC-32K/4.2 (*op) (Koopman) checksum. This is a good HD=4
 * checksum up to 2,147,483,615 bits and HD=5/6 up to 6,167 bits.
 *
 * Hamming Distance and properties:
 *
 * - Polynomial: 0x93a409eb
 * - reflect-in: false
 * - initial value (xor-in): provided by caller as crc argument (0xFFFFFFFF is OK)
 * - reflect-out: false
 * - xor-out: 0
 * - HD=4 @ 2,147,483,615 bits
 * - HD=5 @ 6,167 bits
 * - HD=6 @ 6,167 bits
 * - HD=7 @ 148 bits
 *
 * Reference: https://users.ece.cmu.edu/~koopman/crc/crc32.html
 *
 * @param crc       CRC32 checksum that needs to be updated.
 * @param data      Pointer to data on which the CRC should be calculated.
 * @param len       Data length.
 *
 * @return CRC32 value.
 */
uint32_t crc32_k_4_2_update(uint32_t crc, const uint8_t *data, size_t len);

/**
 * @brief Compute CCITT variant of CRC 8
 *
 * Normal CCITT variant of CRC 8 is using 0x07.
 *
 * @param initial_value Initial value for the CRC computation
 * @param buf Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC8 value
 */
uint8_t crc8_ccitt(uint8_t initial_value, const void *buf, size_t len);

/**
 * @brief Compute ROHC variant of CRC 8
 *
 * ROHC (Robust Header Compression) variant of CRC 8.
 * Uses 0x07 as the polynomial with reflection.
 *
 * @param initial_value Initial value for the CRC computation
 * @param buf Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC8 value
 */
uint8_t crc8_rohc(uint8_t initial_value, const void *buf, size_t len);

/**
 * @brief Compute the CRC-7 checksum of a buffer.
 *
 * See JESD84-A441.  Used by the MMC protocol.  Uses 0x09 as the
 * polynomial with no reflection.  The CRC is left
 * justified, so bit 7 of the result is bit 6 of the CRC.
 *
 * @param seed Value to seed the CRC with
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC7 value
 */
uint8_t crc7_be(uint8_t seed, const uint8_t *src, size_t len);

/**
 * @brief Compute the CRC-4 checksum of a buffer.
 *
 * Used by the TMAG5170 sensor. Uses 0x03 as the
 * polynomial with no reflection. 4 most significant
 * bits of the CRC result will be set to zero.
 *
 * @param seed Value to seed the CRC with
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 *
 * @return The computed CRC4 value
 */
uint8_t crc4_ti(uint8_t seed, const uint8_t *src, size_t len);

/**
 * @brief Generic function for computing CRC 4
 *
 * Compute CRC 4 by passing in the address of the input, the input length
 * and polynomial used in addition to the initial value. The input buffer
 * must be aligned to a whole byte. It is guaranteed that 4 most significant
 * bits of the result will be set to zero.
 *
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 * @param polynomial The polynomial to use omitting the leading x^4
 *        coefficient
 * @param initial_value Initial value for the CRC computation
 * @param reversed Should we use reflected/reversed values or not
 *
 * @return The computed CRC4 value
 */
uint8_t crc4(const uint8_t *src, size_t len, uint8_t polynomial, uint8_t initial_value,
             bool reversed);

/**
 * @brief Generate an OpenPGP CRC-24 checksum as defined in RFC 4880 section 6.1.
 *
 * @param data A pointer to the data on which the CRC will be calculated.
 * @param len Data length in bytes.
 *
 * @return The CRC-24 value.
 */
uint32_t crc24_pgp(const uint8_t *data, size_t len);

/**
 * @brief Update an OpenPGP CRC-24 checksum.
 *
 * @param crc The CRC-24 checksum that needs to be updated. The full 32-bit value of the CRC needs
 *            to be used between calls, do not mask the value to keep only the last 24 bits.
 * @param data A pointer to the data on which the CRC will be calculated.
 * @param len  Data length in bytes.
 *
 * @return The CRC-24 value. When the last buffer of data has been processed, mask the value
 *         with CRC24_FINAL_VALUE_MASK to keep only the meaningful 24 bits of the CRC result.
 */
uint32_t crc24_pgp_update(uint32_t crc, const uint8_t *data, size_t len);

/**
 * @brief Calculate an RTCM3 CRC24Q frame checksum
 *
 * @param[in] data RTCM3 Frame
 * @param[in] len Frame length in bytes.
 *
 * @return 0 if the data-frame contains a checksum and it matches.
 * @return Result if data-frame does not contain checksum.
 */
uint32_t crc24q_rtcm3(const uint8_t *data, size_t len);

/**
 * @brief Compute a CRC checksum, in a generic way.
 *
 * This is a dispatch function that calls the individual CRC routine
 * determined by @p type.
 *
 * For 7, 8, 16 and 24-bit CRCs, the relevant @p seed and @p poly values should
 * be passed in via the least-significant byte(s).
 *
 * Similarly, for 7, 8, 16 and 24-bit CRCs, the relevant result is stored in the
 * least-significant byte(s) of the returned value.
 *
 * @param type CRC algorithm to use.
 * @param src Input bytes for the computation
 * @param len Length of the input in bytes
 * @param seed Seed or existing CRC value to update
 * @param poly The polynomial to use omitting the leading coefficient
 * @param reflect Should we use reflected/reversed values or not
 * @param first Whether this is the first packet in the stream.
 * @param last Whether this is the last packet in the stream.
 * @return uint32_t the computed CRC value
 */
static inline uint32_t crc_by_type(enum crc_type type, const uint8_t *src, size_t len,
                                   uint32_t seed, uint32_t poly, bool reflect, bool first,
                                   bool last)
{
    switch (type) {
    case CRC4:
        return crc4(src, len, poly, seed, reflect);
    case CRC4_TI:
        return crc4_ti(seed, src, len);
    case CRC7_BE:
        return crc7_be(seed, src, len);
    case CRC8:
        return crc8(src, len, poly, seed, reflect);
    case CRC8_CCITT:
        return crc8_ccitt(seed, src, len);
    case CRC8_ROHC:
        return crc8_rohc(seed, src, len);
    case CRC16:
        if (reflect) {
            return crc16_reflect(poly, seed, src, len);
        } else {
            return crc16(poly, seed, src, len);
        }
    case CRC16_ANSI:
        return crc16_ansi(src, len);
    case CRC16_CCITT:
        return crc16_ccitt(seed, src, len);
    case CRC16_ITU_T:
        return crc16_itu_t(seed, src, len);
    case CRC24_PGP: {
        uint32_t crc = crc24_pgp_update(seed, src, len);

        if (last) {
            crc &= CRC24_FINAL_VALUE_MASK;
        }
        return crc;
    }
    case CRC32_C:
        return crc32_c(seed, src, len, first, last);
    case CRC32_IEEE:
        return crc32_ieee_update(seed, src, len);
    case CRC32_K_4_2:
        return crc32_k_4_2_update(seed, src, len);
    default:
        break;
    }

    __ASSERT_NO_MSG(false);
    return -1;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
