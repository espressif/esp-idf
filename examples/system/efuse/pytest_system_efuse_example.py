# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import unicode_literals

import logging
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_qemu.dut import QemuDut


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
def test_examples_efuse(dut: Dut) -> None:
    dut.expect(r'example: Coding Scheme (3/4)|(NONE)|(REPEAT)|(RS \(Reed-Solomon coding\))', timeout=20)
    dut.expect(['example: read efuse fields',
                r'example: 1. read MAC address: {}'.format(r':'.join((r'[0-9a-f]{2}',) * 6)),
                'example: 2. read secure_version: 0',
                'example: 3. read custom fields',
                'example: module_version = 0',
                'example: device_role = None',
                'example: setting_1 = 0',
                'example: setting_2 = 0',
                'example: custom_secure_version = 0',
                'example: This example does not burn any efuse in reality only virtually',
                'example: Write operations in efuse fields are performed virtually',
                'example: write custom efuse fields',
                'efuse: Virtual efuses enabled: Not really burning eFuses',
                'example: module_version = 1',
                'example: device_role = Slave',
                'example: setting_1 = 3',
                'example: setting_2 = 4',
                'example: custom_secure_version = 5',
                'example: Done'], expect_all=True)


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.parametrize('config', ['virt_flash_enc',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_flash_enc(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_flash_enc_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')
    if dut.app.target == 'esp32':
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 2')
        dut.expect('Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('Not disabling UART bootloader encryption')
        dut.expect('Disable UART bootloader decryption...')
        dut.expect('Disable UART bootloader MMU cache...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
    else:
        if dut.app.target == 'esp32c2':
            dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 1')
        else:
            dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 4')
        dut.expect('Not disabling UART bootloader encryption')
        if dut.app.target != 'esp32h2':
            dut.expect('Disable UART bootloader cache...')
        dut.expect('Disable JTAG...')
    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    if dut.app.target == 'esp32':
        dut.expect_exact('flash encryption is enabled (3 plaintext flashes left)', timeout=3)
    else:
        dut.expect_exact('flash encryption is enabled (1 plaintext flashes left)')

    dut.expect_exact('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.generic
@pytest.mark.esp32s2
@pytest.mark.parametrize('config', ['virt_flash_enc_aes_256',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_flash_enc_aes_256(dut: Dut) -> None:
    # Only ESP32-S2 has support AES-256 FLASH_ENCRYPTION key
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_flash_enc_aes_256_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')

    dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 2')
    dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 3')
    dut.expect('Not disabling UART bootloader encryption')
    if dut.app.target != 'esp32h2':
        dut.expect('Disable UART bootloader cache...')
    dut.expect('Disable JTAG...')

    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    dut.expect_exact('flash encryption is enabled (1 plaintext flashes left)')

    dut.expect_exact('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.parametrize('config', ['virt_flash_enc',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_flash_enc_pre_loaded(dut: Dut) -> None:

    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect_exact('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    if dut.app.target == 'esp32':
        print(' - Flash emul_efuse with pre-loaded efuses (FLASH_CRYPT_CNT 1 -> 0)')
        # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
        FLASH_CRYPT_CNT = 20
        # Resets eFuse, which enables Flash encryption feature
        dut.serial.erase_field_on_emul_efuse([FLASH_CRYPT_CNT])
    elif dut.app.target == 'esp32c2':
        FLASH_CRYPT_CNT = 39
        dut.serial.erase_field_on_emul_efuse([FLASH_CRYPT_CNT])
    else:
        # offset of this eFuse is taken from components/efuse/{target}/esp_efuse_table.csv
        print(' - Flash emul_efuse with pre-loaded efuses (SPI_BOOT_CRYPT_CNT 1 -> 0)')
        SPI_BOOT_CRYPT_CNT = 82
        # Resets eFuse, which enables Flash encryption feature
        dut.serial.erase_field_on_emul_efuse([SPI_BOOT_CRYPT_CNT])

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    dut.expect('Using pre-loaded flash encryption key in efuse')
    if dut.app.target == 'esp32':
        dut.expect('Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('Not disabling UART bootloader encryption')
        dut.expect('Disable UART bootloader decryption...')
        dut.expect('Disable UART bootloader MMU cache...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
    else:
        dut.expect('Not disabling UART bootloader encryption')
        if dut.app.target != 'esp32h2':
            dut.expect('Disable UART bootloader cache...')
        dut.expect('Disable JTAG...')
    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    if dut.app.target == 'esp32':
        dut.expect_exact('flash encryption is enabled (3 plaintext flashes left)')
    else:
        dut.expect_exact('flash encryption is enabled (1 plaintext flashes left)')

    dut.expect_exact('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.parametrize('config', ['virt_flash_enc_release',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_flash_enc_release(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_flash_enc_release_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    dut.serial.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')
    if dut.app.target == 'esp32':
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 2')
        dut.expect('Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('Disable UART bootloader encryption...')
        dut.expect('Disable UART bootloader decryption...')
        dut.expect('Disable UART bootloader MMU cache...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
    else:
        if dut.app.target == 'esp32c2':
            dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 1')
        else:
            dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 4')
        dut.expect('Disable UART bootloader encryption')
        if dut.app.target != 'esp32h2':
            dut.expect('Disable UART bootloader cache...')
        dut.expect('Disable JTAG...')
    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Setting CRYPT_CNT for permanent encryption', timeout=90)
    dut.expect('Flash encryption completed')
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    dut.expect_exact('flash encryption is enabled (0 plaintext flashes left)', timeout=5)
    dut.expect('Flash encryption mode is RELEASE')
    dut.expect('Start eFuse example')
    dut.expect('Flash Encryption is in RELEASE mode')
    dut.expect('example: Done')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize('config', ['virt_secure_boot_v1',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_secure_boot_v1(dut: Dut) -> None:
    # only for ESP32
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_secure_boot_v1_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Flash bootloader')
    dut.serial.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: Generating new secure boot key...')
    dut.expect('secure_boot_v1: Generating secure boot digest...')
    dut.expect('secure_boot_v1: Digest generation complete')
    dut.expect('Checking secure boot...')

    dut.expect('secure_boot_v1: blowing secure boot efuse...')
    dut.expect('Read & write protecting new key...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('secure_boot_v1: secure boot is now enabled for bootloader image')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled. No need to generate digest. continuing..')
    dut.expect('boot: Checking secure boot...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize('config', ['virt_secure_boot_v1',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_secure_boot_v1_pre_loaded(dut: Dut) -> None:
    print(' - Erase flash')
    dut.serial.erase_flash()
    print(' - Flash bootloader')
    dut.serial.bootloader_flash()
    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()

    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (ABS_DONE_0 1 -> 0)')
    # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
    ABS_DONE_0 = 196
    # Resets eFuse, which enables Secure boot (V1) feature
    dut.serial.erase_field_on_emul_efuse([ABS_DONE_0])

    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from flash')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: Using pre-loaded secure boot key in EFUSE block 2')
    dut.expect('secure_boot_v1: Generating secure boot digest...')
    dut.expect('secure_boot_v1: Digest generation complete')
    dut.expect('Checking secure boot...')

    dut.expect('secure_boot_v1: blowing secure boot efuse...')
    dut.expect('Read & write protecting new key...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('secure_boot_v1: secure boot is now enabled for bootloader image')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled. No need to generate digest. continuing..')
    dut.expect('Checking secure boot...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.esp32
@pytest.mark.esp32eco3
@pytest.mark.parametrize('config', [('virt_secure_boot_v2'),], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_secure_boot_v2(dut: Dut) -> None:
    # only for ESP32 ECO3
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_secure_boot_v2_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()
    print(' - Flash bootloader')
    dut.serial.bootloader_flash()
    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...', timeout=20)
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 3')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Prevent read disabling of additional efuses...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')
    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Flash bootloader and app')
    dut.serial.bootloader_flash()
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (ABS_DONE_1 1 -> 0)')
    # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
    ABS_DONE_1 = 197
    # Resets eFuse, which enables Secure boot (V2) feature
    dut.serial.erase_field_on_emul_efuse([ABS_DONE_1])

    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from flash')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests already present')
    dut.expect('secure_boot_v2: Using pre-loaded public key digest in eFuse')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Prevent read disabling of additional efuses...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.esp32
@pytest.mark.esp32eco3
@pytest.mark.parametrize('config', [('virt_secure_boot_v2'),], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_secure_boot_v2_pre_loaded(dut: Dut) -> None:

    print(' - Erase flash')
    dut.serial.erase_flash()
    print(' - Flash bootloader and app')
    dut.serial.bootloader_flash()
    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (ABS_DONE_1 1 -> 0)')
    # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
    ABS_DONE_1 = 197
    # Resets eFuse, which enables Secure boot (V2) feature
    dut.serial.erase_field_on_emul_efuse([ABS_DONE_1])

    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from flash')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests already present')
    dut.expect('secure_boot_v2: Using pre-loaded public key digest in eFuse')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Prevent read disabling of additional efuses...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', ['virt_secure_boot_v2'], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_secure_boot_v2_esp32xx(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_secure_boot_v2_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()
    print(' - Flash bootloader')
    dut.serial.bootloader_flash()
    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    if dut.app.target == 'esp32c2':
        signed_scheme = 'ECDSA'
    else:
        signed_scheme = 'RSA-PSS'
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    if dut.app.target == 'esp32c2':
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 3')
    else:
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 9')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')
    if dut.app.target != 'esp32c2':
        dut.expect_exact('secure_boot_v2: Revoking empty key digest slot (1)...')
        dut.expect_exact('secure_boot_v2: Revoking empty key digest slot (2)...')
    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Disable hardware & software JTAG...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', ['virt_secure_boot_v2'], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_example_efuse_with_virt_secure_boot_v2_esp32xx_pre_loaded(dut: Dut) -> None:
    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Flash bootloader and app')
    dut.serial.bootloader_flash()
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (SECURE_BOOT_EN 1 -> 0, SECURE_BOOT_KEY_REVOKE[0..2] -> 0)')
    # offsets of eFuses are taken from components/efuse/{target}/esp_efuse_table.csv
    if dut.app.target == 'esp32c2':
        SECURE_BOOT_EN = 53
        dut.serial.erase_field_on_emul_efuse([SECURE_BOOT_EN])
    else:
        SECURE_BOOT_EN = 116
        SECURE_BOOT_KEY_REVOKE0 = 85
        SECURE_BOOT_KEY_REVOKE1 = 86
        SECURE_BOOT_KEY_REVOKE2 = 87
        # Resets eFuse, which enables Secure boot feature
        # Resets eFuses, which control digest slots
        dut.serial.erase_field_on_emul_efuse([SECURE_BOOT_EN, SECURE_BOOT_KEY_REVOKE0, SECURE_BOOT_KEY_REVOKE1, SECURE_BOOT_KEY_REVOKE2])

    print(' - Start app (flash partition_table and app)')
    dut.serial.flash()
    dut.expect('Loading virtual efuse blocks from flash')

    dut.expect('Verifying image signature...')
    if dut.app.target == 'esp32c2':
        signed_scheme = 'ECDSA'
    else:
        signed_scheme = 'RSA-PSS'

    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests already present')
    dut.expect('secure_boot_v2: Using pre-loaded public key digest in eFuse')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    if dut.app.target != 'esp32c2':
        dut.expect_exact('secure_boot_v2: Revoking empty key digest slot (1)...')
        dut.expect_exact('secure_boot_v2: Revoking empty key digest slot (2)...')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Disable hardware & software JTAG...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled', timeout=20)

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.serial.hard_reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize('config', ['virt_sb_v1_and_fe',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_sb_v1_and_fe(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_sb_v1_and_fe_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Flash bootloader')
    dut.serial.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: Generating new secure boot key...')
    dut.expect('secure_boot_v1: Generating secure boot digest...')
    dut.expect('secure_boot_v1: Digest generation complete')

    dut.expect('Checking flash encryption...')
    dut.expect('flash_encrypt: Generating new flash encryption key...')
    dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 2')
    dut.expect('flash_encrypt: Setting CRYPT_CONFIG efuse to 0xF')
    dut.expect('flash_encrypt: Not disabling UART bootloader encryption')
    dut.expect('flash_encrypt: Disable UART bootloader decryption...')
    dut.expect('flash_encrypt: Disable UART bootloader MMU cache...')
    dut.expect('flash_encrypt: Disable JTAG...')
    dut.expect('flash_encrypt: Disable ROM BASIC interpreter fallback...')

    dut.expect('flash_encrypt: bootloader encrypted successfully')
    dut.expect('flash_encrypt: partition table encrypted and loaded successfully')

    dut.expect('Verifying image signature...')
    dut.expect('flash_encrypt: Flash encryption completed', timeout=90)

    dut.expect('Checking secure boot...')
    dut.expect('secure_boot_v1: blowing secure boot efuse...')
    dut.expect('Read & write protecting new key...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('secure_boot_v1: secure boot is now enabled for bootloader image')
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled. No need to generate digest. continuing..')
    dut.expect('Checking flash encryption...')
    dut.expect_exact('flash_encrypt: flash encryption is enabled (3 plaintext flashes left)')
    dut.expect('Checking secure boot...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled, continuing..')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect_exact('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Flash Encryption is NOT in RELEASE mode')
    dut.expect('example: Secure Boot is in RELEASE mode')
    dut.expect('example: Done')


@pytest.mark.esp32
@pytest.mark.esp32eco3
@pytest.mark.parametrize('config', ['virt_sb_v2_and_fe',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_efuse_with_virt_sb_v2_and_fe(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_sb_v2_and_fe_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    print(' - Erase flash')
    dut.serial.erase_flash()

    print(' - Flash bootloader')
    dut.serial.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 3')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('Disable ROM Download mode...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Checking flash encryption...')
    dut.expect('flash_encrypt: Generating new flash encryption key...')
    dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 2')
    dut.expect('flash_encrypt: Setting CRYPT_CONFIG efuse to 0xF')
    dut.expect('flash_encrypt: Not disabling UART bootloader encryption')
    dut.expect('flash_encrypt: Disable UART bootloader decryption...')
    dut.expect('flash_encrypt: Disable UART bootloader MMU cache...')
    dut.expect('flash_encrypt: Disable JTAG...')
    dut.expect('flash_encrypt: Disable ROM BASIC interpreter fallback...')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('flash_encrypt: bootloader encrypted successfully')
    dut.expect('flash_encrypt: partition table encrypted and loaded successfully')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('flash_encrypt: Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect_exact('flash_encrypt: flash encryption is enabled (3 plaintext flashes left)')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect_exact('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Flash Encryption is NOT in RELEASE mode')
    dut.expect('example: Secure Boot is in RELEASE mode')
    dut.expect('example: Done')


@pytest.mark.esp32
@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize('config', ['virt_sb_v2_and_fe.esp32.qemu',], indirect=True)
@pytest.mark.parametrize(
    'qemu_extra_args',
    [
        f'-drive file={os.path.join(os.path.dirname(__file__), "test", "esp32eco3_efuses.bin")},if=none,format=raw,id=efuse '
        '-global driver=nvram.esp32.efuse,property=drive,value=efuse '
        '-global driver=timer.esp32.timg,property=wdt_disable,value=true',
    ],
    indirect=True,
)
def test_examples_efuse_with_virt_sb_v2_and_fe_qemu(dut: QemuDut) -> None:
    try:
        dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
        dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
        dut.expect('secure_boot_v2: Signature verified successfully!')

        dut.expect('secure_boot_v2: enabling secure boot v2...')
        dut.expect('Verifying image signature...')
        dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
        dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
        dut.expect('secure_boot_v2: Signature verified successfully')
        dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
        dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
        dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

        dut.expect('Writing EFUSE_BLK_KEY1 with purpose 3')
        dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
        dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
        dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')

        dut.expect('secure_boot_v2: blowing secure boot efuse...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
        dut.expect('Disable ROM Download mode...')
        dut.expect('secure_boot_v2: Secure boot permanently enabled')

        dut.expect('Checking flash encryption...')
        dut.expect('flash_encrypt: Generating new flash encryption key...')
        dut.expect('Writing EFUSE_BLK_KEY0 with purpose 2')
        dut.expect('flash_encrypt: Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('flash_encrypt: Not disabling UART bootloader encryption')
        dut.expect('flash_encrypt: Disable UART bootloader decryption...')
        dut.expect('flash_encrypt: Disable UART bootloader MMU cache...')
        dut.expect('flash_encrypt: Disable JTAG...')
        dut.expect('flash_encrypt: Disable ROM BASIC interpreter fallback...')

        dut.expect('Verifying image signature...')
        dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
        dut.expect('secure_boot_v2: Signature verified successfully!')
        dut.expect('flash_encrypt: bootloader encrypted successfully')
        dut.expect('flash_encrypt: partition table encrypted and loaded successfully')

        dut.expect('Verifying image signature...')
        dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
        dut.expect('secure_boot_v2: Signature verified successfully!')
        dut.expect('flash_encrypt: Flash encryption completed', timeout=120)
        dut.expect('Resetting with flash encryption enabled...')

        dut.expect('Verifying image signature...', timeout=180)
        dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
        dut.expect('secure_boot_v2: Signature verified successfully!')
        dut.expect('secure_boot_v2: enabling secure boot v2...')
        dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
        dut.expect_exact('flash_encrypt: flash encryption is enabled (3 plaintext flashes left)')
        dut.expect_exact('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
        dut.expect('main_task: Calling app_main()')
        dut.expect('Start eFuse example')
        dut.expect('example: Flash Encryption is NOT in RELEASE mode')
        dut.expect('example: Secure Boot is in RELEASE mode')
        dut.expect('example: Done')

    finally:
        # the above example test burns the efuses, and hence the efuses file which the
        # qemu uses to emulate the efuses, "test/esp32eco3_efuses.bin", gets modified.
        # Thus, restore the efuses file values back to the default ESP32-ECO3 efuses values.
        with open(os.path.join(os.path.dirname(__file__), 'test', 'esp32eco3_efuses.bin'), 'wb') as efuse_file:
            esp32eco3_efuses = '0' * 26 + '8' + '0' * 17 + '1' + '0' * 203
            efuse_file.write(bytearray.fromhex(esp32eco3_efuses))


@pytest.mark.esp32c3
@pytest.mark.esp32c2
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
@pytest.mark.parametrize('config', ['virt_sb_v2_and_fe'], indirect=True)
def test_examples_efuse_with_virt_sb_v2_and_fe_esp32xx(dut: Dut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('{}_bootloader_virt_sb_v2_and_fe_bin_size: {}KB'.format(dut.app.target, bin_size // 1024))

    dut.serial.erase_flash()

    print(' - Flash bootloader')
    dut.serial.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    signed_scheme = 'ECDSA' if dut.app.target == 'esp32c2' else 'RSA-PSS'
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    if dut.app.target == 'esp32c2':
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 3')
    else:
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 9')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect_exact('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect_exact('secure_boot_v2: Application key(0) matches with bootloader key(0)')
    if dut.app.target != 'esp32c2':
        dut.expect_exact('secure_boot_v2: Revoking empty key digest slot (1)...')
        dut.expect_exact('secure_boot_v2: Revoking empty key digest slot (2)...')
    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Enabling Security download mode...')
    dut.expect('Disable hardware & software JTAG...')

    if dut.app.target != 'esp32c2':
        dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Checking flash encryption...')
    dut.expect('flash_encrypt: Generating new flash encryption key...')
    if dut.app.target == 'esp32c2':
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 2')
    else:
        dut.expect(r'Writing EFUSE_BLK_KEY\d with purpose 4')

    dut.expect('Not disabling UART bootloader encryption')
    if dut.app.target != 'esp32h2':
        dut.expect('Disable UART bootloader cache...')
    dut.expect('Disable JTAG...')

    if dut.app.target == 'esp32c2':
        dut.expect('boot: Secure boot permanently enabled')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('flash_encrypt: bootloader encrypted successfully')
    dut.expect('flash_encrypt: partition table encrypted and loaded successfully')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('flash_encrypt: Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with %s...' % signed_scheme)
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect_exact('flash_encrypt: flash encryption is enabled (1 plaintext flashes left)')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect_exact('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('main_task: Calling app_main()')
    dut.expect('Start eFuse example')
    dut.expect('example: Flash Encryption is NOT in RELEASE mode')
    dut.expect('example: Secure Boot is in RELEASE mode')
    dut.expect('example: Done')
