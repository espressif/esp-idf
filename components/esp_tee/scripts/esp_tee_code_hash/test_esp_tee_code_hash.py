# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import hashlib
import importlib.util
import os
import struct
from collections.abc import Callable
from pathlib import Path

import pytest

TOOL = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'esp_tee_code_hash.py')
spec = importlib.util.spec_from_file_location('esp_tee_code_hash', TOOL)
assert spec is not None and spec.loader is not None
tool = importlib.util.module_from_spec(spec)
spec.loader.exec_module(tool)

# Code hash definition version 1 (any change to the header must bump the version and this reference)
CODE_HASH_VER = 1

# Image format (esp_image_header_t, esp_image_segment_header_t, esp_app_desc_t)
IMG_HDR_LEN = 24
SEG_HDR_LEN = 8
APP_DESC_LEN = 256
ESP_IMAGE_MAGIC = 0xE9
ESP_APP_DESC_MAGIC = 0xABCD5432
ESP_CHECKSUM_MAGIC = 0xEF


def make_app_desc(version: bytes = b'1.2.0~v6.2.0', project: bytes = b'esp_tee', mmu_page_size: int = 16) -> bytes:
    desc = bytearray(APP_DESC_LEN)
    struct.pack_into('<II', desc, 0, ESP_APP_DESC_MAGIC, 3)  # magic, secure_version
    desc[0x10 : 0x10 + len(version)] = version
    desc[0x30 : 0x30 + len(project)] = project
    desc[0x50:0x60] = b'12:34:56'.ljust(16, b'\0')  # time (empty in a real TEE image)
    desc[0x70:0x90] = b'v6.2-dev-1-gabc'.ljust(32, b'\0')  # idf_ver
    desc[0x90:0xB0] = bytes(range(32))  # app_elf_sha256
    struct.pack_into('<HHB', desc, 0xB0, 0, 199, mmu_page_size)  # efuse rev bounds, mmu_page_size
    desc[0xF0:0x100] = b'reserved'.ljust(16, b'\0')  # reserved tail
    return bytes(desc)


def make_image(
    payload: bytes = b'code' * 64,
    desc: bytes | None = None,
    flash_byte: int = 0x2F,
    hash_appended: int = 1,
    extra_segments: list[tuple[int, bytes]] | None = None,
    entry: int = 0x42000020,
    reserved: bytes = b'',
) -> bytes:
    segs = [(0x42000020, (desc or make_app_desc()) + payload)] + (extra_segments or [])
    hdr = bytearray(IMG_HDR_LEN)
    hdr[0], hdr[1], hdr[2], hdr[3] = ESP_IMAGE_MAGIC, len(segs), 0x02, flash_byte
    struct.pack_into('<I', hdr, 4, entry)
    hdr[8] = 0xEE
    struct.pack_into('<H', hdr, 0x0C, 23)  # chip_id
    struct.pack_into('<BHH', hdr, 0x0E, 1, 100, 199)
    hdr[0x13 : 0x13 + len(reserved)] = reserved  # reserved header bytes
    hdr[0x17] = hash_appended
    body = bytes(hdr)
    checksum = ESP_CHECKSUM_MAGIC
    for load_addr, seg in segs:
        body += struct.pack('<II', load_addr, len(seg)) + seg
        for b in seg:
            checksum ^= b
    body += bytes((15 - len(body) % 16) % 16) + bytes([checksum])  # pad so the checksum ends a 16-byte block
    return body + hashlib.sha256(body).digest()


def reference_hash(image: bytes) -> str:
    # Image segments (headers and data) without the app descriptor
    seg_count = image[1]
    stream = b''
    offs = IMG_HDR_LEN
    for i in range(seg_count):
        data_len = struct.unpack_from('<I', image, offs + 4)[0]
        seg = image[offs : offs + SEG_HDR_LEN + data_len]
        if i == 0:
            seg = seg[:SEG_HDR_LEN] + seg[SEG_HDR_LEN + APP_DESC_LEN :]
        stream += seg
        offs += SEG_HDR_LEN + data_len
    return hashlib.sha256(stream).hexdigest()


def test_code_hash_definition_is_pinned() -> None:
    assert tool.load_code_hash_version() == CODE_HASH_VER


def test_code_hash_matches_reference() -> None:
    img = make_image()
    res = tool.compute_code_hash(img)
    assert res['code_hash_ver'] == CODE_HASH_VER
    assert res['code_hash'] == reference_hash(img)
    assert res['info']['version'] == '1.2.0~v6.2.0'
    assert res['info']['image_len'] == len(img)
    assert res['info']['digest'] == img[-32:].hex()
    assert len(res['info']['segments']) == 1


def test_multi_segment_image_matches_reference() -> None:
    img = make_image(extra_segments=[(0x40800000, b'iram' * 100), (0x40810000, b'dram' * 7)])
    res = tool.compute_code_hash(img)
    assert res['code_hash'] == reference_hash(img)
    assert len(res['info']['segments']) == 3
    assert res['code_hash'] != tool.compute_code_hash(make_image())['code_hash']


def test_metadata_does_not_change_the_hash() -> None:
    base = tool.compute_code_hash(make_image())['code_hash']
    assert tool.compute_code_hash(make_image(desc=make_app_desc(version=b'9.9.9~v7.0.0')))['code_hash'] == base
    assert tool.compute_code_hash(make_image(flash_byte=0x1F))['code_hash'] == base
    assert tool.compute_code_hash(make_image(desc=make_app_desc(mmu_page_size=15)))['code_hash'] == base
    # Reserved header/descriptor bytes are invisible as well
    assert tool.compute_code_hash(make_image(reserved=b'\xaa\xbb\xcc\xdd'))['code_hash'] == base
    assert tool.compute_code_hash(make_image(entry=0x42000100))['code_hash'] == base


def test_code_changes_the_hash() -> None:
    assert (
        tool.compute_code_hash(make_image(b'code' * 64))['code_hash']
        != tool.compute_code_hash(make_image(b'c0de' * 64))['code_hash']
    )


def test_segment_layout_changes_the_hash() -> None:
    base = tool.compute_code_hash(make_image(extra_segments=[(0x40800000, b'iram' * 100)]))['code_hash']
    moved = tool.compute_code_hash(make_image(extra_segments=[(0x40800100, b'iram' * 100)]))['code_hash']
    assert base != moved


def test_trailing_bytes_are_ignored() -> None:
    img = make_image()
    res = tool.compute_code_hash(img + bytes(0xFF for _ in range(4096 - len(img) % 4096)) + b'SIGNATURE BLOCK')
    assert res['code_hash'] == tool.compute_code_hash(img)['code_hash']
    assert res['info']['trailing'] > 0


@pytest.mark.parametrize(
    'mutate, match',
    [
        (lambda img: img[:-40], 'truncated'),  # cut inside the checksum padding
        (lambda img: img[:-400], 'truncated'),  # cut inside a segment
        (lambda img: img[:20], 'bad magic'),  # cut inside the image header
        (lambda img: img[: len(img) - 33] + bytes([img[-33] ^ 0x01]) + img[-32:], 'checksum mismatch'),
        (lambda img: img[:-1] + bytes([img[-1] ^ 0x01]), 'digest mismatch'),
        (lambda img: b'\x00' + img[1:], 'bad magic'),
        (lambda img: make_image(desc=make_app_desc(project=b'hello_world')), 'Not a TEE image'),
        (lambda img: make_image(hash_appended=0), 'no appended SHA256'),
    ],
)
def test_damaged_or_foreign_images_are_rejected(mutate: Callable[[bytes], bytes], match: str) -> None:
    with pytest.raises(ValueError, match=match):
        tool.compute_code_hash(mutate(make_image()))


def test_bad_definition_header_is_rejected(tmp_path: Path) -> None:
    hdr = tmp_path / 'esp_tee_code_hash.h'
    hdr.write_text('/* no version here */\n')
    with pytest.raises(ValueError, match='Failed to parse'):
        tool.load_code_hash_version(str(hdr))
