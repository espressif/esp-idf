# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
#
# Per-runner pytest entry points for the persistent-storage format
# stability tests. Each runner flashes the SAME shared NVS fixture
# (fixtures/nvs_efuse_v1.bin) — pre-populated with three persistent
# eFuse keys, one per driver — and runs the consume tests for whichever
# drivers it has hardware for. The capture-side test is tagged
# [fixture_capture] (NOT [persistent_format]) so it is never picked up
# by the normal CI groups.
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


# nvs_encr_hmac runner (esp32c3) covers BOTH HMAC and RSA-DS:
#   - real eFuse HMAC key burned in block 0 with HMAC_UP purpose →
#     genuine psa_mac_compute roundtrip
#   - DS peripheral on c3, DS HW wrapped → RSA-DS consume runs without
#     needing a real HMAC-DOWN-DIGITAL-SIGNATURE eFuse key
# A single runner exercises both drivers' persistent extract paths.
@pytest.mark.nvs_encr_hmac
@pytest.mark.parametrize('config', ['hmac'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_persistent_storage_format_hmac_and_rsa_ds(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='persistent_format')


# ECDSA runner (esp32h2) — has a SECP256R1 ECDSA key burned in
# EFUSE_BLK_KEY1 with ECDSA_KEY purpose (matches mbedtls_ut's existing
# ecdsa_sign volatile tests).
@pytest.mark.ecdsa_efuse
@pytest.mark.parametrize('config', ['ecdsa'], indirect=True)
@idf_parametrize('target', ['esp32h2'], indirect=['target'])
def test_persistent_storage_format_ecdsa(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='ecdsa_efuse_key')
