/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <string.h>
#include <sys/param.h>
#include "rom/ecdsa.h"

#define ROM_FUNC_TYPECAST  int(*)(const uint8_t*, const uint8_t*, int, const uint8_t*, uint8_t*)

extern uint32_t _rom_eco_version;
int (*_rom_ets_ecdsa_verify)(const uint8_t*, const uint8_t*, int, const uint8_t*, uint8_t*);

/* On ESP32-C6 ECO 0, the ROM interface hasn't exposed ets_ecdsa_verify symbol, so for that we have defined
 * the function here and then jump to the absolute address in ROM.
 *
 * There is a possibility of updating the ROM in the future chip revisions without any major upgrades,
 * in that case, the same binary should work as is on the new chip revision. For that, we check the _rom_eco_version
 * and if its a newer one, we jump to the new ROM interface. These addresses won't change in the future
 *
 * ets_ecdsa_verify symbol will be present in the upcoming ROM ECO versions so even though we have defined it here,
 * linker will pick the symbol address from rom.ld file
 */
int ets_ecdsa_verify(const uint8_t *key, const uint8_t *sig, ECDSA_CURVE curve_id, const uint8_t *image_digest, uint8_t *verified_digest)
{
    if (_rom_eco_version == 0) {
        _rom_ets_ecdsa_verify = (ROM_FUNC_TYPECAST)0x4001a824;
        return _rom_ets_ecdsa_verify(key, sig, curve_id, image_digest, verified_digest);
    } else {
        _rom_ets_ecdsa_verify = (ROM_FUNC_TYPECAST)0x40001490;
        return _rom_ets_ecdsa_verify(key, sig, curve_id, image_digest, verified_digest);
    }
}
