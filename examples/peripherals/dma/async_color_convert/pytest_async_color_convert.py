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

IMAGE_META_PATTERN = (
    r'IMAGE_META width=(?P<width>\d+) height=(?P<height>\d+) '
    r'format=(?P<format>\w+) encoding=(?P<encoding>\w+)'
)
IMAGE_META_RE = re.compile(IMAGE_META_PATTERN)
IMAGE_CHUNK_PATTERN = r'IMAGE_BASE64 (?P<payload>[A-Za-z0-9+/=]+)'
IMAGE_CHUNK_RE = re.compile(IMAGE_CHUNK_PATTERN)
IMAGE_OUTPUT_NAME = 'async_color_convert_result.ppm'
GOLDEN_IMAGE_NAME = 'golden_result.ppm'
EXPECTED_PIXEL_FORMAT = 'BGR24'
EXPECTED_ENCODING = 'base64'
RGB888_BYTES_PER_PIXEL = 3
PPM_MAGIC = b'P6'
PPM_MAX_VALUE = b'255'
PPM_HEADER_RE = re.compile(rb'^P6\s+(?P<width>\d+)\s+(?P<height>\d+)\s+(?P<max_value>\d+)\s')


@dataclass(frozen=True)
class ImageMetadata:
    width: int
    height: int
    pixel_format: str
    encoding: str

    @property
    def bgr24_size(self) -> int:
        return self.width * self.height * RGB888_BYTES_PER_PIXEL


@dataclass(frozen=True)
class RgbImage:
    width: int
    height: int
    pixels_rgb888: bytes

    def __post_init__(self) -> None:
        expected_size = self.width * self.height * RGB888_BYTES_PER_PIXEL
        if len(self.pixels_rgb888) != expected_size:
            raise ValueError(f'Expected {expected_size} RGB bytes, got {len(self.pixels_rgb888)}')


def parse_image_metadata(meta_line: str) -> ImageMetadata:
    match = IMAGE_META_RE.fullmatch(meta_line)
    if not match:
        raise ValueError(f'Invalid image metadata line: {meta_line}')

    return ImageMetadata(
        width=int(match.group('width')),
        height=int(match.group('height')),
        pixel_format=match.group('format'),
        encoding=match.group('encoding'),
    )


def collect_base64_payload(dut: Dut) -> list[str]:
    payload_chunks: list[str] = []
    while True:
        match = dut.expect(rf'(?P<line>IMAGE_BASE64_END|{IMAGE_CHUNK_PATTERN}\r?\n)')
        line = match.group('line').decode('utf-8').strip()
        if line == 'IMAGE_BASE64_END':
            return payload_chunks

        chunk_match = IMAGE_CHUNK_RE.fullmatch(line)
        assert chunk_match is not None
        payload_chunks.append(chunk_match.group('payload'))


def _bgr24_to_rgb888(raw_bytes: bytes) -> bytes:
    rgb_bytes = bytearray(len(raw_bytes))
    for offset in range(0, len(raw_bytes), 3):
        blue, green, red = raw_bytes[offset : offset + 3]
        rgb_bytes[offset : offset + 3] = (red, green, blue)
    return bytes(rgb_bytes)


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
    expected_size = width * height * RGB888_BYTES_PER_PIXEL
    if len(pixel_data) != expected_size:
        raise ValueError(f'Expected {expected_size} PPM pixel bytes, got {len(pixel_data)}')

    return RgbImage(width=width, height=height, pixels_rgb888=pixel_data)


def decode_bgr24_base64_image(metadata: ImageMetadata, base64_chunks: list[str]) -> RgbImage:
    if metadata.pixel_format != EXPECTED_PIXEL_FORMAT:
        raise ValueError(f'Unsupported pixel format: {metadata.pixel_format}')
    if metadata.encoding != EXPECTED_ENCODING:
        raise ValueError(f'Unsupported payload encoding: {metadata.encoding}')

    raw_bytes = base64.b64decode(''.join(base64_chunks), validate=True)
    if len(raw_bytes) != metadata.bgr24_size:
        raise ValueError(f'Expected {metadata.bgr24_size} decoded bytes, got {len(raw_bytes)}')

    return RgbImage(width=metadata.width, height=metadata.height, pixels_rgb888=_bgr24_to_rgb888(raw_bytes))


def save_ppm_artifact(image: RgbImage, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        output_path.write_bytes(_encode_ppm(image))
    except OSError:
        logging.exception('Failed to save async color convert artifact to %s', output_path)
        return

    logging.info('Saved async color convert artifact to %s', output_path)


def image_digest(image: RgbImage) -> str:
    digest = hashlib.sha256()
    digest.update(image.width.to_bytes(4, 'big'))
    digest.update(image.height.to_bytes(4, 'big'))
    digest.update(image.pixels_rgb888)
    return digest.hexdigest()


def assert_image_matches_golden(result_image: RgbImage, golden_path: Path) -> None:
    assert golden_path.is_file(), f'Golden image not found: {golden_path}'
    golden_image = _load_ppm(golden_path)

    assert image_digest(result_image) == image_digest(golden_image), (
        f'Generated image does not match golden file: {golden_path.name}'
    )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_DMA2D_SUPPORTED == 1'), indirect=['target'])
def test_async_color_convert_example(dut: Dut) -> None:
    dut.expect_exact('Loading embedded UYVY image from flash...')
    dut.expect(r'Embedded image size: \d+ bytes')
    dut.expect_exact('Converting UYVY422 -> RGB888...')
    dut.expect(r'Converted image size: \d+ bytes')

    metadata_line = dut.expect(IMAGE_META_PATTERN).group(0).decode('utf-8')
    metadata = parse_image_metadata(metadata_line)

    dut.expect_exact('IMAGE_BASE64_BEGIN')
    base64_chunks = collect_base64_payload(dut)

    result_image = decode_bgr24_base64_image(metadata, base64_chunks)
    output_path = Path(dut.logdir) / IMAGE_OUTPUT_NAME
    save_ppm_artifact(result_image, output_path)
    assert_image_matches_golden(result_image, Path(__file__).with_name(GOLDEN_IMAGE_NAME))

    dut.expect_exact('Async color convert visual demo done.')
