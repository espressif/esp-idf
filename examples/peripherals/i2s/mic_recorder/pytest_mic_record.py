# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import base64
import logging
import os
import re
import struct
import wave
from dataclasses import dataclass
from pathlib import Path

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

I2S_TARGETS = soc_filtered_targets('SOC_I2S_SUPPORTED == 1')
PDM2PCM_TARGETS = soc_filtered_targets('SOC_I2S_SUPPORTS_PDM2PCM == 1')

MIC_RECORDING_OUTPUT_NAME = 'mic_recording.wav'
# Keep these values in sync with the PCM format constants in the firmware example.
EXPECTED_SAMPLE_RATE_HZ = 16000
EXPECTED_BITS_PER_SAMPLE = 16
EXPECTED_CHANNELS = 2
AUDIO_META_PATTERN = (
    r'AUDIO_META sample_rate=(?P<sample_rate>\d+) bits_per_sample=(?P<bits_per_sample>\d+) '
    r'channels=(?P<channels>\d+) data_size=(?P<data_size>\d+) encoding=(?P<encoding>\w+)'
)
AUDIO_META_RE = re.compile(AUDIO_META_PATTERN)
AUDIO_CHUNK_PATTERN = r'AUDIO_BASE64 (?P<payload>[A-Za-z0-9+/=]+)'
AUDIO_CHUNK_RE = re.compile(AUDIO_CHUNK_PATTERN)

# The example prints a mic-type specific banner before starting a recording.
DMIC_START_MARKER = 'PDM MIC recording example start'
AMIC_START_MARKER = 'ES8389 MIC recording example start'
DONE_MARKER = 'mic recorder example done'


@dataclass(frozen=True)
class AudioMetadata:
    sample_rate: int
    bits_per_sample: int
    channels: int
    data_size: int
    encoding: str

    @property
    def bytes_per_sample(self) -> int:
        if self.bits_per_sample % 8:
            raise ValueError(f'Unsupported sample size: {self.bits_per_sample} bits')
        return self.bits_per_sample // 8

    @property
    def frame_size(self) -> int:
        return self.channels * self.bytes_per_sample


def _parse_audio_metadata(meta_line: str) -> AudioMetadata:
    match = AUDIO_META_RE.fullmatch(meta_line)
    if not match:
        raise ValueError(f'Invalid audio metadata line: {meta_line}')

    return AudioMetadata(
        sample_rate=int(match.group('sample_rate')),
        bits_per_sample=int(match.group('bits_per_sample')),
        channels=int(match.group('channels')),
        data_size=int(match.group('data_size')),
        encoding=match.group('encoding'),
    )


def _collect_base64_payload(dut: Dut) -> list[str]:
    payload_chunks: list[str] = []
    while True:
        # Accept only framed audio lines, ignoring unrelated console output.
        match = dut.expect(rf'(?P<line>AUDIO_BASE64_END|{AUDIO_CHUNK_PATTERN}\r?\n)', timeout=40)
        line = match.group('line').decode('utf-8').strip()
        if line == 'AUDIO_BASE64_END':
            return payload_chunks

        chunk_match = AUDIO_CHUNK_RE.fullmatch(line)
        assert chunk_match is not None
        payload_chunks.append(chunk_match.group('payload'))


def _decode_pcm_base64(metadata: AudioMetadata, base64_chunks: list[str]) -> bytes:
    """Decode the console payload and verify its declared PCM layout."""
    if metadata.encoding != 'base64':
        raise ValueError(f'Unsupported payload encoding: {metadata.encoding}')
    if metadata.sample_rate <= 0 or metadata.channels not in (1, 2) or metadata.bits_per_sample != 16:
        raise ValueError(f'Unsupported PCM format: {metadata}')

    pcm_data = base64.b64decode(''.join(base64_chunks), validate=True)
    if len(pcm_data) != metadata.data_size:
        raise ValueError(f'Expected {metadata.data_size} PCM bytes, got {len(pcm_data)}')
    if len(pcm_data) % metadata.frame_size:
        raise ValueError(f'PCM data size is not aligned to {metadata.frame_size}-byte frames')
    return pcm_data


def _save_wav_artifact(metadata: AudioMetadata, pcm_data: bytes, output_path: Path) -> None:
    output_path.parent.mkdir(parents=True, exist_ok=True)
    try:
        with wave.open(str(output_path), 'wb') as wav_file:
            wav_file.setnchannels(metadata.channels)
            wav_file.setsampwidth(metadata.bytes_per_sample)
            wav_file.setframerate(metadata.sample_rate)
            wav_file.writeframes(pcm_data)
    except (OSError, wave.Error):
        logging.exception('Failed to save microphone recording artifact to %s', output_path)
        raise

    logging.info('Saved microphone recording artifact to %s', output_path)


def _log_pcm_stats(pcm_data: bytes, metadata: AudioMetadata) -> None:
    """Report the measured peak amplitude. Informational only: a peak check would be
    flaky on a board with no audio playing, where the recorded signal is near silence."""
    interleaved_sample_count = len(pcm_data) // metadata.bytes_per_sample
    peak = max((abs(v) for v in struct.unpack(f'<{interleaved_sample_count}h', pcm_data)), default=0)
    duration = len(pcm_data) / metadata.frame_size / metadata.sample_rate
    logging.info('PCM statistics: peak amplitude %d/32767, duration %.2f s', peak, duration)


def _collect_recording(dut: Dut) -> AudioMetadata:
    """Collect one recording, save its WAV artifact, and verify the console protocol."""
    metadata_line = dut.expect(AUDIO_META_PATTERN, timeout=10).group(0).decode('utf-8')
    metadata = _parse_audio_metadata(metadata_line)
    assert metadata.sample_rate == EXPECTED_SAMPLE_RATE_HZ
    assert metadata.bits_per_sample == EXPECTED_BITS_PER_SAMPLE
    assert metadata.channels == EXPECTED_CHANNELS
    dut.expect_exact('AUDIO_BASE64_BEGIN', timeout=5)
    pcm_data = _decode_pcm_base64(metadata, _collect_base64_payload(dut))
    _log_pcm_stats(pcm_data, metadata)
    _save_wav_artifact(metadata, pcm_data, Path(dut.logdir) / MIC_RECORDING_OUTPUT_NAME)

    dut.expect_exact(DONE_MARKER, timeout=5)
    return metadata


@pytest.mark.generic
@pytest.mark.parametrize('config', ['dmic'], indirect=True)
@idf_parametrize(
    'target',
    PDM2PCM_TARGETS,
    indirect=['target'],
)
def test_mic_recorder_dmic_example(dut: Dut) -> None:
    dut.expect_exact(DMIC_START_MARKER, timeout=10)
    _collect_recording(dut)


@pytest.mark.generic
@pytest.mark.parametrize('config', ['amic_es8389'], indirect=True)
@pytest.mark.skipif(os.getenv('CI_JOB_ID'), reason='no CI runner with an ES8389 codec')
@idf_parametrize(
    'target',
    I2S_TARGETS,
    indirect=['target'],
)
def test_mic_recorder_amic_example(dut: Dut) -> None:
    dut.expect_exact(AMIC_START_MARKER, timeout=10)
    _collect_recording(dut)
