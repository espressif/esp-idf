# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import struct
import zlib

import pytest
from pytest_embedded import Dut

# To prepare a runner for these tests,
# 1. Connect an FPGA with C3 image
# 2. Use a COM port for programming and export it as ESPPORT
#    e.g export ESPPORT=/dev/ttyUSB0
# 3. Use another COM port for resetting efuses and connect its DTR pin to efuse reset pin on the FPGA board.
#    Export it as EFUSEPORT
#    e.g export EFUSEPORT=/dev/ttyUSB1
# 4. Run these tests

CORRUPT_ALL_BLOCKS = 0xFF


def corrupt_signature(signed_bootloader, seed=0, corrupt_sig=True, corrupt_crc=False, corrupt_block=CORRUPT_ALL_BLOCKS):
    # type: (bytes, int, bool, bool, int) -> bytes
    image = signed_bootloader[:-4096]
    signature = signed_bootloader[-4096:]
    sig_blocks = (signature[0:1216], signature[1216:2432], signature[2432:3648])
    new_blocks = tuple(corrupt_sig_block(s, seed, corrupt_sig, corrupt_crc) for s in sig_blocks)

    # if corrupt_block is CORRUPT_ALL_BLOCKS, corrupt all blocks
    # otherwise, only corrupt the one with that index set
    corr_sig_blocks = tuple(new_blocks[n] if corrupt_block in [CORRUPT_ALL_BLOCKS, n] else sig_blocks[n] for n in range(3))

    return image + b''.join(corr_sig_blocks) + signature[3648:]


def corrupt_sig_block(sig_block, seed=0, corrupt_sig=True, corrupt_crc=False):
    # type: (bytes, int, bool, bool) -> bytes
    assert len(sig_block) == 1216
    magic = sig_block[0]
    assert magic in [0xe7, 0xff]
    if magic != 0xe7:
        return sig_block  # not valid
    data = sig_block[:812]
    new_sig = sig = sig_block[812:1196]
    crc = sig_block[1196:1200]
    padding = sig_block[1200:1216]

    if corrupt_sig:
        corrupt_idx = seed % len(sig)
        corrupt_delta = zlib.crc32(bytes(seed)) & 0xFF
        if corrupt_delta == 0:
            corrupt_delta = 1

        new_byte = sig[corrupt_idx] ^ corrupt_delta

        new_sig = sig[0:corrupt_idx] + bytes([new_byte]) + sig[corrupt_idx + 1:]

        assert new_sig != sig

    if not corrupt_crc:
        crc = struct.pack('<I', zlib.crc32(data + new_sig) & 0xffffffff)
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


# Test secure boot flow.
# Correctly signed bootloader + correctly signed app should work
@pytest.mark.esp32c3
# TODO: [ESP32C5] IDF-10043
# @pytest.mark.esp32c5
# TODO: [ESP32C5] IDF-10102
# @pytest.mark.esp32c61
@pytest.mark.esp32s3
@pytest.mark.esp32p4
def test_examples_security_secure_boot(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test efuse key index and key block combination.
# Any key index can be written to any key block and should work
@pytest.mark.esp32c3
# TODO: [ESP32C5] IDF-10043
# @pytest.mark.esp32c5
# TODO: [ESP32C5] IDF-10102
# @pytest.mark.esp32c61
@pytest.mark.esp32s3
@pytest.mark.esp32p4
# Increasing the test timeout to 1200s as the test runs for 18 iterations
# and thus the default 600s timeout is not sufficient
@pytest.mark.timeout(1200)
def test_examples_security_secure_boot_key_combo(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)
    for index in range(3):
        for block in range(6):
            dut.serial.reset_efuses()
            dut.burn_wafer_version()
            dut.secure_boot_burn_en_bit()
            dut.secure_boot_burn_digest('test_rsa_3072_key.pem', index, block)
            dut.expect('Secure Boot is enabled', timeout=10)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test secure boot key revoke.
# If a key is revoked, bootloader signed with that key should fail verification
@pytest.mark.esp32c3
# TODO: [ESP32C5] IDF-10043
# @pytest.mark.esp32c5
# TODO: [ESP32C5] IDF-10102
# @pytest.mark.esp32c61
@pytest.mark.esp32s3
@pytest.mark.esp32p4
def test_examples_security_secure_boot_key_revoke(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)
    for index in range(3):
        dut.serial.reset_efuses()
        dut.burn_wafer_version()
        dut.secure_boot_burn_en_bit()
        dut.serial.burn_efuse('SECURE_BOOT_KEY_REVOKE%d' % index, 1)
        dut.secure_boot_burn_digest('test_rsa_3072_key.pem', index, 0)
        dut.expect('secure boot verification failed', timeout=5)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test bootloader signature corruption.
# Corrupt one byte at a time of bootloader signature and test that the verification fails
@pytest.mark.esp32c3
# TODO: [ESP32C5] IDF-10043
# @pytest.mark.esp32c5
# TODO: [ESP32C5] IDF-10102
# @pytest.mark.esp32c61
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.timeout(18000)
# Increasing the test timeout to 18000s as the test runs for 384 iterations
# and thus the default 600s timeout is not sufficient
def test_examples_security_secure_boot_corrupt_bl_sig(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)

    bootloader_bin = os.path.join(dut.app.binary_path, 'bootloader/bootloader.bin')
    with open(bootloader_bin, 'rb') as f:
        signed_bl = f.read()

    seeds = range(0, 384)
    max_seed = max(seeds)

    for seed in seeds:
        print('Case %d / %d' % (seed, max_seed))
        corrupt_bl = corrupt_signature(signed_bl, seed=seed)
        with open('corrupt_bl.bin', 'wb') as corrupt_file:
            corrupt_file.write(corrupt_bl)
        dut.serial.reset_efuses()
        dut.burn_wafer_version()
        dut.serial.bootloader_flash('corrupt_bl.bin')
        dut.secure_boot_burn_en_bit()
        dut.secure_boot_burn_digest('test_rsa_3072_key.pem', 0, 0)
        # Though the order of flashing and burning efuse would not effect the test,
        # if we flash bootlader before burning en bit, even with no_stub = True
        # it still calls run_stub() and throws an error as it fails to start stub.
        dut.expect('Signature Check Failed', timeout=10)
    dut.serial.reset_efuses()
    dut.burn_wafer_version()


# Test app signature corruption.
# Corrupt app signature, one byte at a time, and test that the verification fails
@pytest.mark.esp32c3
# TODO: [ESP32C5] IDF-10043
# @pytest.mark.esp32c5
# TODO: [ESP32C5] IDF-10102
# @pytest.mark.esp32c61
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.timeout(18000)
# Increasing the test timeout to 18000s as the test runs for 385 iterations
# and thus the default 600s timeout is not sufficient
def test_examples_security_secure_boot_corrupt_app_sig(dut: Dut) -> None:
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=10)

    app_bin = os.path.join(dut.app.binary_path, 'secure_boot.bin')
    with open(app_bin, 'rb') as f:
        signed_app = f.read()

    seeds = range(0, 384)
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

    dut.expect('Sig block 0 invalid: {}'.format('CRC mismatch' if dut.target == 'esp32p4' or dut.target == 'esp32c5' else 'Stored CRC ends'), timeout=2)
    dut.expect('Secure boot signature verification failed', timeout=2)
    dut.expect('No bootable app partitions in the partition table', timeout=2)
