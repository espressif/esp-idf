/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_rom_sys.h"
#include "multi_heap.h"
#include "esp_rom_tlsf.h"

typedef struct {
    int *ptr;
    size_t size;
} alloc_block_t;

static size_t try_malloc_write_free(void)
{
    size_t blk_sz = 512;
    size_t max_blks = esp_tee_heap_get_free_size() / blk_sz;
    size_t total_mem = 0;

    alloc_block_t *blocks = malloc(sizeof(alloc_block_t) * (max_blks + 1));
    if (!blocks) {
        return -1;
    }

    int n_blks = 0;

    // Attempt to allocate memory blocks and write to them
    for (int i = 0; i < max_blks; i++) {
        blocks[i].ptr = malloc(blk_sz);
        if (!blocks[i].ptr) {
            break;
        }
        blocks[i].size = blk_sz;
        total_mem += blk_sz;
        memset(blocks[i].ptr, 0xEF, blk_sz);
        n_blks++;
    }

    // Check if there is leftover memory that can be allocated
    size_t left = esp_tee_heap_get_free_size();
    if (left > 24 && n_blks == max_blks) {
        size_t rm_sz = left - 24; // Calculate remaining size to allocate
        blocks[n_blks].ptr = malloc(rm_sz);
        if (blocks[n_blks].ptr) {
            blocks[n_blks].size = rm_sz;
            total_mem += rm_sz;
            memset(blocks[n_blks].ptr, 0xEF, rm_sz);
            n_blks++;
        } else {
            esp_rom_printf("Failed to allocate leftover bytes\n");
            free(blocks);
            return -1;
        }
    }

    int *ptr = malloc(blk_sz);
    if (ptr) {
        esp_rom_printf("Illegal allocation\n");
        free(blocks);
        return -1;
    }

    // Verify the integrity of allocated memory blocks
    for (int i = 0; i < n_blks; i++) {
        unsigned char *block_ptr = (unsigned char *)blocks[i].ptr;
        for (size_t j = 0; j < blocks[i].size; j++) {
            if (block_ptr[j] != 0xEF) {
                esp_rom_printf("Corrupt heap\n");
                free(blocks);
                return -1;
            }
        }
        free(blocks[i].ptr);
    }

    free(blocks);
    return total_mem;
}

int _ss_esp_tee_test_heap_malloc_write_free(void)
{
    size_t prev_sz = 0;
    for (int i = 0; i < 3; i++) {
        size_t cur_sz = try_malloc_write_free();
        if (cur_sz == -1) {
            return -1;
        }

        if (i > 0 && cur_sz != prev_sz) {
            esp_rom_printf("Processed memory sizes differ between iterations!\n");
            return -1;
        }

        prev_sz = cur_sz;
    }

    return 0;
}
