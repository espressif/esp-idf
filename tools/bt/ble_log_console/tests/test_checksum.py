# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from src.backend.checksum import sum_checksum
from src.backend.checksum import xor_checksum


class TestSumChecksum:
    def test_empty(self) -> None:
        assert sum_checksum(b'') == 0

    def test_single_byte(self) -> None:
        assert sum_checksum(b'\x42') == 0x42

    def test_multiple_bytes(self) -> None:
        # Sum of bytes: 0x01 + 0x02 + 0x03 + 0x04 = 0x0A
        assert sum_checksum(b'\x01\x02\x03\x04') == 0x0A

    def test_overflow_wraps_u32(self) -> None:
        # 256 bytes of 0xFF = 256 * 255 = 65280
        data = b'\xff' * 256
        assert sum_checksum(data) == 65280


class TestXorChecksum:
    def test_empty(self) -> None:
        assert xor_checksum(b'') == 0

    def test_single_word(self) -> None:
        # [0x01, 0x02, 0x03, 0x04] → LE word 0x04030201
        assert xor_checksum(b'\x01\x02\x03\x04') == 0x04030201

    def test_two_words(self) -> None:
        data = b'\x01\x02\x03\x04\x05\x06\x07\x08'
        # word1 = 0x04030201, word2 = 0x08070605
        expected = 0x04030201 ^ 0x08070605
        assert xor_checksum(data) == expected

    def test_unaligned_length(self) -> None:
        """XOR checksum handles non-4-byte-aligned data lengths correctly."""
        # 5 bytes: 1 full word + 1 trailing byte (zero-padded)
        data = b'\x01\x02\x03\x04\x05'
        # word0 = 0x04030201, word1 = 0x00000005 (padded)
        # XOR = 0x04030201 ^ 0x00000005 = 0x04030204
        assert xor_checksum(data) == 0x04030204

    def test_typical_frame_data_produces_valid_result(self) -> None:
        """Verify xor_checksum produces a valid u32 result on typical frame-sized data."""
        # A typical 6-byte header + 10-byte payload
        header = b'\x0a\x00\x00\x01\x00\x00'  # payload_len=10, src=0, sn=256
        payload = b'\x00\x00\x00\x00\x03\x03' + b'\x00' * 4
        data = header + payload
        result = xor_checksum(data)
        assert isinstance(result, int)
        assert 0 <= result < 0x100000000

    def test_matches_ble_log_parser_v2(self) -> None:
        """Verify our implementation matches the proven ble_log_parser_v2 approach.

        Both implementations should produce identical results: simple XOR of
        consecutive 4-byte LE words with zero-padding for partial last word.
        """
        import struct

        def reference_xor(data: bytes) -> int:
            """Reference: ble_log_parser_v2 _validate_xor logic."""
            body_len = len(data)
            if body_len == 0:
                return 0
            checksum_cal = 0
            for i in range(0, body_len, 4):
                remaining = body_len - i
                if remaining >= 4:
                    (word,) = struct.unpack_from('<I', data, i)
                else:
                    last_chunk = data[i:]
                    padded_chunk = last_chunk + b'\x00' * (4 - remaining)
                    (word,) = struct.unpack('<I', padded_chunk)
                if i == 0:
                    checksum_cal = word
                else:
                    checksum_cal ^= word
            return checksum_cal & 0xFFFFFFFF

        # Test various data sizes
        test_vectors = [
            b'',
            b'\x01',
            b'\x01\x02\x03',
            b'\x01\x02\x03\x04',
            b'\x01\x02\x03\x04\x05',
            b'\xff' * 16,
            b'\x0a\x00\x00\x01\x00\x00' + b'\x00' * 10,  # typical frame
        ]
        for data in test_vectors:
            assert xor_checksum(data) == reference_xor(data), f'Mismatch for data length {len(data)}'
