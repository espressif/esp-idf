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
#ifdef  BT_SUPPORT_NVM
#include <unistd.h>
#endif /* BT_SUPPORT_NVM */
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

#if( defined BLE_INCLUDED ) && (BLE_INCLUDED == TRUE)
#if( defined BTA_GATT_INCLUDED ) && (GATTC_INCLUDED == TRUE)
// #if( defined GATTC_CACHE_NVS ) && (GATTC_CACHE_NVS == TRUE)

#define GATT_CACHE_PREFIX "gatt_"
#define INVALID_ADDR_NUM 0xff
#define MAX_DEVICE_IN_CACHE 50
#define MAX_ADDR_LIST_CACHE_BUF 2048

#ifdef BT_SUPPORT_NVM
static FILE *sCacheFD = 0;
static void getFilename(char *buffer, BD_ADDR bda)
{
    sprintf(buffer, "%s%02x%02x%02x%02x%02x%02x", GATT_CACHE_PREFIX
            , bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
}

static void cacheClose(void)
{
    if (sCacheFD != 0) {
        fclose(sCacheFD);
        sCacheFD = 0;
    }
}

static bool cacheOpen(BD_ADDR bda, bool to_save)
{
    char fname[255] = {0};
    getFilename(fname, bda);

    cacheClose();
    sCacheFD = fopen(fname, to_save ? "w" : "r");

    return (sCacheFD != 0);
}

static void cacheReset(BD_ADDR bda)
{
    char fname[255] = {0};
    getFilename(fname, bda);
    unlink(fname);
}

#else

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

static void getFilename(char *buffer, hash_key_t hash)
{
    sprintf(buffer, "%s%02x%02x%02x%02x", GATT_CACHE_PREFIX,
            hash[0], hash[1], hash[2], hash[3]);
}

static void cacheClose(BD_ADDR bda)
{
    UINT8 index = 0;
    if ((index = bta_gattc_co_find_addr_in_cache(bda)) != INVALID_ADDR_NUM) {
        if (cache_env->cache_addr[index].is_open) {
            nvs_close(cache_env->cache_addr[index].cache_fp);
            cache_env->cache_addr[index].is_open = FALSE;
        }
    }
}

static bool cacheOpen(BD_ADDR bda, bool to_save, UINT8 *index)
{
    UNUSED(to_save);
    char fname[255] = {0};
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
    char fname[255] = {0};
    getFilename(fname, bda);
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
            cacheOpen(bda, false, &index);
            if (index == INVALID_ADDR_NUM) {
                APPL_TRACE_ERROR("%s INVALID ADDR NUM", __func__);
                return;
            }
            if (cache_env->cache_addr[index].is_open) {
                nvs_erase_all(cache_env->cache_addr[index].cache_fp);
                nvs_close(cache_env->cache_addr[index].cache_fp);
                cache_env->cache_addr[index].is_open = FALSE;
            } else {
                APPL_TRACE_ERROR("%s cacheOpen failed", __func__);
                return;
            }
        }
        if(cache_env->num_addr == 0) {
            APPL_TRACE_ERROR("%s cache addr list error", __func__);
            return;
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

#endif /* BT_SUPPORT_NVM */
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
    /* open NV cache and send call in */
    tBTA_GATT_STATUS status = BTA_GATT_OK;
    if (!cacheOpen(server_bda, to_save, index)) {
        status = BTA_GATT_ERROR;
    }

    APPL_TRACE_DEBUG("%s() - status=%d", __func__, status);
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
#if (!CONFIG_BT_STACK_NO_LOG)
    UINT16              num_attr = 0;
#endif
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    size_t length = 0;
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

    return status;
 }

size_t bta_gattc_get_cache_attr_length(UINT8 index)
{
    size_t length = 0;
    if (index == INVALID_ADDR_NUM) {
        return 0;
    }

    // Read the size of memory space required for blob
    nvs_get_blob(cache_env->cache_addr[index].cache_fp, cache_key, NULL, &length);
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
    UNUSED(conn_id);
//#ifdef BT_SUPPORT_NVM
    cacheClose(server_bda);
//#endif /* BT_SUPPORT_NVM */
    /* close NV when server cache is done saving or loading,
       does not need to do anything for now on Insight */

    BTIF_TRACE_DEBUG("%s()", __FUNCTION__);
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
    cacheReset(server_bda, TRUE);
}

void bta_gattc_co_cache_addr_init(void)
{
    nvs_handle_t fp;
    esp_err_t err_code;
    UINT8 num_addr;
    size_t length = MAX_ADDR_LIST_CACHE_BUF;
    UINT8 *p_buf = osi_malloc(MAX_ADDR_LIST_CACHE_BUF);
    if (p_buf == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        return;
    }

    cache_env = (cache_env_t *)osi_malloc(sizeof(cache_env_t));
    if (cache_env == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        osi_free(p_buf);
        return;
    }

    memset(cache_env, 0x0, sizeof(cache_env_t));

    if ((err_code = nvs_open(cache_addr, NVS_READWRITE, &fp)) == ESP_OK) {
        cache_env->addr_fp = fp;
        cache_env->is_open = TRUE;
        // Read previously saved blob if available
        if ((err_code = nvs_get_blob(fp, cache_key, p_buf, &length)) != ESP_OK) {
            if(err_code != ESP_ERR_NVS_NOT_FOUND) {
                APPL_TRACE_ERROR("%s, Line = %d, nvs flash get blob data fail, err_code = 0x%x", __func__, __LINE__, err_code);
            }
            osi_free(p_buf);
            return;
        }
        num_addr = length / (sizeof(BD_ADDR) + sizeof(hash_key_t));
        cache_env->num_addr = num_addr;
        //read the address from nvs flash to cache address list.
        for (UINT8 i = 0; i < num_addr; i++) {
            memcpy(cache_env->cache_addr[i].addr, p_buf + i*(sizeof(BD_ADDR) + sizeof(hash_key_t)), sizeof(BD_ADDR));
            memcpy(cache_env->cache_addr[i].hash_key,
                    p_buf + i*(sizeof(BD_ADDR) + sizeof(hash_key_t)) + sizeof(BD_ADDR), sizeof(hash_key_t));

            APPL_TRACE_DEBUG("cache_addr[%x] = %x:%x:%x:%x:%x:%x", i, cache_env->cache_addr[i].addr[0], cache_env->cache_addr[i].addr[1], cache_env->cache_addr[i].addr[2],
                             cache_env->cache_addr[i].addr[3], cache_env->cache_addr[i].addr[4], cache_env->cache_addr[i].addr[5]);
            APPL_TRACE_DEBUG("hash_key[%x] = %x%x%x%x", i, cache_env->cache_addr[i].hash_key[0], cache_env->cache_addr[i].hash_key[1],
                             cache_env->cache_addr[i].hash_key[2], cache_env->cache_addr[i].hash_key[3]);
            bta_gattc_co_cache_new_assoc_list(cache_env->cache_addr[i].addr, i);
        }
    } else {
        APPL_TRACE_ERROR("%s, Line = %d, nvs flash open fail, err_code = %x", __func__, __LINE__, err_code);
        osi_free(p_buf);
        return;
    }

    osi_free(p_buf);
    return;
}

void bta_gattc_co_cache_addr_deinit(void)
{
    if(!cache_env->is_open) {
        return;
    }
    nvs_close(cache_env->addr_fp);
    cache_env->is_open = false;

    for(UINT8 i = 0; i< cache_env->num_addr; i++) {
        cache_addr_info_t *addr_info = &cache_env->cache_addr[i];
        if(addr_info) {
            nvs_close(addr_info->cache_fp);
            addr_info->is_open = false;
            if(addr_info->assoc_addr) {
                list_free(addr_info->assoc_addr);
            }
        }
    }

    osi_free(cache_env);
    cache_env = NULL;
}

BOOLEAN bta_gattc_co_addr_in_cache(BD_ADDR bda)
{
    UINT8 addr_index = 0;
    UINT8 num = cache_env->num_addr;
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];
    for (addr_index = 0; addr_index < num; addr_index++) {
        if (!memcmp(addr_info->addr, bda, sizeof(BD_ADDR))) {
            return TRUE;
        }
    }

    return FALSE;
}

UINT8 bta_gattc_co_find_addr_in_cache(BD_ADDR bda)
{
    UINT8 addr_index = 0;
    UINT8 num = cache_env->num_addr;
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];

    for (addr_index = 0; addr_index < num; addr_index++, addr_info++) {
        if (!memcmp(addr_info->addr, bda, sizeof(BD_ADDR))) {
            return addr_index;
        }
    }

    return INVALID_ADDR_NUM;
}

UINT8 bta_gattc_co_find_hash_in_cache(hash_key_t hash_key)
{
    UINT8 index = 0;
    UINT8 num = cache_env->num_addr;
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];
    for (index = 0; index < num; index++) {
        if (!memcmp(addr_info->hash_key, hash_key, sizeof(hash_key_t))) {
            return index;
        }
    }

    return INVALID_ADDR_NUM;
}

UINT8 bta_gattc_co_get_addr_num(void)
{
    if (cache_env == NULL) {
        return 0;
    }

    return cache_env->num_addr;
}

void bta_gattc_co_get_addr_list(BD_ADDR *addr_list)
{
    UINT8 num = cache_env->num_addr;
    for (UINT8 i = 0; i < num; i++) {
        memcpy(addr_list[i], cache_env->cache_addr[i].addr, sizeof(BD_ADDR));
    }
}

void bta_gattc_co_cache_addr_save(BD_ADDR bd_addr, hash_key_t hash_key)
{
    esp_err_t err_code;
    UINT8 index = 0;
    UINT8 new_index = cache_env->num_addr;
    UINT8 *p_buf = osi_malloc(MAX_ADDR_LIST_CACHE_BUF);
    if (p_buf == NULL) {
        APPL_TRACE_ERROR("%s malloc failed!", __func__);
        return;
    }

    // check the address list has the same address or not
    // for the same address, it's hash key may be change due to service change
    if ((index = bta_gattc_co_find_addr_in_cache(bd_addr)) != INVALID_ADDR_NUM) {
        APPL_TRACE_DEBUG("%s the bd_addr already in the cache list, index = %x", __func__, index);
        //if the bd_addr already in the address list, update the hash key in it.
        memcpy(cache_env->cache_addr[index].addr, bd_addr, sizeof(BD_ADDR));
        memcpy(cache_env->cache_addr[index].hash_key, hash_key, sizeof(hash_key_t));
    } else {
        if (cache_env->num_addr >= MAX_DEVICE_IN_CACHE) {
            APPL_TRACE_WARNING("%s cache list full and remove the oldest addr info", __func__);
            cacheReset(cache_env->cache_addr[0].addr, FALSE);
        }
        new_index = cache_env->num_addr;
        assert(new_index < MAX_DEVICE_IN_CACHE);
        memcpy(cache_env->cache_addr[new_index].addr, bd_addr, sizeof(BD_ADDR));
        memcpy(cache_env->cache_addr[new_index].hash_key, hash_key, sizeof(hash_key_t));
        cache_env->num_addr++;
        APPL_TRACE_DEBUG("%s(), num = %d", __func__, cache_env->num_addr);
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
    return;
}

BOOLEAN bta_gattc_co_cache_new_assoc_list(BD_ADDR src_addr, UINT8 index)
{
    cache_addr_info_t *addr_info = &cache_env->cache_addr[index];
    addr_info->assoc_addr = list_new(osi_free_func);
    return (addr_info->assoc_addr != NULL ? TRUE : FALSE);
}

BOOLEAN bta_gattc_co_cache_append_assoc_addr(BD_ADDR src_addr, BD_ADDR assoc_addr)
{
    UINT8 addr_index = 0;
    cache_addr_info_t *addr_info;
    UINT8 *p_assoc_buf = osi_malloc(sizeof(BD_ADDR));
    if(!p_assoc_buf) {
        return FALSE;
    }
    memcpy(p_assoc_buf, assoc_addr, sizeof(BD_ADDR));
    if ((addr_index = bta_gattc_co_find_addr_in_cache(src_addr)) != INVALID_ADDR_NUM) {
        addr_info = &cache_env->cache_addr[addr_index];
        if (addr_info->assoc_addr == NULL) {
            addr_info->assoc_addr =list_new(NULL);
        }
        return list_append(addr_info->assoc_addr, p_assoc_buf);
    } else {
        osi_free(p_assoc_buf);
    }

    return FALSE;
}

BOOLEAN bta_gattc_co_cache_remove_assoc_addr(BD_ADDR src_addr, BD_ADDR assoc_addr)
{
    UINT8 addr_index = 0;
    cache_addr_info_t *addr_info;
    if ((addr_index = bta_gattc_co_find_addr_in_cache(src_addr)) != INVALID_ADDR_NUM) {
        addr_info = &cache_env->cache_addr[addr_index];
        if (addr_info->assoc_addr != NULL) {
            for (list_node_t *sn = list_begin(addr_info->assoc_addr);
             sn != list_end(addr_info->assoc_addr); sn = list_next(sn)) {
                void *addr = list_node(sn);
                if (!memcmp(addr, assoc_addr, sizeof(BD_ADDR))) {
                    return list_remove(addr_info->assoc_addr, addr);
                }
            }
            //return list_remove(addr_info->assoc_addr, assoc_addr);
        } else {
            return FALSE;
        }
    }

    return FALSE;
}

UINT8* bta_gattc_co_cache_find_src_addr(BD_ADDR assoc_addr, UINT8 *index)
{
    UINT8 num = cache_env->num_addr;
    cache_addr_info_t *addr_info = &cache_env->cache_addr[0];
    UINT8 *addr_data;
    //Check the assoc_addr list is NULL or not
    if (addr_info->assoc_addr == NULL) {
        *index = INVALID_ADDR_NUM;
        return NULL;
    }

    for (int i = 0; i < num; i++) {
       for (const list_node_t *node = list_begin(addr_info->assoc_addr); node != list_end(addr_info->assoc_addr);
            node = list_next(node)) {
            addr_data = (UINT8 *)list_node(node);
           if (!memcmp(addr_data, assoc_addr, sizeof(BD_ADDR))) {
               *index = i;
               return (UINT8 *)addr_info->addr;
           }
       }
       addr_info++;

       if (addr_info->assoc_addr == NULL) {
           *index = INVALID_ADDR_NUM;
           return NULL;
       }
    }

    *index = INVALID_ADDR_NUM;
    return NULL;
}

BOOLEAN bta_gattc_co_cache_clear_assoc_addr(BD_ADDR src_addr)
{
    UINT8 addr_index = 0;
    cache_addr_info_t *addr_info;
    if ((addr_index = bta_gattc_co_find_addr_in_cache(src_addr)) != INVALID_ADDR_NUM) {
        addr_info = &cache_env->cache_addr[addr_index];
        if (addr_info->assoc_addr != NULL) {
            list_clear(addr_info->assoc_addr);
        } else {
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

// #endif /* #if( defined GATTC_CACHE_NVS ) && (GATTC_CACHE_NVS == TRUE) */
#endif /* #if( defined BLE_INCLUDED ) && (BLE_INCLUDED == TRUE) */
#endif /* #if( defined BTA_GATT_INCLUDED ) && (BTA_GATT_INCLUDED == TRUE) */
