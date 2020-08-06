#!/usr/bin/env python
import datetime as dt
import requests
import json
import os
import re

import matplotlib.dates
import matplotlib.font_manager as font_manager
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import numpy as np
from dateutil import parser
from dateutil.relativedelta import relativedelta
from matplotlib.dates import WEEKLY, DateFormatter, RRuleLocator, rrulewrapper


class Version(object):
    def __init__(self, version_name,
                 explicit_start_date=None,
                 explicit_end_date=None,
                 explicit_end_service_date=None,
                 is_lts=None):
        self.version_name = version_name

        self.is_lts = is_lts or self.is_version_lts()

        self.is_major_minor = Version.is_minor_major_version(self.version_name)

        self._start_date = parser.parse(
            explicit_start_date) if explicit_start_date is not None else self._retrieve_start_date()
        self._end_of_life_date = parser.parse(
            explicit_end_date) if explicit_end_date is not None else self._retrieve_end_of_life_date()
        self._end_service_date = parser.parse(
            explicit_end_service_date) if explicit_end_service_date is not None else self.get_end_service_date()

        self.start_date_matplotlib_format = matplotlib.dates.date2num(self._start_date)
        self.end_of_life_date_matplotlib_format = matplotlib.dates.date2num(self._end_of_life_date)

        self.end_service_date_matplotlib_format = matplotlib.dates.date2num(
            self._end_service_date) if self._end_service_date is not None else None

    @staticmethod
    def get_config(config_path):
        return json.load(open(config_path))

    @staticmethod
    def add_months(source_date, months):
        return source_date + relativedelta(months=+months)

    @staticmethod
    def is_minor_major_version(version_name):
        return True if len(version_name.split(".")) <= 2 else False

    def is_version_lts(self):
        version = self.version_name
        return version >= 'v4.1'

    def get_start_date(self):
        return self._start_date

    def get_end_of_life_date(self):
        return self._end_of_life_date

    def _retrieve_start_date(self):
        return parser.parse(os.popen("git log -1 --format=%ai " + self.version_name))

    def _retrieve_end_of_life_date(self):
        return self.add_months(self._start_date, 30 if self.is_lts else 18)

    def get_end_service_date(self):
        return self.add_months(self._start_date, 12)


class ChartVersions(object):
    def __init__(self):
        self.all_versions = self.get_all_versions_from_git()

        self._releases = self._get_releases_from_url(url="https://dl.espressif.com/dl/esp-idf/idf_versions.js")
        self._patches = self._get_patches_from_url(url="https://dl.espressif.com/dl/esp-idf/idf_versions.js")

        self.sorted_releases_supported = sorted(self.filter_old_versions(self._releases), key=lambda x: x.version_name)
        self.patches_supported = self.filter_old_versions(self._patches)

        # TODO test higher versions
        # self.sorted_releases_supported.append(Version(version_name='v4.1', explicit_start_date="28-9-2020"))
        # self.sorted_releases_supported.append(Version(version_name='v4.2', explicit_start_date="28-5-2021"))

    def get_releases_as_json(self):
        return {
            x.version_name: {
                "start_date": x.get_start_date().strftime("%Y-%m-%d"),
                "end_date": x.get_end_of_life_date().strftime("%Y-%m-%d"),
                "is_lts": x.is_lts
            } for x in self.sorted_releases_supported + self.patches_supported
        }

    @staticmethod
    def parse_chart_releases_from_js(js_as_string):
        return json.loads(js_as_string[js_as_string.find("RELEASES: ") + len("RELEASES: "):js_as_string.rfind("};")])

    def _get_all_version_from_url(self, url=None, filename=None):
        releases_file = requests.get(url).text if url is not None else "".join(open(filename).readlines())
        return self.parse_chart_releases_from_js(releases_file)

    def _get_releases_from_url(self, url=None, filename=None):
        all_versions = self._get_all_version_from_url(url, filename)
        return [
            Version(version_name=x,
                    explicit_start_date=all_versions[x]['start_date'],
                    explicit_end_date=all_versions[x]['end_date'] if 'end_date' in all_versions[x].keys() else None,
                    explicit_end_service_date=all_versions[x]['end_service'] if 'end_service' in all_versions[x].keys() else None)
            for x in all_versions.keys() if Version.is_minor_major_version(x)
        ]

    def _get_patches_from_url(self, url=None, filename=None):
        all_versions = self._get_all_version_from_url(url, filename)
        return [
            Version(version_name=x,
                    explicit_start_date=all_versions[x]['start_date'],
                    explicit_end_date=all_versions[x]['end_date'] if 'end_date' in all_versions[x].keys() else None,
                    is_lts=all_versions[x]['new_policy'] if 'new_policy' in all_versions[x].keys() else None)
            for x in all_versions.keys() if not Version.is_minor_major_version(x)
        ]

    def _get_releases(self, all_versions):
        return [Version(release) for release in self._get_releases_names(all_versions)]

    def _get_patches(self, all_versions):
        return [Version(patch, explicit_end_date=self.get_super_end_date_as_string(patch),
                        is_lts=self.get_super_lts_state(patch)) for patch in self._get_patches_names(all_versions)]

    @staticmethod
    def _get_releases_names(all_versions):
        return list(filter(lambda x: Version.is_minor_major_version(x), all_versions))

    @staticmethod
    def _get_patches_names(all_versions):
        return list(filter(lambda x: not Version.is_minor_major_version(x), all_versions))

    def get_super_version(self, patch):
        return list(filter(lambda x: x.version_name == self.get_super_version_name(patch), self._releases))[0]

    def get_super_end_date_as_string(self, patch):
        return self.get_super_version(patch).get_end_of_life_date().strftime("%m/%d/%Y, %H:%M:%S")

    def get_super_lts_state(self, patch):
        return self.get_super_version(patch).is_lts

    @staticmethod
    def get_all_versions_from_git():
        """
        :returns: list of string variables meaning name of the versions or patches (e.g. v4.1, v3.3.1, etc.)
        """
        all_git_tags = os.popen("git tag")
        stable_releases_regex = "(?:^|)(v\d+\.\d+(?:\.\d+){0,1})(?=\n|$)"

        results = [re.match(stable_releases_regex, line) for line in all_git_tags]
        all_versions = [regex_result.group(1) for regex_result in results if regex_result is not None]
        return all_versions

    @staticmethod
    def filter_old_versions(versions):
        return list(
            filter(lambda x: x.get_end_of_life_date() >= dt.datetime.now(x.get_end_of_life_date().tzinfo), versions))

    @staticmethod
    def get_super_version_name(version_name):
        return ".".join(version_name.split(".")[:2])

    def create_chart(self,
                     figure_size=(20, 8),
                     subplot=111,
                     step_size=0.5,
                     bar_height=0.3,
                     version_alpha=0.8,
                     patch_width=1,
                     lts_service_color='darkred',
                     lts_maintenance_color='red',
                     bar_align='center',
                     patch_color='black',
                     patch_alpha=1,
                     date_interval=10,
                     output_chart_name='docs/chart',
                     output_chart_extension='.png',
                     months_surrounding_chart=5):
        fig = plt.figure(figsize=figure_size)
        ax = fig.add_subplot(subplot)

        labels_count = len(self.sorted_releases_supported)

        pos = np.arange(step_size, labels_count * step_size + step_size, step_size)
        mapping_releases_index = {release.version_name: i for release, i in
                                  zip(self.sorted_releases_supported, range(labels_count))}

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

        for patch, i in zip(self.patches_supported, range(len(self.patches_supported))):
            start_date = patch.start_date_matplotlib_format

            ax.barh(mapping_releases_index[self.get_super_version_name(patch.version_name)] * step_size + step_size,
                    patch_width, left=start_date, height=bar_height, align=bar_align, color=patch_color,
                    alpha=patch_alpha,
                    edgecolor=patch_color)
            ax.text(start_date - 5, mapping_releases_index[
                self.get_super_version_name(patch.version_name)] * step_size + step_size - 0.2,
                    patch.version_name.split('.')[-1],
                    fontsize='xx-small', color='darkred')

        plt.setp(plt.yticks(pos, map(lambda x: x.version_name, self.sorted_releases_supported))[1], fontsize=14)

        ax.set_ylim(bottom=0, ymax=labels_count * step_size + step_size)

        ax.set_xlim(
            xmin=Version.add_months(
                min(self.sorted_releases_supported,
                    key=lambda version: version.get_start_date().replace(tzinfo=None)).get_start_date(),
                -months_surrounding_chart),
            xmax=Version.add_months(
                max(self.sorted_releases_supported,
                    key=lambda version: version.get_end_of_life_date().replace(tzinfo=None)).get_end_of_life_date(),
                months_surrounding_chart))

        ax.grid(color='g', linestyle=':')
        ax.xaxis_date()

        rule = rrulewrapper(WEEKLY, interval=date_interval)
        loc = RRuleLocator(rule)
        formatter = DateFormatter("%d-%b '%y")

        ax.xaxis.set_major_locator(loc)
        ax.xaxis.set_major_formatter(formatter)
        x_labels = ax.get_xticklabels()
        plt.setp(x_labels, rotation=30, fontsize=10)

        ax.legend(loc=1, prop=font_manager.FontProperties(size='small'))

        ax.invert_yaxis()
        fig.autofmt_xdate()

        darkred_patch = mpatches.Patch(color='darkred', label='Service period (Recommended for new designs)')
        red_patch = mpatches.Patch(color='red', label='Maintenance period')

        plt.legend(handles=[darkred_patch, red_patch], prop={'size': 6})
        fig.set_size_inches(10, 5, forward=True)
        plt.savefig(output_chart_name + output_chart_extension)
        plt.show()


if __name__ == '__main__':
    ChartVersions().create_chart()
