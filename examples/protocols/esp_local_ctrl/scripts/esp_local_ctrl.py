#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

from __future__ import print_function
from future.utils import tobytes
from builtins import input
import os
import sys
import struct
import argparse

import proto

try:
    import esp_prov

except ImportError:
    idf_path = os.environ['IDF_PATH']
    sys.path.insert(1, idf_path + "/tools/esp_prov")
    import esp_prov


# Set this to true to allow exceptions to be thrown
config_throw_except = False


# Property types enum
PROP_TYPE_TIMESTAMP = 0
PROP_TYPE_INT32     = 1
PROP_TYPE_BOOLEAN   = 2
PROP_TYPE_STRING    = 3


# Property flags enum
PROP_FLAG_READONLY = (1 << 0)


def prop_typestr(prop):
    if prop["type"] == PROP_TYPE_TIMESTAMP:
        return "TIME(us)"
    elif prop["type"] == PROP_TYPE_INT32:
        return "INT32"
    elif prop["type"] == PROP_TYPE_BOOLEAN:
        return "BOOLEAN"
    elif prop["type"] == PROP_TYPE_STRING:
        return "STRING"
    return "UNKNOWN"


def encode_prop_value(prop, value):
    try:
        if prop["type"] == PROP_TYPE_TIMESTAMP:
            return struct.pack('q', value)
        elif prop["type"] == PROP_TYPE_INT32:
            return struct.pack('i', value)
        elif prop["type"] == PROP_TYPE_BOOLEAN:
            return struct.pack('?', value)
        elif prop["type"] == PROP_TYPE_STRING:
            return tobytes(value)
        return value
    except struct.error as e:
        print(e)
    return None


def decode_prop_value(prop, value):
    try:
        if prop["type"] == PROP_TYPE_TIMESTAMP:
            return struct.unpack('q', value)[0]
        elif prop["type"] == PROP_TYPE_INT32:
            return struct.unpack('i', value)[0]
        elif prop["type"] == PROP_TYPE_BOOLEAN:
            return struct.unpack('?', value)[0]
        elif prop["type"] == PROP_TYPE_STRING:
            return value.decode('latin-1')
        return value
    except struct.error as e:
        print(e)
    return None


def str_to_prop_value(prop, strval):
    try:
        if prop["type"] == PROP_TYPE_TIMESTAMP:
            return int(strval)
        elif prop["type"] == PROP_TYPE_INT32:
            return int(strval)
        elif prop["type"] == PROP_TYPE_BOOLEAN:
            return bool(strval)
        elif prop["type"] == PROP_TYPE_STRING:
            return strval
        return strval
    except ValueError as e:
        print(e)
        return None


def prop_is_readonly(prop):
    return (prop["flags"] & PROP_FLAG_READONLY) is not 0


def on_except(err):
    if config_throw_except:
        raise RuntimeError(err)
    else:
        print(err)


def get_transport(sel_transport, service_name):
    try:
        tp = None
        if (sel_transport == 'http'):
            example_path = os.environ['IDF_PATH'] + "/examples/protocols/esp_local_ctrl"
            cert_path = example_path + "/main/certs/rootCA.pem"
            tp = esp_prov.transport.Transport_HTTP(service_name, cert_path)
        elif (sel_transport == 'ble'):
            tp = esp_prov.transport.Transport_BLE(
                devname=service_name, service_uuid='0000ffff-0000-1000-8000-00805f9b34fb',
                nu_lookup={'esp_local_ctrl/version': '0001',
                           'esp_local_ctrl/session': '0002',
                           'esp_local_ctrl/control': '0003'}
            )
        return tp
    except RuntimeError as e:
        on_except(e)
        return None


def version_match(tp, expected, verbose=False):
    try:
        response = tp.send_data('esp_local_ctrl/version', expected)
        return (response.lower() == expected.lower())
    except Exception as e:
        on_except(e)
        return None


def get_all_property_values(tp):
    try:
        props = []
        message = proto.get_prop_count_request()
        response = tp.send_data('esp_local_ctrl/control', message)
        count = proto.get_prop_count_response(response)
        if count == 0:
            raise RuntimeError("No properties found!")
        indices = [i for i in range(count)]
        message = proto.get_prop_vals_request(indices)
        response = tp.send_data('esp_local_ctrl/control', message)
        props = proto.get_prop_vals_response(response)
        if len(props) != count:
            raise RuntimeError("Incorrect count of properties!")
        for p in props:
            p["value"] = decode_prop_value(p, p["value"])
        return props
    except RuntimeError as e:
        on_except(e)
        return []


def set_property_values(tp, props, indices, values, check_readonly=False):
    try:
        if check_readonly:
            for index in indices:
                if prop_is_readonly(props[index]):
                    raise RuntimeError("Cannot set value of Read-Only property")
        message = proto.set_prop_vals_request(indices, values)
        response = tp.send_data('esp_local_ctrl/control', message)
        return proto.set_prop_vals_response(response)
    except RuntimeError as e:
        on_except(e)
        return False


if __name__ == '__main__':
    parser = argparse.ArgumentParser(add_help=False)

    parser = argparse.ArgumentParser(description="Control an ESP32 running esp_local_ctrl service")

    parser.add_argument("--version", dest='version', type=str,
                        help="Protocol version", default='')

    parser.add_argument("--transport", dest='transport', type=str,
                        help="transport i.e http or ble", default='http')

    parser.add_argument("--name", dest='service_name', type=str,
                        help="BLE Device Name / HTTP Server hostname or IP", default='')

    parser.add_argument("-v", "--verbose", dest='verbose', help="increase output verbosity", action="store_true")
    args = parser.parse_args()

    if args.version != '':
        print("==== Esp_Ctrl Version: " + args.version + " ====")

    if args.service_name == '':
        args.service_name = 'my_esp_ctrl_device'
        if args.transport == 'http':
            args.service_name += '.local'

    obj_transport = get_transport(args.transport, args.service_name)
    if obj_transport is None:
        print("---- Invalid transport ----")
        exit(1)

    if args.version != '':
        print("\n==== Verifying protocol version ====")
        if not version_match(obj_transport, args.version, args.verbose):
            print("---- Error in protocol version matching ----")
            exit(2)
        print("==== Verified protocol version successfully ====")

    while True:
        properties = get_all_property_values(obj_transport)
        if len(properties) == 0:
            print("---- Error in reading property values ----")
            exit(4)

        print("\n==== Available Properties ====")
        print("{0: >4} {1: <16} {2: <10} {3: <16} {4: <16}".format(
            "S.N.", "Name", "Type", "Flags", "Value"))
        for i in range(len(properties)):
            print("[{0: >2}] {1: <16} {2: <10} {3: <16} {4: <16}".format(
                i + 1, properties[i]["name"], prop_typestr(properties[i]),
                ["","Read-Only"][prop_is_readonly(properties[i])],
                str(properties[i]["value"])))

        select = 0
        while True:
            try:
                inval = input("\nSelect properties to set (0 to re-read, 'q' to quit) : ")
                if inval.lower() == 'q':
                    print("Quitting...")
                    exit(5)
                invals = inval.split(',')
                selections = [int(val) for val in invals]
                if min(selections) < 0 or max(selections) > len(properties):
                    raise ValueError("Invalid input")
                break
            except ValueError as e:
                print(str(e) + "! Retry...")

        if len(selections) == 1 and selections[0] == 0:
            continue

        set_values = []
        set_indices = []
        for select in selections:
            while True:
                inval = input("Enter value to set for property (" + properties[select - 1]["name"] + ") : ")
                value = encode_prop_value(properties[select - 1],
                                          str_to_prop_value(properties[select - 1], inval))
                if value is None:
                    print("Invalid input! Retry...")
                    continue
                break
            set_values += [value]
            set_indices += [select - 1]

        if not set_property_values(obj_transport, properties, set_indices, set_values):
            print("Failed to set values!")
