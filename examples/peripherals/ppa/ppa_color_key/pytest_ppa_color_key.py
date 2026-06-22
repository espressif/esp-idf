# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
    r'IMAGE_META effect=(?P<effect>\w+) width=(?P<width>\d+) height=(?P<height>\d+) '
    r'format=(?P<format>\w+) encoding=(?P<encoding>\w+)'
)
IMAGE_META_RE = re.compile(IMAGE_META_PATTERN)
IMAGE_CHUNK_PATTERN = r'IMAGE_BASE64 (?P<payload>[A-Za-z0-9+/=]+)'
IMAGE_CHUNK_RE = re.compile(IMAGE_CHUNK_PATTERN)
REPLACE_EFFECT_NAME = 'replace_keyed_text_with_glow'
BLEND_EFFECT_NAME = 'blend_glow_around_keyed_text'
EXPECTED_PIXEL_FORMAT = 'RGB565'
EXPECTED_ENCODING = 'base64'
RGB565_BYTES_PER_PIXEL = 2
RGB888_BYTES_PER_PIXEL = 3
PPM_MAGIC = b'P6'
PPM_MAX_VALUE = b'255'
PPM_HEADER_RE = re.compile(rb'^P6\s+(?P<width>\d+)\s+(?P<height>\d+)\s+(?P<max_value>\d+)\s')


@dataclass(frozen=True)
class ImageMetadata:
    effect: str
    width: int
    height: int
    pixel_format: str
    encoding: str

    @property
    def rgb565_size(self) -> int:
        return self.width * self.height * RGB565_BYTES_PER_PIXEL


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
        effect=match.group('effect'),
        width=int(match.group('width')),
        height=int(match.group('height')),
        pixel_format=match.group('format'),
        encoding=match.group('encoding'),
    )


def collect_base64_payload(dut: Dut) -> list[str]:
    payload_lines: list[str] = []
    while True:
        match = dut.expect(rf'(?P<line>IMAGE_BASE64_END|{IMAGE_CHUNK_PATTERN}\r?\n)')
        line = match.group('line').decode('utf-8').strip()
        if line == 'IMAGE_BASE64_END':
            return payload_lines

        chunk_match = IMAGE_CHUNK_RE.fullmatch(line)
        assert chunk_match is not None
        payload_lines.append(chunk_match.group('payload'))


def _rgb565_to_rgb888(raw_bytes: bytes) -> bytes:
    # PPM stores 8-bit RGB triplets, so expand each RGB565 pixel before writing the artifact.
    rgb_bytes = bytearray(len(raw_bytes) // 2 * 3)
    for pixel_index, offset in enumerate(range(0, len(raw_bytes), 2)):
        pixel = raw_bytes[offset] | (raw_bytes[offset + 1] << 8)
        red = (pixel >> 11) & 0x1F
        green = (pixel >> 5) & 0x3F
        blue = pixel & 0x1F
        rgb_offset = pixel_index * 3
        rgb_bytes[rgb_offset : rgb_offset + 3] = (
            (red << 3) | (red >> 2),
            (green << 2) | (green >> 4),
            (blue << 3) | (blue >> 2),
        )
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


def decode_rgb565_base64_image(metadata: ImageMetadata, payload_lines: list[str]) -> RgbImage:
    if metadata.pixel_format != EXPECTED_PIXEL_FORMAT:
        raise ValueError(f'Unsupported pixel format: {metadata.pixel_format}')
    if metadata.encoding != EXPECTED_ENCODING:
        raise ValueError(f'Unsupported payload encoding: {metadata.encoding}')

    raw_bytes = base64.b64decode(''.join(payload_lines), validate=True)
    if len(raw_bytes) != metadata.rgb565_size:
        raise ValueError(f'Expected {metadata.rgb565_size} decoded bytes, got {len(raw_bytes)}')

    return RgbImage(width=metadata.width, height=metadata.height, pixels_rgb888=_rgb565_to_rgb888(raw_bytes))


def save_ppm_artifact(image: RgbImage, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        output_path.write_bytes(_encode_ppm(image))
    except OSError:
        logging.exception('Failed to save PPA artifact to %s', output_path)
        return

    logging.info('Saved PPA artifact to %s', output_path)


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


def effect_output_name(effect: str) -> str:
    return f'ppa_color_key_{effect}.ppm'


def effect_golden_name(effect: str) -> str:
    return f'golden_{effect}.ppm'


def expect_and_check_effect_image(dut: Dut, effect: str) -> None:
    metadata_line = dut.expect(IMAGE_META_PATTERN).group(0).decode('utf-8')
    metadata = parse_image_metadata(metadata_line)
    assert metadata.effect == effect, f'Expected effect {effect}, got {metadata.effect}'

    dut.expect_exact('IMAGE_BASE64_BEGIN')
    base64_chunks = collect_base64_payload(dut)

    result_image = decode_rgb565_base64_image(metadata, base64_chunks)
    output_path = Path(dut.logdir) / effect_output_name(effect)
    save_ppm_artifact(result_image, output_path)
    assert_image_matches_golden(result_image, Path(__file__).with_name(effect_golden_name(effect)))


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_PPA_SUPPORTED == 1'), indirect=['target'])
def test_ppa_color_key(dut: Dut) -> None:
    dut.expect_exact('Loading embedded RGB565 image from flash...')
    dut.expect(r'Embedded raw image size: \d+ bytes')
    dut.expect_exact('Generating shared glow foreground...')

    dut.expect_exact('Replacing keyed red text with glow foreground...')
    expect_and_check_effect_image(dut, REPLACE_EFFECT_NAME)

    dut.expect_exact('Blending glow around keyed red text...')
    expect_and_check_effect_image(dut, BLEND_EFFECT_NAME)

    dut.expect_exact('PPA color key demo done.')
