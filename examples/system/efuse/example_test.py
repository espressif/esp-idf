from __future__ import unicode_literals

import os
import re

import ttfw_idf


def erase_field_on_emul_efuse(dut, pos_of_bits):  # type: (ttfw_idf.TinyFW.Env, list) -> None
    emul_efuse_bin_path = os.path.join(dut.app.binary_path, 'emul_efuse.bin')
    dut.dump_flash(emul_efuse_bin_path, partition='emul_efuse')

    def erase_bit(pos_of_bit):  # type: (int) -> None
        nbytes, nbits = divmod(pos_of_bit, 8)
        with open(emul_efuse_bin_path, 'r+b') as f:
            f.seek(nbytes)
            data = ord(f.read(1))
            data &= ~(1 << nbits)
            f.seek(-1, os.SEEK_CUR)
            f.write(bytes([data]))

    for pos_of_bit in sorted(pos_of_bits):
        erase_bit(pos_of_bit)

    offs = dut.app.partition_table['emul_efuse']['offset']
    flash_files = [(offs, emul_efuse_bin_path)]
    dut.write_flash(flash_files)


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32c3'])
def test_examples_efuse(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse')
    dut.start_app()
    dut.expect_all(re.compile(r'example: Coding Scheme (3/4)|(NONE)|(REPEAT)|(RS \(Reed-Solomon coding\))'),
                   'example: read efuse fields',
                   re.compile(r'example: 1. read MAC address: {}'.format(r':'.join((r'[0-9a-f]{2}',) * 6))),
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
                   'example: Done',
                   timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_examples_efuse_with_virt_flash_enc(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_flash_enc')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')
    if dut.TARGET == 'esp32':
        dut.expect('Writing EFUSE_BLK_KEY0 with purpose 2')
        dut.expect('Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('Not disabling UART bootloader encryption')
        dut.expect('Disable UART bootloader decryption...')
        dut.expect('Disable UART bootloader MMU cache...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
    else:
        dut.expect('Writing EFUSE_BLK_KEY0 with purpose 4')
        dut.expect('Not disabling UART bootloader encryption')
        dut.expect('Disable UART bootloader cache...')
        dut.expect('Disable JTAG...')
    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    if dut.TARGET == 'esp32':
        dut.expect('flash encryption is enabled (3 plaintext flashes left)')
    else:
        dut.expect('flash encryption is enabled (1 plaintext flashes left)')

    dut.expect('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32s2'])
def test_examples_efuse_with_virt_flash_enc_aes_256(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    # Only ESP32-S2 has support AES-256 FLASH_ENCRYPTION key
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_flash_enc_aes_256')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')

    dut.expect('Writing EFUSE_BLK_KEY0 with purpose 2')
    dut.expect('Writing EFUSE_BLK_KEY1 with purpose 3')
    dut.expect('Not disabling UART bootloader encryption')
    dut.expect('Disable UART bootloader cache...')
    dut.expect('Disable JTAG...')

    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    dut.expect('flash encryption is enabled (1 plaintext flashes left)')

    dut.expect('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_examples_efuse_with_virt_flash_enc_pre_loaded(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_flash_enc')

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    if dut.TARGET == 'esp32':
        print(' - Flash emul_efuse with pre-loaded efuses (FLASH_CRYPT_CNT 1 -> 0)')
        # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
        FLASH_CRYPT_CNT = 20
        # Resets eFuse, which enables Flash encryption feature
        erase_field_on_emul_efuse(dut, [FLASH_CRYPT_CNT])
    else:
        # offset of this eFuse is taken from components/efuse/{target}/esp_efuse_table.csv
        print(' - Flash emul_efuse with pre-loaded efuses (SPI_BOOT_CRYPT_CNT 1 -> 0)')
        SPI_BOOT_CRYPT_CNT = 82
        # Resets eFuse, which enables Flash encryption feature
        erase_field_on_emul_efuse(dut, [SPI_BOOT_CRYPT_CNT])

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    dut.expect('Using pre-loaded flash encryption key in efuse')
    if dut.TARGET == 'esp32':
        dut.expect('Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('Not disabling UART bootloader encryption')
        dut.expect('Disable UART bootloader decryption...')
        dut.expect('Disable UART bootloader MMU cache...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
    else:
        dut.expect('Not disabling UART bootloader encryption')
        dut.expect('Disable UART bootloader cache...')
        dut.expect('Disable JTAG...')
    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    if dut.TARGET == 'esp32':
        dut.expect('flash encryption is enabled (3 plaintext flashes left)')
    else:
        dut.expect('flash encryption is enabled (1 plaintext flashes left)')

    dut.expect('Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_examples_efuse_with_virt_flash_enc_release(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_flash_enc_release')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('Checking flash encryption...')
    dut.expect('Generating new flash encryption key...')
    if dut.TARGET == 'esp32':
        dut.expect('Writing EFUSE_BLK_KEY0 with purpose 2')
        dut.expect('Setting CRYPT_CONFIG efuse to 0xF')
        dut.expect('Disable UART bootloader encryption...')
        dut.expect('Disable UART bootloader decryption...')
        dut.expect('Disable UART bootloader MMU cache...')
        dut.expect('Disable JTAG...')
        dut.expect('Disable ROM BASIC interpreter fallback...')
    else:
        dut.expect('Writing EFUSE_BLK_KEY0 with purpose 4')
        dut.expect('Disable UART bootloader encryption')
        dut.expect('Disable UART bootloader cache...')
        dut.expect('Disable JTAG...')
    dut.expect('bootloader encrypted successfully')
    dut.expect('partition table encrypted and loaded successfully')
    dut.expect('Setting CRYPT_CNT for permanent encryption', timeout=90)
    dut.expect('Flash encryption completed')
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Checking flash encryption...')
    dut.expect('flash encryption is enabled (0 plaintext flashes left)')

    dut.expect('Flash encryption mode is RELEASE')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32'])
def test_examples_efuse_with_virt_secure_boot_v1(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    # only for ESP32
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_secure_boot_v1')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader')
    dut.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app()
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

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled. No need to generate digest. continuing..')
    dut.expect('boot: Checking secure boot...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32'])
def test_examples_efuse_with_virt_secure_boot_v1_pre_loaded(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    # only for ESP32
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_secure_boot_v1')

    print(' - Erase flash')
    dut.erase_flash()

    dut.bootloader_flash()
    dut.start_app()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (ABS_DONE_0 1 -> 0)')
    # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
    ABS_DONE_0 = 196
    # Resets eFuse, which enables Secure boot (V1) feature
    erase_field_on_emul_efuse(dut, [ABS_DONE_0])

    print(' - Start app (flash partition_table and app)')
    dut.start_app()
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

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled. No need to generate digest. continuing..')
    dut.expect('Checking secure boot...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV12', target=['esp32'])
def test_examples_efuse_with_virt_secure_boot_v2(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    # only for ESP32 ECO3
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_secure_boot_v2')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader')
    dut.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    dut.expect('Writing EFUSE_BLK_KEY1 with purpose 3')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Prevent read disabling of additional efuses...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV12', target=['esp32'])
def test_examples_efuse_with_virt_secure_boot_v2_pre_loaded(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    # only for ESP32 ECO3
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_secure_boot_v2')

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader and app')
    dut.bootloader_flash()
    dut.start_app()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (ABS_DONE_1 1 -> 0)')
    # offset of this eFuse is taken from components/efuse/esp32/esp_efuse_table.csv
    ABS_DONE_1 = 197
    # Resets eFuse, which enables Secure boot (V2) feature
    erase_field_on_emul_efuse(dut, [ABS_DONE_1])

    print(' - Start app (flash partition_table and app)')
    dut.start_app()
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
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Prevent read disabling of additional efuses...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32s2', 'esp32c3'])
def test_examples_efuse_with_virt_secure_boot_v2_esp32xx(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_secure_boot_v2')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader')
    dut.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    dut.expect('Writing EFUSE_BLK_KEY0 with purpose 9')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect('secure_boot_v2: Application key(0) matches with bootloader key(0)')
    dut.expect('secure_boot_v2: Revoking empty key digest slot (1)...')
    dut.expect('secure_boot_v2: Revoking empty key digest slot (2)...')
    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Disable hardware & software JTAG...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32s2', 'esp32c3'])
def test_examples_efuse_with_virt_secure_boot_v2_esp32xx_pre_loaded(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_secure_boot_v2')

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader and app')
    dut.bootloader_flash()
    dut.start_app()
    dut.expect('Loading virtual efuse blocks from real efuses')
    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    print(' - Flash emul_efuse with pre-loaded efuses (SECURE_BOOT_EN 1 -> 0, SECURE_BOOT_KEY_REVOKE[0..2] -> 0)')
    # offsets of eFuses are taken from components/efuse/{target}/esp_efuse_table.csv
    SECURE_BOOT_EN = 116
    SECURE_BOOT_KEY_REVOKE0 = 85
    SECURE_BOOT_KEY_REVOKE1 = 86
    SECURE_BOOT_KEY_REVOKE2 = 87
    # Resets eFuse, which enables Secure boot feature
    # Resets eFuses, which control digest slots
    erase_field_on_emul_efuse(dut, [SECURE_BOOT_EN, SECURE_BOOT_KEY_REVOKE0, SECURE_BOOT_KEY_REVOKE1, SECURE_BOOT_KEY_REVOKE2])

    print(' - Start app (flash partition_table and app)')
    dut.start_app()
    dut.expect('Loading virtual efuse blocks from flash')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests already present')
    dut.expect('secure_boot_v2: Using pre-loaded public key digest in eFuse')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect('secure_boot_v2: Application key(0) matches with bootloader key(0)')
    dut.expect('secure_boot_v2: Revoking empty key digest slot (1)...')
    dut.expect('secure_boot_v2: Revoking empty key digest slot (2)...')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Disable hardware & software JTAG...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')

    dut.reset()
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32'])
def test_examples_efuse_with_virt_sb_v1_and_fe(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_sb_v1_and_fe')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader')
    dut.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v1: Generating new secure boot key...')
    dut.expect('secure_boot_v1: Generating secure boot digest...')
    dut.expect('secure_boot_v1: Digest generation complete')

    dut.expect('Checking flash encryption...')
    dut.expect('flash_encrypt: Generating new flash encryption key...')
    dut.expect('Writing EFUSE_BLK_KEY0 with purpose 2')
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
    dut.expect('flash_encrypt: flash encryption is enabled (3 plaintext flashes left)')
    dut.expect('Checking secure boot...')
    dut.expect('secure_boot_v1: bootloader secure boot is already enabled, continuing..')

    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV12', target=['esp32'])
def test_examples_efuse_with_virt_sb_v2_and_fe(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    # only for ESP32 ECO3
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_sb_v2_and_fe')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader')
    dut.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
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
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    dut.expect('Writing EFUSE_BLK_KEY1 with purpose 3')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect('secure_boot_v2: Application key(0) matches with bootloader key(0)')

    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('Disable JTAG...')
    dut.expect('Disable ROM BASIC interpreter fallback...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
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
    dut.expect('flash_encrypt: Flash encryption completed', timeout=90)
    dut.expect('Resetting with flash encryption enabled...')

    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('secure_boot_v2: secure boot v2 is already enabled, continuing..')
    dut.expect('flash_encrypt: flash encryption is enabled (3 plaintext flashes left)')
    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32s2', 'esp32c3'])
def test_examples_efuse_with_virt_sb_v2_and_fe_esp32xx(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    dut = env.get_dut('efuse', 'examples/system/efuse', app_config_name='virt_sb_v2_and_fe')
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('{}_bootloader_{}_bin_size'.format(dut.app.target, dut.app.config_name), '{}KB'.format(bin_size // 1024))

    print(' - Erase flash')
    dut.erase_flash()

    print(' - Flash bootloader')
    dut.bootloader_flash()

    print(' - Start app (flash partition_table and app)')
    dut.start_app_no_enc()
    dut.expect('Loading virtual efuse blocks from real efuses')

    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')

    dut.expect('secure_boot_v2: enabling secure boot v2...')
    dut.expect('Verifying image signature...')
    dut.expect('secure_boot_v2: Secure boot V2 is not enabled yet and eFuse digest keys are not set')
    dut.expect('secure_boot_v2: Verifying with RSA-PSS...')
    dut.expect('secure_boot_v2: Signature verified successfully!')
    dut.expect('secure_boot_v2: Secure boot digests absent, generating..')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the bootloader')

    dut.expect('Writing EFUSE_BLK_KEY0 with purpose 9')
    dut.expect('secure_boot_v2: Digests successfully calculated, 1 valid signatures')
    dut.expect('secure_boot_v2: 1 signature block(s) found appended to the app')
    dut.expect('secure_boot_v2: Application key(0) matches with bootloader key(0)')
    dut.expect('secure_boot_v2: Revoking empty key digest slot (1)...')
    dut.expect('secure_boot_v2: Revoking empty key digest slot (2)...')
    dut.expect('secure_boot_v2: blowing secure boot efuse...')
    dut.expect('UART ROM Download mode kept enabled - SECURITY COMPROMISED')
    dut.expect('Disable hardware & software JTAG...')
    dut.expect('secure_boot_v2: Secure boot permanently enabled')

    dut.expect('Checking flash encryption...')
    dut.expect('flash_encrypt: Generating new flash encryption key...')
    dut.expect('Writing EFUSE_BLK_KEY1 with purpose 4')
    dut.expect('Not disabling UART bootloader encryption')
    dut.expect('Disable UART bootloader cache...')
    dut.expect('Disable JTAG...')

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
    dut.expect('flash_encrypt: flash encryption is enabled (1 plaintext flashes left)')
    dut.expect('cpu_start: Pro cpu up')
    dut.expect('Loading virtual efuse blocks from flash')
    dut.expect('flash_encrypt: Flash encryption mode is DEVELOPMENT (not secure)')
    dut.expect('Start eFuse example')
    dut.expect('example: Done')


if __name__ == '__main__':
    test_examples_efuse()
    test_examples_efuse_with_virt_flash_enc()
    test_examples_efuse_with_virt_flash_enc_pre_loaded()
    test_examples_efuse_with_virt_flash_enc_aes_256()
    test_examples_efuse_with_virt_flash_enc_release()
    test_examples_efuse_with_virt_secure_boot_v1()
    test_examples_efuse_with_virt_secure_boot_v1_pre_loaded()
    test_examples_efuse_with_virt_secure_boot_v2()
    test_examples_efuse_with_virt_secure_boot_v2_pre_loaded()
    test_examples_efuse_with_virt_secure_boot_v2_esp32xx()
    test_examples_efuse_with_virt_secure_boot_v2_esp32xx_pre_loaded()
    test_examples_efuse_with_virt_sb_v1_and_fe()
    test_examples_efuse_with_virt_sb_v2_and_fe()
    test_examples_efuse_with_virt_sb_v2_and_fe_esp32xx()
