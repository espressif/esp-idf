/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#pragma once
#include "esp_err.h"

#ifdef ESPWROOM32SE
/*
 * Certificate Type
 */
typedef enum cert_type {
    CERT_TYPE_DEVICE = 0,
    CERT_TYPE_CA,
} cert_type_t;
#endif

esp_err_t mfg_rsa_keygen(int key_size, unsigned char *private_key_buf, size_t private_key_buf_len);
esp_err_t mfg_csr_gen(const char * subject_name, unsigned char *csr_buf, size_t csr_buf_len);
esp_err_t mfg_input_cert(unsigned char *cert_buf, size_t cert_len
#ifdef ESPWROOM32SE
, cert_type_t cert_type
#endif
);
esp_err_t mfg_overwrite_bootloader(unsigned char * private_key_buf, unsigned char * device_cert_buf, unsigned char * ca_cert_buf);
