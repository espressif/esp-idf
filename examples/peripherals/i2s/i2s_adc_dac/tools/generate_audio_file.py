# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import print_function

import os
import struct
import wave
from builtins import range

try:
    from typing import List
except ImportError:
    pass


def get_wave_array_str(filename, target_bits):  # type: (str, int) -> str
    wave_read = wave.open(filename, 'r')
    array_str = ''
    nchannels, sampwidth, framerate, nframes, comptype, compname = wave_read.getparams()
    sampwidth *= 8
    for i in range(wave_read.getnframes()):
        val, = struct.unpack('<H', wave_read.readframes(1))
        scale_val = (1 << target_bits) - 1
        cur_lim   = (1 << sampwidth) - 1
        # scale current data to 8-bit data
        val       = val * scale_val / cur_lim
        val       = int(val + ((scale_val + 1) // 2)) & scale_val
        array_str += '0x%x, ' % (val)
        if (i + 1) % 16 == 0:
            array_str += '\n'
    return array_str


def gen_wave_table(wav_file_list, target_file_name, scale_bits=8):  # type: (List[str], str, int) -> None
    with open(target_file_name, 'w') as audio_table:
        print('#include <stdio.h>', file=audio_table)
        print('const unsigned char audio_table[] = {', file=audio_table)
        for wav in wav_file_list:
            print('processing: {}'.format(wav))
            print(get_wave_array_str(filename=wav, target_bits=scale_bits), file=audio_table)
        print('};\n', file=audio_table)
    print('Done...')


if __name__ == '__main__':
    print('Generating audio array...')
    wav_list = []
    for wavefile in os.listdir('./'):
        if wavefile.endswith('.wav'):
            wav_list.append(wavefile)
    gen_wave_table(wav_file_list=wav_list, target_file_name='audio_example_file.h')
