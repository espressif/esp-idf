// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <strings.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_secure_boot.h"

#ifndef BOOTLOADER_BUILD
static __attribute__((unused)) const char *TAG = "secure_boot";

#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

static void rsa_check_signature_on_update_check(void)
{
    // We rely on the keys used to sign this app to verify the next app on OTA, so make sure there is at
    // least one to avoid a stuck firmware
    esp_image_sig_public_key_digests_t digests = { 0 };

    esp_err_t err = esp_secure_boot_get_signature_blocks_for_running_app(false, &digests);

    if (err != ESP_OK || digests.num_digests == 0) {
        ESP_LOGE(TAG, "This app is not signed, but check signature on update is enabled in config. It won't be possible to verify any update.");
        abort();
    }
#if CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT && SECURE_BOOT_NUM_BLOCKS > 1
    if (digests.num_digests > 1) {
        ESP_LOGW(TAG, "App has %d signatures. Only the first position of signature blocks is used to verify any update", digests.num_digests);
    }
#endif
}
#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

void esp_secure_boot_init_checks(void)
{



#if CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT
    rsa_check_signature_on_update_check();
#endif // CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

}
#endif // not BOOTLOADER_BUILD
