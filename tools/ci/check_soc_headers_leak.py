# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# This check script is used to ensure the public APIs won't expose the unstable soc files like register files
# public API header files are those taken by doxygen and have full documented docs

import fnmatch
import os
import re
import sys
import typing
from string import Template

# The following header files in soc component is treated as stable, so is allowed to be used in any public header files
allowed_soc_headers = (
    'soc/soc_caps.h',
    'soc/gpio_num.h',
    'soc/reset_reasons.h',
    'soc/reg_base.h',
    'soc/clk_tree_defs.h',
)

include_header_pattern = re.compile(r'[\s]*#[\s]*include ["<](.*)[">].*')
doxyfile_target_pattern = re.compile(r'Doxyfile_(.*)')


class PublicAPIVisits:
    def __init__(self, doxyfile_path: str, idf_path: str, target: str) -> None:
        self.doxyfile_path = doxyfile_path
        self._target = target
        self._idf_path = idf_path

    def __iter__(self) -> typing.Generator:
        with open(self.doxyfile_path, 'r', encoding='utf8') as f:
            for line in f:
                line = line.strip()
                if line.startswith('$(PROJECT_PATH)'):
                    # $(PROJECT_PATH)/components/soc/$(IDF_TARGET)/include/soc/uart_channel.h \
                    # -> ${PROJECT_PATH}/components/soc/${IDF_TARGET}/include/soc/uart_channel.h
                    line = line.replace('(', '{').replace(')', '}').rstrip('\\ ')
                    file_path = Template(line).substitute(
                        PROJECT_PATH=self._idf_path, IDF_TARGET=self._target
                    )
                    yield file_path


def check_soc_not_in(
    idf_path: str,
    target: str,
    doxyfile_path: str,
    violation_dict: typing.Dict[str, set],
) -> None:
    for file_path in PublicAPIVisits(
        os.path.join(idf_path, doxyfile_path), idf_path, target
    ):
        with open(file_path, 'r', encoding='utf8') as f:
            for line in f:
                match_data = re.match(include_header_pattern, line)
                if match_data:
                    header = match_data.group(1)
                    if header.startswith('soc') and header not in allowed_soc_headers:
                        if file_path not in violation_dict:
                            violation_dict[file_path] = set()
                        violation_dict[file_path].add(header)


def main() -> None:
    idf_path = os.environ.get('IDF_PATH', None)
    if idf_path is None:
        print('IDF_PATH must be set before running this script', file=sys.stderr)
        sys.exit(1)

    # list all doxyfiles
    doxyfiles = fnmatch.filter(
        os.listdir(os.path.join(idf_path, 'docs/doxygen')), 'Doxyfile*'
    )
    print(f'Found Doxyfiles:{doxyfiles}')

    # targets are judged from Doxyfile name
    targets = []
    for file in doxyfiles:
        res = doxyfile_target_pattern.match(file)
        if res:
            targets.append(res.group(1))
    if not targets:
        print('No targets found', file=sys.stderr)
        sys.exit(1)

    soc_violation_dict: typing.Dict[str, set] = {}
    for target in targets:
        check_soc_not_in(
            idf_path,
            target,
            os.path.join(idf_path, 'docs/doxygen/Doxyfile'),
            soc_violation_dict,
        )
        check_soc_not_in(
            idf_path,
            target,
            os.path.join(idf_path, f'docs/doxygen/Doxyfile_{target}'),
            soc_violation_dict,
        )

    if len(soc_violation_dict) > 0:
        for file_path, headers_set in soc_violation_dict.items():
            print(f'{file_path} includes non-public soc header file: {headers_set}')
        sys.exit(1)
    else:
        print('No violation found')


if __name__ == '__main__':
    main()
