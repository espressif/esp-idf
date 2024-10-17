# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
from argparse import ArgumentParser

from idf_build_apps.constants import ALL_TARGETS
from idf_build_apps.manifest.soc_header import SocHeader

if __name__ == '__main__':
    parser = ArgumentParser(description='Dump parsed SOC headers for all supported targets')
    parser.add_argument('output', help='Output file')
    args = parser.parse_args()

    d = {target: SocHeader(target) for target in ALL_TARGETS}
    with open(args.output, 'w') as f:
        json.dump(d, f, indent=2)
