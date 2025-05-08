/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include "nvs_bootloader.h"
#include "nvs_bootloader_example_utils.h"

#include "nvs_sec_provider.h"

static const char* TAG = "nvs_bootloader_example";

// Function used to tell the linker to include this file
// with all its symbols.
//
void bootloader_hooks_include(void){
}

// not used in this example
void bootloader_before_init(void) {
}


void log_request_call_read_evaluate_output(const char* nvs_partition_label, nvs_bootloader_read_list_t read_list[], const size_t read_list_count, const nvs_sec_cfg_t* sec_cfg)
{
    esp_err_t ret = ESP_FAIL;
    // call the read function
    ret = nvs_bootloader_read(nvs_partition_label, read_list_count, read_list);

    // Error code ESP_OK means that the read function was successful and individual, per record results are stored in the read_list
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Data read from NVS partition");

        // log the request structure after the read to see the results
        // some records may indicate problems with the read i.e. not found, type mismatch, etc.
        log_nvs_bootloader_read_list(ret, read_list, read_list_count);

    // Error code ESP_ERR_INVALID_ARG means that the read function was called with invalid arguments
    } else if(ret == ESP_ERR_INVALID_ARG) {
        ESP_LOGE(TAG, "Invalid arguments passed to the function");

        // some records may indicate problems with the input parameters
        // application developer may evaluate the read_list to see what went wrong with input parameters
        log_nvs_bootloader_read_list(ret, read_list, read_list_count);

    // Other error codes mean that the read function failed to read the data from the NVS partition
    // The read list doesn't contain any useful data in this case
    } else {
        ESP_LOGE(TAG, "Failed to read NVS partition ret = %04x", ret);
    }
}


// function hook called at the end of the bootloader standard code
// this is the 'main' function of the example
void bootloader_after_init(void) {
    ESP_LOGI(TAG, "Before reading from NVS partition");

    // we are going to read from the default nvs partition labelled 'nvs'
    const char* nvs_partition_label = "nvs";

    #define STR_BUFF_LEN_10 10+1       // 10 characters + null terminator
    #define STR_BUFF_LEN_66 66+1       // 66 characters + null terminator
    char str_buff_10[STR_BUFF_LEN_10];
    char str_buff_66[STR_BUFF_LEN_66];

    nvs_sec_cfg_t* sec_cfg = NULL;

#if CONFIG_NVS_ENCRYPTION
    nvs_sec_cfg_t cfg = {};
    nvs_sec_scheme_t *sec_scheme_handle = NULL;
#if CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC
    nvs_sec_config_hmac_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_HMAC_DEFAULT();
    if (nvs_sec_provider_register_hmac(&sec_scheme_cfg, &sec_scheme_handle) != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Registering the HMAC scheme failed");
        return;
    }
#elif CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC
    nvs_sec_config_flash_enc_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_FLASH_ENC_DEFAULT();
    if (sec_scheme_cfg.nvs_keys_part == NULL) {
        ESP_EARLY_LOGE(TAG, "partition with subtype \"nvs_keys\" not found");
    }

    if (nvs_sec_provider_register_flash_enc(&sec_scheme_cfg, &sec_scheme_handle) != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Registering the Flash Encryption scheme failed");
        return;
    }
#endif /* CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC */

    if (nvs_bootloader_read_security_cfg(sec_scheme_handle, &cfg) != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "Reading the NVS security configuration failed");
        return;
    }

    if (nvs_bootloader_secure_init(&cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Secure initialization of NVS failed");
        return;
    }
#endif /* CONFIG_NVS_ENCRYPTION*/

    // --- This is the request structure for the read function showing validation errors - function will return ESP_ERR_INVALID_ARG ---
    ESP_EARLY_LOGI(TAG, "Trying to read the NVS partition by passing invalid arguments");

    nvs_bootloader_read_list_t bad_read_list_indicate_problems[] = {
        { .namespace_name = "sunny_day",           .key_name = "u8",                .value_type = NVS_TYPE_U8 },    // ESP_ERR_NVS_NOT_FOUND
                                                                                                                    // this is correct request, not found is expected default result code
        { .namespace_name = "too_long_sunny_day",  .key_name = "u8",                .value_type = NVS_TYPE_I8 },    // ESP_ERR_NVS_INVALID_NAME
                                                                                                                    // too long namespace name
        { .namespace_name = "sunny_day",           .key_name = "too_long_dark_key", .value_type = NVS_TYPE_I32 },   // ESP_ERR_NVS_KEY_TOO_LONG
                                                                                                                    // too long key name
        { .namespace_name = "clowny_day",          .key_name = "blobeee",           .value_type = NVS_TYPE_BLOB },  // ESP_ERR_INVALID_ARG
                                                                                                                    // not supported data type
        { .namespace_name = "sunny_day",  .key_name = "string_10_chars",            .value_type = NVS_TYPE_STR, .value.str_val = { .buff_ptr = str_buff_10, .buff_len = 0 } },
                                                                                                                    // ESP_ERR_INVALID_SIZE
                                                                                                                    // buffer size is 0
        { .namespace_name = "sunny_day",  .key_name = "string_66_chars",            .value_type = NVS_TYPE_STR, .value.str_val = { .buff_ptr = NULL, .buff_len = 66 } }
                                                                                                                    // ESP_ERR_INVALID_SIZE
                                                                                                                    // buffer pointer is invalid
    };

    size_t bad_read_list_indicate_problems_count = sizeof(bad_read_list_indicate_problems) / sizeof(bad_read_list_indicate_problems[0]);
    log_request_call_read_evaluate_output(nvs_partition_label, bad_read_list_indicate_problems, bad_read_list_indicate_problems_count, sec_cfg);

    // --- This is the request structure for the read function showing runtime errors - function will return ESP_OK ---
    // but some records will have result_code set to ESP_ERR_NVS_NOT_FOUND, ESP_ERR_NVS_TYPE_MISMATCH, ESP_ERR_INVALID_SIZE
    ESP_EARLY_LOGI(TAG, "Trying to read the NVS partition by expecting incorrect data");

    nvs_bootloader_read_list_t good_read_list_bad_results[] = {
        { .namespace_name = "sunny_day",  .key_name = "u8",  .value_type = NVS_TYPE_I8 },   // ESP_ERR_NVS_TYPE_MISMATCH
                                                                                            // data in the partition is of different type (NVS_TYPE_U8)
        { .namespace_name = "sunny_day",  .key_name = "i32_", .value_type = NVS_TYPE_I32 }, // ESP_ERR_NVS_NOT_FOUND
                                                                                            // data in the partition won't be found, because there is a typo in the key name
        { .namespace_name = "clowny_day", .key_name = "i8",  .value_type = NVS_TYPE_I8 },   // ESP_ERR_NVS_NOT_FOUND
                                                                                            // data in the partition won't be found, because there is typo in namespace name
        { .namespace_name = "sunny_day",  .key_name = "string_10_chars", .value_type = NVS_TYPE_STR, .value.str_val = { .buff_ptr = str_buff_10, .buff_len = 2 } },
                                                                                            // ESP_ERR_INVALID_SIZE
                                                                                            // buffer is too small
        { .namespace_name = "sunny_day",  .key_name = "u32", .value_type = NVS_TYPE_U32 },  // ESP_OK
                                                                                            // this value will be read correctly
        { .namespace_name = "sunny_day",  .key_name = "u32", .value_type = NVS_TYPE_U32 },   // ESP_ERR_NVS_NOT_FOUND
                                                                                            // this value won't be read as function doesn't support duplicate readings
        { .namespace_name = "sunny_day",  .key_name = "string_66_chars", .value_type = NVS_TYPE_STR, .value.str_val = { .buff_ptr = str_buff_66, .buff_len = 65 } }
                                                                                            // ESP_ERR_INVALID_SIZE
                                                                                            // buffer is just small
    };

    size_t good_read_list_bad_results_count = sizeof(good_read_list_bad_results) / sizeof(good_read_list_bad_results[0]);
    log_request_call_read_evaluate_output(nvs_partition_label, good_read_list_bad_results, good_read_list_bad_results_count, sec_cfg);


    // --- This is the request structure for the read function showing all records found---
    // function will return ESP_OK, all individual records will have result_code set to ESP_OK
    // Order of the requested keys and namespaces is not important
    // We mix different types of data to demonstrate the usage of different data types and also mix-in different namespaces
    // For NVS_TYPE_I* and NVS_TYPE_U* the value field is used directly to store the read value
    // For NVS_TYPE_STR the value field is a structure with a pointer to the buffer and the buffer length is povided
    // In this case, the buffer is a stack allocated array of 10 characters plus space for the null terminator

    ESP_EARLY_LOGI(TAG, "Trying to read the NVS partition correctly");

    nvs_bootloader_read_list_t good_read_list[] = {
        { .namespace_name = "sunny_day",  .key_name = "u8",  .value_type = NVS_TYPE_U8 },
        { .namespace_name = "sunny_day",  .key_name = "i32", .value_type = NVS_TYPE_I32 },
        { .namespace_name = "cloudy_day", .key_name = "i8",  .value_type = NVS_TYPE_I8 },  // mixed in different namespace
        { .namespace_name = "sunny_day",  .key_name = "u16", .value_type = NVS_TYPE_U16 },
        { .namespace_name = "sunny_day",  .key_name = "string_10_chars", .value_type = NVS_TYPE_STR, .value.str_val = { .buff_ptr = str_buff_10, .buff_len = STR_BUFF_LEN_10 } },
        { .namespace_name = "sunny_day",  .key_name = "string_66_chars", .value_type = NVS_TYPE_STR, .value.str_val = { .buff_ptr = str_buff_66, .buff_len = STR_BUFF_LEN_66 } }
    };

    size_t good_read_list_count = sizeof(good_read_list) / sizeof(good_read_list[0]);
    log_request_call_read_evaluate_output(nvs_partition_label, good_read_list, good_read_list_count, sec_cfg);

#if CONFIG_NVS_ENCRYPTION
    nvs_bootloader_secure_deinit();
#endif /* CONFIG_NVS_ENCRYPTION */

    ESP_LOGI(TAG, "Finished bootloader part");
}
