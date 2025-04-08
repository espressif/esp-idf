/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_macros.h"
#include "esp_fault.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "riscv/rv_utils.h"

#include "esp_tee.h"
#include "multi_heap.h"

#include "esp_tee_brownout.h"
#include "esp_tee_flash.h"
#include "esp_tee_sec_storage.h"
#include "bootloader_utility_tee.h"

#if __has_include("esp_app_desc.h")
#define WITH_APP_IMAGE_INFO
#include "esp_app_desc.h"
#endif

/* TEE symbols */
extern uint32_t _tee_stack;
extern uint32_t _tee_bss_start;
extern uint32_t _tee_bss_end;
extern uint32_t _tee_s_intr_handler;

extern uint8_t _tee_heap_start[];
extern uint8_t _tee_heap_end[];
#define TEE_HEAP_SIZE ((size_t)(_tee_heap_end - _tee_heap_start))

static const char *TAG = "esp_tee_init";

/* Initializes the TEE configuration structure with fields required for
 * the REE-TEE interface from the TEE binary
 */
static void tee_init_app_config(void)
{
    /* TODO: Integrate these compatibility checks into the bootloader
     * so it can provide fallback behavior
     */
    if (esp_tee_app_config.magic_word != ESP_TEE_APP_CFG_MAGIC) {
        ESP_LOGE(TAG, "Configuration structure missing from the TEE app!");
        ESP_INFINITE_LOOP();  // WDT will reset us
    }

    if (esp_tee_app_config.api_major_version != ESP_TEE_API_MAJOR_VER) {
        ESP_LOGE(TAG, "TEE API version mismatch: app v%d != binary v%d",
                 esp_tee_app_config.api_major_version, ESP_TEE_API_MAJOR_VER);
        ESP_INFINITE_LOOP();
    }

    /* Set the TEE API minor version */
    esp_tee_app_config.api_minor_version = ESP_TEE_API_MINOR_VER;

    /* Set the TEE-related fields (from the TEE binary) that the REE will use to interface with TEE */
    esp_tee_app_config.s_int_handler = &_tee_s_intr_handler;
}

/* Print the TEE application info */
static void tee_print_app_info(void)
{
#ifdef WITH_APP_IMAGE_INFO
    const esp_app_desc_t *app_desc = esp_app_get_description();
    ESP_LOGI(TAG, "TEE information:");
    ESP_LOGI(TAG, "Project name:     %s", app_desc->project_name);
    ESP_LOGI(TAG, "App version:      %s", app_desc->version);
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
    ESP_LOGI(TAG, "Secure version:   %d", app_desc->secure_version);
#endif
    ESP_LOGI(TAG, "Compile time:     %s %s", app_desc->date, app_desc->time);

    char buf[17];
    esp_app_get_elf_sha256(buf, sizeof(buf));
    ESP_LOGI(TAG, "ELF file SHA256:  %s...", buf);
    ESP_LOGI(TAG, "ESP-IDF:          %s", app_desc->idf_ver);
#endif
}

/* Mark the current TEE image as valid and cancel rollback */
static void tee_mark_app_and_valid_cancel_rollback(void)
{
    esp_partition_info_t tee_ota_info;
    esp_err_t err = esp_tee_flash_find_partition(PART_TYPE_DATA, PART_SUBTYPE_DATA_TEE_OTA, NULL, &tee_ota_info);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "No TEE OTA data partition found");
        return;
    }

    const esp_partition_pos_t tee_ota_pos = tee_ota_info.pos;
    err = bootloader_utility_tee_mark_app_valid_and_cancel_rollback(&tee_ota_pos);
    if (err != ESP_OK) {
        if (err == ESP_ERR_INVALID_STATE) {
            /* NOTE: App already marked valid */
            return;
        }
        ESP_LOGE(TAG, "Failed to cancel rollback (0x%08x)", err);
        abort();
    }

    ESP_LOGD(TAG, "Rollback succeeded, erasing the passive TEE partition...");
    uint8_t tee_next_part = bootloader_utility_tee_get_next_update_partition(&tee_ota_pos);
    esp_partition_info_t tee_next_part_info;

    err = esp_tee_flash_find_partition(PART_TYPE_APP, tee_next_part, NULL, &tee_next_part_info);
    err |= esp_tee_flash_erase_range(tee_next_part_info.pos.offset, tee_next_part_info.pos.size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to find/erase the passive TEE partition!");
        return;
    }
}

void __attribute__((noreturn)) esp_tee_init(uint32_t ree_entry_addr, uint32_t ree_drom_addr, uint8_t tee_boot_part)
{
    /* Clear BSS */
    memset(&_tee_bss_start, 0, (&_tee_bss_end - &_tee_bss_start) * sizeof(_tee_bss_start));

    static uint32_t btld_sp;

    /* Take backup of bootloader stack. */
    asm volatile("mv %0, sp" : "=r"(btld_sp));

    /* Switch to secure world stack. */
    asm volatile("mv sp, %0" :: "r"((uint32_t)&_tee_stack));

    /* TEE compatibility check and App config data initialization. */
    tee_init_app_config();

    /* TEE heap initialization. */
    esp_err_t err = esp_tee_heap_init((void *)_tee_heap_start, TEE_HEAP_SIZE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to setup the TEE heap!");
        abort();
    }

    /* SoC specific secure initialization. */
    esp_tee_soc_secure_sys_init();

    /* Brownout detection initialization */
    esp_tee_brownout_init();

    /* Switch back to bootloader stack. */
    asm volatile("mv sp, %0" :: "r"(btld_sp));

    ESP_LOGI(TAG, "Initializing. RAM available for dynamic allocation:");
    ESP_LOGI(TAG, "At %08X len %08X (%d KiB): %s",
             ((void *)&_tee_heap_start), TEE_HEAP_SIZE, TEE_HEAP_SIZE / 1024, "RAM");

    /* Setting up the permissible flash operation address range */
    err = esp_tee_flash_setup_prot_ctx(tee_boot_part);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to setup the TEE flash memory protection!");
        abort();
    }
    ESP_FAULT_ASSERT(err == ESP_OK);

    /* Initializing the secure storage */
    err = esp_tee_sec_storage_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the secure storage! (0x%08x)", err);
        abort();
    }
    ESP_FAULT_ASSERT(err == ESP_OK);

    /* Setting up the running non-secure app partition as per the address provided by the bootloader */
    err = esp_tee_flash_set_running_ree_partition(ree_drom_addr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to setup the active REE partition!");
        abort();
    }

    tee_print_app_info();

    /* NOTE: As image rollback is mandatorily enabled for TEE OTA,
     * the most optimum checkpoint to mark the current app valid and
     * cancel rollback is right before the TEE ends and is about to
     * pass control to the non-secure app (see below).
     */
    tee_mark_app_and_valid_cancel_rollback();

    /* Switch to the REE and launch app */
    esp_tee_switch_to_ree(ree_entry_addr);

    /* App entry function should not return here. */
    ESP_INFINITE_LOOP(); /* WDT will reset us */
}
