/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include "nvs_bootloader_example_utils.h"

static const char* TAG = "nvs_bootloader_example_utils";
static const char* result_code_to_string(const esp_err_t result_code);


/* logs the result of the call to the nvs_bootloader_read function */
void log_nvs_bootloader_read_list(const esp_err_t return_code, const nvs_bootloader_read_list_t read_list[], const size_t read_list_count)
{
    switch (return_code) {
    // read list contains data after successful read
    case ESP_OK: {
        ESP_LOGI(TAG, "Result data. Return code: %s", result_code_to_string(return_code));
        log_nvs_bootloader_read_list_OK(read_list, read_list_count);
        break;
    }

    // read list contains data failing validation
    case ESP_ERR_INVALID_ARG: {
        ESP_LOGI(TAG, "Request data with invalid arguments. Return code: %s", result_code_to_string(return_code));
        log_nvs_bootloader_read_list_INVALID_ARG(read_list, read_list_count);
        break;
    }

    // request data only
    case ESP_ERR_NOT_FINISHED: {
        ESP_LOGI(TAG, "Showing request data only. Return code: %s", result_code_to_string(return_code));
        log_nvs_bootloader_read_list_NOT_FINISHED(read_list, read_list_count);
        break;
    }
    default: {
        ESP_LOGI(TAG, "Function returned: %04x %s", return_code, result_code_to_string(return_code));
        break;
    }
    }
}

static const char* result_code_to_string(const esp_err_t result_code)
{
    switch (result_code) {
    case ESP_ERR_NVS_NOT_FOUND:     return "ESP_ERR_NVS_NOT_FOUND";
    case ESP_OK:                    return "ESP_OK";
    case ESP_ERR_NVS_TYPE_MISMATCH: return "ESP_ERR_NVS_TYPE_MISMATCH";
    case ESP_ERR_INVALID_ARG:       return "ESP_ERR_INVALID_ARG";
    case ESP_ERR_NVS_INVALID_NAME:  return "ESP_ERR_NVS_INVALID_NAME";
    case ESP_ERR_NVS_KEY_TOO_LONG:  return "ESP_ERR_NVS_KEY_TOO_LONG";
    case ESP_ERR_INVALID_SIZE:      return "ESP_ERR_INVALID_SIZE";
    default:                        return "Unknown";
    }
}

static const char* value_type_to_string(const nvs_type_t value_type)
{
    switch (value_type) {
    case NVS_TYPE_U8:  return "U8";
    case NVS_TYPE_U16: return "U16";
    case NVS_TYPE_U32: return "U32";
    case NVS_TYPE_U64: return "U64";
    case NVS_TYPE_I8:  return "I8";
    case NVS_TYPE_I16: return "I16";
    case NVS_TYPE_I32: return "I32";
    case NVS_TYPE_I64: return "I64";
    case NVS_TYPE_STR: return "STR";
    case NVS_TYPE_BLOB: return "BLOB";
    default:           return "Unknown";
    }
}

void log_nvs_bootloader_read_list_OK(const nvs_bootloader_read_list_t read_list[], const size_t read_list_count)
{
    bool i_type, u_type = false;
    uint64_t u64_val = 0;
    int64_t i64_val = 0;

    ESP_LOGI(TAG, "## Result                    Namespace        Key              DT  Value");

    for (size_t i = 0; i < read_list_count; i++) {
        switch (read_list[i].result_code) {
        case ESP_OK: {
            switch (read_list[i].value_type) {
            case NVS_TYPE_U8: {
                u64_val = read_list[i].value.u8_val;
                u_type = true;
                break;
            }
            case NVS_TYPE_U16: {
                u64_val = read_list[i].value.u16_val;
                u_type = true;
                break;
            }
            case NVS_TYPE_U32: {
                u64_val = read_list[i].value.u32_val;
                u_type = true;
                break;
            }
            case NVS_TYPE_U64: {
                u64_val = read_list[i].value.u64_val;
                u_type = true;
                break;
            }
            case NVS_TYPE_I8: {
                i64_val = read_list[i].value.i8_val;
                i_type = true;
                break;
            }
            case NVS_TYPE_I16: {
                i64_val = read_list[i].value.i16_val;
                i_type = true;
                break;
            }
            case NVS_TYPE_I32: {
                i64_val = read_list[i].value.i32_val;
                i_type = true;
                break;
            }
            case NVS_TYPE_I64: {
                i64_val = read_list[i].value.i64_val;
                i_type = true;
                break;
            }
            case NVS_TYPE_STR: {
                ESP_LOGI(TAG, "%2d %-25s %-16s %-16s %-3s %s",
                         i,
                         result_code_to_string(read_list[i].result_code),
                         read_list[i].namespace_name,
                         read_list[i].key_name,
                         value_type_to_string(read_list[i].value_type),
                         read_list[i].value.str_val.buff_ptr);
                continue;
            }
            default: {
                u64_val = 0;
                i64_val = 0;
                break;
            }

            }
            if (i_type) {
                ESP_LOGI(TAG, "%2d %-25s %-16s %-16s %-3s %lld",
                         i,
                         result_code_to_string(read_list[i].result_code),
                         read_list[i].namespace_name,
                         read_list[i].key_name,
                         value_type_to_string(read_list[i].value_type),
                         i64_val);
            } else if (u_type) {
                ESP_LOGI(TAG, "%2d %-25s %-16s %-16s %-3s %llu",
                         i,
                         result_code_to_string(read_list[i].result_code),
                         read_list[i].namespace_name,
                         read_list[i].key_name,
                         value_type_to_string(read_list[i].value_type),
                         u64_val);
            }
            i_type = false;
            u_type = false;
            break;
        }
        default: {
            // data was not found or other error
            ESP_LOGE(TAG, "%2d %-25s %-16s %-16s %-3s",
                     i,
                     result_code_to_string(read_list[i].result_code),
                     read_list[i].namespace_name,
                     read_list[i].key_name,
                     value_type_to_string(read_list[i].value_type));
            break;
        }
        }
    }
}

void log_nvs_bootloader_read_list_INVALID_ARG(const nvs_bootloader_read_list_t read_list[], const size_t read_list_count)
{

    ESP_LOGI(TAG, "## Result                    Namespace        Key              DT");

    for (size_t i = 0; i < read_list_count; i++) {
        if (read_list[i].result_code != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "%2d %-25s %-16s %-16s %-3s",
                     i,
                     result_code_to_string(read_list[i].result_code),
                     read_list[i].namespace_name,
                     read_list[i].key_name,
                     value_type_to_string(read_list[i].value_type));
        } else {
            ESP_LOGI(TAG, "%2d %-25s %-16s %-16s %-3s",
                     i,
                     result_code_to_string(read_list[i].result_code),
                     read_list[i].namespace_name,
                     read_list[i].key_name,
                     value_type_to_string(read_list[i].value_type));
        }
    }
}

void log_nvs_bootloader_read_list_NOT_FINISHED(const nvs_bootloader_read_list_t read_list[], const size_t read_list_count)
{

    ESP_LOGI(TAG, "## Namespace        Key              DT");

    for (size_t i = 0; i < read_list_count; i++) {
        ESP_LOGI(TAG, "%2d %-16s %-16s %-3s",
                 i,
                 read_list[i].namespace_name,
                 read_list[i].key_name,
                 value_type_to_string(read_list[i].value_type));
    }
}
