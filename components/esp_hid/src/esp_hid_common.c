/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_hid_common.h"
#if (CONFIG_GATTS_ENABLE || CONFIG_GATTC_ENABLE)
#include "esp_gatt_defs.h"
#endif
static const char *TAG = "hid_parser";

typedef struct {
    uint16_t appearance;
    uint8_t usage_mask;
    uint8_t reports_len;
    esp_hid_report_item_t reports[64];
} temp_hid_report_map_t;

typedef struct {
    uint8_t cmd;
    uint8_t len;
    union {
        uint32_t value;
        uint8_t data[4];
    };
} hid_report_cmd_t;

typedef struct {
    uint16_t usage_page;
    uint16_t usage;
    uint16_t inner_usage_page;
    uint16_t inner_usage;
    uint8_t report_id;
    uint16_t input_len;
    uint16_t output_len;
    uint16_t feature_len;
} hid_report_params_t;

typedef enum {
    PARSE_WAIT_USAGE_PAGE, PARSE_WAIT_USAGE, PARSE_WAIT_COLLECTION_APPLICATION, PARSE_WAIT_END_COLLECTION
} s_parse_step_t;


static s_parse_step_t s_parse_step = PARSE_WAIT_USAGE_PAGE;
static uint8_t s_collection_depth = 0;
static hid_report_params_t s_report_params = {0,};
static uint16_t s_report_size = 0;
static uint16_t s_report_count = 0;

static bool s_new_map = false;
static temp_hid_report_map_t *s_temp_hid_report_map;

static int add_report(temp_hid_report_map_t *map, esp_hid_report_item_t *item)
{
    if (map->reports_len >= 64) {
        ESP_LOGE(TAG, "reports overflow");
        return -1;
    }
    memcpy(&(map->reports[map->reports_len]), item, sizeof(esp_hid_report_item_t));
    map->reports_len++;
    return 0;
}

static int handle_report(hid_report_params_t *report, bool first)
{
    if (s_temp_hid_report_map == NULL) {
        s_temp_hid_report_map = (temp_hid_report_map_t *)calloc(1, sizeof(temp_hid_report_map_t));
        if (s_temp_hid_report_map == NULL) {
            ESP_LOGE(TAG, "malloc failed");
            return -1;
        }
    }
    temp_hid_report_map_t *map = s_temp_hid_report_map;
    if (first) {
        memset(map, 0, sizeof(temp_hid_report_map_t));
    }

    if (report->usage_page == HID_USAGE_PAGE_GENERIC_DESKTOP && report->usage == HID_USAGE_KEYBOARD) {
        //Keyboard
        map->usage_mask |= ESP_HID_USAGE_KEYBOARD;
        if (report->input_len > 0) {
            esp_hid_report_item_t item = {
                .usage = ESP_HID_USAGE_KEYBOARD,
                .report_id = report->report_id,
                .report_type = ESP_HID_REPORT_TYPE_INPUT,
                .protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT,
                .value_len = report->input_len / 8,
            };
            if (add_report(map, &item) != 0) {
                return -1;
            }

            item.protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
            item.value_len = 8;
            if (add_report(map, &item) != 0) {
                return -1;
            }
        }
        if (report->output_len > 0) {
            esp_hid_report_item_t item = {
                .usage = ESP_HID_USAGE_KEYBOARD,
                .report_id = report->report_id,
                .report_type = ESP_HID_REPORT_TYPE_OUTPUT,
                .protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT,
                .value_len = report->output_len / 8,
            };
            if (add_report(map, &item) != 0) {
                return -1;
            }

            item.protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
            item.value_len = 1;
            if (add_report(map, &item) != 0) {
                return -1;
            }
        }
    } else if (report->usage_page == HID_USAGE_PAGE_GENERIC_DESKTOP && report->usage == HID_USAGE_MOUSE) {
        //Mouse
        map->usage_mask |= ESP_HID_USAGE_MOUSE;
        if (report->input_len > 0) {
            esp_hid_report_item_t item = {
                .usage = ESP_HID_USAGE_MOUSE,
                .report_id = report->report_id,
                .report_type = ESP_HID_REPORT_TYPE_INPUT,
                .protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT,
                .value_len = report->input_len / 8,
            };
            if (add_report(map, &item) != 0) {
                return -1;
            }

            item.protocol_mode = ESP_HID_PROTOCOL_MODE_BOOT;
            item.value_len = 3;
            if (add_report(map, &item) != 0) {
                return -1;
            }
        }
    } else {
        esp_hid_usage_t cusage = ESP_HID_USAGE_GENERIC;
        if (report->usage_page == HID_USAGE_PAGE_GENERIC_DESKTOP) {
            if (report->usage == HID_USAGE_JOYSTICK) {
                //Joystick
                map->usage_mask |= ESP_HID_USAGE_JOYSTICK;
                cusage = ESP_HID_USAGE_JOYSTICK;
            } else if (report->usage == HID_USAGE_GAMEPAD) {
                //Gamepad
                map->usage_mask |= ESP_HID_USAGE_GAMEPAD;
                cusage = ESP_HID_USAGE_GAMEPAD;
            }
        } else if (report->usage_page == HID_USAGE_PAGE_CONSUMER_DEVICE && report->usage == HID_USAGE_CONSUMER_CONTROL) {
            //Consumer Control
            map->usage_mask |= ESP_HID_USAGE_CCONTROL;
            cusage = ESP_HID_USAGE_CCONTROL;
        } else if (report->usage_page >= 0xFF) {
            //Vendor
            map->usage_mask |= ESP_HID_USAGE_VENDOR;
            cusage = ESP_HID_USAGE_VENDOR;
        }
        //Generic
        esp_hid_report_item_t item = {
            .usage = cusage,
            .report_id = report->report_id,
            .report_type = ESP_HID_REPORT_TYPE_INPUT,
            .protocol_mode = ESP_HID_PROTOCOL_MODE_REPORT,
            .value_len = report->input_len / 8,
        };
        if (report->input_len > 0) {
            if (add_report(map, &item) != 0) {
                return -1;
            }
        }
        if (report->output_len > 0) {
            item.report_type = ESP_HID_REPORT_TYPE_OUTPUT;
            item.value_len = report->output_len / 8;
            if (add_report(map, &item) != 0) {
                return -1;
            }
        }
        if (report->feature_len > 0) {
            item.report_type = ESP_HID_REPORT_TYPE_FEATURE;
            item.value_len = report->feature_len / 8;
            if (add_report(map, &item) != 0) {
                return -1;
            }
        }
    }
    return 0;
}


static int parse_cmd(const uint8_t *data, size_t len, size_t index, hid_report_cmd_t **out)
{
    if (index == len) {
        return 0;
    }
    hid_report_cmd_t *cmd = (hid_report_cmd_t *)malloc(sizeof(hid_report_cmd_t));
    if (cmd == NULL) {
        return -1;
    }
    const uint8_t *dp = data + index;
    cmd->cmd = *dp & 0xFC;
    cmd->len = *dp & 0x03;
    cmd->value = 0;
    if (cmd->len == 3) {
        cmd->len = 4;
    }
    if ((len - index - 1) < cmd->len) {
        ESP_LOGE(TAG, "not enough bytes! cmd: 0x%02x, len: %u, index: %u", cmd->cmd, cmd->len, index);
        free(cmd);
        return -1;
    }
    memcpy(cmd->data, dp + 1, cmd->len);
    *out = cmd;
    return cmd->len + 1;
}

static int handle_cmd(hid_report_cmd_t *cmd)
{
    switch (s_parse_step) {
    case PARSE_WAIT_USAGE_PAGE: {
        if (cmd->cmd != HID_RM_USAGE_PAGE) {
            ESP_LOGE(TAG, "expected USAGE_PAGE, but got 0x%02x", cmd->cmd);
            return -1;
        }
        s_report_size = 0;
        s_report_count = 0;
        memset(&s_report_params, 0, sizeof(hid_report_params_t));
        s_report_params.usage_page = cmd->value;
        s_parse_step = PARSE_WAIT_USAGE;
        break;
    }
    case PARSE_WAIT_USAGE: {
        if (cmd->cmd != HID_RM_USAGE) {
            ESP_LOGE(TAG, "expected USAGE, but got 0x%02x", cmd->cmd);
            s_parse_step = PARSE_WAIT_USAGE_PAGE;
            return -1;
        }
        s_report_params.usage = cmd->value;
        s_parse_step = PARSE_WAIT_COLLECTION_APPLICATION;
        break;
    }
    case PARSE_WAIT_COLLECTION_APPLICATION: {
        if (cmd->cmd != HID_RM_COLLECTION) {
            ESP_LOGE(TAG, "expected COLLECTION, but got 0x%02x", cmd->cmd);
            s_parse_step = PARSE_WAIT_USAGE_PAGE;
            return -1;
        }
        if (cmd->value != 1) {
            ESP_LOGE(TAG, "expected APPLICATION, but got 0x%02x", cmd->value);
            s_parse_step = PARSE_WAIT_USAGE_PAGE;
            return -1;
        }
        s_report_params.report_id = 0;
        s_collection_depth = 1;
        s_parse_step = PARSE_WAIT_END_COLLECTION;
        break;
    }
    case PARSE_WAIT_END_COLLECTION: {
        if (cmd->cmd == HID_RM_REPORT_ID) {
            if (s_report_params.report_id && s_report_params.report_id != cmd->value) {
                //report id changed mid collection
                if (s_report_params.input_len & 0x7) {
                    ESP_LOGE(TAG, "ERROR: INPUT report does not amount to full bytes! %d (%d)", s_report_params.input_len, s_report_params.input_len & 0x7);
                } else if (s_report_params.output_len & 0x7) {
                    ESP_LOGE(TAG, "ERROR: OUTPUT report does not amount to full bytes! %d (%d)", s_report_params.output_len, s_report_params.output_len & 0x7);
                } else if (s_report_params.feature_len & 0x7) {
                    ESP_LOGE(TAG, "ERROR: FEATURE report does not amount to full bytes! %d (%d)", s_report_params.feature_len, s_report_params.feature_len & 0x7);
                } else {
                    //SUCCESS!!!
                    int res = handle_report(&s_report_params, s_new_map);
                    if (res != 0) {
                        s_parse_step = PARSE_WAIT_USAGE_PAGE;
                        return -1;
                    }
                    s_new_map = false;

                    s_report_params.input_len = 0;
                    s_report_params.output_len = 0;
                    s_report_params.feature_len = 0;
                    s_report_params.usage = s_report_params.inner_usage;
                    s_report_params.usage_page = s_report_params.inner_usage_page;
                }
            }
            s_report_params.report_id = cmd->value;
        } else if (cmd->cmd == HID_RM_USAGE_PAGE) {
            s_report_params.inner_usage_page = cmd->value;
        } else if (cmd->cmd == HID_RM_USAGE) {
            s_report_params.inner_usage = cmd->value;
        } else if (cmd->cmd == HID_RM_REPORT_SIZE) {
            s_report_size = cmd->value;
        } else if (cmd->cmd == HID_RM_REPORT_COUNT) {
            s_report_count = cmd->value;
        } else if (cmd->cmd == HID_RM_INPUT) {
            s_report_params.input_len += (s_report_size * s_report_count);
        } else if (cmd->cmd == HID_RM_OUTPUT) {
            s_report_params.output_len += (s_report_size * s_report_count);
        } else if (cmd->cmd == HID_RM_FEATURE) {
            s_report_params.feature_len += (s_report_size * s_report_count);
        } else if (cmd->cmd == HID_RM_COLLECTION) {
            s_collection_depth += 1;
        } else if (cmd->cmd == HID_RM_END_COLLECTION) {
            s_collection_depth -= 1;
            if (s_collection_depth == 0) {
                if (s_report_params.input_len & 0x7) {
                    ESP_LOGE(TAG, "ERROR: INPUT report does not amount to full bytes! %d (%d)", s_report_params.input_len, s_report_params.input_len & 0x7);
                } else if (s_report_params.output_len & 0x7) {
                    ESP_LOGE(TAG, "ERROR: OUTPUT report does not amount to full bytes! %d (%d)", s_report_params.output_len, s_report_params.output_len & 0x7);
                } else if (s_report_params.feature_len & 0x7) {
                    ESP_LOGE(TAG, "ERROR: FEATURE report does not amount to full bytes! %d (%d)", s_report_params.feature_len, s_report_params.feature_len & 0x7);
                } else {
                    //SUCCESS!!!
                    int res = handle_report(&s_report_params, s_new_map);
                    if (res != 0) {
                        s_parse_step = PARSE_WAIT_USAGE_PAGE;
                        return -1;
                    }
                    s_new_map = false;
                }
                s_parse_step = PARSE_WAIT_USAGE_PAGE;
            }
        }

        break;
    }
    default:
        s_parse_step = PARSE_WAIT_USAGE_PAGE;
        break;
    }
    return 0;
}


esp_hid_report_map_t *esp_hid_parse_report_map(const uint8_t *hid_rm, size_t hid_rm_len)
{
    size_t index = 0;
    int res;
    s_new_map = true;

    while (index < hid_rm_len) {
        hid_report_cmd_t *cmd;
        res = parse_cmd(hid_rm, hid_rm_len, index, &cmd);
        if (res < 0) {
            ESP_LOGE(TAG, "Failed parsing the descriptor at index: %u", index);
            return NULL;
        }
        index += res;
        res = handle_cmd(cmd);
        free(cmd);
        if (res != 0) {
            return NULL;
        }
    }

    esp_hid_report_map_t *out = (esp_hid_report_map_t *)calloc(1, sizeof(esp_hid_report_map_t));
    if (out == NULL) {
        ESP_LOGE(TAG, "hid_report_map malloc failed");
        free(s_temp_hid_report_map);
        s_temp_hid_report_map = NULL;
        return NULL;
    }

    temp_hid_report_map_t *map = s_temp_hid_report_map;

    esp_hid_report_item_t *reports = (esp_hid_report_item_t *)calloc(1, map->reports_len * sizeof(esp_hid_report_item_t));
    if (reports == NULL) {
        ESP_LOGE(TAG, "hid_report_items malloc failed! %u maps", map->reports_len);
        free(out);
        free(s_temp_hid_report_map);
        s_temp_hid_report_map = NULL;
        return NULL;
    }

    if (map->usage_mask & ESP_HID_USAGE_KEYBOARD) {
        out->usage = ESP_HID_USAGE_KEYBOARD;
        out->appearance = ESP_HID_APPEARANCE_KEYBOARD;
    } else if (map->usage_mask & ESP_HID_USAGE_MOUSE) {
        out->usage = ESP_HID_USAGE_MOUSE;
        out->appearance = ESP_HID_APPEARANCE_MOUSE;
    } else if (map->usage_mask & ESP_HID_USAGE_JOYSTICK) {
        out->usage = ESP_HID_USAGE_JOYSTICK;
        out->appearance = ESP_HID_APPEARANCE_JOYSTICK;
    } else if (map->usage_mask & ESP_HID_USAGE_GAMEPAD) {
        out->usage = ESP_HID_USAGE_GAMEPAD;
        out->appearance = ESP_HID_APPEARANCE_GAMEPAD;
    } else if (map->usage_mask & ESP_HID_USAGE_CCONTROL) {
        out->usage = ESP_HID_USAGE_CCONTROL;
        out->appearance = ESP_HID_APPEARANCE_KEYBOARD;
    } else {
        out->usage = ESP_HID_USAGE_GENERIC;
        out->appearance = ESP_HID_APPEARANCE_GENERIC;
    }
    out->reports_len = map->reports_len;
    memcpy(reports, map->reports, map->reports_len * sizeof(esp_hid_report_item_t));
    out->reports = reports;
    free(s_temp_hid_report_map);
    s_temp_hid_report_map = NULL;

    return out;
}

void esp_hid_free_report_map(esp_hid_report_map_t *map)
{
    if (map != NULL){
        free(map->reports);
        free(map);
    }
}

esp_hid_usage_t esp_hid_usage_from_appearance(uint16_t appearance)
{
    return ESP_HID_USAGE_GENERIC;
}

esp_hid_usage_t esp_hid_usage_from_cod(uint32_t cod)
{
    return ESP_HID_USAGE_GENERIC;
}

static const char *s_unknown_str = "UNKNOWN";
static const char *s_hid_protocol_names[] = {"BOOT", "REPORT"};
static const char *s_hid_report_type_names[] = {"NULL", "INPUT", "OUTPUT", "FEATURE"};
static const char *s_hid_cod_major_names[] = {"MISC", "COMPUTER", "PHONE", "LAN_NAP", "AV", "PERIPHERAL", "IMAGING", "WEARABLE", "TOY", "HEALTH"};
static const char *s_hid_cod_minor_names[7] = {"GENERIC", "JOYSTICK", "GAMEPAD", "REMOTE", "SENSOR", "TABLET", "CARD_READER"};

const char *esp_hid_usage_str(esp_hid_usage_t usage)
{
    switch (usage) {
    case ESP_HID_USAGE_GENERIC: return "GENERIC";
    case ESP_HID_USAGE_KEYBOARD: return "KEYBOARD";
    case ESP_HID_USAGE_MOUSE: return "MOUSE";
    case ESP_HID_USAGE_JOYSTICK: return "JOYSTICK";
    case ESP_HID_USAGE_GAMEPAD: return "GAMEPAD";
    case ESP_HID_USAGE_CCONTROL: return "CCONTROL";
    case ESP_HID_USAGE_VENDOR: return "VENDOR";
    default: break;
    }
    return s_unknown_str;
}

const char *esp_hid_protocol_mode_str(uint8_t protocol)
{
    if (protocol >= (sizeof(s_hid_protocol_names)/sizeof(s_hid_protocol_names[0]))) {
        return s_unknown_str;
    }
    return s_hid_protocol_names[protocol];
}

const char *esp_hid_report_type_str(uint8_t report_type)
{
    if (report_type >= (sizeof(s_hid_report_type_names)/sizeof(s_hid_report_type_names[0]))) {
        return s_unknown_str;
    }
    return s_hid_report_type_names[report_type];
}

const char *esp_hid_cod_major_str(uint8_t cod_major)
{
    if (cod_major >= (sizeof(s_hid_cod_major_names)/sizeof(s_hid_cod_major_names[0]))) {
        return s_unknown_str;
    }
    return s_hid_cod_major_names[cod_major];
}

void esp_hid_cod_minor_print(uint8_t cod_min, FILE *fp)
{
    if (cod_min & ESP_HID_COD_MIN_KEYBOARD) {
        fputs("KEYBOARD", fp);
    }
    if (cod_min & ESP_HID_COD_MIN_MOUSE) {
        if (cod_min & ESP_HID_COD_MIN_KEYBOARD) {
            fputs("+", fp);
        }
        fputs("MOUSE", fp);
    }
    if (cod_min & 0xF0) {
        if (cod_min & 0x0F) {
            fputs("+", fp);
        } else {
            return;
        }
    }
    cod_min &= 0x0F;
    if (cod_min < ESP_HID_COD_MIN_MAX) {
        fprintf(fp, "%s", s_hid_cod_minor_names[cod_min]);
    }
}

const char *esp_hid_disconnect_reason_str(esp_hid_transport_t transport, int reason)
{
    if (transport == ESP_HID_TRANSPORT_BLE) {
#if (CONFIG_GATTS_ENABLE || CONFIG_GATTC_ENABLE)
        switch ((esp_gatt_conn_reason_t)reason) {
        case ESP_GATT_CONN_L2C_FAILURE: return "L2C_FAILURE";
        case ESP_GATT_CONN_TIMEOUT: return "TIMEOUT";
        case ESP_GATT_CONN_TERMINATE_PEER_USER: return "TERMINATE_PEER_USER";
        case ESP_GATT_CONN_TERMINATE_LOCAL_HOST: return "TERMINATE_LOCAL_HOST";
        case ESP_GATT_CONN_LMP_TIMEOUT: return "LMP_TIMEOUT";
        case ESP_GATT_CONN_FAIL_ESTABLISH: return "FAIL_ESTABLISH";
        case ESP_GATT_CONN_CONN_CANCEL: return "CONN_CANCEL";
        case ESP_GATT_CONN_NONE: return "NONE";
        default: break;
        }
#endif /* CONFIG_GATTS_ENABLE || CONFIG_GATTC_ENABLE */
    }
    return s_unknown_str;
}
