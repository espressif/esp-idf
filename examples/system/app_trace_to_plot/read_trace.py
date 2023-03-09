# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import datetime
import json
import os
import signal
import sys
from enum import Enum
from functools import partial
from typing import Any, List

try:
    import espytrace.apptrace
except ImportError:  # cheat and use IDF's copy of espytrace if available
    idf_path = os.getenv('IDF_PATH')
    if not idf_path or not os.path.exists(idf_path):
        print('IDF not found. Please export idf')
        raise SystemExit(1)
    sys.path.insert(0, os.path.join(idf_path, 'tools', 'esp_app_trace'))
    import espytrace.apptrace

try:
    import dash
    from dash import dcc, html
    from dash.dependencies import Input, Output
    from plotly.subplots import make_subplots
except ImportError:
    print('Dash not found. Try to run \'pip install dash\'')
    raise SystemExit(1)

plots = []  # type: List[Any]
output_lines = []  # type: List[Any]
COMMENT_LINE = '//'


class States(Enum):
    STX_WAIT = 1
    LENGTH_WAIT = 2
    DATA_WAIT = 3
    ETX_WAIT = 4


app = dash.Dash(__name__)
app.layout = html.Div(
    html.Div([
        html.H2('Telemetry Data'),
        html.Div(id='live-update-data'),
        dcc.Graph(id='live-update-graph', style={'height': 800}),  # Height of the plotting area setted to 800px
        dcc.Interval(
            id='interval-component',
            interval=5 * 100,  # Graph will be updated every 500 ms
            n_intervals=0
        )
    ])
)


# Multiple components can update everytime interval gets fired.
@app.callback(Output('live-update-graph', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_graph_live(_n: Any) -> Any:  # pylint: disable=undefined-argument
    excluded_keys_for_plot = {'id', 'x_axis_data_size','y_axis_data_size', 'data_type', 'precision', 'x_axis_timestamp'}
    fig = make_subplots(rows=len(plots), cols=1, vertical_spacing=0.2, subplot_titles=[each_plot['title'] for each_plot in plots])

    for i, each_plot in enumerate(plots, start=1):
        for each_subplot in each_plot['plots']:
            plot_dict = {k: each_subplot[k] for k in each_subplot.keys() - excluded_keys_for_plot}
            fig.append_trace(plot_dict, i, 1)
        fig['layout']['xaxis{}'.format(i)]['title'] = each_plot['xaxis_title']
        fig['layout']['yaxis{}'.format(i)]['title'] = each_plot['yaxis_title']

    return fig


def get_value_from_key(input_id: int, key: str) -> Any:
    for each_plot in plots:
        for each_subplot in each_plot['plots']:
            if each_subplot['id'] == input_id:
                return each_subplot[key]
    return None


def parse_data_and_print(packet: bytes, offset_time: Any) -> None:
    ID_LENGTH = 4
    x_axis_data_length = 4
    y_axis_data_length = 4

    input_id = int.from_bytes(packet[:ID_LENGTH], 'little')
    data_size = get_value_from_key(input_id, 'x_axis_data_size')
    x_axis_data_length = data_size
    x_axis_raw_data = int.from_bytes(packet[ID_LENGTH:ID_LENGTH + x_axis_data_length], 'little')
    is_timestamp = get_value_from_key(input_id, 'x_axis_timestamp')
    if is_timestamp:
        x_axis_data = offset_time + datetime.timedelta(seconds=x_axis_raw_data / 1000)
    else:
        x_axis_data = float(x_axis_raw_data)
    data_size = get_value_from_key(input_id, 'y_axis_data_size')
    y_axis_data_length = data_size
    y_axis_data = int.from_bytes(packet[x_axis_data_length + ID_LENGTH:x_axis_data_length + ID_LENGTH + y_axis_data_length], 'little', signed=True)
    if get_value_from_key(input_id, 'data_type') in ['float', 'double']:
        precision = get_value_from_key(input_id, 'precision')
        y_axis_data = y_axis_data / (10 ** precision)

    ct = datetime.datetime.now()
    str_ctr = ct.strftime('%x-%X.%f')

    output_str = str_ctr + '-> ' + str(packet)
    output_lines.append(output_str)

    arr = [x_axis_data, y_axis_data]
    update_specific_graph_list(int(input_id), arr)


class CustomRequestHandler(espytrace.apptrace.TCPRequestHandler):
    """
        Handler for incoming TCP connections
    """
    def handle(self) -> None:
        STX = b'esp32'
        ETX = b'\x03'
        STX_LEN = 5

        ETX_LEN = 1
        DATA_LENGTH_LEN = 1
        PACKET_TIMEOUT_SECOND = 3

        state = States.STX_WAIT
        val_to_parse = b''
        length = 0
        offset_time = datetime.datetime.now()
        start_time = datetime.datetime.now()
        data = b''
        while not self.server.need_stop:
            data += self.request.recv(1024)
            if state == States.STX_WAIT:
                if len(data) >= STX_LEN and data.find(STX) != -1:
                    data = data[data.find(STX) + STX_LEN:]
                    state = States.LENGTH_WAIT
                    start_time = datetime.datetime.now()
                else:
                    data = data[-STX_LEN:]
            if state == States.LENGTH_WAIT:
                if len(data) > 0:
                    state = States.DATA_WAIT
                    length = int.from_bytes(data[:DATA_LENGTH_LEN], 'little')
                    data = data[DATA_LENGTH_LEN:]
            if state == States.DATA_WAIT:
                if len(data) >= length:
                    state = States.ETX_WAIT
                    val_to_parse = data[:length]
                    data = data[length:]
            if state == States.ETX_WAIT:
                if len(data) >= ETX_LEN and data[:ETX_LEN] == ETX:
                    state = States.STX_WAIT
                    parse_data_and_print(val_to_parse, offset_time)
                    data = data[ETX_LEN:]
            if state != States.STX_WAIT and (datetime.datetime.now() - start_time).seconds > PACKET_TIMEOUT_SECOND:
                print('Packet timed out. Dropping!')
                state = States.STX_WAIT


def read_json(file_path: str) -> Any:
    with open(file_path, 'r') as f:
        data = json.load(f)
        return data


def save_data(file_path: str) -> None:
    with open(file_path, 'w') as f:
        f.writelines(output_lines)


def signal_handler(output_file_path: str, reader: Any, sig: Any, frame: Any) -> None:
    del sig, frame
    reader.cleanup()
    if output_file_path is not None:
        save_data(output_file_path)
    sys.exit(0)


def update_specific_graph_list(input_id: int, val: List[Any]) -> None:
    for each_plot in plots:
        for each_subplot in each_plot['plots']:
            if each_subplot['id'] == input_id:
                each_subplot['x'].append((val[0]))
                each_subplot['y'].append(float(val[1]))
                return


def check_entry_and_get_struct(entry: dict, data_label: str) -> dict:
    mandatory_key = 'id'
    other_keys = {'x': [], 'y': [], 'type': 'scatter'}

    ret_dict = entry
    if ret_dict.get(mandatory_key) is None:
        raise KeyError('ID key is missing')

    ret_dict['name'] = data_label
    for each_key, each_value in other_keys.items():
        if ret_dict.get(each_key) is None:
            ret_dict[each_key] = each_value

    return ret_dict


def validate_json(json_file: Any) -> None:
    mandatory_keys = {'id':int, 'x_axis_data_size': int, 'y_axis_data_size': int, 'data_type': str, 'x_axis_timestamp': bool}
    for each_plot in json_file:
        if each_plot.startswith(COMMENT_LINE):
            continue
        try:
            each_subplot = json_file[each_plot]['data_streams']
        except KeyError:
            print('data_streams key not found. Aborting')
            raise SystemExit(1)
        for each_subplot in json_file[each_plot]['data_streams']:
            for key, value in mandatory_keys.items():
                try:
                    val = json_file[each_plot]['data_streams'][each_subplot][key]
                    if not isinstance(val, value):
                        print('[{}][data_streams][{}][{}] expected {} found {}'.format(each_plot, each_subplot, key, mandatory_keys[key], type(val)))
                        raise SystemExit(1)
                except KeyError:
                    print('[{}][data_streams][{}][{}] key not found. Aborting'.format(each_plot, each_subplot, key))
                    raise SystemExit(1)


def configure_plots(json_file: Any) -> None:
    for each_plot in json_file:
        if each_plot.startswith(COMMENT_LINE):
            continue
        data_struct = {'title': each_plot, 'plots': [], 'xaxis_title': json_file[each_plot]['xaxis_title'], 'yaxis_title': json_file[each_plot]['yaxis_title']}
        for each_subplot in json_file[each_plot]['data_streams']:
            subplot_items = json_file[each_plot]['data_streams'][each_subplot]
            plot_data_struct = check_entry_and_get_struct(subplot_items, each_subplot)
            data_struct['plots'].append(plot_data_struct)
        plots.append(data_struct)


def main() -> None:
    parser = argparse.ArgumentParser(description='Apptrace Visualizing Tool')
    parser.add_argument('--plot-config', help='Path to json file', required=False, type=str,
                        default=os.path.realpath(os.path.join(os.path.dirname(__file__), 'data.json')))
    parser.add_argument('--source', help='Data source path', required=True, type=str)
    parser.add_argument('--output-file', help='Path to program output file in txt format', type=str)

    args = parser.parse_args()
    output_file_path = args.output_file
    json_file_name = args.plot_config
    data_source = args.source

    json_file = read_json(json_file_name)
    validate_json(json_file)
    configure_plots(json_file)
    reader = espytrace.apptrace.reader_create(data_source, 1, CustomRequestHandler)
    signal.signal(signal.SIGINT, partial(signal_handler, output_file_path, reader))

    app.run_server(debug=True, use_reloader=False, port=8055)


if __name__ == '__main__':
    main()
