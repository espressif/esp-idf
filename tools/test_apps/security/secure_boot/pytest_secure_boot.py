# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import itertools
import os
import struct
import zlib

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# To prepare a runner for these tests,
# 1. Connect an FPGA with C3 image
# 2. Use a COM port for programming and export it as ESPPORT
#    e.g export ESPPORT=/dev/ttyUSB0
# 3. Use another COM port for resetting efuses and connect its DTR pin to efuse reset pin on the FPGA board.
#    Export it as EFUSEPORT
#    e.g export EFUSEPORT=/dev/ttyUSB1
# 4. Run these tests

CORRUPT_ALL_BLOCKS = 0xFF

SIGNATURE_TYPE_RSA = 0
SIGNATURE_TYPE_RSA_3072 = 1

SIGNATURE_TYPE_ECDSA = 10
SIGNATURE_TYPE_ECDSA_P192 = 11
SIGNATURE_TYPE_ECDSA_P256 = 12
SIGNATURE_TYPE_ECDSA_P384 = 13

SIGNATURE_TYPE_RSA_3072_SIZE = 384
SIGNATURE_TYPE_ECDSA_P192_SIZE = 64
SIGNATURE_TYPE_ECDSA_P256_SIZE = 64
SIGNATURE_TYPE_ECDSA_P384_SIZE = 96

EFUSE_KEY_BLOCKS = 6

SECURE_BOOT_RSA_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c5',
    'esp32c6',
    'esp32c61',
    'esp32h2',
    'esp32h21',
    'esp32s2',
    'esp32s3',
    'esp32p4',
]
SECURE_BOOT_ECDSA_TARGETS = ['esp32c2', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32h21', 'esp32p4']
SECURE_BOOT_ECDSA_P384_TARGETS = ['esp32c5']

CONFIGS_SECURE_BOOT_ECDSA = list(
    itertools.chain(
        itertools.product(['ecdsa_p192', 'ecdsa_p256'], SECURE_BOOT_ECDSA_TARGETS),
        itertools.product(['ecdsa_p384'], SECURE_BOOT_ECDSA_P384_TARGETS),
    )
)

CONFIGS_SECURE_BOOT_RSA = list(
    itertools.chain(
        itertools.product(['rsa_3072'], SECURE_BOOT_RSA_TARGETS),
    )
)


def corrupt_signature(
    signed_bootloader,
    seed=0,
    corrupt_sig=True,
    corrupt_crc=False,
    corrupt_block=CORRUPT_ALL_BLOCKS,
    signature_type=SIGNATURE_TYPE_RSA_3072,
):
    # type: (bytes, int, bool, bool, int, int) -> bytes
    image = signed_bootloader[:-4096]
    signature = signed_bootloader[-4096:]
    sig_blocks = (signature[0:1216], signature[1216:2432], signature[2432:3648])
    new_blocks = tuple(corrupt_sig_block(s, seed, corrupt_sig, corrupt_crc, signature_type) for s in sig_blocks)

    # if corrupt_block is CORRUPT_ALL_BLOCKS, corrupt all blocks
    # otherwise, only corrupt the one with that index set
    corr_sig_blocks = tuple(
        new_blocks[n] if corrupt_block in [CORRUPT_ALL_BLOCKS, n] else sig_blocks[n] for n in range(3)
    )

    return image + b''.join(corr_sig_blocks) + signature[3648:]


def corrupt_sig_block(sig_block, seed=0, corrupt_sig=True, corrupt_crc=False, signature_type=SIGNATURE_TYPE_RSA_3072):
    # type: (bytes, int, bool, bool, int) -> bytes
    assert len(sig_block) == 1216
    magic = sig_block[0]
    assert magic in [0xE7, 0xFF]
    if magic != 0xE7:
        return sig_block  # not valid

    if signature_type == SIGNATURE_TYPE_RSA_3072:
        data = sig_block[:812]
        new_sig = sig = sig_block[812:1196]
    elif signature_type in [SIGNATURE_TYPE_ECDSA_P192, SIGNATURE_TYPE_ECDSA_P256]:
        data = sig_block[:101]
        new_sig = sig = sig_block[101:165]
    elif signature_type == SIGNATURE_TYPE_ECDSA_P384:
        data = sig_block[:149]
        new_sig = sig = sig_block[149:245]
    else:
        raise ValueError('Invalid signature type: {}'.format(signature_type))

    crc = sig_block[1196:1200]
    padding = sig_block[1200:1216]

    if corrupt_sig:
        corrupt_idx = seed % len(sig)
        corrupt_delta = zlib.crc32(bytes(seed)) & 0xFF
        if corrupt_delta == 0:
            corrupt_delta = 1

        new_byte = sig[corrupt_idx] ^ corrupt_delta

        new_sig = sig[0:corrupt_idx] + bytes([new_byte]) + sig[corrupt_idx + 1 :]

        assert new_sig != sig

    if not corrupt_crc:
        crc = struct.pack('<I', zlib.crc32(data + new_sig) & 0xFFFFFFFF)
    else:
        crc = struct.pack('<I', zlib.crc32(crc))

    result = data + new_sig + crc + padding
    assert len(result) == len(sig_block)
    return result


def dut_start_secure_app(dut: Dut) -> None:
    dut.serial.erase_app_header()
    dut.serial.reset_efuses()
    dut.burn_wafer_version()

    dut.serial.bootloader_flash(os.path.join(dut.app.binary_path, 'bootloader/bootloader.bin'))
    dut.serial.partition_table_flash(os.path.join(dut.app.binary_path, 'partition_table/partition-table.bin'))
    dut.serial.app_flash(os.path.join(dut.app.binary_path, 'secure_boot.bin'))


def _examples_security_secure_boot(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test secure boot flow.
# Correctly signed bootloader + correctly signed app should work
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_RSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_rsa(dut: Dut) -> None:
    _examples_security_secure_boot(dut)


@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_ECDSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_ecdsa(dut: Dut) -> None:
    _examples_security_secure_boot(dut)


# Test secure boot flow.
# Correctly signed bootloader + correctly signed app should work
@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize(
    'qemu_extra_args',
    [
        f'-drive file={os.path.join(os.path.dirname(__file__), "test", "esp32c3_efuses.bin")},'
        f'if=none,format=raw,id=efuse '
        '-global driver=nvram.esp32c3.efuse,property=drive,value=efuse '
        '-global driver=timer.esp32c3.timg,property=wdt_disable,value=true',
    ],
    indirect=True,
)
@pytest.mark.parametrize('config', ['qemu'], indirect=True)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_examples_security_secure_boot_qemu(dut: Dut) -> None:
    try:
        dut.expect('Secure Boot is enabled', timeout=10)
        dut.expect('Restarting now.', timeout=10)
        dut.expect('Secure Boot is enabled', timeout=10)

    finally:
        # the above example test burns the efuses, and hence the efuses file which the
        # qemu uses to emulate the efuses, "esp32c3_efuses.bin", gets modified.
        # Thus, restore the efuses file values back to the default ESP32C3 efuses values.
        with open(os.path.join(os.path.dirname(__file__), 'test', 'esp32c3_efuses.bin'), 'wb') as efuse_file:
            esp32c3_efuses = '0' * 77 + 'c' + '0' * 1970
            efuse_file.write(bytearray.fromhex(esp32c3_efuses))


def _examples_security_secure_boot_key_combo(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)
    efuse_secure_boot_key_digests = dut.app.sdkconfig.get('SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS')
    secure_boot_key = dut.app.sdkconfig.get('SECURE_BOOT_SIGNING_KEY')
    for index in range(efuse_secure_boot_key_digests):
        for block in range(EFUSE_KEY_BLOCKS):
            dut.serial.reset_efuses()
            dut.burn_wafer_version()
            dut.secure_boot_burn_en_bit()
            dut.secure_boot_burn_digest(secure_boot_key, index, block)
            dut.expect('Secure Boot is enabled', timeout=10)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test efuse key index and key block combination.
# Any key index can be written to any key block and should work
# Increasing the test timeout to 1200s as the test runs for 18 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.timeout(1200)
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_RSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_key_combo_rsa(dut: Dut) -> None:
    _examples_security_secure_boot_key_combo(dut)


# Test efuse key index and key block combination.
# Any key index can be written to any key block and should work
# Increasing the test timeout to 1200s as the test runs for 18 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.timeout(1200)
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_ECDSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_key_combo_ecdsa(dut: Dut) -> None:
    _examples_security_secure_boot_key_combo(dut)


def _examples_security_secure_boot_key_revoke(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)
    efuse_secure_boot_key_digests = dut.app.sdkconfig.get('SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS')
    secure_boot_key = dut.app.sdkconfig.get('SECURE_BOOT_SIGNING_KEY')

    for index in range(efuse_secure_boot_key_digests):
        dut.serial.reset_efuses()
        dut.burn_wafer_version()
        dut.secure_boot_burn_en_bit()
        dut.serial.burn_efuse('SECURE_BOOT_KEY_REVOKE%d' % index, 1)
        dut.secure_boot_burn_digest(secure_boot_key, index, 0)
        dut.expect('secure boot verification failed', timeout=5)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test secure boot key revoke.
# If a key is revoked, bootloader signed with that key should fail verification
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_RSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_key_revoke_rsa(dut: Dut) -> None:
    _examples_security_secure_boot_key_revoke(dut)


# Test secure boot key revoke.
# If a key is revoked, bootloader signed with that key should fail verification
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_ECDSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_key_revoke_ecdsa(dut: Dut) -> None:
    _examples_security_secure_boot_key_revoke(dut)


def get_signature_type_size(dut: Dut, signature_type: int) -> int:
    signature_type_size = 0
    if signature_type == SIGNATURE_TYPE_RSA:
        signature_type_size = SIGNATURE_TYPE_RSA_3072_SIZE
    elif signature_type == SIGNATURE_TYPE_ECDSA:
        if dut.app.sdkconfig.get('CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_192_BITS'):
            signature_type_size = SIGNATURE_TYPE_ECDSA_P192_SIZE
        elif dut.app.sdkconfig.get('CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_256_BITS'):
            signature_type_size = SIGNATURE_TYPE_ECDSA_P256_SIZE
        elif dut.app.sdkconfig.get('CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_384_BITS'):
            signature_type_size = SIGNATURE_TYPE_ECDSA_P384_SIZE
        else:
            raise ValueError('Invalid signature type: {}'.format(signature_type))
    else:
        raise ValueError('Invalid signature type: {}'.format(signature_type))
    return signature_type_size


def _examples_security_secure_boot_corrupt_bl_sig(dut: Dut, signature_type: int) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)

    bootloader_bin = os.path.join(dut.app.binary_path, 'bootloader/bootloader.bin')
    with open(bootloader_bin, 'rb') as f:
        signed_bl = f.read()

    signature_type_size = get_signature_type_size(dut, signature_type)
    seeds = range(0, signature_type_size)
    max_seed = max(seeds)
    secure_boot_key = dut.app.sdkconfig.get('SECURE_BOOT_SIGNING_KEY')

    for seed in seeds:
        print('Case %d / %d' % (seed, max_seed))
        corrupt_bl = corrupt_signature(signed_bl, seed=seed)
        with open('corrupt_bl.bin', 'wb') as corrupt_file:
            corrupt_file.write(corrupt_bl)
        dut.serial.reset_efuses()
        dut.burn_wafer_version()
        dut.serial.bootloader_flash('corrupt_bl.bin')
        dut.secure_boot_burn_en_bit()
        dut.secure_boot_burn_digest(secure_boot_key, 0, 0)
        # Though the order of flashing and burning efuse would not effect the test,
        # if we flash bootlader before burning en bit, even with no_stub = True
        # it still calls run_stub() and throws an error as it fails to start stub.
        dut.expect('Signature Check Failed', timeout=10)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test bootloader signature corruption.
# Corrupt one byte at a time of bootloader signature and test that the verification fails
@pytest.mark.timeout(18000)
# Increasing the test timeout to 18000s as the test runs for 384 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_RSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_corrupt_bl_sig_rsa(dut: Dut) -> None:
    _examples_security_secure_boot_corrupt_bl_sig(dut, signature_type=SIGNATURE_TYPE_RSA)


# Test bootloader signature corruption.
# Corrupt one byte at a time of bootloader signature and test that the verification fails
@pytest.mark.timeout(18000)
# Increasing the test timeout to 18000s as the test runs for 384 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_ECDSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_corrupt_bl_sig_ecdsa(dut: Dut) -> None:
    _examples_security_secure_boot_corrupt_bl_sig(dut, signature_type=SIGNATURE_TYPE_ECDSA)


def _examples_security_secure_boot_corrupt_app_sig(dut: Dut, signature_type: int) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)

    app_bin = os.path.join(dut.app.binary_path, 'secure_boot.bin')
    with open(app_bin, 'rb') as f:
        signed_app = f.read()

    signature_size = get_signature_type_size(dut, signature_type)
    seeds = range(0, signature_size)
    max_seed = max(seeds)

    for seed in seeds:
        print('Case %d / %d' % (seed, max_seed))
        corrupt_app = corrupt_signature(signed_app, seed=seed)
        with open('corrupt_app.bin', 'wb') as corrupt_file:
            corrupt_file.write(corrupt_app)
        dut.serial.reset_efuses()
        dut.burn_wafer_version()
        dut.serial.app_flash('corrupt_app.bin')
        dut.secure_boot_burn_en_bit()
        dut.secure_boot_burn_digest('test_rsa_3072_key.pem', 0, 0)
        dut.expect('Signature Check Failed', timeout=10)
        dut.expect('image valid, signature bad', timeout=2)

    print('Testing invalid CRC...')
    # Valid signature but invalid CRC
    dut.serial.reset_efuses()
    dut.burn_wafer_version()

    corrupt_app = corrupt_signature(signed_app, corrupt_sig=False, corrupt_crc=True)
    with open('corrupt_app.bin', 'wb') as corrupt_file:
        corrupt_file.write(corrupt_app)

    dut.serial.app_flash('corrupt_app.bin')

    dut.secure_boot_burn_en_bit()
    dut.secure_boot_burn_digest('test_rsa_3072_key.pem', 0, 0)

    dut.expect(
        'Sig block 0 invalid: {}'.format(
            'CRC mismatch' if dut.target == 'esp32p4' or dut.target == 'esp32c5' else 'Stored CRC ends'
        ),
        timeout=2,
    )
    dut.expect('Secure boot signature verification failed', timeout=2)
    dut.expect('No bootable app partitions in the partition table', timeout=2)


# Test app signature corruption.
# Corrupt app signature, one byte at a time, and test that the verification fails
@pytest.mark.timeout(18000)
# Increasing the test timeout to 18000s as the test runs for 385 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_RSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_corrupt_app_sig_rsa(dut: Dut) -> None:
    _examples_security_secure_boot_corrupt_app_sig(dut, signature_type=SIGNATURE_TYPE_RSA)


# Test app signature corruption.
# Corrupt app signature, one byte at a time, and test that the verification fails
@pytest.mark.timeout(18000)
# Increasing the test timeout to 18000s as the test runs for 385 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.generic
@idf_parametrize('config, target', CONFIGS_SECURE_BOOT_ECDSA, indirect=['config', 'target'])
def test_examples_security_secure_boot_corrupt_app_sig_ecdsa(dut: Dut) -> None:
    _examples_security_secure_boot_corrupt_app_sig(dut, signature_type=SIGNATURE_TYPE_ECDSA)
