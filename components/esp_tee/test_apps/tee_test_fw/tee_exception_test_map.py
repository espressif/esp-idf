# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from typing import Any

# Base configuration - shared by all targets
_BASE_CONFIG = {
    'tee_violation': {
        'Reserved-W1': 'Store access fault',
        'Reserved-X1': 'Instruction access fault',
        'IRAM-W1': 'Store access fault',
        'IRAM-W2': 'Store access fault',
        'DRAM-X1': 'Instruction access fault',
        'DRAM-X2': 'Instruction access fault',
        'Illegal Instruction': 'Illegal instruction',
    },
    'ree_isolation': {
        'DRAM-R1': 'Load access fault',
        'DRAM-W1': 'Store access fault',
        'IRAM-R1': 'Load access fault',
        'IRAM-W1': 'Store access fault',
        'IROM-R1': 'Load access fault',
        'IROM-W1': 'Store access fault',
        'DROM-R1': 'Load access fault',
        'DROM-W1': 'Store access fault',
        'MMU-spillover': 'Illegal instruction',
    },
    'apm_violation': {
        'eFuse': 'APM - Space exception',
        'MMU': 'APM - Space exception',
        'SWDT/BOD': 'APM - Space exception',
        'AES': 'APM - Space exception',
        'HMAC': 'APM - Space exception',
        'DS': 'APM - Space exception',
        'SHA PCR': 'APM - Space exception',
        'ECC PCR': 'APM - Space exception',
    },
}

# Per-target overrides only
_TARGET_OVERRIDES: dict[str, dict[str, Any]] = {
    'esp32c6': {
        'ree_isolation': {
            # NOTE: For ESP32-C6, the MMU-spillover test triggers a Cache error
            # panic as invalid MMU m'mapings are not handled gracefully
            'MMU-spillover': 'Cache error',
        },
        # NOTE: For ESP32-C6, illegal APM accesses result in Authority exceptions
        # due to the fix required for granting unrestricted access in TEE mode
        'apm_violation': {
            'eFuse': 'APM - Authority exception',
            'MMU': 'APM - Authority exception',
            # NOTE: For ESP32-C6, the SWDT/BOD peripherals are protected using PMP
            # due to lack of sufficient APM entries
            'SWDT/BOD': 'Store access fault',
            'AES': 'APM - Authority exception',
            'HMAC': 'APM - Authority exception',
            'DS': 'APM - Authority exception',
            'SHA PCR': 'APM - Authority exception',
            'ECC PCR': 'APM - Authority exception',
        },
    },
    'esp32h2': {
        'ree_isolation': {
            # NOTE: For ESP32-H2, the MMU-spillover test triggers a Cache error
            # panic as invalid MMU m'mapings are not handled gracefully
            'MMU-spillover': 'Cache error',
        },
        # NOTE: For ESP32-H2, illegal APM accesses result in Authority exceptions
        # due to the fix required for granting unrestricted access in TEE mode
        'apm_violation': {
            'MMU': 'APM - Authority exception',
            'SWDT/BOD': 'Store access fault',
            'AES': 'APM - Authority exception',
            'HMAC': 'APM - Authority exception',
            'DS': 'APM - Authority exception',
            'SHA PCR': 'APM - Authority exception',
            'ECC PCR': 'APM - Authority exception',
        },
    },
    'esp32c5': {
        'tee_violation': {
            # NOTE: For ESP32-C5, access to the region before the SRAM does
            # not generate exceptions due to TEE PMA configuration
            '_remove': ['IRAM-W1'],
        },
    },
    'esp32c61': {
        # NOTE: ESP32-C61 does not support the following peripherals
        'apm_violation': {
            '_remove': ['AES', 'HMAC', 'DS'],
        },
    },
}


def _build_config(base: dict[str, Any], overrides: dict[str, Any]) -> dict[str, Any]:
    import copy

    result = copy.deepcopy(base)
    for category, changes in overrides.items():
        if category not in result:
            result[category] = {}
        # Handle removals
        if '_remove' in changes:
            for key in changes['_remove']:
                result[category].pop(key, None)
            changes = {k: v for k, v in changes.items() if k != '_remove'}
        result[category].update(changes)
    return result


# Build final map
TEE_EXCEPTION_TEST_MAP = {
    target: _build_config(_BASE_CONFIG, _TARGET_OVERRIDES.get(target, {}))
    for target in ['esp32c6', 'esp32h2', 'esp32c5', 'esp32c61']
}
