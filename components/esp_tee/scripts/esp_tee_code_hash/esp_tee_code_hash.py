#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import hashlib
import json
import os
import re
import struct
import sys
from typing import NamedTuple

import rich_click as click
from esp_pylib.logger import log

# The code hash definition is single-sourced from esp_tee_code_hash.h (shared with the
# device-side implementation of the code hash)
DEF_HEADER = os.path.join('components', 'esp_tee', 'include', 'esp_tee_code_hash.h')

# === Image Format (esp_image_header_t, esp_image_segment_header_t, esp_app_desc_t) ===

ESP_IMAGE_MAGIC = 0xE9
ESP_APP_DESC_MAGIC = 0xABCD5432
ESP_CHECKSUM_MAGIC = 0xEF
SHA256_LEN = 32
TEE_PROJECT_NAME = 'esp_tee'


class ImageHeader(NamedTuple):
    magic: int
    segment_count: int
    spi_mode: int
    spi_speed_size: int
    entry_addr: int
    wp_pin: int
    spi_pin_drv: bytes
    chip_id: int
    min_chip_rev: int
    min_chip_rev_full: int
    max_chip_rev_full: int
    reserved: bytes
    hash_appended: int


IMG_HDR_FMT = '<BBBBIB3sHBHH4sB'


class SegmentHeader(NamedTuple):
    load_addr: int
    data_len: int


SEG_HDR_FMT = '<II'


class AppDesc(NamedTuple):
    magic_word: int
    secure_version: int
    reserv1: bytes
    version: bytes
    project_name: bytes
    time: bytes
    date: bytes
    idf_ver: bytes
    app_elf_sha256: bytes
    min_efuse_blk_rev_full: int
    max_efuse_blk_rev_full: int
    mmu_page_size: int
    reserv3: bytes
    reserv2: bytes


APP_DESC_FMT = '<II8s32s32s16s16s32s32sHHB3s72s'


IMG_HDR_LEN = struct.calcsize(IMG_HDR_FMT)
SEG_HDR_LEN = struct.calcsize(SEG_HDR_FMT)
APP_DESC_LEN = struct.calcsize(APP_DESC_FMT)
APP_DESC_OFFS = IMG_HDR_LEN + SEG_HDR_LEN  # the app descriptor leads the first segment


# === Code Hash Definition ===


def load_code_hash_version(header: str | None = None) -> int:
    if header is None:
        here = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'include', 'esp_tee_code_hash.h')
        idf = os.path.join(os.environ.get('IDF_PATH', ''), DEF_HEADER)
        header = next((os.path.normpath(p) for p in (here, idf) if os.path.isfile(p)), None)
        if header is None:
            raise ValueError(f'Code hash definition header not found ({DEF_HEADER}); set IDF_PATH or pass --header')

    try:
        with open(header, encoding='utf-8') as f:
            match = re.search(r'#define\s+ESP_TEE_IMG_CODE_HASH_VER\s+\((\d+)\)', f.read())
    except OSError as e:
        raise ValueError(f'Cannot read the code hash definition header: {e}')
    if not match:
        raise ValueError(f'Failed to parse the code hash definition version from {header}')
    return int(match.group(1))


# === Image Parsing ===


def cstr(b: bytes) -> str:
    return b.split(b'\0', 1)[0].decode(errors='replace')


def parse_image(data: bytes) -> dict:
    if len(data) < IMG_HDR_LEN or data[0] != ESP_IMAGE_MAGIC:
        raise ValueError('Not an ESP image (bad magic)')
    hdr = ImageHeader(*struct.unpack_from(IMG_HDR_FMT, data))
    if not hdr.hash_appended:
        raise ValueError('Image has no appended SHA256 digest - not a TEE image?')

    # Walk the segments to find the true image length, accumulating the esptool XOR checksum
    offs = IMG_HDR_LEN
    checksum = ESP_CHECKSUM_MAGIC
    segments = []
    for _ in range(hdr.segment_count):
        if offs + SEG_HDR_LEN > len(data):
            raise ValueError('Image truncated (segment header exceeds the file size)')
        seg = SegmentHeader(*struct.unpack_from(SEG_HDR_FMT, data, offs))
        seg_data = data[offs + SEG_HDR_LEN : offs + SEG_HDR_LEN + seg.data_len]
        if len(seg_data) != seg.data_len:
            raise ValueError('Image truncated (segment data exceeds the file size)')
        for b in seg_data:
            checksum ^= b
        segments.append(seg)
        offs += SEG_HDR_LEN + seg.data_len
    segments_end = offs
    checksum_offs = ((offs + 1 + 15) & ~15) - 1  # the XOR checksum byte ends a 16-byte block
    image_len = checksum_offs + 1 + SHA256_LEN
    if image_len > len(data):
        raise ValueError('Image truncated (segment walk exceeds the file size)')
    if data[checksum_offs] != checksum:
        raise ValueError(f'Image checksum mismatch (expected 0x{checksum:02x}, found 0x{data[checksum_offs]:02x})')
    digest = data[checksum_offs + 1 : image_len]
    if hashlib.sha256(data[: checksum_offs + 1]).digest() != digest:
        raise ValueError('Image digest mismatch (corrupted image?)')

    if not segments or segments[0].data_len < APP_DESC_LEN:
        raise ValueError('Image too short to carry an app descriptor')
    desc = AppDesc(*struct.unpack_from(APP_DESC_FMT, data, APP_DESC_OFFS))
    if desc.magic_word != ESP_APP_DESC_MAGIC:
        raise ValueError('App descriptor magic not found - not a TEE image?')
    project_name = cstr(desc.project_name)
    if project_name != TEE_PROJECT_NAME:
        raise ValueError(f'Not a TEE image (project name: {project_name!r})')

    return {
        'image_len': image_len,
        'trailing': len(data) - image_len,
        'digest': digest.hex(),
        'checksum': checksum,
        'segments': segments,
        'segments_end': segments_end,
        'entry_addr': hdr.entry_addr,
        'chip_id': hdr.chip_id,
        'spi_mode': hdr.spi_mode,
        'spi_speed_size': hdr.spi_speed_size,
        'min_chip_rev_full': hdr.min_chip_rev_full,
        'max_chip_rev_full': hdr.max_chip_rev_full,
        'secure_version': desc.secure_version,
        'version': cstr(desc.version),
        'project_name': project_name,
        'idf_ver': cstr(desc.idf_ver),
        'elf_sha256': desc.app_elf_sha256.hex(),
        'min_efuse_blk_rev_full': desc.min_efuse_blk_rev_full,
        'max_efuse_blk_rev_full': desc.max_efuse_blk_rev_full,
        'mmu_page_size': desc.mmu_page_size,
    }


# === Code Hash ===


def compute_code_hash(data: bytes, header: str | None = None) -> dict:
    code_hash_ver = load_code_hash_version(header)
    info = parse_image(data)

    # Image segments (headers and data) without the app descriptor; the image header,
    # the XOR checksum (with its padding) and the appended digest are left out
    h = hashlib.sha256(data[IMG_HDR_LEN:APP_DESC_OFFS])
    h.update(data[APP_DESC_OFFS + APP_DESC_LEN : info['segments_end']])

    return {'code_hash': h.hexdigest(), 'code_hash_ver': code_hash_ver, 'info': info}


# === CLI ===


@click.command(context_settings=dict(help_option_names=['-h', '--help']))
@click.argument('image', type=click.Path(exists=True, dir_okay=False))
@click.option('-q', '--quiet', is_flag=True, default=False, help='print only the code hash')
@click.option('--json', 'as_json', is_flag=True, default=False, help='print the result as JSON')
@click.option('--expect', metavar='HEX', default=None, help='exit with status 1 unless the code hash equals HEX')
@click.option(
    '--header',
    type=click.Path(exists=True, dir_okay=False),
    default=None,
    help='path to esp_tee_code_hash.h (default: from this checkout or IDF_PATH)',
)
def main(image: str, quiet: bool, as_json: bool, expect: str | None, header: str | None) -> None:
    """Compute the code hash of an ESP-TEE firmware image."""
    try:
        with open(image, 'rb') as f:
            data = f.read()
        res = compute_code_hash(data, header)
    except (ValueError, OSError) as e:
        raise click.ClickException(str(e))

    matched = expect is None or res['code_hash'] == expect.strip().lower()
    info = res['info']

    if as_json:
        click.echo(json.dumps({'code_hash': res['code_hash'], 'code_hash_ver': res['code_hash_ver'], **info}, indent=2))
    elif quiet:
        click.echo(res['code_hash'])
    else:
        excluded = [
            ('version', f'{info["version"]} (project: {info["project_name"]})'),
            ('secure_version', info['secure_version']),
            ('idf_ver', info['idf_ver']),
            ('chip_id', f'{info["chip_id"]} (entry: 0x{info["entry_addr"]:08x})'),
            ('flash', f'mode=0x{info["spi_mode"]:02x} speed_size=0x{info["spi_speed_size"]:02x}'),
            ('chip_rev_full', f'min={info["min_chip_rev_full"]} max={info["max_chip_rev_full"]}'),
            ('efuse_blk_rev_full', f'min={info["min_efuse_blk_rev_full"]} max={info["max_efuse_blk_rev_full"]}'),
            ('mmu_page_size', f'2^{info["mmu_page_size"]}'),
            ('elf_sha256', f'{info["elf_sha256"][:16]}...'),
            ('xor_checksum', f'0x{info["checksum"]:02x}'),
        ]
        log.print(f'[+] Image: {image} ({info["image_len"]} bytes)', markup=False, soft_wrap=True)
        log.print(f'[+] Hashed segments (definition version {res["code_hash_ver"]}):', markup=False)
        for i, seg in enumerate(info['segments']):
            note = ' (app descriptor excluded)' if i == 0 else ''
            log.print(f'    {i}: load_addr=0x{seg.load_addr:08x} len=0x{seg.data_len:x}{note}', markup=False)
        log.print('[+] Excluded fields (invisible to the code hash):', markup=False)
        for name, value in excluded:
            log.print(f'    {name + ":":<20}{value}', markup=False)
        if info['trailing']:
            log.print(
                f'[!] {info["trailing"]} trailing bytes beyond the image (signature block?) were ignored',
                markup=False,
                soft_wrap=True,
            )
        log.print(f'[{"✓" if matched else "✗"}] Code hash: {res["code_hash"]}', markup=False, soft_wrap=True)
        if not matched:
            log.print(f'[✗] Expected:  {expect}', markup=False, soft_wrap=True)

    if not matched:
        sys.exit(1)


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
