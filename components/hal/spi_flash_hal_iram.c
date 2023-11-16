/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#include "hal/spi_flash_hal.h"

#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
void spi_flash_hal_setup_auto_suspend_mode(spi_flash_host_inst_t *host);
void spi_flash_hal_disable_auto_resume_mode(spi_flash_host_inst_t *host);
void spi_flash_hal_disable_auto_suspend_mode(spi_flash_host_inst_t *host);
void spi_flash_hal_setup_auto_resume_mode(spi_flash_host_inst_t *host);
#define SPI_FLASH_TSHSL2_SAFE_VAL_NS (30)
#endif //SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

#ifndef CONFIG_SPI_FLASH_ROM_IMPL

#include "spi_flash_hal_common.inc"

// HAL for
//  - MEMSPI
//  - SPI1~3 on ESP32/S2/S3/C3/H4/C2
// The common part is in spi_flash_hal_common.inc

void spi_flash_hal_erase_chip(spi_flash_host_inst_t *host)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_erase_chip(dev);
#if SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE
    if((((spi_flash_hal_context_t*)host)->flags & SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_SUSPEND) == 0) {
        host->driver->poll_cmd_done(host);
    }
#else
    host->driver->poll_cmd_done(host);
#endif
}

// Only support 24bit address
void spi_flash_hal_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_addr_bitlen(dev, 24);
    spi_flash_ll_set_address(dev, start_address & ADDRESS_MASK_24BIT);
    spi_flash_ll_erase_sector(dev);

#if SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE
    if((((spi_flash_hal_context_t*)host)->flags & SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_SUSPEND) == 0) {
        host->driver->poll_cmd_done(host);
    }
#else
    host->driver->poll_cmd_done(host);
#endif
}

// Only support 24bit address
void spi_flash_hal_erase_block(spi_flash_host_inst_t *host, uint32_t start_address)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_addr_bitlen(dev, 24);
    spi_flash_ll_set_address(dev, start_address & ADDRESS_MASK_24BIT);
    spi_flash_ll_erase_block(dev);
#if SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE
    if((((spi_flash_hal_context_t*)host)->flags & SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_SUSPEND) == 0) {
        host->driver->poll_cmd_done(host);
    }
#else
    host->driver->poll_cmd_done(host);
#endif
}

// Only support 24bit address
void spi_flash_hal_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_addr_bitlen(dev, 24);
    spi_flash_ll_set_address(dev, (address & ADDRESS_MASK_24BIT) | (length << 24));
    spi_flash_ll_program_page(dev, buffer, length);
    host->driver->poll_cmd_done(host);
}

esp_err_t spi_flash_hal_set_write_protect(spi_flash_host_inst_t *host, bool wp)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_write_protect(dev, wp);
    host->driver->poll_cmd_done(host);
    return ESP_OK;
}

#else // defined CONFIG_SPI_FLASH_ROM_IMPL

static inline spi_dev_t *get_spi_dev(spi_flash_host_inst_t *host)
{
    return ((spi_flash_hal_context_t*)host)->spi;
}

static inline int get_host_id(spi_flash_host_inst_t* host)
{
    spi_dev_t *dev = get_spi_dev(host);
    return spi_flash_ll_hw_get_id(dev);
}

#endif // !CONFIG_SPI_FLASH_ROM_IMPL

uint32_t spi_flash_hal_check_status(spi_flash_host_inst_t *host)
{
    spi_dev_t *dev = get_spi_dev(host);
    uint32_t status = spi_flash_ll_host_idle(dev);
#if SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE
    uint32_t sus_status = spimem_flash_ll_sus_status((spi_mem_dev_t*)dev) << 1;
#else
    uint32_t sus_status = 0;
#endif
    // Not clear if this is necessary, or only necessary if
    // chip->spi == SPI1. But probably doesn't hurt...
    if ((void*) dev == spi_flash_ll_get_hw(SPI1_HOST)) {
#if CONFIG_IDF_TARGET_ESP32
        status &= spi_flash_ll_host_idle(&SPI0);
#endif
    }

    //status and sus_status should be mutual exclusion
    return (status | sus_status);
}

esp_err_t spi_flash_hal_setup_read_suspend(spi_flash_host_inst_t *host, const spi_flash_sus_cmd_conf *sus_conf)
{
#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
    spi_mem_dev_t *dev = (spi_mem_dev_t *)spi_flash_ll_get_hw(SPI1_HOST);
    spi_flash_hal_context_t* ctx = (spi_flash_hal_context_t*)host;
    memcpy(&(ctx->sus_cfg), sus_conf, sizeof(spi_flash_sus_cmd_conf));
    spimem_flash_ll_suspend_cmd_setup(dev, sus_conf->sus_cmd);
    spimem_flash_ll_resume_cmd_setup(dev, sus_conf->res_cmd);
#if SOC_SPI_MEM_SUPPORT_CHECK_SUS
    spimem_flash_ll_set_read_sus_status(dev, sus_conf->sus_mask);
    spimem_flash_ll_rd_sus_cmd_setup(dev, sus_conf->cmd_rdsr);
#endif // SOC_SPI_MEM_SUPPORT_CHECK_SUS
#endif // SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
    return ESP_OK;
}

#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
void spi_flash_hal_setup_auto_suspend_mode(spi_flash_host_inst_t *host)
{
    spi_mem_dev_t *dev = (spi_mem_dev_t*)spi_flash_ll_get_hw(SPI1_HOST);
    spi_flash_hal_context_t* ctx = (spi_flash_hal_context_t*)host;
    spimem_flash_ll_auto_wait_idle_init(dev, true);
    if (ctx->freq_mhz == 120) {
        spimem_flash_ll_set_wait_idle_dummy_phase(dev, ctx->extra_dummy);
    }
    spimem_flash_ll_auto_suspend_init(dev, true);
    // tsus = ceil(ctx->tsus_val * ctx->freq_mhz / spimem_flash_ll_get_tsus_unit_in_cycles);
    uint32_t tsus = (ctx->tsus_val * ctx->freq_mhz / spimem_flash_ll_get_tsus_unit_in_cycles(dev)) + ((ctx->tsus_val * ctx->freq_mhz) % spimem_flash_ll_get_tsus_unit_in_cycles(dev) != 0);
    spimem_flash_ll_set_sus_delay(dev, tsus);
    // tshsl2 = ceil(SPI_FLASH_TSHSL2_SAFE_VAL_NS * spimem_flash_ll_get_source_freq_mhz() * 0.001);
    uint32_t tshsl2 = (SPI_FLASH_TSHSL2_SAFE_VAL_NS * spimem_flash_ll_get_source_freq_mhz() / 1000) + ((SPI_FLASH_TSHSL2_SAFE_VAL_NS * spimem_flash_ll_get_source_freq_mhz()) % 1000 != 0);
    spimem_flash_set_cs_hold_delay(dev, tshsl2);
    spimem_flash_ll_sus_set_spi0_lock_trans(dev, SPIMEM_FLASH_LL_SPI0_MAX_LOCK_VAL_MSPI_TICKS);
#if SOC_SPI_MEM_SUPPORT_CHECK_SUS
    spimem_flash_ll_sus_check_sus_setup(dev, true);
#endif
}

void spi_flash_hal_setup_auto_resume_mode(spi_flash_host_inst_t *host)
{
    spi_mem_dev_t *dev = (spi_mem_dev_t*)spi_flash_ll_get_hw(SPI1_HOST);
    spimem_flash_ll_auto_resume_init(dev, true);
#if SOC_SPI_MEM_SUPPORT_CHECK_SUS
    spimem_flash_ll_res_check_sus_setup(dev, true);
#endif
}

void spi_flash_hal_disable_auto_suspend_mode(spi_flash_host_inst_t *host)
{
    spi_mem_dev_t *dev = (spi_mem_dev_t *)spi_flash_ll_get_hw(SPI1_HOST);
    spimem_flash_ll_auto_wait_idle_init(dev, false);
    spimem_flash_ll_auto_suspend_init(dev, false);
#if SOC_SPI_MEM_SUPPORT_CHECK_SUS
    spimem_flash_ll_sus_check_sus_setup(dev, false);
#endif
}

void spi_flash_hal_disable_auto_resume_mode(spi_flash_host_inst_t *host)
{
    spi_mem_dev_t *dev = (spi_mem_dev_t*)spi_flash_ll_get_hw(SPI1_HOST);
    spimem_flash_ll_auto_resume_init(dev, false);
#if SOC_SPI_MEM_SUPPORT_CHECK_SUS
    spimem_flash_ll_res_check_sus_setup(dev, false);
#endif
}
#endif // SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

void spi_flash_hal_resume(spi_flash_host_inst_t *host)
{
#if SOC_SPI_MEM_SUPPORT_SW_SUSPEND
    spimem_flash_ll_resume((spi_mem_dev_t*)(((spi_flash_hal_context_t *)host)->spi));
#else
    abort();
#endif
}

void spi_flash_hal_suspend(spi_flash_host_inst_t *host)
{
#if SOC_SPI_MEM_SUPPORT_SW_SUSPEND
    spimem_flash_ll_suspend((spi_mem_dev_t *)(((spi_flash_hal_context_t *)host)->spi));
#else
    abort();
#endif
}
