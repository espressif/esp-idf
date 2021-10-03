from __future__ import print_function

import os
import struct
import zlib
from io import BytesIO

import ttfw_idf

# To prepare a runner for these tests,
# 1. Connect an FPGA with C3 image
# 2. Use a COM port for programming and export it as ESPPORT
#    e.g export ESPPORT=/dev/ttyUSB0
# 3. Use another COM port for resetting efuses and connect its DTR pin to efuse reset pin on the FPGA board.
#    Export it as EFUSEPORT
#    e.g export EFUSEPORT=/dev/ttyUSB1
# 4. Run these tests


def corrupt_signature(signed_bootloader, seed=0, corrupt_sig=True, corrupt_crc=False, corrupt_single_block=None):
    # type: (bytes, int, bool, bool, int) -> bytes
    image = signed_bootloader[:-4096]
    signature = signed_bootloader[-4096:]
    sig_blocks = (signature[0:1216], signature[1216:2432], signature[2432:3648])
    new_blocks = tuple(corrupt_sig_block(s, seed, corrupt_sig, corrupt_crc) for s in sig_blocks)

    # if corrupt_single_block is None, corrupt all blocks
    # otherwise, only corrupt the one with that index set
    corr_sig_blocks = tuple(new_blocks[n] if corrupt_single_block in [None, n] else sig_blocks[n] for n in range(3))

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


def dut_start_secure_app(dut):      # type: (ttfw_idf.IDFDUT) -> None
    dut.reset_efuses()
    bootloader_bin = os.path.join(dut.app.binary_path, 'bootloader/bootloader.bin')
    with open(bootloader_bin, 'rb') as f:
        dut.write_flash_data([(0x0, f)], None, True, False)
    dut.start_app()


# Test secure boot flow.
# Correctly signed bootloader + correctly signed app should work
@ttfw_idf.idf_custom_test(env_tag='Example_Secure_Boot', target=['esp32c3fpga', 'esp32s3fpga'], ignore=True)
def test_examples_security_secure_boot(env, _):    # type: (ttfw_idf.TinyFW.Env, None) -> None
    efuse_port = os.getenv('EFUSEPORT')
    dut = env.get_dut('secure_boot', 'tools/test_apps/security/secure_boot', efuse_reset_port=efuse_port)
    dut_start_secure_app(dut)
    dut.expect('Secure Boot is enabled', timeout=2)


# Test efuse key index and key block combination.
# Any key index can be written to any key block and should work
@ttfw_idf.idf_custom_test(env_tag='Example_Secure_Boot', target=['esp32c3fpga', 'esp32s3fpga'], ignore=True)
def test_examples_security_secure_boot_key_combo(env, _):      # type: (ttfw_idf.TinyFW.Env, None) -> None
    efuse_port = os.getenv('EFUSEPORT')
    dut = env.get_dut('secure_boot', 'tools/test_apps/security/secure_boot', efuse_reset_port=efuse_port)
    dut_start_secure_app(dut)
    for index in range(3):
        for block in range(6):
            dut.reset_efuses()
            dut.secure_boot_burn_en_bit()
            dut.secure_boot_burn_digest('test_rsa_3072_key.pem', index, block)
            dut.reset()
            dut.expect('Secure Boot is enabled', timeout=2)


# Test secure boot key revoke.
# If a key is revoked, bootloader signed with that key should fail verification
@ttfw_idf.idf_custom_test(env_tag='Example_Secure_Boot', target=['esp32c3fpga', 'esp32s3fpga'], ignore=True)
def test_examples_security_secure_boot_key_revoke(env, _):     # type: (ttfw_idf.TinyFW.Env, None) -> None
    efuse_port = os.getenv('EFUSEPORT')
    dut = env.get_dut('secure_boot', 'tools/test_apps/security/secure_boot', efuse_reset_port=efuse_port)
    dut_start_secure_app(dut)
    for index in range(3):
        dut.reset_efuses()
        dut.secure_boot_burn_en_bit()
        dut.secure_boot_burn_digest('test_rsa_3072_key.pem', index, 0)
        dut.burn_efuse('SECURE_BOOT_KEY_REVOKE%d' % index, 1)
        dut.reset()
        dut.expect('secure boot verification failed', timeout=2)


# Test bootloader signature corruption.
# Corrupt one byte at a time of bootloader signature and test that the verification fails
@ttfw_idf.idf_custom_test(env_tag='Example_Secure_Boot', target=['esp32c3fpga', 'esp32s3fpga'], ignore=True)
def test_examples_security_secure_boot_corrupt_bl_sig(env, _):     # type: (ttfw_idf.TinyFW.Env, None) -> None
    efuse_port = os.getenv('EFUSEPORT')
    dut = env.get_dut('secure_boot', 'tools/test_apps/security/secure_boot', efuse_reset_port=efuse_port)
    dut.reset_efuses()
    dut.secure_boot_burn_en_bit()
    dut.secure_boot_burn_digest('test_rsa_3072_key.pem', 0, 0)
    bootloader_bin = os.path.join(dut.app.binary_path, 'bootloader/bootloader.bin')
    with open(bootloader_bin, 'rb') as f:
        signed_bl = f.read()

    seeds = range(0, 384)
    max_seed = max(seeds)

    for seed in seeds:
        print('Case %d / %d' % (seed, max_seed))
        corrupt_bl = corrupt_signature(signed_bl, seed=seed)
        dut.write_flash_data([(0x0, BytesIO(corrupt_bl))])
        dut.expect('Signature Check Failed', timeout=2)


# Test app signature corruption.
# Corrupt app signature, one byte at a time, and test that the verification fails
@ttfw_idf.idf_custom_test(env_tag='Example_Secure_Boot', target=['esp32c3fpga', 'esp32s3fpga'], ignore=True)
def test_examples_security_secure_boot_corrupt_app_sig(env, _):    # type: (ttfw_idf.TinyFW.Env, None) -> None
    efuse_port = os.getenv('EFUSEPORT')
    dut = env.get_dut('secure_boot', 'tools/test_apps/security/secure_boot', efuse_reset_port=efuse_port)
    dut_start_secure_app(dut)
    dut.reset_efuses()
    dut.secure_boot_burn_en_bit()
    dut.secure_boot_burn_digest('test_rsa_3072_key.pem', 0, 0)
    app_bin = os.path.join(dut.app.binary_path, 'secure_boot.bin')
    with open(app_bin, 'rb') as f:
        signed_app = f.read()

    seeds = range(0, 384)
    max_seed = max(seeds)

    for seed in seeds:
        print('Case %d / %d' % (seed, max_seed))
        corrupt_app = corrupt_signature(signed_app, seed=seed)
        dut.write_flash_data([(0x20000, BytesIO(corrupt_app))])
        dut.expect('Signature Check Failed', timeout=2)
        dut.expect('image valid, signature bad', timeout=2)

    print('Testing invalid CRC...')
    # Valid signature but invalid CRC
    dut.reset_efuses()
    dut.secure_boot_burn_en_bit()
    dut.secure_boot_burn_digest('test_rsa_3072_key.pem', 0, 0)

    corrupt_app = corrupt_signature(signed_app, corrupt_sig=False, corrupt_crc=True)
    dut.write_flash_data([(0x20000, BytesIO(corrupt_app))])
    dut.expect('Sig block 0 invalid: Stored CRC ends', timeout=2)
    dut.expect('Secure boot signature verification failed', timeout=2)
    dut.expect('No bootable app partitions in the partition table', timeout=2)


if __name__ == '__main__':
    test_examples_security_secure_boot()
    test_examples_security_secure_boot_key_combo()
    test_examples_security_secure_boot_key_revoke()
    test_examples_security_secure_boot_corrupt_bl_sig()
    test_examples_security_secure_boot_corrupt_app_sig()
