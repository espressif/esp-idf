"""
This file is used to generate soc header constants into sub-package soc_headers
"""
import os
from ast import literal_eval

from corefile import IDF_PATH, SUPPORTED_TARGETS


def main():  # type: () -> None
    constants = [
        'SOC_DRAM_LOW',
        'SOC_DRAM_HIGH',
        'SOC_IRAM_LOW',
        'SOC_IRAM_HIGH',
        'SOC_RTC_DATA_LOW',
        'SOC_RTC_DATA_HIGH',
        'SOC_RTC_DRAM_LOW',
        'SOC_RTC_DRAM_HIGH',
    ]

    for target in SUPPORTED_TARGETS:
        target_constants = {}
        soc_header_fp = os.path.join(IDF_PATH, 'components/soc/{}/include/soc/soc.h'.format(target))
        module_fp = os.path.join(IDF_PATH, 'components', 'espcoredump', 'corefile', 'soc_headers',
                                 '{}.py'.format(target))

        with open(soc_header_fp) as fr:
            for line in fr.readlines():
                for attr in constants:
                    if '#define {}'.format(attr) in line:
                        target_constants[attr] = literal_eval(line.strip().split()[-1])

        for attr in constants:
            if attr not in target_constants:
                raise ValueError('ERROR: Attr {} is missing in {}'.format(attr, soc_header_fp))

        with open(module_fp, 'w') as fw:
            for k, v in target_constants.items():
                fw.write('{} = {}\n'.format(k, hex(v)))


if __name__ == '__main__':
    main()
