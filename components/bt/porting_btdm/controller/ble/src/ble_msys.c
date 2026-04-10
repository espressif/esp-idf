/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "queue.h"
#include "sdkconfig.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "syscfg/syscfg.h"
#endif

/*
 ***************************************************************************************************
 * Local Defined Macros
 ***************************************************************************************************
 */
#define BLE_MSYS_ALIGN(__n, __a)                                                                   \
    ((((__n) & ((__a) - 1)) == 0) ? (__n) : ((__n) + ((__a) - ((__n) & ((__a) - 1)))))

#if CONFIG_BT_NIMBLE_ENABLED
#define BLE_MSYS_1_BLOCK_COUNT       MYNEWT_VAL(MSYS_1_BLOCK_COUNT)
#define BLE_MSYS_1_BLOCK_SIZE        MYNEWT_VAL(MSYS_1_BLOCK_SIZE)
#define BLE_MSYS_2_BLOCK_COUNT       MYNEWT_VAL(MSYS_2_BLOCK_COUNT)
#define BLE_MSYS_2_BLOCK_SIZE        MYNEWT_VAL(MSYS_2_BLOCK_SIZE)
#define BLE_MSYS_BLOCK_FROM_HEAP     CONFIG_BT_NIMBLE_MSYS_BUF_FROM_HEAP
#else
#define BLE_MSYS_1_BLOCK_COUNT       CONFIG_BT_LE_MSYS_1_BLOCK_COUNT
#define BLE_MSYS_1_BLOCK_SIZE        CONFIG_BT_LE_MSYS_1_BLOCK_SIZE
#define BLE_MSYS_2_BLOCK_COUNT       CONFIG_BT_LE_MSYS_2_BLOCK_COUNT
#define BLE_MSYS_2_BLOCK_SIZE        CONFIG_BT_LE_MSYS_2_BLOCK_SIZE
#define BLE_MSYS_BLOCK_FROM_HEAP     CONFIG_BT_LE_MSYS_BUF_FROM_HEAP
#endif /* CONFIG_BT_NIMBLE_ENABLED */

#if BLE_MSYS_1_BLOCK_COUNT > 0
#define SYSINIT_MSYS_1_MEMBLOCK_SIZE BLE_MSYS_ALIGN(BLE_MSYS_1_BLOCK_SIZE, 4)
#define SYSINIT_MSYS_1_MEMPOOL_SIZE                                                                \
    OS_MEMPOOL_SIZE(BLE_MSYS_1_BLOCK_COUNT, SYSINIT_MSYS_1_MEMBLOCK_SIZE)
#else
#error "BLE_MSYS_1_BLOCK_COUNT should be larger than 0!"
#endif /* BLE_MSYS_1_BLOCK_COUNT > 0 */

#if BLE_MSYS_2_BLOCK_COUNT > 0
#define SYSINIT_MSYS_2_MEMBLOCK_SIZE BLE_MSYS_ALIGN(BLE_MSYS_2_BLOCK_SIZE, 4)
#define SYSINIT_MSYS_2_MEMPOOL_SIZE                                                                \
    OS_MEMPOOL_SIZE(BLE_MSYS_2_BLOCK_COUNT, SYSINIT_MSYS_2_MEMBLOCK_SIZE)
#else
#error #error "BLE_MSYS_2_BLOCK_COUNT should be larger than 0!"
#endif /* BLE_MSYS_2_BLOCK_COUNT > 0 */

/*
 ***************************************************************************************************
 * External Functions
 ***************************************************************************************************
 */
extern int r_esp_ble_msys_init(uint16_t msys_size1, uint16_t msys_size2, uint16_t msys_cnt1,
                               uint16_t msys_cnt2, uint8_t from_heap);
extern void r_esp_ble_msys_deinit(void);

/*
 ***************************************************************************************************
 * BLE MSYS Initialization
 ***************************************************************************************************
 */
int
ble_msys_init(void)
{
    return r_esp_ble_msys_init(SYSINIT_MSYS_1_MEMBLOCK_SIZE, SYSINIT_MSYS_2_MEMBLOCK_SIZE,
                               BLE_MSYS_1_BLOCK_COUNT, BLE_MSYS_2_BLOCK_COUNT,
                               BLE_MSYS_BLOCK_FROM_HEAP);
}

void
ble_msys_deinit(void)
{
    r_esp_ble_msys_deinit();
}
