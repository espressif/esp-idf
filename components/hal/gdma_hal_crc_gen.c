/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//
// Serially shift {data_in,lfsr_cur} N times to get {lfsr_next}
//
static uint32_t lfsr_serial_shift_crc(uint32_t data_in, uint32_t lfsr_cur,
                                      size_t crc_width, uint32_t crc_poly_hex,
                                      size_t num_bits_to_shift)
{
    uint32_t lfsr_next = lfsr_cur;

    for (int j = 0; j < num_bits_to_shift; j++) {
        uint32_t lfsr_upper_bit = lfsr_next >> (crc_width - 1);

        // shift the entire LFSR
        for (int i = crc_width - 1; i > 0; i--) {
            lfsr_next &= ~(1 << i);
            if (crc_poly_hex & (1 << i)) {
                // lfsr_next[i] = lfsr_next[i - 1] ^ lfsr_upper_bit ^ data_in[j];
                lfsr_next |= (((lfsr_next >> (i - 1)) ^ lfsr_upper_bit ^ (data_in >> j)) & 0x01) << i;
            } else {
                // lfsr_next[i] = lfsr_next[i - 1];
                lfsr_next |= (lfsr_next & (1 << (i - 1))) << 1;
            }
        }
        // lfsr_next[0] = lfsr_upper_bit ^ data_in[j];
        lfsr_next &= ~0x01;
        lfsr_next |= (lfsr_upper_bit ^ (data_in >> j)) & 0x01;
    }

    return lfsr_next;
}

void gdma_hal_build_parallel_crc_matrix(int crc_width, uint32_t crc_poly_hex, int data_width,
                                        uint32_t *lfsr_transform_matrix, uint32_t *data_transform_matrix)
{
    int N = crc_width;
    int M = data_width;
    uint32_t lfsr_cur = 0;  // N-bit LFSR
    uint32_t lfsr_next = 0; // N-bit LFSR
    uint32_t data_in = 0;   // M-bit data

    // LFSR-2-LFSR matrix[NxN], data_in=0
    for (int n1 = 0; n1 < N; n1++) {
        lfsr_cur = 1 << n1;
        lfsr_next = lfsr_serial_shift_crc(data_in, lfsr_cur,
                                          crc_width, crc_poly_hex,
                                          data_width);
        lfsr_transform_matrix[n1] = lfsr_next;
    }

    // Data-2-LFSR matrix[MxN], lfsr_cur=0
    lfsr_cur = 0;
    for (int m1 = 0; m1 < M; m1++) {
        data_in = 1 << m1;
        lfsr_next = lfsr_serial_shift_crc(data_in, lfsr_cur,
                                          crc_width, crc_poly_hex,
                                          data_width);
        // Invert CRC data bits
        data_transform_matrix[M - m1 - 1] = lfsr_next;
    }
}

uint32_t gdma_hal_get_data_mask_from_matrix(uint32_t *data_transform_matrix, int data_width, int crc_bit)
{
    uint32_t data_mask = 0;
    for (int j = 0; j < data_width; j++) {
        data_mask |= (data_transform_matrix[j] & (1 << crc_bit)) ? (1 << j) : 0;
    }
    return data_mask;
}

uint32_t gdma_hal_get_lfsr_mask_from_matrix(uint32_t *lfsr_transform_matrix, int crc_width, int crc_bit)
{
    uint32_t lfsr_mask = 0;
    for (int j = 0; j < crc_width; j++) {
        lfsr_mask |= (lfsr_transform_matrix[j] & (1 << crc_bit)) ? (1 << j) : 0;
    }
    return lfsr_mask;
}
