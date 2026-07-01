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

DECODE_OUTPUT_NAME = 'jpeg_decode_result.ppm'
GOLDEN_OUTPUT_NAME = 'golden_output.ppm'
GOLDEN_OUTPUT_PATH = Path(__file__).with_name(GOLDEN_OUTPUT_NAME)
EXPECTED_PIXEL_FORMAT = 'RGB888'
EXPECTED_ENCODING = 'base64'
RGB888_BYTES_PER_PIXEL = 3
PPM_MAGIC = b'P6'
PPM_MAX_VALUE = b'255'
DECODE_INFO_PATTERN = (
    r'JPEG_DECODE_INFO width=(?P<width>\d+) height=(?P<height>\d+) '
    r'padded_width=(?P<padded_width>\d+) padded_height=(?P<padded_height>\d+) '
    r'format=(?P<format>\w+) encoding=(?P<encoding>\w+) size=(?P<size>\d+)'
)
DECODE_INFO_RE = re.compile(DECODE_INFO_PATTERN)
DECODE_CHUNK_PATTERN = r'JPEG_DECODE_BASE64 (?P<payload>[A-Za-z0-9+/=]+)'
DECODE_CHUNK_RE = re.compile(DECODE_CHUNK_PATTERN)
PPM_HEADER_RE = re.compile(rb'^P6\s+(?P<width>\d+)\s+(?P<height>\d+)\s+(?P<max_value>\d+)\s')


@dataclass(frozen=True, slots=True)
class DecodeMetadata:
    width: int
    height: int
    padded_width: int
    padded_height: int
    pixel_format: str
    encoding: str
    size: int

    def __post_init__(self) -> None:
        if self.width <= 0 or self.height <= 0:
            raise ValueError(f'Invalid dimensions: {self.width}x{self.height}')
        if self.padded_width < self.width or self.padded_height < self.height:
            raise ValueError(
                f'Padded size ({self.padded_width}x{self.padded_height}) '
                f'smaller than visible size ({self.width}x{self.height})'
            )
        if self.pixel_format != EXPECTED_PIXEL_FORMAT:
            raise ValueError(f'Unsupported pixel format: {self.pixel_format}')
        if self.encoding != EXPECTED_ENCODING:
            raise ValueError(f'Unsupported encoding: {self.encoding}')

    @property
    def padded_image_size(self) -> int:
        return self.padded_width * self.padded_height * RGB888_BYTES_PER_PIXEL


@dataclass(frozen=True, slots=True)
class RgbImage:
    width: int
    height: int
    pixels_rgb888: bytes

    def __post_init__(self) -> None:
        expected_size = self.width * self.height * RGB888_BYTES_PER_PIXEL
        if len(self.pixels_rgb888) != expected_size:
            raise ValueError(f'Expected {expected_size} RGB bytes, got {len(self.pixels_rgb888)}')


def parse_decode_metadata(meta_line: str) -> DecodeMetadata:
    match = DECODE_INFO_RE.fullmatch(meta_line)
    if not match:
        raise ValueError(f'Invalid decode metadata line: {meta_line}')

    return DecodeMetadata(
        width=int(match.group('width')),
        height=int(match.group('height')),
        padded_width=int(match.group('padded_width')),
        padded_height=int(match.group('padded_height')),
        pixel_format=match.group('format'),
        encoding=match.group('encoding'),
        size=int(match.group('size')),
    )


def collect_base64_payload(dut: Dut) -> list[str]:
    payload_lines: list[str] = []
    while True:
        # The example prints the decoded frame as multiple short UART lines
        # instead of one giant base64 blob, so collect and join them here.
        match = dut.expect(rf'(?P<line>JPEG_DECODE_BASE64_END|{DECODE_CHUNK_PATTERN}\r?\n)', timeout=60)
        line = match.group('line').decode('utf-8').strip()
        if line == 'JPEG_DECODE_BASE64_END':
            return payload_lines

        chunk_match = DECODE_CHUNK_RE.fullmatch(line)
        assert chunk_match is not None
        payload_lines.append(chunk_match.group('payload'))


def _crop_visible_bgr888(raw_bytes: bytes, metadata: DecodeMetadata) -> bytes:
    # The hardware can write into a padded decode buffer whose width/height are
    # rounded up to JPEG block boundaries. Pytest only wants the visible image,
    # so keep the useful bytes from each row and discard the padded tail rows.
    if len(raw_bytes) != metadata.padded_image_size:
        raise ValueError(f'Expected {metadata.padded_image_size} padded BGR bytes, got {len(raw_bytes)}')

    visible_row_size = metadata.width * RGB888_BYTES_PER_PIXEL
    padded_row_size = metadata.padded_width * RGB888_BYTES_PER_PIXEL
    return b''.join(
        raw_bytes[offset : offset + visible_row_size]
        for offset in range(0, padded_row_size * metadata.height, padded_row_size)
    )


def _bgr888_to_rgb888(raw_bytes: bytes) -> bytes:
    # The decoder's RGB888 mode uses BGR24 byte layout by default. Swap the
    # first and third byte in each pixel so the PPM artifact becomes standard
    # RGB order that common desktop image tools expect.
    rgb_bytes = bytearray(raw_bytes)
    rgb_bytes[0::3], rgb_bytes[2::3] = raw_bytes[2::3], raw_bytes[0::3]
    return bytes(rgb_bytes)


def decode_base64_image(metadata: DecodeMetadata, payload_lines: list[str]) -> RgbImage:
    # The DUT sends the raw decode buffer as base64 over UART because the test
    # environment only observes text logs. Rebuild bytes on the host, crop away
    # decoder padding, then normalize the pixel order for image comparison.
    raw_bytes = base64.b64decode(''.join(payload_lines), validate=True)
    if len(raw_bytes) != metadata.size:
        raise ValueError(f'Expected {metadata.size} decoded bytes, got {len(raw_bytes)}')

    visible_bgr888 = _crop_visible_bgr888(raw_bytes, metadata)
    return RgbImage(
        width=metadata.width,
        height=metadata.height,
        pixels_rgb888=_bgr888_to_rgb888(visible_bgr888),
    )


def _encode_ppm(image: RgbImage) -> bytes:
    header = b'%s\n%d %d\n%s\n' % (PPM_MAGIC, image.width, image.height, PPM_MAX_VALUE)
    return header + image.pixels_rgb888


def _load_ppm(path: Path) -> RgbImage:
    ppm_bytes = path.read_bytes()
    header_match = PPM_HEADER_RE.match(ppm_bytes)
    if not header_match:
        raise ValueError('Invalid PPM header')

    width = int(header_match.group('width'))
    height = int(header_match.group('height'))
    max_value = header_match.group('max_value')
    if width <= 0 or height <= 0:
        raise ValueError('Unsupported PPM dimensions')
    if max_value != PPM_MAX_VALUE:
        raise ValueError(f'Unsupported PPM max value: {max_value.decode("ascii", errors="replace")}')

    pixel_data = ppm_bytes[header_match.end() :]
    return RgbImage(width=width, height=height, pixels_rgb888=pixel_data)


def save_ppm_artifact(image: RgbImage, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        output_path.write_bytes(_encode_ppm(image))
    except OSError:
        logging.exception('Failed to save JPEG decode artifact to %s', output_path)
        return

    logging.info('Saved JPEG decode artifact to %s', output_path)


def image_digest(image: RgbImage) -> str:
    digest = hashlib.sha256()
    digest.update(image.width.to_bytes(4, 'big'))
    digest.update(image.height.to_bytes(4, 'big'))
    digest.update(image.pixels_rgb888)
    return digest.hexdigest()


def assert_image_matches_golden(result_image: RgbImage, golden_path: Path) -> None:
    assert golden_path.is_file(), f'Golden PPM not found: {golden_path}'
    golden_image = _load_ppm(golden_path)

    assert image_digest(result_image) == image_digest(golden_image), (
        f'Generated image does not match golden file: {golden_path.name}'
    )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_JPEG_DECODE_SUPPORTED == 1'), indirect=['target'])
def test_jpeg_decode_example(dut: Dut) -> None:
    dut.expect_exact('Loading embedded JPEG from flash...')
    dut.expect(r'Embedded JPEG size: \d+ bytes')
    dut.expect(r'JPEG header parsed: width=\d+ height=\d+')
    dut.expect_exact('Decoding JPEG -> RGB888...')
    dut.expect(r'Decoded RGB888 size: \d+ bytes')

    metadata_line = dut.expect(DECODE_INFO_PATTERN).group(0).decode('utf-8')
    metadata = parse_decode_metadata(metadata_line)

    dut.expect_exact('JPEG_DECODE_BASE64_BEGIN')
    # Collect the machine-readable payload before the example prints its final
    # completion line so we keep the UART parsing strictly in output order.
    payload_lines = collect_base64_payload(dut)
    dut.expect_exact('JPEG decode demo done.')

    result_image = decode_base64_image(metadata, payload_lines)
    output_path = Path(dut.logdir) / DECODE_OUTPUT_NAME
    save_ppm_artifact(result_image, output_path)
    assert_image_matches_golden(result_image, GOLDEN_OUTPUT_PATH)
