# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import matplotlib
# fix can't draw figure with docker
matplotlib.use('Agg')
import matplotlib.pyplot as plt  # noqa: E402 - matplotlib.use('Agg') need to be before this


# candidate colors
LINE_STYLE_CANDIDATE = ['b-o', 'r-o', 'k-o', 'm-o', 'c-o', 'g-o', 'y-o',
                        'b-s', 'r-s', 'k-s', 'm-s', 'c-s', 'g-s', 'y-s']


def draw_line_chart(file_name, title, x_label, y_label, data_list):
    """
    draw line chart and save to file.

    :param file_name: abs/relative file name to save chart figure
    :param title: chart title
    :param x_label: x-axis label
    :param y_label: y-axis label
    :param data_list: a list of line data.
                      each line is a dict of ("x-axis": list, "y-axis": list, "label": string)
    """

    plt.figure(figsize=(12, 6))
    plt.grid(True)

    for i, data in enumerate(data_list):
        plt.plot(data["x-axis"], data["y-axis"], LINE_STYLE_CANDIDATE[i], label=data["label"])

    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.legend(fontsize=12)
    plt.title(title)
    plt.tight_layout(pad=3, w_pad=3, h_pad=3)
    plt.savefig(file_name)
    plt.close()
