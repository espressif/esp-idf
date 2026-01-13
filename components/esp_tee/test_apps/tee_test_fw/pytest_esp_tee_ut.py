# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import re
from enum import Enum

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from tee_exception_cfg import TEE_EXCEPTION_TEST_MAP

# ---------------- Pytest build parameters ----------------

TESTING_TARGETS = ['esp32c6', 'esp32h2', 'esp32c5', 'esp32c61']

CONFIG_DEFAULT = [
    # 'config, target, markers',
    ('tee_default', target, (pytest.mark.generic,))
    for target in TESTING_TARGETS
]

CONFIG_OTA = [
    # 'config, target, skip_autoflash, markers',
    ('tee_ota', target, 'y', (pytest.mark.generic,))
    for target in TESTING_TARGETS
]

CONFIG_ALL = [
    # 'config, target, markers',
    (config, target, (pytest.mark.generic,))
    for config in ['tee_default', 'tee_ota']
    for target in TESTING_TARGETS
]

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
@pytest.mark.skipif(targets=['esp32c61'], reason='Not supported')
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
@pytest.mark.skipif(targets=['esp32c61'], reason='Not supported')
def test_esp_tee_aes_perf(dut: IdfDut) -> None:
    for i in range(10):
        dut.run_all_single_board_cases(name=['mbedtls AES performance'])


# ---------------- TEE Exceptions generation Tests ----------------


def run_exception_case(
    dut: IdfDut, menu_prefix: str, test_name: str, expected: str, check_origin: bool = False
) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write(f'"{menu_prefix}: {test_name}"')

    actual = dut.expect(r"Core ([01]) panic'ed \(([^)]+)\)", timeout=10).group(2).decode()
    if actual != expected:
        raise RuntimeError(f"{menu_prefix} / {test_name}: expected '{expected}', got '{actual}'")

    if check_origin:
        dut.expect('Origin: U-mode')


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_violation_checks(dut: IdfDut) -> None:
    cfg = TEE_EXCEPTION_TEST_MAP[dut.target]['tee_violation']

    for test_name, expected in cfg.items():
        run_exception_case(dut, 'Test TEE-TEE violation', test_name, expected)


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_isolation_checks(dut: IdfDut) -> None:
    cfg = TEE_EXCEPTION_TEST_MAP[dut.target]['ree_isolation']

    for test_name, expected in cfg.items():
        run_exception_case(dut, 'Test REE-TEE isolation', test_name, expected, check_origin=True)


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_apm_violation(dut: IdfDut) -> None:
    cfg = TEE_EXCEPTION_TEST_MAP[dut.target]['apm_violation']

    for test_name, expected in cfg.items():
        run_exception_case(dut, 'Test APM violation', test_name, expected)


@idf_parametrize(
    'config, target, markers',
    CONFIG_DEFAULT,
    indirect=['config', 'target'],
)
def test_esp_tee_stack_smashing(dut: IdfDut) -> None:
    for env in ('REE', 'TEE'):
        for case in ('overflow', 'underflow'):
            dut.expect_exact('Press ENTER to see the list of tests')
            dut.write(f'"Test {env} stack {case}"')

            match = dut.expect(r"Core ([01]) panic'ed \(([^)]+)\)", timeout=10)
            exc = match.group(2).decode()
            if exc != 'Stack protection fault':
                raise RuntimeError('Incorrect exception received!')


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
    # Constants
    SOC_TEE_FLASH_OP_FAIL_FAULT = ('esp32c6', 'esp32h2')
    INVALID_MMU_ACCESS_ERR = re.compile(r'\[_ss_mmu_hal_map_region] Illegal flash access at\s+\S+\s*\|\s*\S+')
    INVALID_SPI1_ACCESS_ERR = re.compile(r'\[_ss_spi_flash_hal_(\w+)\] Illegal flash access at \s*(0x[0-9a-fA-F]+)')
    EXPECTED_OPS_MAP = {
        TeeFlashAccessApi.ESP_PARTITION: (
            'read',
            'program_page|common_command',
            'program_page|common_command',
            'erase_sector|common_command',
        ),
        TeeFlashAccessApi.ESP_FLASH: (
            'program_page|common_command',
            'read',
            'erase_sector|common_command',
            'program_page|common_command',
        ),
    }

    flash_enc_enabled = dut.app.sdkconfig.get('SECURE_FLASH_ENC_ENABLED', True)
    target_mmu_fault = dut.target in SOC_TEE_FLASH_OP_FAIL_FAULT

    expected_ops = EXPECTED_OPS_MAP.get(api)

    for stage in range(1, stages + 1):
        dut.write(str(test_case_num))
        dut.expect(r'\s+\((\d+)\)\s+"([^"]+)"\r?\n', timeout=10)
        dut.write(str(stage))

        if stage > 1:
            if api in (TeeFlashAccessApi.ESP_PARTITION_MMAP, TeeFlashAccessApi.SPI_FLASH_MMAP):
                if target_mmu_fault:
                    expect_panic_rsn(dut, 'Cache error')
                else:
                    dut.expect(INVALID_MMU_ACCESS_ERR, timeout=10)
            elif api == TeeFlashAccessApi.ESP_ROM_SPIFLASH:
                # NOTE: ROM flash APIs are not supported with ESP-TEE,
                # thus using them results in APM violations
                expected_rsn = 'APM - Authority exception' if target_mmu_fault else 'APM - Space exception'
                expect_panic_rsn(dut, expected_rsn)
            elif api in (TeeFlashAccessApi.ESP_PARTITION, TeeFlashAccessApi.ESP_FLASH):
                assert expected_ops is not None
                op_index = stage - 2
                curr_op = expected_ops[op_index]
                # NOTE: The esp_partition_read API handles both decrypted
                # and plaintext reads. When flash encryption is enabled,
                # it uses the MMU HAL instead of the SPI flash HAL.
                if flash_enc_enabled and api == TeeFlashAccessApi.ESP_PARTITION and curr_op == 'read':
                    if target_mmu_fault:
                        expect_panic_rsn(dut, 'Cache error')
                    else:
                        dut.expect(INVALID_MMU_ACCESS_ERR, timeout=10)
                else:
                    match = dut.expect(INVALID_SPI1_ACCESS_ERR, timeout=10)
                    actual_op = match.group(1).decode()
                    if not re.fullmatch(curr_op, actual_op):
                        raise RuntimeError(f'Unexpected flash operation: {actual_op} (expected: {curr_op})')

        if stage != stages:
            dut.expect_exact('Press ENTER to see the list of tests.')


def run_flash_access_test(dut: IdfDut, api: TeeFlashAccessApi, test_name: str) -> None:
    dut.serial.custom_flash()

    extra_data = dut._parse_test_menu()
    test_case = next((tc for tc in extra_data if tc.name == test_name), None)

    if not test_case:
        raise RuntimeError(f"Test case '{test_name}' not found")

    run_multiple_stages(dut, test_case.index, len(test_case.subcases), api)


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_partition_mmap(dut: IdfDut) -> None:
    run_flash_access_test(
        dut, TeeFlashAccessApi.ESP_PARTITION_MMAP, 'Test REE-TEE isolation: Flash - SPI0 (esp_partition_mmap)'
    )


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_spi_flash_mmap(dut: IdfDut) -> None:
    run_flash_access_test(
        dut, TeeFlashAccessApi.SPI_FLASH_MMAP, 'Test REE-TEE isolation: Flash - SPI0 (spi_flash_mmap)'
    )


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_rom_spiflash(dut: IdfDut) -> None:
    run_flash_access_test(
        dut, TeeFlashAccessApi.ESP_ROM_SPIFLASH, 'Test REE-TEE isolation: Flash - SPI1 (esp_rom_spiflash)'
    )


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_partition(dut: IdfDut) -> None:
    run_flash_access_test(dut, TeeFlashAccessApi.ESP_PARTITION, 'Test REE-TEE isolation: Flash - SPI1 (esp_partition)')


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_flash_prot_esp_flash(dut: IdfDut) -> None:
    run_flash_access_test(dut, TeeFlashAccessApi.ESP_FLASH, 'Test REE-TEE isolation: Flash - SPI1 (esp_flash)')


# ---------------- TEE Local OTA tests ----------------


@pytest.mark.generic
@idf_parametrize('config', ['tee_ota'], indirect=['config'])
@idf_parametrize('target', TESTING_TARGETS, indirect=['target'])
def test_esp_tee_ota_negative(dut: IdfDut) -> None:
    # start test
    dut.run_all_single_board_cases(group='ota_neg_1', timeout=10)


@idf_parametrize(
    'config, target, skip_autoflash, markers',
    CONFIG_OTA,
    indirect=['config', 'target', 'skip_autoflash'],
)
def test_esp_tee_ota_corrupted_img(dut: IdfDut) -> None:
    # Flashing the TEE app to the non-secure app's passive partition
    dut.serial.custom_flash_w_test_tee_img_gen()

    # start test
    dut.run_all_single_board_cases(name=['Test TEE OTA - Corrupted image'], timeout=10)


class TeeOtaStage(Enum):
    PRE = 1
    BEGIN = 2
    REBOOT = 3


def tee_ota_stage_checks(dut: IdfDut, stage: TeeOtaStage, offset: str) -> None:
    if stage == TeeOtaStage.PRE:
        dut.expect(f'Loaded TEE app from partition at offset {offset}', timeout=10)
        dut.expect('Current image already has been marked VALID', timeout=10)
    elif stage == TeeOtaStage.BEGIN:
        dut.expect('Starting TEE OTA...', timeout=10)
        dut.expect('Running partition - Subtype: 0x30', timeout=10)
        dut.expect_exact(f'Next partition - Subtype: 0x31 (Offset: {offset})', timeout=10)
    elif stage == TeeOtaStage.REBOOT:
        dut.expect(f'Loaded TEE app from partition at offset {offset}', timeout=10)
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
    dut.write('"Test TEE OTA - Valid image"')

    # OTA begin checks
    tee_ota_stage_checks(dut, TeeOtaStage.BEGIN, '0x40000')
    dut.expect('TEE OTA update successful!', timeout=10)

    # after reboot 1
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x40000')

    # resetting device to check for image validity
    dut.serial.hard_reset()

    # after reboot 2
    dut.expect('TEE otadata - Current image state: VALID', timeout=10)
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x40000')


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
    dut.write('"Test TEE OTA - Rollback"')

    # OTA begin checks
    tee_ota_stage_checks(dut, TeeOtaStage.BEGIN, '0x40000')
    dut.expect('TEE OTA update successful!', timeout=10)

    # after reboot 1
    dut.expect('TEE otadata - Current image state: NEW', timeout=10)
    dut.expect('Loaded TEE app from partition at offset 0x40000', timeout=10)
    rst_rsn = dut.expect(r'rst:(0x[0-9A-Fa-f]+) \(([^)]+)\)', timeout=10).group(2).decode()
    # NOTE: LP_WDT_SYS (C6/H2) and RTC_WDT_SYS (C5) are expected as bootloader fails to load the dummy TEE app
    if rst_rsn not in {'LP_WDT_SYS', 'RTC_WDT_SYS'}:
        raise RuntimeError('Incorrect reset reason observed after TEE image failure!')

    # after rollback
    dut.expect('TEE otadata - Current image state: PENDING_VERIFY', timeout=10)
    tee_ota_stage_checks(dut, TeeOtaStage.REBOOT, '0x10000')


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
