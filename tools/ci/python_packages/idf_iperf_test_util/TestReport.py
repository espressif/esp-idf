# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
this module generates markdown format test report for throughput test.

The test report contains 2 parts:

1. throughput with different configs
2. throughput with RSSI
"""
import os


class ThroughputForConfigsReport(object):
    THROUGHPUT_TYPES = ['tcp_tx', 'tcp_rx', 'udp_tx', 'udp_rx']

    REPORT_FILE_NAME = 'ThroughputForConfigs.md'

    def __init__(self, output_path, ap_ssid, throughput_results, sdkconfig_files):
        """
        :param ap_ssid: the ap we expected to use
        :param throughput_results: config with the following type::
            {
                "config_name": {
                    "tcp_tx": result,
                    "tcp_rx": result,
                    "udp_tx": result,
                    "udp_rx": result,
                },
                "config_name2": {},
            }
        """
        self.output_path = output_path
        self.ap_ssid = ap_ssid
        self.results = throughput_results
        self.sdkconfigs = dict()
        for config_name in sdkconfig_files:
            self.sdkconfigs[config_name] = self._parse_config_file(sdkconfig_files[config_name])
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        self.sort_order = list(self.sdkconfigs.keys())
        self.sort_order.sort()

    @staticmethod
    def _parse_config_file(config_file_path):
        sdkconfig = {}
        with open(config_file_path, 'r') as f:
            for line in f:
                if not line.isspace():
                    if line[0] == '#':
                        continue
                    name, value = line.split('=')
                    value = value.strip('\r\n')
                    sdkconfig[name] = value if value else 'n'
        return sdkconfig

    def _generate_the_difference_between_configs(self):
        """
        generate markdown list for different configs::

            default: esp-idf default
            low:
                * `config name 1`: old value -> new value
                * `config name 2`: old value -> new value
                * ...
            ...

        """

        data = '## Config Definition:\r\n\r\n'

        def find_difference(base, new):
            _difference = {}
            all_configs = set(base.keys())
            all_configs.update(set(new.keys()))
            for _config in all_configs:
                try:
                    _base_value = base[_config]
                except KeyError:
                    _base_value = 'null'
                try:
                    _new_value = new[_config]
                except KeyError:
                    _new_value = 'null'
                if _base_value != _new_value:
                    _difference[_config] = '{} -> {}'.format(_base_value, _new_value)
            return _difference

        for i, _config_name in enumerate(self.sort_order):
            current_config = self.sdkconfigs[_config_name]

            if i > 0:
                previous_config_name = self.sort_order[i - 1]
                previous_config = self.sdkconfigs[previous_config_name]
            else:
                previous_config = previous_config_name = None

            if previous_config:
                # log the difference
                difference = find_difference(previous_config, current_config)
                data += '* {} (compared to {}):\r\n'.format(_config_name, previous_config_name)
                for diff_name in difference:
                    data += '    * `{}`: {}\r\n'.format(diff_name, difference[diff_name])
        return data

    def _generate_report_for_one_type(self, throughput_type):
        """
        generate markdown table with the following format::

            | config name | throughput (Mbps) | free heap size (bytes) |
            |-------------|-------------------|------------------------|
            | default     | 32.11             | 147500                 |
            | low         | 32.11             | 147000                 |
            | medium      | 33.22             | 120000                 |
            | high        | 43.11             | 100000                 |
            | max         | 45.22             | 79000                  |
        """
        empty = True

        ret = '\r\n### {} {}\r\n\r\n'.format(*throughput_type.split('_'))
        ret += '| config name | throughput (Mbps) | free heap size (bytes) |\r\n'
        ret += '|-------------|-------------------|------------------------|\r\n'
        for config in self.sort_order:
            try:
                result = self.results[config][throughput_type]
                throughput = '{:.02f}'.format(max(result.throughput_by_att[self.ap_ssid].values()))
                heap_size = str(result.heap_size)
                # although markdown table will do alignment
                # do align here for better text editor presentation
                ret += '| {:<12}| {:<18}| {:<23}|\r\n'.format(config, throughput, heap_size)
                empty = False
            except KeyError:
                pass
        return ret if not empty else ''

    def generate_report(self):
        data = '# Throughput for different configs\r\n'
        data += '\r\nAP: {}\r\n'.format(self.ap_ssid)

        for throughput_type in self.THROUGHPUT_TYPES:
            data += self._generate_report_for_one_type(throughput_type)
            data += '\r\n------\r\n'

        data += self._generate_the_difference_between_configs()

        with open(os.path.join(self.output_path, self.REPORT_FILE_NAME), 'w') as f:
            f.write(data)


class ThroughputVsRssiReport(object):

    REPORT_FILE_NAME = 'ThroughputVsRssi.md'

    def __init__(self, output_path, throughput_results):
        """
        :param throughput_results: config with the following type::
            {
                "tcp_tx": result,
                "tcp_rx": result,
                "udp_tx": result,
                "udp_rx": result,
            }
        """
        self.output_path = output_path
        self.raw_data_path = os.path.join(output_path, 'raw_data')
        self.results = throughput_results
        self.throughput_types = list(self.results.keys())
        self.throughput_types.sort()
        if not os.path.exists(self.raw_data_path):
            os.makedirs(self.raw_data_path)

    def _generate_summary(self):
        """
        generate summary with the following format::

            | item    | curve analysis | max throughput (Mbps) |
            |---------|----------------|-----------------------|
            | tcp tx  | Success        | 32.11                 |
            | tcp rx  | Success        | 32.11                 |
            | udp tx  | Success        | 45.22                 |
            | udp rx  | Failed         | 55.44                 |
        """

        ret = '\r\n### Summary\r\n\r\n'
        ret += '| item    | curve analysis | max throughput (Mbps) |\r\n'
        ret += '|---------|----------------|-----------------------|\r\n'

        for _type in self.throughput_types:
            result = self.results[_type]
            max_throughput = 0.0
            curve_analysis = 'Failed' if result.error_list else 'Success'
            for ap_ssid in result.throughput_by_att:
                _max_for_ap = max(result.throughput_by_rssi[ap_ssid].values())
                if _max_for_ap > max_throughput:
                    max_throughput = _max_for_ap
            max_throughput = '{:.02f}'.format(max_throughput)
            ret += '| {:<8}| {:<15}| {:<22}|\r\n'.format('{}_{}'.format(result.proto, result.direction),
                                                         curve_analysis, max_throughput)
        return ret

    def _generate_report_for_one_type(self, result):
        """
        generate markdown table with the following format::

            ### tcp rx

            Errors:

            * detected error 1
            * ...

            AP: ap_ssid

            ![throughput Vs RSSI](path to figure)

            AP: ap_ssid

            ![throughput Vs RSSI](path to figure)

        """
        result.post_analysis()
        ret = '\r\n### {} {}\r\n'.format(result.proto, result.direction)
        if result.error_list:
            ret += '\r\nErrors:\r\n\r\n'
            for error in result.error_list:
                ret += '* ' + error + '\r\n'
        for ap_ssid in result.throughput_by_rssi:
            ret += '\r\nAP: {}\r\n'.format(ap_ssid)
            # draw figure
            file_name = result.draw_throughput_figure(self.raw_data_path, ap_ssid, 'rssi')
            result.draw_throughput_figure(self.raw_data_path, ap_ssid, 'att')
            result.draw_rssi_vs_att_figure(self.raw_data_path, ap_ssid)

            ret += '\r\n[throughput Vs RSSI]({})\r\n'.format(os.path.join('raw_data', file_name))

        return ret

    def generate_report(self):
        data = '# Throughput Vs RSSI\r\n'

        data += self._generate_summary()

        for _type in self.throughput_types:
            data += self._generate_report_for_one_type(self.results[_type])

        with open(os.path.join(self.output_path, self.REPORT_FILE_NAME), 'w') as f:
            f.write(data)
