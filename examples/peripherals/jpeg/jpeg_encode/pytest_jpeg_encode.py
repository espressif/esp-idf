# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import base64
import hashlib
import logging
import re
from dataclasses import dataclass
from pathlib import Path

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

JPEG_META_PATTERN = r'JPEG_META width=(\d+) height=(\d+) format=(\w+) encoding=(\w+) size=(\d+)'
JPEG_META_RE = re.compile(rf'^{JPEG_META_PATTERN}$')
JPEG_CHUNK_RE = re.compile(r'^JPEG_BASE64 ([A-Za-z0-9+/=]+)$')
JPEG_OUTPUT_NAME = 'jpeg_encode_result.jpeg'
GOLDEN_IMAGE_NAME = 'golden_output.jpeg'
GOLDEN_IMAGE_PATH = Path(__file__).with_name(GOLDEN_IMAGE_NAME)
EXPECTED_FORMAT = 'JPEG'
EXPECTED_ENCODING = 'base64'
EXPECTED_WIDTH = 1280
EXPECTED_HEIGHT = 720


@dataclass(frozen=True, slots=True)
class JpegMetadata:
    width: int
    height: int
    image_format: str
    encoding: str
    size: int


def parse_jpeg_metadata(meta_line: str) -> JpegMetadata:
    match = JPEG_META_RE.match(meta_line)
    if not match:
        raise ValueError(f'Invalid JPEG metadata line: {meta_line}')

    return JpegMetadata(
        width=int(match.group(1)),
        height=int(match.group(2)),
        image_format=match.group(3),
        encoding=match.group(4),
        size=int(match.group(5)),
    )


def collect_base64_payload(dut: Dut) -> list[str]:
    payload_chunks: list[str] = []
    while True:
        match = dut.expect(r'(JPEG_BASE64_END|JPEG_BASE64 [A-Za-z0-9+/=]+\r?\n)')
        line = match.group(1).decode('utf-8').strip()
        if line == 'JPEG_BASE64_END':
            return payload_chunks

        chunk_match = JPEG_CHUNK_RE.match(line)
        assert chunk_match is not None
        payload_chunks.append(chunk_match.group(1))


def decode_jpeg_base64_payload(metadata: JpegMetadata, payload_lines: list[str]) -> bytes:
    if metadata.image_format != EXPECTED_FORMAT:
        raise ValueError(f'Unsupported image format: {metadata.image_format}')
    if metadata.encoding != EXPECTED_ENCODING:
        raise ValueError(f'Unsupported payload encoding: {metadata.encoding}')

    jpeg_bytes = base64.b64decode(''.join(payload_lines), validate=True)
    if len(jpeg_bytes) != metadata.size:
        raise ValueError(f'Expected {metadata.size} JPEG bytes, got {len(jpeg_bytes)}')

    return jpeg_bytes


def save_jpeg_artifact(jpeg_bytes: bytes, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        output_path.write_bytes(jpeg_bytes)
    except OSError:
        logging.exception('Failed to save JPEG artifact to %s', output_path)
        return

    logging.info('Saved JPEG artifact to %s', output_path)


def _sha256_digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def assert_jpeg_matches_golden(result_bytes: bytes, golden_path: Path) -> None:
    assert golden_path.is_file(), f'Golden JPEG not found: {golden_path}'
    golden_bytes = golden_path.read_bytes()
    result_digest = _sha256_digest(result_bytes)
    golden_digest = _sha256_digest(golden_bytes)

    assert result_digest == golden_digest, (
        f'Generated JPEG does not match golden file: {golden_path.name} '
        f'(result sha256={result_digest}, golden sha256={golden_digest})'
    )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_JPEG_ENCODE_SUPPORTED == 1'), indirect=['target'])
def test_jpeg_encode_example(dut: Dut) -> None:
    dut.expect_exact('Loading embedded BGR24 image from flash...')
    dut.expect(r'Embedded raw image size: \d+ bytes')
    dut.expect_exact('JPEG encoder will read the embedded raw buffer directly from flash.')
    dut.expect_exact('Encoding BGR24(raw) -> JPEG...')
    dut.expect(r'Encoded JPEG size: \d+ bytes')

    metadata = parse_jpeg_metadata(dut.expect(JPEG_META_PATTERN).group(0).decode('utf-8'))
    assert metadata.width == EXPECTED_WIDTH
    assert metadata.height == EXPECTED_HEIGHT

    dut.expect_exact('JPEG_BASE64_BEGIN')
    payload_lines = collect_base64_payload(dut)

    jpeg_bytes = decode_jpeg_base64_payload(metadata, payload_lines)
    output_path = Path(dut.logdir) / JPEG_OUTPUT_NAME
    save_jpeg_artifact(jpeg_bytes, output_path)
    assert_jpeg_matches_golden(jpeg_bytes, GOLDEN_IMAGE_PATH)

    dut.expect_exact('JPEG encode demo done.')
