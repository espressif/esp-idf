# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import re
from enum import Enum
from typing import Dict
from typing import Tuple

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

# ---------------- Pytest build parameters ----------------

SUPPORTED_TARGETS = ['esp32c6', 'esp32h2']

CONFIG_DEFAULT = [
    # 'config, target, markers',
    ('default', target, (pytest.mark.generic,))
    for target in SUPPORTED_TARGETS
]

CONFIG_OTA = [
    # 'config, target, skip_autoflash, markers',
    ('ota', target, 'y', (pytest.mark.host_test,))
    for target in SUPPORTED_TARGETS
]

CONFIG_ALL = [
    # 'config, target, markers',
    (config, target, (pytest.mark.generic,))
    for config in ['default', 'ota']
    for target in SUPPORTED_TARGETS
]

# ---------------- Exception test-cases reasons  ----------------

TEE_VIOLATION_TEST_EXC_RSN: Dict[Tuple[str, str], str] = {
    ('Reserved', 'W1'): 'Store access fault',
    ('Reserved', 'X1'): 'Instruction access fault',
    ('IRAM', 'W1'): 'Store access fault',
    ('IRAM', 'W2'): 'Store access fault',
    ('DRAM', 'X1'): 'Instruction access fault',
    ('DRAM', 'X2'): 'Instruction access fault',
}

REE_ISOLATION_TEST_EXC_RSN: Dict[Tuple[str, str], str] = {
    ('DRAM', 'R1'): 'Load access fault',
    ('DRAM', 'W1'): 'Store access fault',
    ('IRAM', 'R1'): 'Load access fault',
    ('IRAM', 'W1'): 'Store access fault',
    ('IROM', 'R1'): 'Load access fault',
    ('IROM', 'W1'): 'Store access fault',
    ('DROM', 'R1'): 'Load access fault',
    ('DROM', 'W1'): 'Store access fault',
    ('SWDT/BOD', 'W'): 'Store access fault',
}

TEE_APM_VIOLATION_EXC_CHK = ['eFuse', 'MMU', 'AES', 'HMAC', 'DS', 'SHA PCR', 'ECC PCR']

# ---------------- TEE default tests ----------------


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='basic')
    dut.run_all_single_board_cases(group='heap')


@idf_parametrize(
    'config, target, markers',
    CONFIG_ALL,
    indirect=['config', 'target'],
)
def test_esp_tee_crypto_aes(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='aes')
    dut.run_all_single_board_cases(group='aes-gcm')


@idf_parametrize(
    'config, target, markers',
    CONFIG_ALL,
    indirect=['config', 'target'],
)
def test_esp_tee_crypto_sha(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='mbedtls')
    dut.run_all_single_board_cases(group='hw_crypto')


# NOTE: Stress testing the AES performance case for interrupt-related edge-cases
@idf_parametrize(
    'config, target, markers',
    CONFIG_ALL,
    indirect=['config', 'target'],
)
def test_esp_tee_aes_perf(dut: IdfDut) -> None:
    # start test
    for i in range(24):
        if not i:
            dut.expect_exact('Press ENTER to see the list of tests')
        else:
            dut.expect_exact("Enter next test, or 'enter' to see menu")
        dut.write('"mbedtls AES performance"')
        dut.expect_unity_test_output(timeout=60)


# ---------------- TEE Exceptions generation Tests ----------------


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_apm_violation(dut: IdfDut) -> None:
    for check in TEE_APM_VIOLATION_EXC_CHK:
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write(f'"Test APM violation interrupt: {check}"')
        exc = dut.expect(r'Core ([01]) panic\'ed \(([^)]+)\)', timeout=30).group(2).decode()
        if dut.target == 'esp32h2' and check == 'eFuse':
            exp_str = 'APM - Space exception'
        else:
            exp_str = 'APM - Authority exception'
        if exc != exp_str:
            raise RuntimeError('Incorrect exception received!')


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_illegal_instruction(dut: IdfDut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"Test TEE-TEE violation: Illegal Instruction"')
    exc = dut.expect(r'Core ([01]) panic\'ed \(([^)]+)\)', timeout=30).group(2).decode()
    if exc != 'Illegal instruction':
        raise RuntimeError('Incorrect exception received!')


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_violation_checks(dut: IdfDut) -> None:
    checks_list = TEE_VIOLATION_TEST_EXC_RSN
    for test in checks_list:
        memory, access_type = test
        expected_exc = checks_list[test]
        if expected_exc is None:
            continue
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write(f'"Test TEE-TEE violation: {memory} ({access_type})"')
        actual_exc = dut.expect(r'Core ([01]) panic\'ed \(([^)]+)\)', timeout=30).group(2).decode()
        if actual_exc != expected_exc:
            raise RuntimeError('Incorrect exception received!')


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_isolation_checks(dut: IdfDut) -> None:
    checks_list = REE_ISOLATION_TEST_EXC_RSN
    for test in checks_list:
        memory, access_type = test
        expected_exc = checks_list[test]
        if expected_exc is None:
            continue
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write(f'"Test REE-TEE isolation: {memory} ({access_type})"')
        actual_exc = dut.expect(r'Core ([01]) panic\'ed \(([^)]+)\)', timeout=30).group(2).decode()
        if actual_exc != expected_exc:
            raise RuntimeError('Incorrect exception received!')
        dut.expect('Origin: U-mode')


# ---------------- TEE Flash Protection Tests ----------------


class TeeFlashAccessApi(Enum):
    ESP_PARTITION_MMAP = 1
    SPI_FLASH_MMAP = 2
    ESP_PARTITION = 3
    ESP_FLASH = 4
    ESP_ROM_SPIFLASH = 5


def expect_panic_rsn(dut: IdfDut, expected_rsn: str) -> None:
    actual_rsn = dut.expect(r'Core ([01]) panic\'ed \(([^)]+)\)', timeout=10).group(2).decode()
    if actual_rsn != expected_rsn:
        raise RuntimeError(f'Incorrect exception: {actual_rsn} (expected: {expected_rsn})')


def run_multiple_stages(dut: IdfDut, test_case_num: int, stages: int, api: TeeFlashAccessApi) -> None:
    expected_ops = {
        TeeFlashAccessApi.ESP_PARTITION: [
            'read',
            'program_page|common_command',
            'program_page|common_command',
            'erase_sector|common_command',
        ],
        TeeFlashAccessApi.ESP_FLASH: [
            'program_page|common_command',
            'read',
            'erase_sector|common_command',
            'program_page|common_command',
        ],
    }

    flash_enc_enabled = dut.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED', True)

    for stage in range(1, stages + 1):
        dut.write(str(test_case_num))
        dut.expect(r'\s+\((\d+)\)\s+"([^"]+)"\r?\n', timeout=30)
        dut.write(str(stage))

        if stage > 1:
            if api in {TeeFlashAccessApi.ESP_PARTITION_MMAP, TeeFlashAccessApi.SPI_FLASH_MMAP}:
                expect_panic_rsn(dut, 'Cache error')
            elif api in {TeeFlashAccessApi.ESP_PARTITION, TeeFlashAccessApi.ESP_FLASH}:
                op_index = stage - 2
                curr_op = expected_ops[api][op_index]
                if api == TeeFlashAccessApi.ESP_PARTITION and curr_op == 'read' and flash_enc_enabled:
                    # NOTE: The esp_partition_read API handles both decrypted
                    # and plaintext reads. When flash encryption is enabled,
                    # it uses the MMU HAL instead of the SPI flash HAL.
                    expect_panic_rsn(dut, 'Cache error')
                else:
                    match = dut.expect(
                        r'\[_ss_spi_flash_hal_(\w+)\] Illegal flash access at \s*(0x[0-9a-fA-F]+)', timeout=10
                    )
                    actual_op = match.group(1).decode()
                    if not re.fullmatch(curr_op, actual_op):
                        raise RuntimeError(f'Unexpected flash operation: {actual_op} (expected: {curr_op})')
            elif api == TeeFlashAccessApi.ESP_ROM_SPIFLASH:
                expect_panic_rsn(dut, 'APM - Authority exception')

        if stage != stages:
            dut.expect_exact('Press ENTER to see the list of tests.')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_partition_mmap(dut: IdfDut) -> None:
    # Flash the bootloader, TEE and REE firmware
    dut.serial.custom_flash()

    # start test
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.name == 'Test REE-TEE isolation: Flash - SPI0 (esp_partition_mmap)':
            run_multiple_stages(dut, test_case.index, len(test_case.subcases), TeeFlashAccessApi.ESP_PARTITION_MMAP)
        else:
            continue


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_spi_flash_mmap(dut: IdfDut) -> None:
    # Flash the bootloader, TEE and REE firmware
    dut.serial.custom_flash()

    # start test
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.name == 'Test REE-TEE isolation: Flash - SPI0 (spi_flash_mmap)':
            run_multiple_stages(dut, test_case.index, len(test_case.subcases), TeeFlashAccessApi.SPI_FLASH_MMAP)
        else:
            continue


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_rom_spiflash(dut: IdfDut) -> None:
    # Flash the bootloader, TEE and REE firmware
    dut.serial.custom_flash()

    # start test
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.name == 'Test REE-TEE isolation: Flash - SPI1 (esp_rom_spiflash)':
            run_multiple_stages(dut, test_case.index, len(test_case.subcases), TeeFlashAccessApi.ESP_ROM_SPIFLASH)
        else:
            continue


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_partition(dut: IdfDut) -> None:
    # Flash the bootloader, TEE and REE firmware
    dut.serial.custom_flash()

    # start test
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.name == 'Test REE-TEE isolation: Flash - SPI1 (esp_partition)':
            run_multiple_stages(dut, test_case.index, len(test_case.subcases), TeeFlashAccessApi.ESP_PARTITION)
        else:
            continue


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_flash(dut: IdfDut) -> None:
    # Flash the bootloader, TEE and REE firmware
    dut.serial.custom_flash()

    # start test
    extra_data = dut.parse_test_menu()
    for test_case in extra_data:
        if test_case.name == 'Test REE-TEE isolation: Flash - SPI1 (esp_flash)':
            run_multiple_stages(dut, test_case.index, len(test_case.subcases), TeeFlashAccessApi.ESP_FLASH)
        else:
            continue


# ---------------- TEE Local OTA tests ----------------


@pytest.mark.generic
@idf_parametrize('config', ['ota'], indirect=['config'])
@idf_parametrize('target', ['esp32c6', 'esp32h2'], indirect=['target'])
def test_esp_tee_ota_negative(dut: IdfDut) -> None:
    # start test
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[ota_neg_1]')
    dut.expect_unity_test_output(timeout=120)

    # erasing TEE otadata
    dut.serial.erase_partition('tee_otadata')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_ota_corrupted_img(dut: IdfDut) -> None:
    # Flashing the TEE app to the non-secure app's passive partition
    dut.serial.custom_flash_w_test_tee_img_gen()

    # start test
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"Test TEE OTA - Corrupted image"')
    dut.expect_unity_test_output(timeout=120)

    # erasing TEE otadata
    dut.serial.erase_partition('tee_otadata')


class TeeOtaStage(Enum):
    PRE = 1
    BEGIN = 2
    REBOOT = 3


def tee_ota_stage_checks(dut: IdfDut, stage: TeeOtaStage, offset: str) -> None:
    if stage == TeeOtaStage.PRE:
        dut.expect(f'Loaded TEE app from partition at offset {offset}', timeout=30)
        dut.expect('Current image already has been marked VALID', timeout=30)
    elif stage == TeeOtaStage.BEGIN:
        dut.expect('Starting TEE OTA...', timeout=30)
        dut.expect('Running partition - Subtype: 0x30', timeout=30)
        dut.expect_exact(f'Next partition - Subtype: 0x31 (Offset: {offset})', timeout=30)
    elif stage == TeeOtaStage.REBOOT:
        dut.expect(f'Loaded TEE app from partition at offset {offset}', timeout=30)
        dut.expect_exact('Press ENTER to see the list of tests')
    else:
        raise ValueError('Undefined stage!')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_ota_reboot_without_ota_end(dut: IdfDut) -> None:
    # Flashing the TEE app to the non-secure app's passive partition
    dut.serial.custom_flash_w_test_tee_img_gen()

    # pre-test checks
    tee_ota_stage_checks(dut, TeeOtaStage.PRE, '0x10000')

    # start test
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"Test TEE OTA - Reboot without ending OTA"')

    # OTA begin checks
    tee_ota_stage_checks(dut, TeeOtaStage.BEGIN, '0x40000')

    # after reboot
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x10000')

    # erasing TEE otadata
    dut.serial.erase_partition('tee_otadata')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_ota_valid_img(dut: IdfDut) -> None:
    # Flashing the TEE app to the non-secure app's passive partition
    dut.serial.custom_flash_w_test_tee_img_gen()

    # pre-test checks
    tee_ota_stage_checks(dut, TeeOtaStage.PRE, '0x10000')

    # start test
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[ota_valid_img]')

    # OTA begin checks
    tee_ota_stage_checks(dut, TeeOtaStage.BEGIN, '0x40000')
    dut.expect('TEE OTA update successful!', timeout=30)

    # after reboot 1
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x40000')

    # resetting device to check for image validity
    dut.serial.hard_reset()

    # after reboot 2
    dut.expect('TEE otadata - Current image state: VALID', timeout=30)
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x40000')

    # erasing TEE otadata
    dut.serial.erase_partition('tee_otadata')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_ota_rollback(dut: IdfDut) -> None:
    # Flashing the TEE app to the non-secure app's passive partition
    dut.serial.custom_flash_w_test_tee_img_rb()

    # pre-test checks
    tee_ota_stage_checks(dut, TeeOtaStage.PRE, '0x10000')

    # start test
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[ota_rollback]')

    # OTA begin checks
    tee_ota_stage_checks(dut, TeeOtaStage.BEGIN, '0x40000')
    dut.expect('TEE OTA update successful!', timeout=30)

    # after reboot 1
    dut.expect('TEE otadata - Current image state: NEW', timeout=10)
    dut.expect('Loaded TEE app from partition at offset 0x40000', timeout=10)
    rst_rsn = dut.expect(r'rst:(0x[0-9A-Fa-f]+) \(([^)]+)\)', timeout=30).group(2).decode()
    # NOTE: LP_WDT_SYS is for ESP32-C6 case as bootloader fails to load the dummy TEE app
    if rst_rsn != 'LP_WDT_SYS':
        raise RuntimeError('Incorrect reset reason observed after TEE image failure!')

    # after rollback
    dut.expect('TEE otadata - Current image state: PENDING_VERIFY', timeout=30)
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x10000')

    # erasing TEE otadata
    dut.serial.erase_partition('tee_otadata')


# ---------------- TEE Secure Storage tests ----------------


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_secure_storage(dut: IdfDut) -> None:
    # Flash image and erase the secure_storage partition
    dut.serial.custom_flash_with_empty_sec_stg()

    dut.run_all_single_board_cases(group='sec_storage')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_secure_storage_with_host_img(dut: IdfDut) -> None:
    # Flash image and write the secure_storage partition with host-generated keys
    dut.serial.custom_flash_with_host_gen_sec_stg_img()

    dut.run_all_single_board_cases(group='sec_storage_host_keygen')


# ---------------- TEE Attestation tests ----------------


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_attestation(dut: IdfDut) -> None:
    # Flash image and erase the secure_storage partition
    dut.serial.custom_flash_with_empty_sec_stg()

    # start test
    dut.run_all_single_board_cases(group='attestation')
