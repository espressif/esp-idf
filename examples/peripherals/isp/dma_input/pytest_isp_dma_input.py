# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import base64
import logging
import re
from dataclasses import dataclass
from pathlib import Path

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

IMAGE_META_PATTERN = (
    r'IMAGE_META frame=(?P<frame>\d+) width=(?P<width>\d+) height=(?P<height>\d+) '
    r'format=(?P<format>\w+) encoding=(?P<encoding>\w+)'
)
IMAGE_META_RE = re.compile(IMAGE_META_PATTERN)
IMAGE_CHUNK_PATTERN = (
    r'IMAGE_BASE64 (?P<payload>[A-Za-z0-9+/=]+?)'
    r'(?=\r?\n|IMAGE_BASE64|IMAGE_BASE64_END|Frame )'
)
IMAGE_OUTPUT_TEMPLATE = 'isp_dma_input_frame{frame:02d}.ppm'
REFERENCE_IMAGE_PATH = Path(__file__).parent / 'golden' / 'golden.ppm'
EXPECTED_PIXEL_FORMAT = 'BGR24'
EXPECTED_ENCODING = 'base64'
RGB888_BYTES_PER_PIXEL = 3
PPM_MAGIC = b'P6'
PPM_MAX_VALUE = b'255'


@dataclass(frozen=True)
class ImageMetadata:
    frame: int
    width: int
    height: int
    pixel_format: str
    encoding: str


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
        frame=int(match.group('frame')),
        width=int(match.group('width')),
        height=int(match.group('height')),
        pixel_format=match.group('format'),
        encoding=match.group('encoding'),
    )


def collect_base64_payload(dut: Dut) -> list[str]:
    payload_lines: list[str] = []
    while True:
        match = dut.expect(rf'IMAGE_BASE64_END|{IMAGE_CHUNK_PATTERN}', timeout=60)
        if match.group(0).decode('utf-8') == 'IMAGE_BASE64_END':
            return payload_lines

        payload_lines.append(match.group('payload').decode('utf-8'))


def _bgr24_to_rgb888(raw_bytes: bytes) -> bytes:
    rgb_bytes = bytearray(len(raw_bytes))
    for offset in range(0, len(raw_bytes), 3):
        blue, green, red = raw_bytes[offset : offset + 3]
        rgb_bytes[offset : offset + 3] = (red, green, blue)
    return bytes(rgb_bytes)


def _encode_ppm(image: RgbImage) -> bytes:
    header = b'%s\n%d %d\n%s\n' % (PPM_MAGIC, image.width, image.height, PPM_MAX_VALUE)
    return header + image.pixels_rgb888


def decode_bgr24_base64_image(metadata: ImageMetadata, payload_lines: list[str]) -> RgbImage:
    if metadata.pixel_format != EXPECTED_PIXEL_FORMAT:
        raise ValueError(f'Unsupported pixel format: {metadata.pixel_format}')
    if metadata.encoding != EXPECTED_ENCODING:
        raise ValueError(f'Unsupported payload encoding: {metadata.encoding}')

    raw_bytes = base64.b64decode(''.join(payload_lines), validate=True)
    expected_size = metadata.width * metadata.height * RGB888_BYTES_PER_PIXEL
    if len(raw_bytes) != expected_size:
        raise ValueError(f'Expected {expected_size} decoded bytes, got {len(raw_bytes)}')

    return RgbImage(width=metadata.width, height=metadata.height, pixels_rgb888=_bgr24_to_rgb888(raw_bytes))


def save_ppm_artifact(image: RgbImage, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        output_path.write_bytes(_encode_ppm(image))
    except OSError:
        logging.exception('Failed to save ISP DMA artifact to %s', output_path)
        return

    logging.info('Saved ISP DMA artifact to %s', output_path)


def load_ppm_image(image_path: Path) -> RgbImage:
    ppm_data = image_path.read_bytes()
    try:
        magic, dimensions, max_value, pixels_rgb888 = ppm_data.split(b'\n', 3)
        width, height = (int(value) for value in dimensions.split())
    except ValueError as error:
        raise ValueError(f'Invalid PPM image: {image_path}') from error

    if magic != PPM_MAGIC or max_value != PPM_MAX_VALUE:
        raise ValueError(f'Unsupported PPM image: {image_path}')

    return RgbImage(width=width, height=height, pixels_rgb888=pixels_rgb888)


def assert_image_matches_reference(result_image: RgbImage, reference_image: RgbImage) -> None:
    assert (result_image.width, result_image.height) == (reference_image.width, reference_image.height), (
        f'ISP output dimensions do not match reference image: '
        f'{result_image.width}x{result_image.height} != {reference_image.width}x{reference_image.height}'
    )

    if result_image.pixels_rgb888 == reference_image.pixels_rgb888:
        return

    mismatch_offset = next(
        offset
        for offset, (actual, expected) in enumerate(zip(result_image.pixels_rgb888, reference_image.pixels_rgb888))
        if actual != expected
    )
    pixel_index, channel = divmod(mismatch_offset, RGB888_BYTES_PER_PIXEL)
    raise AssertionError(
        f'ISP output does not match reference image at pixel {pixel_index}, channel {channel}: '
        f'{result_image.pixels_rgb888[mismatch_offset]} != {reference_image.pixels_rgb888[mismatch_offset]}'
    )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_ISP_SUPPORTED == 1'), indirect=['target'])
def test_isp_dma_input_example(dut: Dut) -> None:
    reference_image = load_ppm_image(REFERENCE_IMAGE_PATH)
    frame_count_match = dut.expect(r'Feeding (?P<frame_count>\d+) frames through ISP DMA input...')
    expected_frame_count = int(frame_count_match.group('frame_count').decode('utf-8'))
    logging.info('Expecting %d ISP DMA frame(s)', expected_frame_count)

    for expected_frame in range(expected_frame_count):
        metadata = parse_image_metadata(dut.expect(IMAGE_META_PATTERN).group(0).decode('utf-8'))
        assert metadata.frame == expected_frame, f'Out-of-order frame: expected {expected_frame}, got {metadata.frame}'
        logging.info(
            'Received frame metadata: frame=%d size=%dx%d format=%s',
            metadata.frame,
            metadata.width,
            metadata.height,
            metadata.pixel_format,
        )
        dut.expect_exact('IMAGE_BASE64_BEGIN')
        logging.info('Receiving base64 image payload for frame %d', metadata.frame)
        payload_lines = collect_base64_payload(dut)
        logging.info('Received %d base64 chunk(s) for frame %d', len(payload_lines), metadata.frame)

        result_image = decode_bgr24_base64_image(metadata, payload_lines)
        logging.info('Decoded frame %d to RGB888 image', metadata.frame)
        output_path = Path(dut.logdir) / IMAGE_OUTPUT_TEMPLATE.format(frame=metadata.frame)
        save_ppm_artifact(result_image, output_path)
        assert_image_matches_reference(result_image, reference_image)
        dut.expect_exact(f'Frame {expected_frame} done')

    dut.expect_exact('ISP DMA visual demo done.')
