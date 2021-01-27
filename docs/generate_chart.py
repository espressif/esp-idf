#!/usr/bin/env python

# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import argparse
import datetime as dt
import json

import matplotlib.dates
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import numpy as np
import requests
from dateutil import parser
from dateutil.relativedelta import relativedelta
from matplotlib.dates import MONTHLY, DateFormatter, RRuleLocator, rrulewrapper


class Version(object):
    def __init__(self, version_name, explicit_start_date, explicit_end_date, explicit_end_service_date=None):
        self.version_name = version_name

        self._start_date = parser.parse(explicit_start_date)
        self._end_of_life_date = parser.parse(explicit_end_date)
        self._end_service_date = parser.parse(
            explicit_end_service_date) if explicit_end_service_date is not None else self.compute_end_service_date()

        self.start_date_matplotlib_format = matplotlib.dates.date2num(self._start_date)
        self.end_of_life_date_matplotlib_format = matplotlib.dates.date2num(self._end_of_life_date)

        self.end_service_date_matplotlib_format = matplotlib.dates.date2num(self._end_service_date)

    @staticmethod
    def add_months(source_date, months):
        return source_date + relativedelta(months=+months)

    def get_start_date(self):
        return self._start_date

    def get_end_of_life_date(self):
        return self._end_of_life_date

    def get_end_service_date(self):
        return self._end_service_date

    def compute_end_service_date(self):
        return self.add_months(self._start_date, 12)


class ChartVersions(object):
    def __init__(self, url=None, filename=None):
        self._releases = self._get_releases_from_url(url=url, filename=filename)
        self.sorted_releases_supported = sorted(self.filter_old_versions(self._releases), key=lambda x: x.version_name,
                                                reverse=True)

    def get_releases_as_json(self):
        return {
            x.version_name: {
                'start_date': x.get_start_date().strftime('%Y-%m-%d'),
                'end_service': x.get_end_service_date().strftime('%Y-%m-%d'),
                'end_date': x.get_end_of_life_date().strftime('%Y-%m-%d')
            } for x in self.sorted_releases_supported
        }

    @staticmethod
    def parse_chart_releases_from_js(js_as_string):
        return json.loads(js_as_string[js_as_string.find('RELEASES: ') + len('RELEASES: '):js_as_string.rfind('};')])

    def _get_all_version_from_url(self, url=None, filename=None):
        releases_file = requests.get(url).text if url is not None else ''.join(open(filename).readlines())
        return self.parse_chart_releases_from_js(releases_file)

    def _get_releases_from_url(self, url=None, filename=None):
        all_versions = self._get_all_version_from_url(url, filename)
        return [
            Version(version_name=x,
                    explicit_start_date=all_versions[x]['start_date'],
                    explicit_end_date=all_versions[x]['end_date'] if 'end_date' in all_versions[x].keys() else None,
                    explicit_end_service_date=all_versions[x]['end_service'] if 'end_service' in all_versions[
                        x].keys() else None)
            for x in all_versions.keys()
        ]

    @staticmethod
    def filter_old_versions(versions):
        return list(
            filter(lambda x: x.get_end_of_life_date() >= dt.datetime.now(x.get_end_of_life_date().tzinfo), versions))

    @staticmethod
    def months_timedelta(datetime_1, datetime2):
        datetime_1, datetime2 = (datetime2, datetime_1) if datetime_1 > datetime2 else (datetime_1, datetime2)
        return (datetime2.year * 12 + datetime2.month) - (datetime_1.year * 12 + datetime_1.month)

    @staticmethod
    def find_next_multiple_of_power_two(number, initial=3):
        """
        Computes the next multiple of the number by some power of two.
        >>> ChartVersions.find_next_multiple_of_power_two(7, 3)
        12
        """
        msb = number.bit_length()
        return 3 if number <= 1 else initial << msb - 2 << (1 & number >> msb - 2)

    def find_nearest_multiple_of_power_two(self, number, initial=3, prefer_next=False):
        next_num = self.find_next_multiple_of_power_two(number=number - 1, initial=initial)
        previous_num = next_num >> 1
        return next_num if abs(next_num - number) < (abs(previous_num - number) + int(prefer_next)) else previous_num

    def create_chart(self,
                     figure_size=(41.8330013267, 16.7332005307),
                     subplot=111,
                     step_size=0.5,
                     bar_height=0.3,
                     version_alpha=0.8,
                     lts_service_color='darkred',
                     lts_maintenance_color='red',
                     bar_align='center',
                     date_interval=None,
                     output_chart_name='docs/chart',
                     output_chart_extension='.png',
                     months_surrounding_chart=4,
                     service_period_label='Service period (Recommended for new designs)',
                     maintenance_period_text='Maintenance period'):
        fig = plt.figure(figsize=figure_size)
        ax = fig.add_subplot(subplot)

        labels_count = len(self.sorted_releases_supported)

        pos = np.arange(step_size, labels_count * step_size + step_size, step_size)

        for release, i in zip(self.sorted_releases_supported, range(labels_count)):
            start_date = release.start_date_matplotlib_format
            end_of_service_date = release.end_service_date_matplotlib_format

            end_date = release.end_of_life_date_matplotlib_format

            ax.barh((i * step_size) + step_size, (end_of_service_date or end_date) - start_date, left=start_date,
                    height=bar_height, align=bar_align,
                    color=lts_service_color,
                    alpha=version_alpha,
                    edgecolor=lts_service_color)
            if end_of_service_date is not None:
                ax.barh((i * step_size) + step_size, end_date - end_of_service_date, left=end_of_service_date,
                        height=bar_height, align=bar_align,
                        color=lts_maintenance_color, alpha=version_alpha, edgecolor=lts_maintenance_color)

        ax.set_ylim(bottom=0, ymax=labels_count * step_size + step_size)

        max_ax_date = Version.add_months(
            max(self.sorted_releases_supported,
                key=lambda version: version.get_end_of_life_date().replace(tzinfo=None)).get_end_of_life_date(),
            months_surrounding_chart + 1).replace(day=1)

        min_ax_date = Version.add_months(
            min(self.sorted_releases_supported,
                key=lambda version: version.get_start_date().replace(tzinfo=None)).get_start_date(),
            -months_surrounding_chart).replace(day=1)

        x_ax_interval = date_interval or self.find_nearest_multiple_of_power_two(
            self.months_timedelta(max_ax_date, min_ax_date) // 10)

        ax.set_xlim(xmin=min_ax_date, xmax=max_ax_date)

        ax.grid(color='g', linestyle=':')
        ax.xaxis_date()

        rule = rrulewrapper(MONTHLY, interval=x_ax_interval)
        loc = RRuleLocator(rule)
        formatter = DateFormatter('%b %Y')

        ax.xaxis.set_major_locator(loc)
        ax.xaxis.set_major_formatter(formatter)
        x_labels = ax.get_xticklabels()
        plt.ylabel('ESP-IDF Release', size=12)

        ax.invert_yaxis()
        fig.autofmt_xdate()

        darkred_patch = mpatches.Patch(color=lts_service_color, label=service_period_label)
        red_patch = mpatches.Patch(color=lts_maintenance_color, label=maintenance_period_text)

        plt.setp(plt.yticks(pos, map(lambda x: x.version_name, self.sorted_releases_supported))[1], rotation=0,
                 fontsize=10, family='Tahoma')
        plt.setp(x_labels, rotation=30, fontsize=11, family='Tahoma')
        plt.legend(handles=[darkred_patch, red_patch], prop={'size': 10, 'family': 'Tahoma'},
                   bbox_to_anchor=(1.01, 1.165), loc='upper right')
        fig.set_size_inches(11, 5, forward=True)
        plt.savefig(output_chart_name + output_chart_extension, bbox_inches='tight')
        print('Saved into ' + output_chart_name + output_chart_extension)


if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser(
        description='Create chart of version support. Set the url or filename with versions.'
                    'If you set both filename and url the script will prefer filename.')
    arg_parser.add_argument('--url', metavar='URL', default='https://dl.espressif.com/dl/esp-idf/idf_versions.js')
    arg_parser.add_argument('--filename',
                            help='Set the name of the source file, if is set, the script ignores the url.')
    arg_parser.add_argument('--output-format', help='Set the output format of the image.', default='svg')
    arg_parser.add_argument('--output-file', help='Set the name of the output file.', default='docs/chart')
    args = arg_parser.parse_args()

    ChartVersions(url=args.url if args.filename is None else None, filename=args.filename).create_chart(
        output_chart_extension='.' + args.output_format.lower()[-3:], output_chart_name=args.output_file)
