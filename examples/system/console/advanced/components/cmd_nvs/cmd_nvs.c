/* Console example — NVS commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "cmd_nvs.h"
#include "nvs.h"

typedef struct {
    nvs_type_t type;
    const char *str;
} type_str_pair_t;

static const type_str_pair_t type_str_pair[] = {
    { NVS_TYPE_I8, "i8" },
    { NVS_TYPE_U8, "u8" },
    { NVS_TYPE_U16, "u16" },
    { NVS_TYPE_I16, "i16" },
    { NVS_TYPE_U32, "u32" },
    { NVS_TYPE_I32, "i32" },
    { NVS_TYPE_U64, "u64" },
    { NVS_TYPE_I64, "i64" },
    { NVS_TYPE_STR, "str" },
    { NVS_TYPE_BLOB, "blob" },
    { NVS_TYPE_ANY, "any" },
};

static const size_t TYPE_STR_PAIR_SIZE = sizeof(type_str_pair) / sizeof(type_str_pair[0]);
static const char *ARG_TYPE_STR = "type can be: i8, u8, i16, u16 i32, u32 i64, u64, str, blob";
static char current_namespace[16] = "storage";
static const char *TAG = "cmd_nvs";

static struct {
    struct arg_str *key;
    struct arg_str *type;
    struct arg_str *value;
    struct arg_end *end;
} set_args;

static struct {
    struct arg_str *key;
    struct arg_str *type;
    struct arg_end *end;
} get_args;

static struct {
    struct arg_str *key;
    struct arg_end *end;
} erase_args;

static struct {
    struct arg_str *namespace;
    struct arg_end *end;
} erase_all_args;

static struct {
    struct arg_str *namespace;
    struct arg_end *end;
} namespace_args;

static struct {
    struct arg_str *partition;
    struct arg_str *namespace;
    struct arg_str *type;
    struct arg_end *end;
} list_args;


static nvs_type_t str_to_type(const char *type)
{
    for (int i = 0; i < TYPE_STR_PAIR_SIZE; i++) {
        const type_str_pair_t *p = &type_str_pair[i];
        if (strcmp(type, p->str) == 0) {
            return  p->type;
        }
    }

    return NVS_TYPE_ANY;
}

static const char *type_to_str(nvs_type_t type)
{
    for (int i = 0; i < TYPE_STR_PAIR_SIZE; i++) {
        const type_str_pair_t *p = &type_str_pair[i];
        if (p->type == type) {
            return  p->str;
        }
    }

    return "Unknown";
}

static esp_err_t store_blob(nvs_handle_t nvs, const char *key, const char *str_values)
{
    uint8_t value;
    size_t str_len = strlen(str_values);
    size_t blob_len = str_len / 2;

    if (str_len % 2) {
        ESP_LOGE(TAG, "Blob data must contain even number of characters");
        return ESP_ERR_NVS_TYPE_MISMATCH;
    }

    char *blob = (char *)malloc(blob_len);
    if (blob == NULL) {
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0, j = 0; i < str_len; i++) {
        char ch = str_values[i];
        if (ch >= '0' && ch <= '9') {
            value = ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            value = ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'f') {
            value = ch - 'a' + 10;
        } else {
            ESP_LOGE(TAG, "Blob data contain invalid character");
            free(blob);
            return ESP_ERR_NVS_TYPE_MISMATCH;
        }

        if (i & 1) {
            blob[j++] += value;
        } else {
            blob[j] = value << 4;
        }
    }

    esp_err_t err = nvs_set_blob(nvs, key, blob, blob_len);
    free(blob);

    if (err == ESP_OK) {
        err = nvs_commit(nvs);
    }

    return err;
}

static void print_blob(const char *blob, size_t len)
{
    for (int i = 0; i < len; i++) {
        printf("%02x", blob[i]);
    }
    printf("\n");
}


static esp_err_t set_value_in_nvs(const char *key, const char *str_type, const char *str_value)
{
    esp_err_t err;
    nvs_handle_t nvs;
    bool range_error = false;

    nvs_type_t type = str_to_type(str_type);

    if (type == NVS_TYPE_ANY) {
        ESP_LOGE(TAG, "Type '%s' is undefined", str_type);
        return ESP_ERR_NVS_TYPE_MISMATCH;
    }

    err = nvs_open(current_namespace, NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        return err;
    }

    if (type == NVS_TYPE_I8) {
        int32_t value = strtol(str_value, NULL, 0);
        if (value < INT8_MIN || value > INT8_MAX || errno == ERANGE) {
            range_error = true;
        } else {
            err = nvs_set_i8(nvs, key, (int8_t)value);
        }
    } else if (type == NVS_TYPE_U8) {
        uint32_t value = strtoul(str_value, NULL, 0);
        if (value > UINT8_MAX || errno == ERANGE) {
            range_error = true;
        } else {
            err = nvs_set_u8(nvs, key, (uint8_t)value);
        }
    } else if (type == NVS_TYPE_I16) {
        int32_t value = strtol(str_value, NULL, 0);
        if (value < INT16_MIN || value > INT16_MAX || errno == ERANGE) {
            range_error = true;
        } else {
            err = nvs_set_i16(nvs, key, (int16_t)value);
        }
    } else if (type == NVS_TYPE_U16) {
        uint32_t value = strtoul(str_value, NULL, 0);
        if (value > UINT16_MAX || errno == ERANGE) {
            range_error = true;
        } else {
            err = nvs_set_u16(nvs, key, (uint16_t)value);
        }
    } else if (type == NVS_TYPE_I32) {
        int32_t value = strtol(str_value, NULL, 0);
        if (errno != ERANGE) {
            err = nvs_set_i32(nvs, key, value);
        }
    } else if (type == NVS_TYPE_U32) {
        uint32_t value = strtoul(str_value, NULL, 0);
        if (errno != ERANGE) {
            err = nvs_set_u32(nvs, key, value);
        }
    } else if (type == NVS_TYPE_I64) {
        int64_t value = strtoll(str_value, NULL, 0);
        if (errno != ERANGE) {
            err = nvs_set_i64(nvs, key, value);
        }
    } else if (type == NVS_TYPE_U64) {
        uint64_t value = strtoull(str_value, NULL, 0);
        if (errno != ERANGE) {
            err = nvs_set_u64(nvs, key, value);
        }
    } else if (type == NVS_TYPE_STR) {
        err = nvs_set_str(nvs, key, str_value);
    } else if (type == NVS_TYPE_BLOB) {
        err = store_blob(nvs, key, str_value);
    }

    if (range_error || errno == ERANGE) {
        nvs_close(nvs);
        return ESP_ERR_NVS_VALUE_TOO_LONG;
    }

    if (err == ESP_OK) {
        err = nvs_commit(nvs);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Value stored under key '%s'", key);
        }
    }

    nvs_close(nvs);
    return err;
}

static esp_err_t get_value_from_nvs(const char *key, const char *str_type)
{
    nvs_handle_t nvs;
    esp_err_t err;

    nvs_type_t type = str_to_type(str_type);

    if (type == NVS_TYPE_ANY) {
        ESP_LOGE(TAG, "Type '%s' is undefined", str_type);
        return ESP_ERR_NVS_TYPE_MISMATCH;
    }

    err = nvs_open(current_namespace, NVS_READONLY, &nvs);
    if (err != ESP_OK) {
        return err;
    }

    if (type == NVS_TYPE_I8) {
        int8_t value;
        err = nvs_get_i8(nvs, key, &value);
        if (err == ESP_OK) {
            printf("%d\n", value);
        }
    } else if (type == NVS_TYPE_U8) {
        uint8_t value;
        err = nvs_get_u8(nvs, key, &value);
        if (err == ESP_OK) {
            printf("%u\n", value);
        }
    } else if (type == NVS_TYPE_I16) {
        int16_t value;
        err = nvs_get_i16(nvs, key, &value);
        if (err == ESP_OK) {
            printf("%u\n", value);
        }
    } else if (type == NVS_TYPE_U16) {
        uint16_t value;
        if ((err = nvs_get_u16(nvs, key, &value)) == ESP_OK) {
            printf("%u\n", value);
        }
    } else if (type == NVS_TYPE_I32) {
        int32_t value;
        if ((err = nvs_get_i32(nvs, key, &value)) == ESP_OK) {
            printf("%d\n", value);
        }
    } else if (type == NVS_TYPE_U32) {
        uint32_t value;
        if ((err = nvs_get_u32(nvs, key, &value)) == ESP_OK) {
            printf("%u\n", value);
        }
    } else if (type == NVS_TYPE_I64) {
        int64_t value;
        if ((err = nvs_get_i64(nvs, key, &value)) == ESP_OK) {
            printf("%lld\n", value);
        }
    } else if (type == NVS_TYPE_U64) {
        uint64_t value;
        if ( (err = nvs_get_u64(nvs, key, &value)) == ESP_OK) {
            printf("%llu\n", value);
        }
    } else if (type == NVS_TYPE_STR) {
        size_t len;
        if ( (err = nvs_get_str(nvs, key, NULL, &len)) == ESP_OK) {
            char *str = (char *)malloc(len);
            if ( (err = nvs_get_str(nvs, key, str, &len)) == ESP_OK) {
                printf("%s\n", str);
            }
            free(str);
        }
    } else if (type == NVS_TYPE_BLOB) {
        size_t len;
        if ( (err = nvs_get_blob(nvs, key, NULL, &len)) == ESP_OK) {
            char *blob = (char *)malloc(len);
            if ( (err = nvs_get_blob(nvs, key, blob, &len)) == ESP_OK) {
                print_blob(blob, len);
            }
            free(blob);
        }
    }

    nvs_close(nvs);
    return err;
}

static esp_err_t erase(const char *key)
{
    nvs_handle_t nvs;

    esp_err_t err = nvs_open(current_namespace, NVS_READWRITE, &nvs);
    if (err == ESP_OK) {
        err = nvs_erase_key(nvs, key);
        if (err == ESP_OK) {
            err = nvs_commit(nvs);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Value with key '%s' erased", key);
            }
        }
        nvs_close(nvs);
    }

    return err;
}

static esp_err_t erase_all(const char *name)
{
    nvs_handle_t nvs;

    esp_err_t err = nvs_open(name, NVS_READWRITE, &nvs);
    if (err == ESP_OK) {
        err = nvs_erase_all(nvs);
        if (err == ESP_OK) {
            err = nvs_commit(nvs);
        }
    }

    ESP_LOGI(TAG, "Namespace '%s' was %s erased", name, (err == ESP_OK) ? "" : "not");

    nvs_close(nvs);
    return ESP_OK;
}

static int list(const char *part, const char *name, const char *str_type)
{
    nvs_type_t type = str_to_type(str_type);

    nvs_iterator_t it = nvs_entry_find(part, NULL, type);
    if (it == NULL) {
        ESP_LOGE(TAG, "No such enty was found");
        return 1;
    }

    do {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        it = nvs_entry_next(it);

        printf("namespace '%s', key '%s', type '%s' \n",
               info.namespace_name, info.key, type_to_str(info.type));
    } while (it != NULL);

    return 0;
}

static int set_value(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &set_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_args.end, argv[0]);
        return 1;
    }

    const char *key = set_args.key->sval[0];
    const char *type = set_args.type->sval[0];
    const char *values = set_args.value->sval[0];

    esp_err_t err = set_value_in_nvs(key, type, values);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(err));
        return 1;
    }

    return 0;
}

static int get_value(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &get_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, get_args.end, argv[0]);
        return 1;
    }

    const char *key = get_args.key->sval[0];
    const char *type = get_args.type->sval[0];

    esp_err_t err = get_value_from_nvs(key, type);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(err));
        return 1;
    }

    return 0;
}

static int erase_value(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &erase_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, erase_args.end, argv[0]);
        return 1;
    }

    const char *key = erase_args.key->sval[0];

    esp_err_t err = erase(key);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(err));
        return 1;
    }

    return 0;
}

static int erase_namespace(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &erase_all_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, erase_all_args.end, argv[0]);
        return 1;
    }

    const char *name = erase_all_args.namespace->sval[0];

    esp_err_t err = erase_all(name);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(err));
        return 1;
    }

    return 0;
}

static int set_namespace(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &namespace_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, namespace_args.end, argv[0]);
        return 1;
    }

    const char *namespace = namespace_args.namespace->sval[0];
    strlcpy(current_namespace, namespace, sizeof(current_namespace));
    ESP_LOGI(TAG, "Namespace set to '%s'", current_namespace);
    return 0;
}

static int list_entries(int argc, char **argv)
{
    list_args.partition->sval[0] = "";
    list_args.namespace->sval[0] = "";
    list_args.type->sval[0] = "";

    int nerrors = arg_parse(argc, argv, (void **) &list_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, list_args.end, argv[0]);
        return 1;
    }

    const char *part = list_args.partition->sval[0];
    const char *name = list_args.namespace->sval[0];
    const char *type = list_args.type->sval[0];

    return list(part, name, type);
}

void register_nvs(void)
{
    set_args.key = arg_str1(NULL, NULL, "<key>", "key of the value to be set");
    set_args.type = arg_str1(NULL, NULL, "<type>", ARG_TYPE_STR);

    set_args.value = arg_str1("v", "value", "<value>", "value to be stored");
    set_args.end = arg_end(2);

    get_args.key = arg_str1(NULL, NULL, "<key>", "key of the value to be read");
    get_args.type = arg_str1(NULL, NULL, "<type>", ARG_TYPE_STR);
    get_args.end = arg_end(2);

    erase_args.key = arg_str1(NULL, NULL, "<key>", "key of the value to be erased");
    erase_args.end = arg_end(2);

    erase_all_args.namespace = arg_str1(NULL, NULL, "<namespace>", "namespace to be erased");
    erase_all_args.end = arg_end(2);

    namespace_args.namespace = arg_str1(NULL, NULL, "<namespace>", "namespace of the partition to be selected");
    namespace_args.end = arg_end(2);

    list_args.partition = arg_str1(NULL, NULL, "<partition>", "partition name");
    list_args.namespace = arg_str0("n", "namespace", "<namespace>", "namespace name");
    list_args.type = arg_str0("t", "type", "<type>", ARG_TYPE_STR);
    list_args.end = arg_end(2);

    const esp_console_cmd_t set_cmd = {
        .command = "nvs_set",
        .help = "Set key-value pair in selected namespace.\n"
        "Examples:\n"
        " nvs_set VarName i32 -v 123 \n"
        " nvs_set VarName str -v YourString \n"
        " nvs_set VarName blob -v 0123456789abcdef \n",
        .hint = NULL,
        .func = &set_value,
        .argtable = &set_args
    };

    const esp_console_cmd_t get_cmd = {
        .command = "nvs_get",
        .help = "Get key-value pair from selected namespace. \n"
        "Example: nvs_get VarName i32",
        .hint = NULL,
        .func = &get_value,
        .argtable = &get_args
    };

    const esp_console_cmd_t erase_cmd = {
        .command = "nvs_erase",
        .help = "Erase key-value pair from current namespace",
        .hint = NULL,
        .func = &erase_value,
        .argtable = &erase_args
    };

    const esp_console_cmd_t erase_namespace_cmd = {
        .command = "nvs_erase_namespace",
        .help = "Erases specified namespace",
        .hint = NULL,
        .func = &erase_namespace,
        .argtable = &erase_all_args
    };

    const esp_console_cmd_t namespace_cmd = {
        .command = "nvs_namespace",
        .help = "Set current namespace",
        .hint = NULL,
        .func = &set_namespace,
        .argtable = &namespace_args
    };

    const esp_console_cmd_t list_entries_cmd = {
        .command = "nvs_list",
        .help = "List stored key-value pairs stored in NVS."
        "Namespace and type can be specified to print only those key-value pairs.\n"
        "Following command list variables stored inside 'nvs' partition, under namespace 'storage' with type uint32_t"
        "Example: nvs_list nvs -n storage -t u32 \n",
        .hint = NULL,
        .func = &list_entries,
        .argtable = &list_args
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&set_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&get_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&erase_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&namespace_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&list_entries_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&erase_namespace_cmd));
}
