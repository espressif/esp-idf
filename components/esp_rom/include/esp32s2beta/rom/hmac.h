// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROM_HMAC_H_
#define _ROM_HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "rom/efuse.h"

void ets_hmac_enable(void);

void ets_hmac_disable(void);

/* Use the "upstream" HMAC key (ETS_EFUSE_KEY_PURPOSE_HMAC_UP)
   to digest a message.
*/
int ets_hmac_calculate_message(ets_efuse_block_t key_block, const void *message, size_t message_len, uint8_t *hmac);

/* Calculate a downstream HMAC message to temporarily enable JTAG, or
   to generate a Digital Signature data decryption key.

   - purpose must be ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE
     or ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG

   - key_block must be in range ETS_EFUSE_BLOCK_KEY0 toETS_EFUSE_BLOCK_KEY6.
     This efuse block must have the corresponding purpose set in "purpose", or
     ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL.

   The result of this HMAC calculation is only made available "downstream" to the
   corresponding hardware module, and cannot be accessed by software.
*/
int ets_hmac_calculate_downstream(ets_efuse_block_t key_block, ets_efuse_purpose_t purpose);

/* Invalidate a downstream HMAC value previously calculated by ets_hmac_calculate_downstream().
 *
 * - purpose must match a previous call to ets_hmac_calculate_downstream().
 *
 * After this function is called, the corresponding internal operation (JTAG or DS) will no longer
 * have access to the generated key.
 */
int ets_hmac_invalidate_downstream(ets_efuse_purpose_t purpose);

#ifdef __cplusplus
}
#endif

#endif // _ROM_HMAC_H_
