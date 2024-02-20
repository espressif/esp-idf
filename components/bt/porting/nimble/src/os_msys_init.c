/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2022 Espressif Systems (Shanghai) CO LTD
 */

#include <assert.h>

#include "os/os.h"
#include "mem_api.h"
#include "bt_osi_mem.h"
#include "esp_err.h"

#if CONFIG_BT_NIMBLE_ENABLED
#include "syscfg/syscfg.h"
#endif

#define SYSINIT_PANIC_ASSERT(rc)        assert(rc);

static STAILQ_HEAD(, os_mbuf_pool) g_msys_pool_list =
    STAILQ_HEAD_INITIALIZER(g_msys_pool_list);

#if CONFIG_BT_NIMBLE_ENABLED
#define OS_MSYS_1_BLOCK_COUNT MYNEWT_VAL(MSYS_1_BLOCK_COUNT)
#define OS_MSYS_1_BLOCK_SIZE MYNEWT_VAL(MSYS_1_BLOCK_SIZE)
#define OS_MSYS_2_BLOCK_COUNT MYNEWT_VAL(MSYS_2_BLOCK_COUNT)
#define OS_MSYS_2_BLOCK_SIZE MYNEWT_VAL(MSYS_2_BLOCK_SIZE)

#define OS_MSYS_1_SANITY_MIN_COUNT MYNEWT_VAL(MSYS_1_SANITY_MIN_COUNT)
#define OS_MSYS_2_SANITY_MIN_COUNT MYNEWT_VAL(MSYS_2_SANITY_MIN_COUNT)
#if CONFIG_BT_NIMBLE_MSYS_BUF_FROM_HEAP
#define OS_MSYS_BLOCK_FROM_HEAP                 (1)
#else
#define OS_MSYS_BLOCK_FROM_HEAP                 (0)
#endif // CONFIG_BT_NIMBLE_MSYS_BUF_FROM_HEAP
#else
#define OS_MSYS_1_BLOCK_COUNT CONFIG_BT_LE_MSYS_1_BLOCK_COUNT
#define OS_MSYS_1_BLOCK_SIZE CONFIG_BT_LE_MSYS_1_BLOCK_SIZE
#define OS_MSYS_2_BLOCK_COUNT CONFIG_BT_LE_MSYS_2_BLOCK_COUNT
#define OS_MSYS_2_BLOCK_SIZE CONFIG_BT_LE_MSYS_2_BLOCK_SIZE

#define OS_MSYS_1_SANITY_MIN_COUNT 0
#define OS_MSYS_2_SANITY_MIN_COUNT 0

#if CONFIG_BT_LE_MSYS_BUF_FROM_HEAP
#define OS_MSYS_BLOCK_FROM_HEAP                 (1)
#else
#define OS_MSYS_BLOCK_FROM_HEAP                 (0)
#endif // CONFIG_BT_LE_MSYS_BUF_FROM_HEAP
#endif



#if OS_MSYS_1_BLOCK_COUNT > 0
#define SYSINIT_MSYS_1_MEMBLOCK_SIZE                \
    OS_ALIGN(OS_MSYS_1_BLOCK_SIZE, 4)
#define SYSINIT_MSYS_1_MEMPOOL_SIZE                 \
    OS_MEMPOOL_SIZE(OS_MSYS_1_BLOCK_COUNT,  \
                    SYSINIT_MSYS_1_MEMBLOCK_SIZE)

#if !CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER
static os_membuf_t *os_msys_init_1_data;
static struct os_mbuf_pool os_msys_init_1_mbuf_pool;
static struct os_mempool os_msys_init_1_mempool;
#endif // !CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER
#endif

#if OS_MSYS_2_BLOCK_COUNT > 0
#define SYSINIT_MSYS_2_MEMBLOCK_SIZE                \
    OS_ALIGN(OS_MSYS_2_BLOCK_SIZE, 4)
#define SYSINIT_MSYS_2_MEMPOOL_SIZE                 \
    OS_MEMPOOL_SIZE(OS_MSYS_2_BLOCK_COUNT,  \
                    SYSINIT_MSYS_2_MEMBLOCK_SIZE)

#if !CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER
static os_membuf_t *os_msys_init_2_data;
static struct os_mbuf_pool os_msys_init_2_mbuf_pool;
static struct os_mempool os_msys_init_2_mempool;
#endif // !CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER
#endif

#if CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER
extern int  esp_ble_msys_init(uint16_t msys_size1, uint16_t msys_size2, uint16_t msys_cnt1, uint16_t msys_cnt2, uint8_t from_heap);
extern void esp_ble_msys_deinit(void);

int os_msys_init(void)
{
    return esp_ble_msys_init(SYSINIT_MSYS_1_MEMBLOCK_SIZE,
                             SYSINIT_MSYS_2_MEMBLOCK_SIZE,
                             OS_MSYS_1_BLOCK_COUNT,
                             OS_MSYS_2_BLOCK_COUNT,
                             OS_MSYS_BLOCK_FROM_HEAP);
}

void os_msys_deinit(void)
{
    esp_ble_msys_deinit();
}

#else // CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER

#if OS_MSYS_SANITY_ENABLED

/**
 * Retrieves the minimum safe buffer count for an msys pool.  That is, the
 * lowest a pool's buffer count can be without causing the sanity check to
 * fail.
 *
 * @param idx                   The index of the msys pool to query.
 *
 * @return                      The msys pool's minimum safe buffer count.
 */
static int
IRAM_ATTR os_msys_sanity_min_count(int idx)
{
    switch (idx) {
    case 0:
        return OS_MSYS_1_SANITY_MIN_COUNT;

    case 1:
        return OS_MSYS_2_SANITY_MIN_COUNT;

    default:
        BLE_LL_ASSERT(0);
        return ESP_OK;
    }
}

static int
IRAM_ATTR os_msys_sanity(struct os_sanity_check *sc, void *arg)
{
    const struct os_mbuf_pool *omp;
    int min_count;
    int idx;

    idx = 0;
    STAILQ_FOREACH(omp, &g_msys_pool_list, omp_next) {
        min_count = os_msys_sanity_min_count(idx);
        if (omp->omp_pool->mp_num_free < min_count) {
            return OS_ENOMEM;
        }

        idx++;
    }

    return ESP_OK;
}
#endif

static void
os_msys_init_once(void *data, struct os_mempool *mempool,
                  struct os_mbuf_pool *mbuf_pool,
                  int block_count, int block_size, const char *name)
{
    int rc;

    rc = mem_init_mbuf_pool(data, mempool, mbuf_pool, block_count, block_size,
                            name);
    SYSINIT_PANIC_ASSERT(rc == 0);

    rc = os_msys_register(mbuf_pool);
    SYSINIT_PANIC_ASSERT(rc == 0);
}

int
os_msys_buf_alloc(void)
{
#if OS_MSYS_1_BLOCK_COUNT > 0
    os_msys_init_1_data = (os_membuf_t *)bt_osi_mem_calloc(1, (sizeof(os_membuf_t) * SYSINIT_MSYS_1_MEMPOOL_SIZE));
    if (!os_msys_init_1_data) {
        return ESP_ERR_NO_MEM;
    }
#endif

#if OS_MSYS_2_BLOCK_COUNT > 0
    os_msys_init_2_data = (os_membuf_t *)bt_osi_mem_calloc(1, (sizeof(os_membuf_t) * SYSINIT_MSYS_2_MEMPOOL_SIZE));
    if (!os_msys_init_2_data) {
#if OS_MSYS_1_BLOCK_COUNT > 0
       bt_osi_mem_free(os_msys_init_1_data);
       os_msys_init_1_data = NULL;
#endif
        return ESP_ERR_NO_MEM;
    }
#endif

    return ESP_OK;
}

void
os_msys_buf_free(void)
{
#if OS_MSYS_1_BLOCK_COUNT > 0
    bt_osi_mem_free(os_msys_init_1_data);
    os_msys_init_1_data = NULL;
#endif

#if OS_MSYS_2_BLOCK_COUNT > 0
    bt_osi_mem_free(os_msys_init_2_data);
    os_msys_init_2_data = NULL;
#endif

}

void os_msys_init(void)
{
#if OS_MSYS_SANITY_ENABLED
    int rc;
#endif

    os_msys_reset();

#if OS_MSYS_1_BLOCK_COUNT > 0
    os_msys_init_once(os_msys_init_1_data,
                      &os_msys_init_1_mempool,
                      &os_msys_init_1_mbuf_pool,
                      OS_MSYS_1_BLOCK_COUNT,
                      SYSINIT_MSYS_1_MEMBLOCK_SIZE,
                      "msys_1");
#endif

#if OS_MSYS_2_BLOCK_COUNT > 0
    os_msys_init_once(os_msys_init_2_data,
                      &os_msys_init_2_mempool,
                      &os_msys_init_2_mbuf_pool,
                      OS_MSYS_2_BLOCK_COUNT,
                      SYSINIT_MSYS_2_MEMBLOCK_SIZE,
                      "msys_2");
#endif

#if OS_MSYS_SANITY_ENABLED
    os_msys_sc.sc_func = os_msys_sanity;
    os_msys_sc.sc_checkin_itvl =
        OS_TICKS_PER_SEC * MYNEWT_VAL(MSYS_SANITY_TIMEOUT) / 1000;
    rc = os_sanity_check_register(&os_msys_sc);
    SYSINIT_PANIC_ASSERT(rc == 0);
#endif
}
#endif // CONFIG_BT_LE_MSYS_INIT_IN_CONTROLLER
