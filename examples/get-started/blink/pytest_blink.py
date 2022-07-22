# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import hashlib
import logging
import os

import pytest
from pytest_embedded_idf.dut import IdfDut


def verify_elf_sha256_embedding(bin_path: str, sha256_reported: str) -> None:
    elf_file = os.path.join(bin_path, 'blink.elf')
    sha256 = hashlib.sha256()
    with open(elf_file, 'rb') as f:
        sha256.update(f.read())
    sha256_expected = sha256.hexdigest()

    logging.info(f'ELF file SHA256: {sha256_expected}')
    logging.info(f'ELF file SHA256 (reported by the app): {sha256_reported}')

    # the app reports only the first several hex characters of the SHA256, check that they match
    if not sha256_expected.startswith(sha256_reported):
        raise ValueError('ELF file SHA256 mismatch')


@pytest.mark.supported_targets
@pytest.mark.generic
def test_blink(dut: IdfDut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'blink.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('blink_bin_size : {}KB'.format(bin_size // 1024))

    sha256_reported = dut.expect(r'ELF file SHA256:\s+([a-f0-9]+)').group(1).decode('utf-8')

    verify_elf_sha256_embedding(dut.app.binary_path, sha256_reported)
