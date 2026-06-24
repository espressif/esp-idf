#!/usr/bin/env python
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from datetime import datetime
from typing import TextIO

from reg_parse import DBParser
from reg_parse import Regdomain

# Not a valid ISO 3166-1 alpha-2 code; marks end of regdomain_table for linear scans (matches wifi_regdomain_t.cn[2]).
REGDOMAIN_TABLE_SENTINEL_CC = '##'


def write_regulatory_rules(cfile: TextIO, perms: list, filter_5g: bool = False) -> None:
    """Helper function to write regulatory rules to the C file."""
    filtered_perms = [perm for perm in perms if not filter_5g or (perm.freqband.start >= 1 and perm.freqband.end <= 14)]
    rule_count = len(filtered_perms)

    cfile.write('    {\n')
    cfile.write(f'        {rule_count},  /* Number of rules */\n')
    cfile.write('        {\n')

    for perm in filtered_perms:
        cfile.write(
            '            {{ {start_chan}, {end_chan}, {max_bandwidth}, {max_eirp}, {is_dfs}, {reserved} }},  '
            '/* chan:{start_chan}~{end_chan}, '
            'max_bw:{bandwidth_comment}, max_power:{max_eirp}(dBm), '
            'dfs:{is_dfs}, reserved.*/\n'.format(
                start_chan=int(perm.freqband.start),
                end_chan=int(perm.freqband.end),
                max_bandwidth=int(perm.freqband.maxbw),
                max_eirp=int(perm.power.max_eirp),
                is_dfs=perm.dfs,
                reserved=0,
                bandwidth_comment={1: '20M', 2: '40M', 3: '80M', 4: '160M'}.get(int(perm.freqband.maxbw), 'Unknown'),
            )
        )

    cfile.write('        }\n')
    cfile.write('    },\n')


def write_regulatory_data(
    cfile: TextIO, reg: Regdomain, type_list: list[str], perm_list: list[list], filter_5g: bool
) -> None:
    """Helper function to write regulatory data."""
    cfile.write('typedef enum {\n')
    for reg_type in type_list:
        cfile.write(f'    ESP_WIFI_REGULATORY_TYPE_{reg_type},\n')
    cfile.write('    ESP_WIFI_REGULATORY_TYPE_MAX,\n')
    cfile.write('} esp_wifi_regulatory_type_t;\n\n')

    country_map = reg.regdomain_countries if filter_5g else reg.regdomain_countries_2g

    cfile.write(
        '/*\n'
        ' * regdomain_table: ISO alpha-2 country codes and their regulatory profile indices.\n'
        ' * Last row is a sentinel (not a real country code).\n'
        ' */\n'
    )
    cfile.write('const wifi_regdomain_t regdomain_table[] = {\n')

    for cc, reg_type_index in country_map.items():
        reg_type = f'ESP_WIFI_REGULATORY_TYPE_{type_list[reg_type_index]}'
        cfile.write(f'    {{"{cc.decode("utf-8")}", {reg_type}}},\n')

    cfile.write(
        '    {"' + REGDOMAIN_TABLE_SENTINEL_CC + '", ESP_WIFI_REGULATORY_TYPE_MAX}, '
        '/* Sentinel: end of table; not a real country code — '
        'do not use as wifi_country_t.cc. */\n'
    )
    cfile.write('};\n\n')
    cfile.write('const wifi_regulatory_t regulatory_data[] = {\n')

    for index, perms in enumerate(perm_list):
        regulatory_type = f'ESP_WIFI_REGULATORY_TYPE_{type_list[index]}'
        cfile.write(f'    /* {regulatory_type} */\n')
        write_regulatory_rules(cfile, perms, not filter_5g)

    cfile.write('};\n')


def main() -> None:
    p = DBParser()
    reg = Regdomain()

    filename = 'esp_wifi_regulatory.txt'
    directory = os.path.dirname(os.path.abspath(__file__))
    file_path = os.path.join(directory, filename)
    try:
        with open(file_path, encoding='utf-8') as file:
            regdomains = p.parse(file)
        # Get the modification time of the source file to determine copyright year
        mtime = os.path.getmtime(file_path)
        copyright_year = datetime.fromtimestamp(mtime).year
    except FileNotFoundError:
        raise Exception(f'File {file_path} not found')

    reg.simplify_countries(regdomains)
    reg.simplify_countries_2g(regdomains)

    total_countries = len(regdomains)
    country_rules_count = {}
    for country_code, country in regdomains.items():
        country_str = country_code.decode('utf-8')
        rules_count = len(country.permissions)
        country_rules_count[country_str] = rules_count

    max_rules_country = max(country_rules_count.items(), key=lambda x: x[1])
    max_rules_count = max_rules_country[1]
    max_rules_countries = [cc for cc, count in country_rules_count.items() if count == max_rules_count]

    print('\n=== Regulatory Statistics ===')
    print(f'Total supported countries: {total_countries}')
    print(f'Country(ies) with most rules: {", ".join(max_rules_countries)}')
    print(f'Maximum number of rules: {max_rules_count}')
    print('=' * 30 + '\n')

    type_list = list(reg.typical_regulatory.keys())
    perm_list = list(reg.typical_regulatory.values())

    type_list_2g = list(reg.typical_regulatory_2g.keys())
    perm_list_2g = list(reg.typical_regulatory_2g.values())

    output_file = 'esp_wifi_regulatory.c'

    with open(output_file, 'w') as cfile:
        cfile.write('/*\n')
        cfile.write(f' * SPDX-FileCopyrightText: 2025-{copyright_year} Espressif Systems (Shanghai) CO LTD\n')
        cfile.write(' *\n')
        cfile.write(' * SPDX-License-Identifier: Apache-2.0\n')
        cfile.write(' */\n\n')
        cfile.write('#include <stdint.h>\n')
        cfile.write('#include "esp_wifi_types_generic.h"\n')
        cfile.write('#include "esp_wifi.h"\n\n')

        # Write for 5G support
        cfile.write('#ifdef CONFIG_SOC_WIFI_SUPPORT_5G\n')
        write_regulatory_data(cfile, reg, type_list, perm_list, filter_5g=True)
        cfile.write('#else\n')
        # Write for 2G only
        write_regulatory_data(cfile, reg, type_list_2g, perm_list_2g, filter_5g=False)
        cfile.write('#endif // CONFIG_SOC_WIFI_SUPPORT_5G\n')

    print(f'{output_file} generated successfully.')


if __name__ == '__main__':
    main()
