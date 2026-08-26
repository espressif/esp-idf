# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from collections import OrderedDict

import pyecharts.options as opts
from pyecharts.charts import Line


def draw_line_chart(file_name, title, x_label, y_label, data_series, range_list):
    """
    draw line chart and save to file.

    :param file_name: abs/relative file name to save chart figure
    :param title: chart title
    :param x_label: x-axis label
    :param y_label: y-axis label
    :param data_series: a dict {"name": data}. data is a dict.
    :param range_list: a list of x-axis range
    """

    line = Line()
    # echarts do not support minus number for x axis, convert to string
    _range_list = [str(x) for x in range_list]
    line.add_xaxis(_range_list)
    for item in data_series:
        _data = OrderedDict.fromkeys(_range_list, None)
        for key in data_series[item]:
            _data[str(key)] = data_series[item][key]
        _data = list(_data.values())
        try:
            legend = item + ' (max: {:.02f})'.format(max([x for x in _data if x]))
        except TypeError:
            legend = item
        line.add_yaxis(legend, _data, is_smooth=True, is_connect_nones=True,
                       label_opts=opts.LabelOpts(is_show=False))
    line.set_global_opts(
        datazoom_opts=opts.DataZoomOpts(range_start=0, range_end=100),
        title_opts=opts.TitleOpts(title=title, pos_left='center'),
        legend_opts=opts.LegendOpts(pos_top='10%', pos_left='right', orient='vertical'),
        tooltip_opts=opts.TooltipOpts(trigger='axis'),
        xaxis_opts=opts.AxisOpts(type_='category', name=x_label, splitline_opts=opts.SplitLineOpts(is_show=True)),
        yaxis_opts=opts.AxisOpts(type_='value', name=y_label,
                                 axistick_opts=opts.AxisTickOpts(is_show=True),
                                 splitline_opts=opts.SplitLineOpts(is_show=True)),
    )
    line.render(file_name)
