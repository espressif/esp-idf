# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""Convert one WAV file to a C array of unsigned DAC codes.

8-bit codes are packed as uint8_t; 10-bit and 12-bit codes are packed as uint16_t.

Examples:
    python generate_audio_file.py --bitwidth 12 -o audio_example_file.h hi_idf_audio.wav
"""

import struct
import wave
from collections.abc import Iterable
from collections.abc import Iterator
from itertools import islice
from pathlib import Path
from typing import NamedTuple
from typing import TypeVar

import rich_click as click
from esp_pylib.logger import log

SUPPORTED_BITWIDTHS = (8, 10, 12)
C_ELEM_TYPES = {
    8: 'uint8_t',
    10: 'uint16_t',
    12: 'uint16_t',
}
T = TypeVar('T')


class PcmData(NamedTuple):
    samples: list[int]
    src_bits: int
    sample_rate: int


def _read_signed_pcm(filename: Path) -> PcmData:
    """Read signed samples and their bit width from a PCM WAV file.

    Multi-channel files use the first channel only. 8-bit WAV is unsigned and is
    converted to signed PCM; 16-bit WAV is treated as signed little-endian.
    """
    with wave.open(str(filename), 'r') as wav:
        params = wav.getparams()
        raw = wav.readframes(params.nframes)

    if params.sampwidth == 1:
        src_bits = 8
        signed = [sample - 128 for sample in raw]
    elif params.sampwidth == 2:
        src_bits = 16
        signed = list(struct.unpack(f'<{params.nframes * params.nchannels}h', raw))
    else:
        raise ValueError(
            f'{filename}: only 8-bit and 16-bit PCM WAV are supported (got {params.sampwidth}-byte samples)'
        )

    if params.nchannels > 1:
        signed = signed[0 :: params.nchannels]
    return PcmData(signed, src_bits, params.framerate)


def pcm_to_dac_code(pcm: int, src_bits: int, target_bits: int) -> int:
    """Map signed PCM to an unsigned DAC code with mid-scale bias."""
    max_code = (1 << target_bits) - 1
    mid = 1 << (target_bits - 1)
    shift = src_bits - target_bits
    if shift >= 0:
        code = (pcm >> shift) + mid
    else:
        code = (pcm << -shift) + mid
    return max(0, min(code, max_code))


def _chunked(iterable: Iterable[T], size: int) -> Iterator[list[T]]:
    if size <= 0:
        raise ValueError('chunk size must be positive')
    iterator = iter(iterable)
    while chunk := list(islice(iterator, size)):
        yield chunk


def get_wave_array_str(pcm_data: PcmData, target_bits: int) -> str:
    hex_width = (target_bits + 3) // 4
    codes = (f'0x{pcm_to_dac_code(pcm, pcm_data.src_bits, target_bits):0{hex_width}x},' for pcm in pcm_data.samples)
    return '\n'.join(' '.join(chunk) for chunk in _chunked(codes, 16))


def c_elem_type(bitwidth: int) -> str:
    return C_ELEM_TYPES[bitwidth]


def gen_wave_table(wav_file: Path, target_file_name: Path, scale_bits: int = 8) -> None:
    pcm_data = _read_signed_pcm(wav_file)
    elem_type = c_elem_type(scale_bits)
    with target_file_name.open('w', encoding='utf-8') as audio_table:
        audio_table.write('#include <stdint.h>\n')
        audio_table.write(f'#define AUDIO_SAMPLE_RATE_HZ {pcm_data.sample_rate}\n\n')
        audio_table.write(f'const {elem_type} audio_table[] = {{\n')
        audio_table.write(get_wave_array_str(pcm_data=pcm_data, target_bits=scale_bits))
        audio_table.write('\n};\n')


@click.command(
    context_settings={'help_option_names': ['-h', '--help']},
    help='Generate a C array of DAC codes from one WAV file.',
)
@click.option(
    '-b',
    '--bitwidth',
    type=click.Choice(tuple(str(bitwidth) for bitwidth in SUPPORTED_BITWIDTHS)),
    default='8',
    show_default=True,
    help='DAC code bit width.',
)
@click.option(
    '-o',
    '--output',
    type=click.Path(dir_okay=False, path_type=Path),
    default='audio_example_file.h',
    show_default=True,
    help='Output header path.',
)
@click.argument(
    'wav_file',
    type=click.Path(exists=True, dir_okay=False, readable=True, path_type=Path),
)
def cli(bitwidth: str, output: Path, wav_file: Path) -> None:
    scale_bits = int(bitwidth)
    log.print(f'Generating audio array from {wav_file} (bitwidth={scale_bits})...', markup=False, soft_wrap=True)
    gen_wave_table(wav_file=wav_file, target_file_name=output, scale_bits=scale_bits)
    log.print(f'Wrote {output}', markup=False, soft_wrap=True)


def main() -> None:
    cli()


if __name__ == '__main__':
    from esp_pylib.excepthook import install_exception_reporting

    install_exception_reporting()
    main()
