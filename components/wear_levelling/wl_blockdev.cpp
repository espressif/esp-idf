/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <new>
#include <sys/lock.h>
#include "wear_levelling.h"
#include "WL_Config.h"
#include "WL_Ext_Cfg.h"
#include "WL_Flash.h"
#include "WL_Ext_Perf.h"
#include "WL_Ext_Safe.h"
#include "BDL_Access.h"

#ifndef WL_DEFAULT_UPDATERATE
#define WL_DEFAULT_UPDATERATE   16
#endif

#ifndef WL_DEFAULT_TEMP_BUFF_SIZE
#define WL_DEFAULT_TEMP_BUFF_SIZE   32
#endif

#ifndef WL_DEFAULT_WRITE_SIZE
#define WL_DEFAULT_WRITE_SIZE   16
#endif

#ifndef WL_DEFAULT_START_ADDR
#define WL_DEFAULT_START_ADDR   0
#endif

#ifndef WL_CURRENT_VERSION
#define WL_CURRENT_VERSION  2
#endif

static const char *TAG = "wl_blockdev";

/* ========================================================================= */
/* BDL path – standalone WL instance, no wl_handle_t                         */
/* ========================================================================= */

typedef struct {
    WL_Flash *wl_instance;
    BDL_Access *bdl_access;
    esp_blockdev_handle_t bottom_bdl;
    _lock_t lock;
} wl_bdl_ctx_t;

static esp_err_t wl_bdl_read(esp_blockdev_handle_t dev, uint8_t *dst, size_t dst_size,
                             uint64_t src_addr, size_t len)
{
    if (dst_size < len) {
        return ESP_ERR_INVALID_ARG;
    }
    if (src_addr % dev->geometry.read_size != 0 || len % dev->geometry.read_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)dev->ctx;
    _lock_acquire(&ctx->lock);
    esp_err_t ret = ctx->wl_instance->read((size_t)src_addr, dst, len);
    _lock_release(&ctx->lock);
    return ret;
}

static esp_err_t wl_bdl_write(esp_blockdev_handle_t dev, const uint8_t *src,
                              uint64_t dst_addr, size_t len)
{
    if (dst_addr % dev->geometry.write_size != 0 || len % dev->geometry.write_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)dev->ctx;
    _lock_acquire(&ctx->lock);
    esp_err_t ret = ctx->wl_instance->write((size_t)dst_addr, src, len);
    _lock_release(&ctx->lock);
    return ret;
}

static esp_err_t wl_bdl_erase(esp_blockdev_handle_t dev, uint64_t start, size_t len)
{
    if (start % dev->geometry.erase_size != 0 || len % dev->geometry.erase_size) {
        return ESP_ERR_INVALID_SIZE;
    }

    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)dev->ctx;
    _lock_acquire(&ctx->lock);
    esp_err_t ret = ctx->wl_instance->erase_range((size_t)start, len);
    _lock_release(&ctx->lock);
    return ret;
}

static esp_err_t wl_bdl_sync(esp_blockdev_handle_t dev)
{
    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)dev->ctx;
    _lock_acquire(&ctx->lock);
    esp_err_t ret = ctx->wl_instance->flush();
    _lock_release(&ctx->lock);
    return ret;
}

static esp_err_t wl_bdl_ioctl(esp_blockdev_handle_t dev, const uint8_t cmd, void *args)
{
    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)dev->ctx;
    if (ctx->bottom_bdl == ESP_BLOCKDEV_HANDLE_INVALID) {
        return ESP_ERR_NOT_ALLOWED;
    }
    if (ctx->bottom_bdl->ops->ioctl == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ctx->bottom_bdl->ops->ioctl(ctx->bottom_bdl, cmd, args);
}

static esp_err_t wl_bdl_release(esp_blockdev_handle_t dev)
{
    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)dev->ctx;

    if (ctx->wl_instance) {
        if (!ctx->bdl_access->is_readonly()) {
            ctx->wl_instance->flush();
        }
        ctx->wl_instance->~WL_Flash();
        free(ctx->wl_instance);
    }
    if (ctx->bdl_access) {
        ctx->bdl_access->~BDL_Access();
        free(ctx->bdl_access);
    }

    _lock_close(&ctx->lock);
    free(ctx);
    free(dev);
    return ESP_OK;
}

static const esp_blockdev_ops_t s_wl_bdl_ops = {
    .read    = wl_bdl_read,
    .write   = wl_bdl_write,
    .erase   = wl_bdl_erase,
    .sync    = wl_bdl_sync,
    .ioctl   = wl_bdl_ioctl,
    .release = wl_bdl_release,
};

esp_err_t wl_get_blockdev(const esp_blockdev_handle_t bdl_bottom_device,
                          esp_blockdev_handle_t *out_bdl_handle_ptr)
{
    if (bdl_bottom_device == ESP_BLOCKDEV_HANDLE_INVALID || out_bdl_handle_ptr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_bdl_handle_ptr = ESP_BLOCKDEV_HANDLE_INVALID;

    if (bdl_bottom_device->geometry.disk_size > UINT32_MAX) {
        ESP_LOGE(TAG, "%s: disk_size 0x%016" PRIx64 " exceeds uint32_t range, "
                 "WL configuration would be silently truncated",
                 __func__, bdl_bottom_device->geometry.disk_size);
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t result = ESP_OK;

    wl_bdl_ctx_t *ctx = (wl_bdl_ctx_t *)calloc(1, sizeof(wl_bdl_ctx_t));
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }
    ctx->bottom_bdl = bdl_bottom_device;

    void *access_mem = malloc(sizeof(BDL_Access));
    if (!access_mem) {
        free(ctx);
        return ESP_ERR_NO_MEM;
    }
    ctx->bdl_access = new (access_mem) BDL_Access(bdl_bottom_device);

    void *wl_mem = NULL;

#if CONFIG_WL_SECTOR_SIZE == 512
#if CONFIG_WL_SECTOR_MODE == 1
    wl_mem = malloc(sizeof(WL_Ext_Safe));
    if (!wl_mem) { result = ESP_ERR_NO_MEM; goto fail; }
    ctx->wl_instance = new (wl_mem) WL_Ext_Safe();
#else
    wl_mem = malloc(sizeof(WL_Ext_Perf));
    if (!wl_mem) { result = ESP_ERR_NO_MEM; goto fail; }
    ctx->wl_instance = new (wl_mem) WL_Ext_Perf();
#endif
#endif
#if CONFIG_WL_SECTOR_SIZE == 4096
    wl_mem = malloc(sizeof(WL_Flash));
    if (!wl_mem) { result = ESP_ERR_NO_MEM; goto fail; }
    ctx->wl_instance = new (wl_mem) WL_Flash();
#endif

    {
        wl_ext_cfg_t cfg;
        cfg.wl_partition_start_addr   = WL_DEFAULT_START_ADDR;
        cfg.wl_partition_size         = (uint32_t)bdl_bottom_device->geometry.disk_size;
        cfg.wl_page_size              = bdl_bottom_device->geometry.erase_size;
        cfg.flash_sector_size         = bdl_bottom_device->geometry.erase_size;
        cfg.wl_update_rate            = WL_DEFAULT_UPDATERATE;
        cfg.wl_pos_update_record_size = WL_DEFAULT_WRITE_SIZE;
        cfg.version                   = WL_CURRENT_VERSION;
        cfg.wl_temp_buff_size         = WL_DEFAULT_TEMP_BUFF_SIZE;
        cfg.fat_sector_size           = CONFIG_WL_SECTOR_SIZE;

        result = ctx->wl_instance->config(&cfg, ctx->bdl_access);
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "%s: config failed, result=0x%x", __func__, result);
            goto fail;
        }

        result = ctx->wl_instance->init();
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "%s: init failed, result=0x%x", __func__, result);
            goto fail;
        }
    }

    {
        esp_blockdev_t *dev = (esp_blockdev_t *)calloc(1, sizeof(esp_blockdev_t));
        if (!dev) { result = ESP_ERR_NO_MEM; goto fail; }

        dev->ctx = ctx;
        dev->ops = &s_wl_bdl_ops;
        dev->device_flags = bdl_bottom_device->device_flags;
        dev->geometry.disk_size              = ctx->wl_instance->get_flash_size();
        dev->geometry.read_size              = bdl_bottom_device->geometry.read_size;
        dev->geometry.write_size             = bdl_bottom_device->geometry.write_size;
        dev->geometry.erase_size             = ctx->wl_instance->get_sector_size();
        dev->geometry.recommended_read_size  = bdl_bottom_device->geometry.recommended_read_size;
        dev->geometry.recommended_write_size = bdl_bottom_device->geometry.recommended_write_size;
        dev->geometry.recommended_erase_size = ctx->wl_instance->get_sector_size();

        _lock_init(&ctx->lock);
        *out_bdl_handle_ptr = dev;
    }
    return ESP_OK;

fail:
    if (ctx->wl_instance) { ctx->wl_instance->~WL_Flash(); free(ctx->wl_instance); }
    if (ctx->bdl_access)  { ctx->bdl_access->~BDL_Access(); free(ctx->bdl_access); }
    free(ctx);
    return result;
}
