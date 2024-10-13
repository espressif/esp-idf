# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# This dumper script run on the host after get the probed raw data on the host
# Usage: python vcd_dumper.py -n <probe_channel_number> -f <path_to_raw_data_file>

import argparse
import time
from io import FileIO, TextIOWrapper


class VCDDumper():
    def __init__(self) -> None:
        pass

    def open_new_vcd_file(self, chan_num: int, vcd_out_io:TextIOWrapper) -> None:
        self.ts = 0
        self.odd_samples = 0
        self.prev = 0
        self.chan_num = chan_num
        self.step = 1 if chan_num <= 8 else 2
        # vcd file name
        if vcd_out_io:
            self.vcd = vcd_out_io
        else:
            dump_file = 'probe_{}.vcd'.format(time.strftime('%m-%d_%H-%M-%S'))
            self.vcd = open(dump_file, 'w', encoding='utf-8')

        # Write VCD header
        self.vcd.write('$date today $end\n'
                       '$timescale 1 ns $end\n'
                       '$scope module logic_analyzer $end\n')
        for i in range(chan_num):
            self.vcd.write(f'$var wire 1 {chr(33 + i)} chan_{i} $end\n')
        self.vcd.write('$upscope $end\n'
                       '$enddefinitions $end\n'
                       '$dumpvars\n')
        for i in range(chan_num):
            self.vcd.write(f'0{chr(33 + i)}\n')
        self.vcd.write('$end\n')

    def close_vcd_file(self) -> None:
        if self.vcd:
            self.vcd.close()

    def _dump_one_sample_to_vcd(self, prev:int, curr:int) -> None:
        change_mask = prev ^ curr
        if change_mask == 0:
            return
        self.vcd.write(f'#{self.ts}\n')
        i = 0
        while change_mask != 0:
            if change_mask & 1:
                self.vcd.write(f'{(curr >> i) & 1}{chr(33 + i)}\n')
            change_mask >>= 1
            i += 1

    def dump_samples(self, raw_data:bytes) -> None:
        # Parse the raw data and write VCD body
        byte_cnt = 0
        # If last time is one byte left, load it
        if self.odd_samples:
            samples = self.odd_samples
            self.odd_samples = 0
            byte_cnt = 1
        else:
            samples = 0

        # Loop the raw data and parse byte by byte
        for byte in raw_data:
            byte_cnt += 1
            if byte_cnt % self.step == 0:
                samples = (samples & 0xff00) | byte
                self.odd_samples = 0
            else:
                samples = byte << 8
                self.odd_samples = samples
                continue
            samples = byte

            # Loop the samples in one byte
            for _ in range((8 * self.step) // self.chan_num):
                curr = samples & ((2 ** self.chan_num) - 1)
                self._dump_one_sample_to_vcd(self.prev, curr)
                samples >>= self.chan_num
                self.prev = curr
                self.ts += 1

    def dump_samples_from_file(self, raw_in_io:FileIO) -> None:
        data_bytes = raw_in_io.read()
        self.dump_samples(data_bytes)


def dump_binary_raw_data_to_vcd(chan_num:int, raw_in_io:FileIO, vcd_out_io:TextIOWrapper) -> None:
    vcd = VCDDumper()
    vcd.open_new_vcd_file(chan_num, vcd_out_io)
    vcd.dump_samples_from_file(raw_in_io)
    vcd.close_vcd_file()
    print('Converted successfully!')


def dump_vcd_main() -> None:
    # Args parser
    parser = argparse.ArgumentParser(description='Dump raw data to VCD format')
    parser.add_argument('-n', '--chan-num', type=int, help='The channel number that probed')
    parser.add_argument('-i', '--input-file', type=argparse.FileType('rb'), help='The the input raw data file')
    parser.add_argument('-o', '--output-file', type=argparse.FileType('w'), help='The output vcd file name (optional)', default=None)

    # Parse args
    args = parser.parse_args()
    chan_num = args.chan_num
    raw_in_io = args.input_file
    vcd_out_io = args.output_file
    if chan_num <= 0:
        raise ValueError('Invalid channel number')

    # Get the actual probe channel number (round up to the nearest number that is power of 2)
    p = 0
    while (1 << p) < chan_num:
        p = p + 1
    chan_num = 1 << p

    # Convert the data to the VCD format and save it into the file
    dump_binary_raw_data_to_vcd(chan_num, raw_in_io, vcd_out_io)


if __name__ == '__main__':
    dump_vcd_main()
