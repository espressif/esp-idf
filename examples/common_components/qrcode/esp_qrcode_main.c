// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <esp_err.h>
#include "esp_log.h"

#include "qrcodegen.h"
#include "qrcode.h"

static const char *TAG = "QRCODE";

static const char *lt[] = {
    /* 0 */ "  ",
    /* 1 */ "\u2580 ",
    /* 2 */ " \u2580",
    /* 3 */ "\u2580\u2580",
    /* 4 */ "\u2584 ",
    /* 5 */ "\u2588 ",
    /* 6 */ "\u2584\u2580",
    /* 7 */ "\u2588\u2580",
    /* 8 */ " \u2584",
    /* 9 */ "\u2580\u2584",
    /* 10 */ " \u2588",
    /* 11 */ "\u2580\u2588",
    /* 12 */ "\u2584\u2584",
    /* 13 */ "\u2588\u2584",
    /* 14 */ "\u2584\u2588",
    /* 15 */ "\u2588\u2588",
};

void esp_qrcode_print_console(esp_qrcode_handle_t qrcode)
{
    int size = qrcodegen_getSize(qrcode);
    int border = 2;
    unsigned char num = 0;

    for (int y = -border; y < size + border; y+=2) {
        for (int x = -border; x < size + border; x+=2) {
            num = 0;
            if (qrcodegen_getModule(qrcode, x, y)) {
                num |= 1 << 0;
            }
            if ((x < size + border) && qrcodegen_getModule(qrcode, x+1, y)) {
                num |= 1 << 1;
            }
            if ((y < size + border) && qrcodegen_getModule(qrcode, x, y+1)) {
                num |= 1 << 2;
            }
            if ((x < size + border) && (y < size + border) && qrcodegen_getModule(qrcode, x+1, y+1)) {
                num |= 1 << 3;
            }
            printf("%s", lt[num]);
        }
        printf("\n");
    }
    printf("\n");
}

esp_err_t esp_qrcode_generate(esp_qrcode_config_t *cfg, const char *text)
{
    enum qrcodegen_Ecc ecc_lvl;
    uint8_t *qrcode, *tempbuf;
    esp_err_t err = ESP_FAIL;

    qrcode = calloc(1, qrcodegen_BUFFER_LEN_FOR_VERSION(cfg->max_qrcode_version));
    if (!qrcode) {
        return ESP_ERR_NO_MEM;
    }

    tempbuf = calloc(1, qrcodegen_BUFFER_LEN_FOR_VERSION(cfg->max_qrcode_version));
    if (!tempbuf) {
        free(qrcode);
        return ESP_ERR_NO_MEM;
    }

    switch(cfg->qrcode_ecc_level) {
        case ESP_QRCODE_ECC_LOW:
            ecc_lvl = qrcodegen_Ecc_LOW;
            break;
        case ESP_QRCODE_ECC_MED:
            ecc_lvl = qrcodegen_Ecc_MEDIUM;
            break;
        case ESP_QRCODE_ECC_QUART:
            ecc_lvl = qrcodegen_Ecc_QUARTILE;
            break;
        case ESP_QRCODE_ECC_HIGH:
            ecc_lvl = qrcodegen_Ecc_HIGH;
            break;
        default:
            ecc_lvl = qrcodegen_Ecc_LOW;
            break;
    }

    ESP_LOGI(TAG, "Encoding below text with ECC LVL %d & QR Code Version %d",
             ecc_lvl, cfg->max_qrcode_version);
    ESP_LOGI(TAG, "%s", text);
    // Make and print the QR Code symbol
    bool ok = qrcodegen_encodeText(text, tempbuf, qrcode, ecc_lvl,
                                   qrcodegen_VERSION_MIN, cfg->max_qrcode_version,
                                   qrcodegen_Mask_AUTO, true);
    if (ok && cfg->display_func) {
        cfg->display_func((esp_qrcode_handle_t)qrcode);
        err = ESP_OK;
    }

    free(qrcode);
    free(tempbuf);
    return err;
}
