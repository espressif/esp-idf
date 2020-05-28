/**
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef ESP_PLATFORM
#include "esp_system.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "random.h"
#include "sha256.h"

#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"

int mbedtls_hmac_vector(mbedtls_md_type_t md_type, const u8 *key, size_t key_len,
		        size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	size_t i;
	const mbedtls_md_info_t *md_info;
	mbedtls_md_context_t md_ctx;
	int ret;

	mbedtls_md_init(&md_ctx);

	if((md_info = mbedtls_md_info_from_type(md_type)) == NULL )
		return -1;

	if ((ret = mbedtls_md_setup( &md_ctx, md_info, 1)) != 0)
		return(ret);

	mbedtls_md_hmac_starts(&md_ctx, key, key_len);

	for( i = 0; i < num_elem; i++)
		mbedtls_md_hmac_update(&md_ctx, addr[i], len[i]);

	mbedtls_md_hmac_finish(&md_ctx, mac);

	mbedtls_md_free(&md_ctx);

	return 0;
}

int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem,
		const u8 *addr[], const size_t *len, u8 *mac)
{
	return mbedtls_hmac_vector(MBEDTLS_MD_SHA384, key, key_len, num_elem, addr,
				   len, mac);
}


int hmac_sha384(const u8 *key, size_t key_len, const u8 *data,
		size_t data_len, u8 *mac)
{
	return hmac_sha384_vector(key, key_len, 1, &data, &data_len, mac);
}
