/* CBOR Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "cbor.h"

static const char *TAG = "example";

#define CBOR_CHECK(a, str, goto_tag, ret_value, ...)                              \
    do                                                                            \
    {                                                                             \
        if ((a) != CborNoError)                                                   \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

static void indent(int nestingLevel)
{
    while (nestingLevel--) {
        printf("  ");
    }
}

static void dumpbytes(const uint8_t *buf, size_t len)
{
    while (len--) {
        printf("%02X ", *buf++);
    }
}

/**
 * Decode CBOR data manuallly
 */
static CborError example_dump_cbor_buffer(CborValue *it, int nestingLevel)
{
    CborError ret = CborNoError;
    while (!cbor_value_at_end(it)) {
        CborType type = cbor_value_get_type(it);

        indent(nestingLevel);
        switch (type) {
        case CborArrayType: {
            CborValue recursed;
            assert(cbor_value_is_container(it));
            puts("Array[");
            ret = cbor_value_enter_container(it, &recursed);
            CBOR_CHECK(ret, "enter container failed", err, ret);
            ret = example_dump_cbor_buffer(&recursed, nestingLevel + 1);
            CBOR_CHECK(ret, "recursive dump failed", err, ret);
            ret = cbor_value_leave_container(it, &recursed);
            CBOR_CHECK(ret, "leave container failed", err, ret);
            indent(nestingLevel);
            puts("]");
            continue;
        }
        case CborMapType: {
            CborValue recursed;
            assert(cbor_value_is_container(it));
            puts("Map{");
            ret = cbor_value_enter_container(it, &recursed);
            CBOR_CHECK(ret, "enter container failed", err, ret);
            ret = example_dump_cbor_buffer(&recursed, nestingLevel + 1);
            CBOR_CHECK(ret, "recursive dump failed", err, ret);
            ret = cbor_value_leave_container(it, &recursed);
            CBOR_CHECK(ret, "leave container failed", err, ret);
            indent(nestingLevel);
            puts("}");
            continue;
        }
        case CborIntegerType: {
            int64_t val;
            ret = cbor_value_get_int64(it, &val);
            CBOR_CHECK(ret, "parse int64 failed", err, ret);
            printf("%lld\n", (long long)val);
            break;
        }
        case CborByteStringType: {
            uint8_t *buf;
            size_t n;
            ret = cbor_value_dup_byte_string(it, &buf, &n, it);
            CBOR_CHECK(ret, "parse byte string failed", err, ret);
            dumpbytes(buf, n);
            puts("");
            free(buf);
            continue;
        }
        case CborTextStringType: {
            char *buf;
            size_t n;
            ret = cbor_value_dup_text_string(it, &buf, &n, it);
            CBOR_CHECK(ret, "parse text string failed", err, ret);
            puts(buf);
            free(buf);
            continue;
        }
        case CborTagType: {
            CborTag tag;
            ret = cbor_value_get_tag(it, &tag);
            CBOR_CHECK(ret, "parse tag failed", err, ret);
            printf("Tag(%lld)\n", (long long)tag);
            break;
        }
        case CborSimpleType: {
            uint8_t type;
            ret = cbor_value_get_simple_type(it, &type);
            CBOR_CHECK(ret, "parse simple type failed", err, ret);
            printf("simple(%u)\n", type);
            break;
        }
        case CborNullType:
            puts("null");
            break;
        case CborUndefinedType:
            puts("undefined");
            break;
        case CborBooleanType: {
            bool val;
            ret = cbor_value_get_boolean(it, &val);
            CBOR_CHECK(ret, "parse boolean type failed", err, ret);
            puts(val ? "true" : "false");
            break;
        }
        case CborHalfFloatType: {
            uint16_t val;
            ret = cbor_value_get_half_float(it, &val);
            CBOR_CHECK(ret, "parse half float type failed", err, ret);
            printf("__f16(%04x)\n", val);
            break;
        }
        case CborFloatType: {
            float val;
            ret = cbor_value_get_float(it, &val);
            CBOR_CHECK(ret, "parse float type failed", err, ret);
            printf("%g\n", val);
            break;
        }
        case CborDoubleType: {
            double val;
            ret = cbor_value_get_double(it, &val);
            CBOR_CHECK(ret, "parse double float type failed", err, ret);
            printf("%g\n", val);
            break;
        }
        case CborInvalidType: {
            ret = CborErrorUnknownType;
            CBOR_CHECK(ret, "unknown cbor type", err, ret);
            break;
        }
        }

        ret = cbor_value_advance_fixed(it);
        CBOR_CHECK(ret, "fix value failed", err, ret);
    }
    return CborNoError;
err:
    return ret;
}


void app_main(void)
{
    CborEncoder root_encoder;
    CborParser root_parser;
    CborValue it;
    uint8_t buf[100];

    // Initialize the outermost cbor encoder
    cbor_encoder_init(&root_encoder, buf, sizeof(buf), 0);

    // Create an array containing several items
    CborEncoder array_encoder;
    CborEncoder map_encoder;
    cbor_encoder_create_array(&root_encoder, &array_encoder, 5); // [
    // 1. Create a map containing several pairs
    cbor_encoder_create_map(&array_encoder, &map_encoder, 3); // {
    // chip:esp32
    cbor_encode_text_stringz(&map_encoder, "chip");
    cbor_encode_text_stringz(&map_encoder, "esp32");
    // unicore:false
    cbor_encode_text_stringz(&map_encoder, "unicore");
    cbor_encode_boolean(&map_encoder, false);
    // ip:[192,168,1,100]
    cbor_encode_text_stringz(&map_encoder, "ip");
    CborEncoder array2;
    cbor_encoder_create_array(&map_encoder, &array2, 4); // [
    // Encode several numbers
    cbor_encode_uint(&array2, 192);
    cbor_encode_uint(&array2, 168);
    cbor_encode_uint(&array2, 1);
    cbor_encode_uint(&array2, 100);
    cbor_encoder_close_container(&map_encoder, &array2);        // ]
    cbor_encoder_close_container(&array_encoder, &map_encoder); // }
    // 2. Encode float number
    cbor_encode_float(&array_encoder, 3.14);
    // 3. Encode simple value
    cbor_encode_simple_value(&array_encoder, 99);
    // 4. Encode a string
    cbor_encode_text_stringz(&array_encoder, "2019-07-10 09:00:00+0000");
    // 5. Encode a undefined value
    cbor_encode_undefined(&array_encoder);
    cbor_encoder_close_container(&root_encoder, &array_encoder); // ]

    // If error happend when encoding, then this value should be meaningless
    ESP_LOGI(TAG, "encoded buffer size %d", cbor_encoder_get_buffer_size(&root_encoder, buf));

    // Initialize the cbor parser and the value iterator
    cbor_parser_init(buf, sizeof(buf), 0, &root_parser, &it);

    ESP_LOGI(TAG, "convert CBOR to JSON");
    // Dump the values in JSON format
    cbor_value_to_json(stdout, &it, 0);
    puts("");

    ESP_LOGI(TAG, "decode CBOR manually");
    // Decode CBOR data manully
    example_dump_cbor_buffer(&it, 0);
}
