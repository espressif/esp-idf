/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "bta/bta_gattc_co.h"
#include "bta/bta_gattc_ci.h"

// #include "btif_util.h"
#include "btm_int.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "osi/list.h"
#include "esp_err.h"
#include "osi/allocator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#if( defined BLE_INCLUDED ) && (BLE_INCLUDED == TRUE)
#if( defined BTA_GATT_INCLUDED ) && (GATTC_INCLUDED == TRUE)
// #if( defined GATTC_CACHE_NVS ) && (GATTC_CACHE_NVS == TRUE)

#define GATT_CACHE_PREFIX "gatt_"
#define INVALID_ADDR_NUM 0xff
#define MAX_DEVICE_IN_CACHE 50
#define MAX_ADDR_LIST_CACHE_BUF 2048

static const char *cache_key = "gattc_cache_key";
static const char *cache_addr = "cache_addr_tab";

typedef struct {
    //save the service data in the list according to the address
    nvs_handle_t cache_fp;
    BOOLEAN is_open;
    BD_ADDR addr;
    hash_key_t hash_key;
    list_t *assoc_addr;
}cache_addr_info_t;

typedef struct {
    //save the address list in the cache
    nvs_handle_t addr_fp;
    BOOLEAN is_open;
    UINT8 num_addr;
    cache_addr_info_t cache_addr[MAX_DEVICE_IN_CACHE];
}cache_env_t;

static cache_env_t *cache_env = NULL;

/* Protect |cache_env|; exported callouts re-enter each other (e.g. cacheOpen -> find_addr).
 *
 * The mutex is created exactly once from bta_gattc_co_cache_addr_init() on the
 * BT host startup task before any other callout becomes reachable, so we do
 * NOT need any spinlock around the creation. The *Static variant places the
 * storage in .bss and avoids any allocation. */
static StaticSemaphore_t s_cache_env_mutex_buf;
static SemaphoreHandle_t s_cache_env_mutex = NULL;

/* Must be called from the BT host startup task (single-threaded context)
 * before any other cache_env_* user becomes reachable. Idempotent. */
static void cache_env_mutex_init_once(void)
{
    if (s_cache_env_mutex == NULL) {
        s_cache_env_mutex = xSemaphoreCreateRecursiveMutexStatic(&s_cache_env_mutex_buf);
    }
}

static void cache_env_lock(void)
{
    if (s_cache_env_mutex != NULL) {
        (void)xSemaphoreTakeRecursive(s_cache_env_mutex, portMAX_DELAY);
    }
}

static void cache_env_unlock(void)
{
    if (s_cache_env_mutex != NULL) {
        xSemaphoreGiveRecursive(s_cache_env_mutex);
    }
}

/* Format a per-device GATT cache NVS namespace name into |buffer|.
 *
 * Output layout: GATT_CACHE_PREFIX (5 chars: "gatt_") + sizeof(hash_key_t)*2
 *                hex chars (currently 8) + NUL = 14 bytes total.
 *
 * Contract:
 *   |buffer| MUST be at least NVS_KEY_NAME_MAX_SIZE (16) bytes, which is also
 *   the NVS limit for a namespace name. Today's 14-byte output leaves only
 *   2 bytes of headroom -- if GATT_CACHE_PREFIX is ever lengthened or
 *   sizeof(hash_key_t) is ever increased so that
 *
 *       strlen(GATT_CACHE_PREFIX) + 2*sizeof(hash_key_t) + 1 > NVS_KEY_NAME_MAX_SIZE
 *
 *   this sprintf() will silently overflow the caller's stack buffer AND
 *   produce a namespace name that nvs_open() will reject. In that case
 *   switch to snprintf(buffer, NVS_KEY_NAME_MAX_SIZE, ...) and propagate
 *   the truncation as an error to callers.
 *
 * Callers (keep this list in sync if a new one is added):
 *   - cacheOpen()                         [open by current hash]
 *   - bta_gattc_co_cache_addr_save()      [erase old namespace on hash change]
 */
static void getFilename(char *buffer, hash_key_t hash)
{
    sprintf(buffer, "%s%02x%02x%02x%02x", GATT_CACHE_PREFIX,
            hash[0], hash[1], hash[2], hash[3]);
}

static void cacheClose(BD_ADDR bda)
{
    UINT8 index = 0;
    if (cache_env == NULL) {
        return;
    }
    if ((index = bta_gattc_co_find_addr_in_cache(bda)) != INVALID_ADDR_NUM) {
        if (cache_env->cache_addr[index].is_open) {
            nvs_close(cache_env->cache_addr[index].cache_fp);
            cache_env->cache_addr[index].is_open = FALSE;
        }
    }
}

static bool cacheOpen(BD_ADDR bda, bool to_save, UINT8 *index)
{
    if (cache_env == NULL) {
        return false;
    }
    UNUSED(to_save);
    /* NVS namespace name is limited to (NVS_KEY_NAME_MAX_SIZE - 1) characters.
     * getFilename() produces GATT_CACHE_PREFIX (5) + 8 hex chars + NUL = 14 bytes. */
    char fname[NVS_KEY_NAME_MAX_SIZE] = {0};
    UINT8 *assoc_addr = NULL;
    esp_err_t status = ESP_FAIL;
    hash_key_t hash_key = {0};
    if (((*index = bta_gattc_co_find_addr_in_cache(bda)) != INVALID_ADDR_NUM) ||
        ((assoc_addr = bta_gattc_co_cache_find_src_addr(bda, index)) != NULL)) {
        if (cache_env->cache_addr[*index].is_open) {
            return TRUE;
        } else {
            memcpy(hash_key, cache_env->cache_addr[*index].hash_key, sizeof(hash_key_t));
            getFilename(fname, hash_key);
            if ((status = nvs_open(fname, NVS_READWRITE, &cache_env->cache_addr[*index].cache_fp)) == ESP_OK) {
                // Set the open flag to TRUE when success to open the hash file.
                cache_env->cache_addr[*index].is_open = TRUE;
            }
        }
    }

    return ((status == ESP_OK) ? true : false);
}

static void cacheReset(BD_ADDR bda, BOOLEAN update)
{
    if (cache_env == NULL) {
        return;
    }
    UINT8 index = 0;
    //cache_env->cache_addr
    if ((index = bta_gattc_co_find_addr_in_cache(bda)) != INVALID_ADDR_NUM) {
        //clear the association address pending in the source address.
        bta_gattc_co_cache_clear_assoc_addr(bda);
        if (cache_env->cache_addr[index].is_open) {
            nvs_erase_all(cache_env->cache_addr[index].cache_fp);
            nvs_close(cache_env->cache_addr[index].cache_fp);
            cache_env->cache_addr[index].is_open = FALSE;
        } else {
            /* The entry exists but is not currently held open in this session;
             * try to (re)open it best-effort so we can erase the on-flash blob.
             * The recursive cache_env lock is held throughout, so cacheOpen()'s
             * internal find_addr_in_cache(bda) cannot disagree with the outer
             * find above, i.e. *index will remain valid here.  We deliberately
             * do NOT bail out on cacheOpen() failure: the entry must still be
             * removed from RAM to keep num_addr/cache_addr[] consistent and to
             * avoid OOB in callers. */
            (void)cacheOpen(bda, false, &index);
            if (cache_env->cache_addr[index].is_open) {
                nvs_erase_all(cache_env->cache_addr[index].cache_fp);
                nvs_close(cache_env->cache_addr[index].cache_fp);
                cache_env->cache_addr[index].is_open = FALSE;
            } else {
                APPL_TRACE_ERROR("%s: cacheOpen fail, evict RAM", __func__);
            }
        }
        if(cache_env->num_addr == 0) {
            APPL_TRACE_ERROR("%s cache addr list error", __func__);
            return;
        }

        // Free the assoc_addr list_t structure before array shift to prevent memory leak
        // Note: bta_gattc_co_cache_clear_assoc_addr only calls list_clear which clears nodes
        // but doesn't free the list_t structure itself
        if (cache_env->cache_addr[index].assoc_addr != NULL) {
            list_free(cache_env->cache_addr[index].assoc_addr);
            cache_env->cache_addr[index].assoc_addr = NULL;
        }

        if (cache_env->num_addr > MAX_DEVICE_IN_CACHE) {
            APPL_TRACE_WARNING("%s: num_addr %u exceeds max, clamping", __func__, cache_env->num_addr);
            cache_env->num_addr = MAX_DEVICE_IN_CACHE;
        }
        UINT8 num = cache_env->num_addr;
        //delete the server_bda in the addr_info list.
        for(UINT8 i = index; i < (num - 1); i++) {
            memcpy(&cache_env->cache_addr[i], &cache_env->cache_addr[i+1], sizeof(cache_addr_info_t));
        }
        //clear the last cache when delete a addr
        memset(&cache_env->cache_addr[num-1], 0, sizeof(cache_addr_info_t));
        //reduced the number address counter also
        cache_env->num_addr--;

        //don't need to update addr list to nvs flash
        if (!update) {
            return;
        }

        //update addr list to nvs flash
        if(cache_env->num_addr > 0) {
            //update
            UINT8 *p_buf = osi_malloc(MAX_ADDR_LIST_CACHE_BUF);
            if(!p_buf) {
                APPL_TRACE_ERROR("%s malloc error", __func__);
                return;
            }
            UINT16 length = cache_env->num_addr*(sizeof(BD_ADDR) + sizeof(hash_key_t));
            for (UINT8 i = 0; i < cache_env->num_addr; i++) {
                //copy the address to the buffer.
                memcpy(p_buf + i*(sizeof(BD_ADDR) + sizeof(hash_key_t)), cache_env->cache_addr[i].addr, sizeof(BD_ADDR));
                //copy the hash key to the buffer.
                memcpy(p_buf + i*(sizeof(BD_ADDR) + sizeof(hash_key_t)) + sizeof(BD_ADDR),
                        cache_env->cache_addr[i].hash_key, sizeof(hash_key_t));
            }
            if (cache_env->is_open) {
                if (nvs_set_blob(cache_env->addr_fp, cache_key, p_buf, length) != ESP_OK) {
                    APPL_TRACE_WARNING("%s, nvs set blob failed", __func__);
                }
            }
            osi_free(p_buf);

        } else {
            //erase
            if (cache_env->is_open) {
                nvs_erase_all(cache_env->addr_fp);
                nvs_close(cache_env->addr_fp);
                cache_env->is_open = FALSE;
            } else {
                APPL_TRACE_WARNING("cache_env status is error");
            }
        }
    }
}

/*****************************************************************************
**  Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         bta_gattc_co_cache_open
**
** Description      This callout function is executed by GATTC when a GATT server
**                  cache is ready to be sent.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  evt: call in event to be passed in when cache open is done.
**                  conn_id: connection ID of this cache operation attach to.
**                  to_save: open cache to save or to load.
**
** Returns          void.
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_co_cache_open(BD_ADDR server_bda, BOOLEAN to_save, UINT8 *index)
{
    cache_env_lock();
    /* open NV cache and send call in */
    tBTA_GATT_STATUS status = BTA_GATT_OK;
    if (cache_env == NULL) {
        cache_env_unlock();
        return BTA_GATT_ERROR;
    }
    if (!cacheOpen(server_bda, to_save, index)) {
        status = BTA_GATT_ERROR;
    }

    APPL_TRACE_DEBUG("%s() - status=%d", __func__, status);
    cache_env_unlock();
    return status;
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_load
**
** Description      This callout function is executed by GATT when server cache
**                  is required to load.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  evt: call in event to be passed in when cache save is done.
**                  num_attr: number of attribute to be save.
**                  attr_index: starting attribute index of the save operation.
**                  conn_id: connection ID of this cache operation attach to.
** Returns
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_co_cache_load(tBTA_GATTC_NV_ATTR *attr, UINT8 index)
{
    cache_env_lock();
#if (!CONFIG_BT_STACK_NO_LOG)
    UINT16              num_attr = 0;
#endif
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    size_t length = 0;

    if (cache_env == NULL) {
        cache_env_unlock();
        return BTA_GATT_ERROR;
    }

    if (index >= MAX_DEVICE_IN_CACHE) {
        cache_env_unlock();
        return BTA_GATT_ERROR;
    }
    // Read the size of memory space required for blob
    nvs_get_blob(cache_env->cache_addr[index].cache_fp, cache_key, NULL, &length);
    // Read previously saved blob if available
    esp_err_t err_code = nvs_get_blob(cache_env->cache_addr[index].cache_fp, cache_key, attr, &length);
#if (!CONFIG_BT_STACK_NO_LOG)
    num_attr = length / sizeof(tBTA_GATTC_NV_ATTR);
#endif
    status = (err_code == ESP_OK && length != 0) ? BTA_GATT_OK : BTA_GATT_ERROR;
    APPL_TRACE_DEBUG("%s() - read=%d, status=%d, err_code = %d",
                     __func__, num_attr, status, err_code);

    cache_env_unlock();
    return status;
 }

size_t bta_gattc_get_cache_attr_length(UINT8 index)
{
    cache_env_lock();
    size_t length = 0;

    if (cache_env == NULL) {
        cache_env_unlock();
        return 0;
    }

    if (index == INVALID_ADDR_NUM || index >= MAX_DEVICE_IN_CACHE) {
        cache_env_unlock();
        return 0;
    }

    // Read the size of memory space required for blob
    nvs_get_blob(cache_env->cache_addr[index].cache_fp, cache_key, NULL, &length);
    cache_env_unlock();
    return length;
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_save
**
** Description      This callout function is executed by GATT when a server cache
**                  is available to save.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  evt: call in event to be passed in when cache save is done.
**                  num_attr: number of attribute to be save.
**                  p_attr: pointer to the list of attributes to save.
**                  attr_index: starting attribute index of the save operation.
**                  conn_id: connection ID of this cache operation attach to.
** Returns
**
*******************************************************************************/
void bta_gattc_co_cache_save (BD_ADDR server_bda, UINT16 num_attr,
                              tBTA_GATTC_NV_ATTR *p_attr_list)
{
    cache_env_lock();

    if (cache_env == NULL) {
        cache_env_unlock();
        return;
    }

    tBTA_GATT_STATUS    status = BTA_GATT_OK;
    hash_key_t hash_key = {0};
    UINT8 index = INVALID_ADDR_NUM;
    //calculate  the hash value of the attribute table which should be added to the nvs flash.
    hash_function_blob((unsigned char *)p_attr_list, sizeof(tBTA_GATTC_NV_ATTR)*num_attr, hash_key);
    //save the address list to the nvs flash
    bta_gattc_co_cache_addr_save(server_bda, hash_key);

    if (cacheOpen(server_bda, TRUE, &index)) {
        esp_err_t err_code = nvs_set_blob(cache_env->cache_addr[index].cache_fp, cache_key,
                                          p_attr_list, sizeof(tBTA_GATTC_NV_ATTR)*num_attr);
        status = (err_code == ESP_OK) ? BTA_GATT_OK : BTA_GATT_ERROR;
    } else {
        status = BTA_GATT_ERROR;
    }

#if CONFIG_BT_STACK_NO_LOG
    (void) status;
#endif
    APPL_TRACE_DEBUG("%s() wrote hash_key = %x%x%x%x, num_attr = %d, status = %d.", __func__, hash_key[0], hash_key[1], hash_key[2], hash_key[3], num_attr, status);
    cache_env_unlock();
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_close
**
** Description      This callout function is executed by GATTC when a GATT server
**                  cache is written completely.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  conn_id: connection ID of this cache operation attach to.
**
** Returns          void.
**
*******************************************************************************/
void bta_gattc_co_cache_close(BD_ADDR server_bda, UINT16 conn_id)
{
    cache_env_lock();
    UNUSED(conn_id);
    cacheClose(server_bda);
    /* close NV when server cache is done saving or loading,
       does not need to do anything for now on Insight */

    BTIF_TRACE_DEBUG("%s()", __FUNCTION__);
    cache_env_unlock();
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_reset
**
** Description      This callout function is executed by GATTC to reset cache in
**                  application
**
** Parameter        server_bda: server bd address of this cache belongs to
**
** Returns          void.
**
*******************************************************************************/
void bta_gattc_co_cache_reset(BD_ADDR server_bda)
{
    cache_env_lock();
    cacheReset(server_bda, TRUE);
    cache_env_unlock();
}

void bta_gattc_co_cache_addr_init(void)
{
    cache_env_mutex_init_once();
    cache_env_lock();
    nvs_handle_t fp;
    esp_err_t err_code;
    size_t length = MAX_ADDR_LIST_CACHE_BUF;
    UINT8 *p_buf = osi_malloc(MAX_ADDR_LIST_CACHE_BUF);
    if (p_buf == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        cache_env_unlock();
        return;
    }

    cache_env = (cache_env_t *)osi_malloc(sizeof(cache_env_t));
    if (cache_env == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        osi_free(p_buf);
        cache_env_unlock();
        return;
    }

    memset(cache_env, 0x0, sizeof(cache_env_t));

    if ((err_code = nvs_open(cache_addr, NVS_READWRITE, &fp)) == ESP_OK) {
        cache_env->addr_fp = fp;
        cache_env->is_open = TRUE;
        // Read previously saved blob if available
        if ((err_code = nvs_get_blob(fp, cache_key, p_buf, &length)) != ESP_OK) {
            if (err_code == ESP_ERR_NVS_NOT_FOUND) {
                length = 0;
            } else {
                APPL_TRACE_ERROR("%s, Line = %d, nvs flash get blob data fail, err_code = 0x%x", __func__, __LINE__, err_code);
                osi_free(p_buf);
                if (cache_env->is_open) {
                    nvs_close(cache_env->addr_fp);
                    cache_env->is_open = FALSE;
                }
                osi_free(cache_env);
                cache_env = NULL;
                cache_env_unlock();
                return;
            }
        }
        const size_t rec_sz = sizeof(BD_ADDR) + sizeof(hash_key_t);
        if (length == 0) {
            cache_env->num_addr = 0;
            osi_free(p_buf);
            cache_env_unlock();
            return;
        }
        if ((length % rec_sz) != 0) {
            APPL_TRACE_ERROR("%s: bad blob len %zu", __func__, length);
            (void)nvs_erase_key(fp, cache_key);
            cache_env->num_addr = 0;
            osi_free(p_buf);
            if (cache_env->is_open) {
                nvs_close(cache_env->addr_fp);
                cache_env->is_open = FALSE;
            }
            osi_free(cache_env);
            cache_env = NULL;
            cache_env_unlock();
            return;
        }
        size_t n_entries = length / rec_sz;
        const BOOLEAN truncated = (n_entries > (size_t)MAX_DEVICE_IN_CACHE) ? TRUE : FALSE;
        if (truncated) {
            APPL_TRACE_WARNING("%s: trunc %zu->%d", __func__, n_entries, MAX_DEVICE_IN_CACHE);
            n_entries = MAX_DEVICE_IN_CACHE;
        }
        cache_env->num_addr = (UINT8)n_entries;
        //read the address from nvs flash to cache address list.
        for (UINT8 i = 0; i < cache_env->num_addr; i++) {
            memcpy(cache_env->cache_addr[i].addr, p_buf + i * rec_sz, sizeof(BD_ADDR));
            memcpy(cache_env->cache_addr[i].hash_key, p_buf + i * rec_sz + sizeof(BD_ADDR), sizeof(hash_key_t));

            APPL_TRACE_DEBUG("cache_addr[%x] = %x:%x:%x:%x:%x:%x", i, cache_env->cache_addr[i].addr[0], cache_env->cache_addr[i].addr[1], cache_env->cache_addr[i].addr[2],
                             cache_env->cache_addr[i].addr[3], cache_env->cache_addr[i].addr[4], cache_env->cache_addr[i].addr[5]);
            APPL_TRACE_DEBUG("hash_key[%x] = %x%x%x%x", i, cache_env->cache_addr[i].hash_key[0], cache_env->cache_addr[i].hash_key[1],
                             cache_env->cache_addr[i].hash_key[2], cache_env->cache_addr[i].hash_key[3]);
            bta_gattc_co_cache_new_assoc_list(cache_env->cache_addr[i].addr, i);
        }
        if (truncated) {
            UINT16 out_len = (UINT16)(cache_env->num_addr * rec_sz);
            if (nvs_set_blob(fp, cache_key, p_buf, out_len) != ESP_OK) {
                APPL_TRACE_WARNING("%s: nvs trunc fail", __func__);
            }
        }
    } else {
        APPL_TRACE_ERROR("%s, Line = %d, nvs flash open fail, err_code = %x", __func__, __LINE__, err_code);
        osi_free(p_buf);
        osi_free(cache_env);
        cache_env = NULL;
        cache_env_unlock();
        return;
    }

    osi_free(p_buf);
    cache_env_unlock();
}

void bta_gattc_co_cache_addr_deinit(void)
{
    cache_env_lock();
    if(cache_env == NULL) {
        cache_env_unlock();
        return;
    }

    APPL_TRACE_DEBUG("%s is_open=%d", __func__, cache_env->is_open);

    if (!cache_env->is_open) {
        osi_free(cache_env);
        cache_env = NULL;
        cache_env_unlock();
        return;
    }

    nvs_close(cache_env->addr_fp);
    cache_env->is_open = false;

    UINT8 num = cache_env->num_addr;
    if (num > MAX_DEVICE_IN_CACHE) {
        num = MAX_DEVICE_IN_CACHE;
    }
    for (UINT8 i = 0; i < num; i++) {
        cache_addr_info_t *addr_info = &cache_env->cache_addr[i];
        if (addr_info->is_open) {
            nvs_close(addr_info->cache_fp);
            addr_info->is_open = false;
        }
        if (addr_info->assoc_addr != NULL) {
            list_free(addr_info->assoc_addr);
            addr_info->assoc_addr = NULL;
        }
    }

    osi_free(cache_env);
    cache_env = NULL;
    cache_env_unlock();
}

BOOLEAN bta_gattc_co_addr_in_cache(BD_ADDR bda)
{
    cache_env_lock();
    if (cache_env == NULL) {
        cache_env_unlock();
        return FALSE;
    }
    UINT8 addr_index = 0;
    UINT8 num = cache_env->num_addr;
    if (num > MAX_DEVICE_IN_CACHE) {
        num = MAX_DEVICE_IN_CACHE;
    }
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];
    for (addr_index = 0; addr_index < num; addr_index++, addr_info++) {
        if (!memcmp(addr_info->addr, bda, sizeof(BD_ADDR))) {
            cache_env_unlock();
            return TRUE;
        }
    }

    cache_env_unlock();
    return FALSE;
}

UINT8 bta_gattc_co_find_addr_in_cache(BD_ADDR bda)
{
    cache_env_lock();
    if (cache_env == NULL) {
        cache_env_unlock();
        return INVALID_ADDR_NUM;
    }
    UINT8 addr_index = 0;
    UINT8 num = cache_env->num_addr;
    if (num > MAX_DEVICE_IN_CACHE) {
        num = MAX_DEVICE_IN_CACHE;
    }
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];

    for (addr_index = 0; addr_index < num; addr_index++, addr_info++) {
        if (!memcmp(addr_info->addr, bda, sizeof(BD_ADDR))) {
            cache_env_unlock();
            return addr_index;
        }
    }

    cache_env_unlock();
    return INVALID_ADDR_NUM;
}

UINT8 bta_gattc_co_find_hash_in_cache(hash_key_t hash_key)
{
    cache_env_lock();
    if (cache_env == NULL) {
        cache_env_unlock();
        return INVALID_ADDR_NUM;
    }
    UINT8 index = 0;
    UINT8 num = cache_env->num_addr;
    if (num > MAX_DEVICE_IN_CACHE) {
        num = MAX_DEVICE_IN_CACHE;
    }
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];
    for (index = 0; index < num; index++, addr_info++) {
        if (!memcmp(addr_info->hash_key, hash_key, sizeof(hash_key_t))) {
            cache_env_unlock();
            return index;
        }
    }

    cache_env_unlock();
    return INVALID_ADDR_NUM;
}

UINT8 bta_gattc_co_get_addr_num(void)
{
    cache_env_lock();
    if (cache_env == NULL) {
        cache_env_unlock();
        return 0;
    }

    if (cache_env->num_addr > MAX_DEVICE_IN_CACHE) {
        cache_env_unlock();
        return MAX_DEVICE_IN_CACHE;
    }
    UINT8 n = cache_env->num_addr;
    cache_env_unlock();
    return n;
}

void bta_gattc_co_get_addr_list(BD_ADDR *addr_list)
{
    cache_env_lock();
    if (cache_env == NULL || addr_list == NULL) {
        cache_env_unlock();
        return;
    }
    UINT8 num = cache_env->num_addr;
    if (num > MAX_DEVICE_IN_CACHE) {
        num = MAX_DEVICE_IN_CACHE;
    }
    for (UINT8 i = 0; i < num; i++) {
        memcpy(addr_list[i], cache_env->cache_addr[i].addr, sizeof(BD_ADDR));
    }
    cache_env_unlock();
}

void bta_gattc_co_cache_addr_save(BD_ADDR bd_addr, hash_key_t hash_key)
{
    cache_env_lock();
    if (cache_env == NULL) {
        cache_env_unlock();
        return;
    }
    esp_err_t err_code;
    UINT8 index = 0;
    UINT8 new_index = cache_env->num_addr;
    UINT8 *p_buf = osi_malloc(MAX_ADDR_LIST_CACHE_BUF);
    if (p_buf == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        cache_env_unlock();
        return;
    }

    if (cache_env->num_addr > MAX_DEVICE_IN_CACHE) {
        APPL_TRACE_WARNING("%s: num_addr %u exceeds max, clamping", __func__, cache_env->num_addr);
        cache_env->num_addr = MAX_DEVICE_IN_CACHE;
    }

    // check the address list has the same address or not
    // for the same address, it's hash key may be change due to service change
    if ((index = bta_gattc_co_find_addr_in_cache(bd_addr)) != INVALID_ADDR_NUM) {
        APPL_TRACE_DEBUG("%s the bd_addr already in the cache list, index = %x", __func__, index);
        /* If the hash key changed (service change on the same peer), the
         * per-device NVS namespace is keyed by the OLD hash. Just overwriting
         * the in-RAM hash would leave cache_fp/is_open pointing at the old
         * namespace, so a subsequent cacheOpen() would short-circuit on
         * is_open==TRUE and the new attribute blob would be written into the
         * stale namespace. After reboot the addr table maps bd_addr to the
         * NEW hash and the load path would miss (and the old namespace is
         * orphaned in NVS). Erase the old namespace and clear is_open so
         * the next cacheOpen() reopens with the new hash-derived filename. */
        if (memcmp(cache_env->cache_addr[index].hash_key, hash_key, sizeof(hash_key_t)) != 0) {
            APPL_TRACE_WARNING("%s: hash chg "MACSTR" %02x%02x%02x%02x->%02x%02x%02x%02x, erase NVS",
                               __func__, MAC2STR(bd_addr),
                               cache_env->cache_addr[index].hash_key[0], cache_env->cache_addr[index].hash_key[1],
                               cache_env->cache_addr[index].hash_key[2], cache_env->cache_addr[index].hash_key[3],
                               hash_key[0], hash_key[1], hash_key[2], hash_key[3]);
            if (cache_env->cache_addr[index].is_open) {
                (void)nvs_erase_all(cache_env->cache_addr[index].cache_fp);
                nvs_close(cache_env->cache_addr[index].cache_fp);
                cache_env->cache_addr[index].is_open = FALSE;
            } else {
                /* Not currently held open in this session; transiently open
                 * the old namespace just to erase its blob. Best-effort:
                 * ignore failures so a missing/corrupt old namespace does
                 * not block writing the new one. */
                char fname_old[NVS_KEY_NAME_MAX_SIZE] = {0};
                nvs_handle_t old_fp;
                getFilename(fname_old, cache_env->cache_addr[index].hash_key);
                if (nvs_open(fname_old, NVS_READWRITE, &old_fp) == ESP_OK) {
                    (void)nvs_erase_all(old_fp);
                    nvs_close(old_fp);
                }
            }
        }
        //if the bd_addr already in the address list, update the hash key in it.
        memcpy(cache_env->cache_addr[index].addr, bd_addr, sizeof(BD_ADDR));
        memcpy(cache_env->cache_addr[index].hash_key, hash_key, sizeof(hash_key_t));
    } else {
        while (cache_env->num_addr >= MAX_DEVICE_IN_CACHE) {
            APPL_TRACE_WARNING("%s cache list full and remove the oldest addr info", __func__);
            UINT8 before = cache_env->num_addr;
            cacheReset(cache_env->cache_addr[0].addr, FALSE);
            if (cache_env->num_addr >= before) {
                APPL_TRACE_ERROR("%s: cache eviction failed", __func__);
                osi_free(p_buf);
                cache_env_unlock();
                return;
            }
        }
        new_index = cache_env->num_addr;
        if (new_index >= MAX_DEVICE_IN_CACHE) {
            APPL_TRACE_ERROR("%s: invalid new_index %u", __func__, new_index);
            osi_free(p_buf);
            cache_env_unlock();
            return;
        }
        memcpy(cache_env->cache_addr[new_index].addr, bd_addr, sizeof(BD_ADDR));
        memcpy(cache_env->cache_addr[new_index].hash_key, hash_key, sizeof(hash_key_t));
        cache_env->num_addr++;
        APPL_TRACE_DEBUG("%s bd_addr="MACSTR" num=%d", __func__, MAC2STR(bd_addr), cache_env->num_addr);
    }

    nvs_handle_t *fp = &cache_env->addr_fp;
    UINT16 length = cache_env->num_addr * (sizeof(BD_ADDR) + sizeof(hash_key_t));

    for (UINT8 i = 0; i < cache_env->num_addr; i++) {
        //copy the address to the buffer.
        memcpy(p_buf + i*(sizeof(BD_ADDR) + sizeof(hash_key_t)), cache_env->cache_addr[i].addr, sizeof(BD_ADDR));
        //copy the hash key to the buffer.
        memcpy(p_buf + i*(sizeof(BD_ADDR) + sizeof(hash_key_t)) + sizeof(BD_ADDR),
                cache_env->cache_addr[i].hash_key, sizeof(hash_key_t));
    }

    if (cache_env->is_open) {
        if ((err_code = nvs_set_blob(cache_env->addr_fp, cache_key, p_buf, length)) != ESP_OK) {
            APPL_TRACE_WARNING("%s(), nvs set blob fail, err %d", __func__, err_code);
        }
    } else {
        if ((err_code = nvs_open(cache_addr, NVS_READWRITE , fp)) == ESP_OK) {
            cache_env->is_open = true;
            if (( err_code = nvs_set_blob(cache_env->addr_fp, cache_key, p_buf, length)) != ESP_OK) {
                APPL_TRACE_WARNING("%s(), nvs set blob fail, err %d", __func__, err_code);
            }
        } else {
            APPL_TRACE_ERROR("%s, Line = %d, nvs flash open fail, err_code = %x", __func__, __LINE__, err_code);
        }
    }

    //free the buffer after used.
    osi_free(p_buf);
    cache_env_unlock();
}

BOOLEAN bta_gattc_co_cache_new_assoc_list(BD_ADDR src_addr, UINT8 index)
{
    cache_env_lock();
    if (cache_env == NULL) {
        cache_env_unlock();
        return FALSE;
    }
    UNUSED(src_addr);
    if (index >= MAX_DEVICE_IN_CACHE) {
        cache_env_unlock();
        return FALSE;
    }
    cache_addr_info_t *addr_info = &cache_env->cache_addr[index];
    /* Idempotent: if a list already exists at this slot (e.g. caller invoked
     * us twice on the same index), free it first so we don't leak the prior
     * list_t. The current sole caller is bta_gattc_co_cache_addr_init() which
     * runs after a fresh memset, so this path is normally a no-op; the guard
     * just keeps the function safe to reuse. */
    if (addr_info->assoc_addr != NULL) {
        list_free(addr_info->assoc_addr);
        addr_info->assoc_addr = NULL;
    }
    addr_info->assoc_addr = list_new(osi_free_func);
    BOOLEAN ok = (addr_info->assoc_addr != NULL ? TRUE : FALSE);
    cache_env_unlock();
    return ok;
}

BOOLEAN bta_gattc_co_cache_append_assoc_addr(BD_ADDR src_addr, BD_ADDR assoc_addr)
{
    cache_env_lock();
    UINT8 addr_index = 0;
    cache_addr_info_t *addr_info;
    UINT8 *p_assoc_buf = osi_malloc(sizeof(BD_ADDR));
    if(!p_assoc_buf) {
        cache_env_unlock();
        return FALSE;
    }
    memcpy(p_assoc_buf, assoc_addr, sizeof(BD_ADDR));
    if ((addr_index = bta_gattc_co_find_addr_in_cache(src_addr)) != INVALID_ADDR_NUM) {
        addr_info = &cache_env->cache_addr[addr_index];
        if (addr_info->assoc_addr == NULL) {
            addr_info->assoc_addr =list_new(osi_free_func);
        }
        if (addr_info->assoc_addr == NULL) {
            APPL_TRACE_ERROR("assoc_addr list creation failed");
            osi_free(p_assoc_buf);
            cache_env_unlock();
            return FALSE;
        }

        for (list_node_t *sn = list_begin(addr_info->assoc_addr);
             sn != list_end(addr_info->assoc_addr); sn = list_next(sn)) {
            if (!memcmp(list_node(sn), assoc_addr, sizeof(BD_ADDR))) {
                APPL_TRACE_WARNING("Association already exists");
                osi_free(p_assoc_buf);
                cache_env_unlock();
                return TRUE;
            }
        }

        if (!list_append(addr_info->assoc_addr, p_assoc_buf)) {
            APPL_TRACE_ERROR("Failed to append to assoc_addr list");
            osi_free(p_assoc_buf);
            cache_env_unlock();
            return FALSE;

        }
        cache_env_unlock();
        return TRUE;
    } else {
        osi_free(p_assoc_buf);
    }

    cache_env_unlock();
    return FALSE;
}

BOOLEAN bta_gattc_co_cache_remove_assoc_addr(BD_ADDR src_addr, BD_ADDR assoc_addr)
{
    cache_env_lock();
    UINT8 addr_index = 0;
    cache_addr_info_t *addr_info;
    if ((addr_index = bta_gattc_co_find_addr_in_cache(src_addr)) != INVALID_ADDR_NUM) {
        addr_info = &cache_env->cache_addr[addr_index];
        if (addr_info->assoc_addr != NULL) {
            for (list_node_t *sn = list_begin(addr_info->assoc_addr);
             sn != list_end(addr_info->assoc_addr); sn = list_next(sn)) {
                void *addr = list_node(sn);
                if (!memcmp(addr, assoc_addr, sizeof(BD_ADDR))) {
                    BOOLEAN removed = list_remove(addr_info->assoc_addr, addr);
                    cache_env_unlock();
                    return removed;
                }
            }
            //return list_remove(addr_info->assoc_addr, assoc_addr);
        } else {
            cache_env_unlock();
            return FALSE;
        }
    }

    cache_env_unlock();
    return FALSE;
}

UINT8* bta_gattc_co_cache_find_src_addr(BD_ADDR assoc_addr, UINT8 *index)
{
    cache_env_lock();
    if (index == NULL) {
        cache_env_unlock();
        return NULL;
    }
    if (cache_env == NULL) {
        *index = INVALID_ADDR_NUM;
        cache_env_unlock();
        return NULL;
    }
    UINT8 num = (cache_env->num_addr > MAX_DEVICE_IN_CACHE) ? MAX_DEVICE_IN_CACHE : cache_env->num_addr;
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];
    UINT8 *addr_data;

    for (int i = 0; i < num; i++) {
        if (addr_info->assoc_addr == NULL) {
            addr_info++;
            continue;
        }
       for (const list_node_t *node = list_begin(addr_info->assoc_addr); node != list_end(addr_info->assoc_addr);
            node = list_next(node)) {
            addr_data = (UINT8 *)list_node(node);
           if (!memcmp(addr_data, assoc_addr, sizeof(BD_ADDR))) {
               *index = i;
               UINT8 *ret = (UINT8 *)addr_info->addr;
               cache_env_unlock();
               return ret;
           }
       }
       addr_info++;
    }

    *index = INVALID_ADDR_NUM;
    cache_env_unlock();
    return NULL;
}

BOOLEAN bta_gattc_co_cache_clear_assoc_addr(BD_ADDR src_addr)
{
    cache_env_lock();
    UINT8 addr_index = 0;
    cache_addr_info_t *addr_info;
    if ((addr_index = bta_gattc_co_find_addr_in_cache(src_addr)) != INVALID_ADDR_NUM) {
        addr_info = &cache_env->cache_addr[addr_index];
        if (addr_info->assoc_addr != NULL) {
            list_clear(addr_info->assoc_addr);
            cache_env_unlock();
            return TRUE;
        } else {
            cache_env_unlock();
            return FALSE;
        }
    }

    cache_env_unlock();
    return FALSE;
}

// #endif /* #if( defined GATTC_CACHE_NVS ) && (GATTC_CACHE_NVS == TRUE) */
#endif /* #if( defined BLE_INCLUDED ) && (BLE_INCLUDED == TRUE) */
#endif /* #if( defined BTA_GATT_INCLUDED ) && (BTA_GATT_INCLUDED == TRUE) */
