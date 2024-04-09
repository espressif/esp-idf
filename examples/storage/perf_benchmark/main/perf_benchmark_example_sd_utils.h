/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once
#include "sdkconfig.h"

#include "sd_protocol_types.h"
#include "soc/soc_caps.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif

extern sdmmc_host_t host_g;
#ifdef CONFIG_EXAMPLE_USE_SDMMC
extern sdmmc_slot_config_t slot_config_g;
#endif // CONFIG_EXAMPLE_USE_SDMMC

#ifdef CONFIG_EXAMPLE_USE_SDSPI
extern sdspi_device_config_t slot_config_g;
#endif // CONFIG_EXAMPLE_USE_SDSPI

#ifdef CONFIG_EXAMPLE_SD_FREQ_PROBING
#define EXAMPLE_SD_FREQ SDMMC_FREQ_PROBING
#elif CONFIG_EXAMPLE_SD_FREQ_DEFAULT
#define EXAMPLE_SD_FREQ SDMMC_FREQ_DEFAULT
#elif CONFIG_EXAMPLE_SD_FREQ_HIGHSPEED
#define EXAMPLE_SD_FREQ SDMMC_FREQ_HIGHSPEED
#elif CONFIG_EXAMPLE_SD_FREQ_CUSTOM
#define EXAMPLE_SD_FREQ CONFIG_EXAMPLE_SD_FREQ_CUSTOM_VAL
#else
#define EXAMPLE_SD_FREQ SDMMC_FREQ_DEFAULT
#endif

#ifdef CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdmmc_slot_config_t *out_slot_config, int freq_khz);
#endif // CONFIG_EXAMPLE_USE_SDMMC

#ifdef CONFIG_EXAMPLE_USE_SDSPI
void init_sd_config(sdmmc_host_t *out_host, sdspi_device_config_t *out_slot_config, int freq_khz);
#endif // CONFIG_EXAMPLE_USE_SDSPI

esp_err_t init_sd_card(sdmmc_card_t **out_card);
void deinit_sd_card(sdmmc_card_t **card);
