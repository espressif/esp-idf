/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_secure_boot.h"

ESP_LOG_ATTR_TAG(TAG, "secure_boot");

#if (CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME) && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT

void esp_secure_boot_check_signature_on_update(void)
{
    /* We rely on the keys used to sign this app to verify the next app on OTA, so make sure
       there is at least one to avoid a stuck firmware. */
    esp_image_sig_public_key_digests_t digests = { 0 };
    esp_err_t err = esp_secure_boot_get_signature_blocks_for_running_app(false, &digests);

    if (err != ESP_OK || digests.num_digests == 0) {
        ESP_LOGE(TAG, "This app is not signed, but check signature on update is enabled in config. It won't be possible to verify any update.");
        abort();
    }
#if SECURE_BOOT_NUM_BLOCKS > 1
    if (digests.num_digests > 1) {
        ESP_LOGW(TAG, "App has %d signatures. Only the first position of signature blocks is used to verify any update", digests.num_digests);
    }
#endif
}

#endif /* (CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME || CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME) && CONFIG_SECURE_SIGNED_ON_UPDATE_NO_SECURE_BOOT */
