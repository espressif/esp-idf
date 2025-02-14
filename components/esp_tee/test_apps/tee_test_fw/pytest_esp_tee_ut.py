# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from enum import Enum
from typing import Any
from typing import Dict

import pytest
from pytest_embedded_idf import IdfDut

CONFIGS_DEFAULT = [
    pytest.param('default', marks=[pytest.mark.esp32c6])
]

CONFIGS_OTA = [
    pytest.param('ota', marks=[pytest.mark.esp32c6])
]

CONFIGS_ALL = [
    pytest.param('default', marks=[pytest.mark.esp32c6]),
    pytest.param('ota', marks=[pytest.mark.esp32c6])
]

TEE_VIOLATION_TEST_EXC_RSN: Dict[str, Any] = {
    'esp32c6': {
        ('Reserved', 'W1'): 'Store access fault',
        ('Reserved', 'X1'): 'Instruction access fault',
        ('IRAM', 'W1'): 'Store access fault',
        ('IRAM', 'W2'): 'Store access fault',
        ('DRAM', 'X1'): 'Instruction access fault',
        ('DRAM', 'X2'): 'Instruction access fault'
    }
}

REE_ISOLATION_TEST_EXC_RSN: Dict[str, Any] = {
    'esp32c6': {
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
}

TEE_APM_VIOLATION_EXC_CHK = ['AES', 'eFuse', 'MMU']

TEST_PARTITION_LABEL = 'test'

# ---------------- TEE default tests ----------------


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_DEFAULT, indirect=True)
def test_esp_tee(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='basic')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_ALL, indirect=True)
def test_esp_tee_crypto_aes(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='aes')
    dut.run_all_single_board_cases(group='aes-gcm')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_ALL, indirect=True)
def test_esp_tee_crypto_sha(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='mbedtls')
    dut.run_all_single_board_cases(group='hw_crypto')


# NOTE: Stress testing the AES performance case for interrupt-related edge-cases
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_ALL, indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_DEFAULT, indirect=True)
def test_esp_tee_apm_violation(dut: IdfDut) -> None:
    for check in TEE_APM_VIOLATION_EXC_CHK:
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write(f'"Test APM violation interrupt: {check}"')
        exc = dut.expect(r"Core ([01]) panic\'ed \(([^)]+)\)", timeout=30).group(2).decode()
        if exc != 'Authority exception':
            raise RuntimeError('Incorrect exception received!')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_DEFAULT, indirect=True)
def test_esp_tee_illegal_instruction(dut: IdfDut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(f'"Test TEE-TEE violation: Illegal Instruction"')
    exc = dut.expect(r"Core ([01]) panic\'ed \(([^)]+)\)", timeout=30).group(2).decode()
    if exc != 'Illegal instruction':
        raise RuntimeError('Incorrect exception received!')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_DEFAULT, indirect=True)
def test_esp_tee_violation_checks(dut: IdfDut) -> None:
    checks_list = TEE_VIOLATION_TEST_EXC_RSN[dut.target]
    for test in checks_list:
        memory, access_type = test
        expected_exc = checks_list[test]
        if expected_exc is None:
            continue
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write(f'"Test TEE-TEE violation: {memory} ({access_type})"')
        actual_exc = dut.expect(r"Core ([01]) panic\'ed \(([^)]+)\)", timeout=30).group(2).decode()
        if actual_exc != expected_exc:
            raise RuntimeError('Incorrect exception received!')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_DEFAULT, indirect=True)
def test_esp_tee_isolation_checks(dut: IdfDut) -> None:
    checks_list = REE_ISOLATION_TEST_EXC_RSN[dut.target]
    for test in checks_list:
        memory, access_type = test
        expected_exc = checks_list[test]
        if expected_exc is None:
            continue
        dut.expect_exact('Press ENTER to see the list of tests')
        dut.write(f'"Test REE-TEE isolation: {memory} ({access_type})"')
        actual_exc = dut.expect(r"Core ([01]) panic\'ed \(([^)]+)\)", timeout=30).group(2).decode()
        if actual_exc != expected_exc:
            raise RuntimeError('Incorrect exception received!')
        dut.expect('Exception origin: U-mode')

# ---------------- TEE Flash Protection Tests ----------------


class TeeFlashAccessApi(Enum):
    ESP_PARTITION_MMAP = 1
    SPI_FLASH_MMAP = 2
    ESP_PARTITION = 3
    ESP_FLASH = 4
    ESP_ROM_SPIFLASH = 5


def check_panic_or_reset(dut: IdfDut) -> None:
    try:
        exc = dut.expect(r"Core ([01]) panic\'ed \(([^)]+)\)", timeout=5).group(2).decode()
        if exc not in {'Cache error', 'Authority exception'}:
            raise RuntimeError('Flash operation incorrect exception')
    except Exception:
        rst_rsn = dut.expect(r'rst:(0x[0-9A-Fa-f]+) \(([^)]+)\)', timeout=5).group(2).decode()
        # Fault assert check produces this reset reason
        if rst_rsn != 'LP_SW_HPSYS':
            raise RuntimeError('Flash operation incorrect reset reason')


def run_multiple_stages(dut: IdfDut, test_case_num: int, stages: int, api: TeeFlashAccessApi) -> None:
    exp_seq = {
        TeeFlashAccessApi.ESP_PARTITION: ['read', 'program_page', 'program_page', 'erase_sector'],
        TeeFlashAccessApi.ESP_FLASH: ['program_page', 'read', 'erase_sector', 'program_page']
    }

    for stage in range(1, stages + 1):
        dut.write(str(test_case_num))
        dut.expect(r'\s+\((\d+)\)\s+"([^"]+)"\r?\n', timeout=30)
        dut.write(str(stage))

        if 1 < stage <= stages:
            if api in exp_seq:
                try:
                    match = dut.expect(r'\[_ss_spi_flash_hal_(\w+)\] Illegal flash access at \s*(0x[0-9a-fA-F]+)', timeout=5)
                    fault_api = match.group(1).decode()
                    if fault_api != exp_seq[api][stage - 2]:
                        raise RuntimeError('Flash operation address check failed')
                except Exception:
                    # NOTE: The esp_partition_read API handles both decrypted
                    # and plaintext reads. When flash encryption is enabled,
                    # it uses the MMU HAL instead of the SPI flash HAL.
                    exc = dut.expect(r"Core ([01]) panic\'ed \(([^)]+)\)", timeout=5).group(2).decode()
                    if exc != 'Cache error':
                        raise RuntimeError('Flash operation incorrect exception')
            else:
                check_panic_or_reset(dut)

        if stage != stages:
            dut.expect_exact('Press ENTER to see the list of tests.')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
def test_esp_tee_ota_negative(dut: IdfDut) -> None:
    # start test
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[ota_neg_1]')
    dut.expect_unity_test_output(timeout=120)

    # erasing TEE otadata
    dut.serial.erase_partition('tee_otadata')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
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


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_esp_tee_secure_storage(dut: IdfDut) -> None:
    # Flash image and erase the secure_storage partition
    dut.serial.custom_flash_with_empty_sec_stg()

    dut.run_all_single_board_cases(group='secure_storage')

# ---------------- TEE Attestation tests ----------------


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS_OTA, indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_esp_tee_attestation(dut: IdfDut) -> None:
    # Flash image and erase the secure_storage partition
    dut.serial.custom_flash_with_empty_sec_stg()

    # start test
    dut.run_all_single_board_cases(group='attestation')
