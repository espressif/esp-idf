#!/usr/bin/env python

from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
import re
import os
import hashlib

from tiny_test_fw import Utility
import ttfw_idf


def verify_elf_sha256_embedding(dut):
    elf_file = os.path.join(dut.app.binary_path, "blink.elf")
    sha256 = hashlib.sha256()
    with open(elf_file, "rb") as f:
        sha256.update(f.read())
    sha256_expected = sha256.hexdigest()

    dut.reset()
    sha256_reported = dut.expect(re.compile(r'ELF file SHA256:\s+([a-f0-9]+)'), timeout=5)[0]

    Utility.console_log('ELF file SHA256: %s' % sha256_expected)
    Utility.console_log('ELF file SHA256 (reported by the app): %s' % sha256_reported)
    # the app reports only the first several hex characters of the SHA256, check that they match
    if not sha256_expected.startswith(sha256_reported):
        raise ValueError('ELF file SHA256 mismatch')


@ttfw_idf.idf_example_test(env_tag="Example_GENERIC")
def test_examples_blink(env, extra_data):
    dut = env.get_dut("blink", "examples/get-started/blink", dut_class=ttfw_idf.ESP32DUT)
    binary_file = os.path.join(dut.app.binary_path, "blink.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("blink_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("blink_bin_size", bin_size // 1024, dut.TARGET)

    dut.start_app()

    verify_elf_sha256_embedding(dut)


if __name__ == '__main__':
    test_examples_blink()
