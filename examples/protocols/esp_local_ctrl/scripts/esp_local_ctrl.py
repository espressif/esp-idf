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
import json
import ssl

import textwrap

import proto_lc

try:
    import esp_prov
    import security

except ImportError:
    idf_path = os.environ['IDF_PATH']
    sys.path.insert(0, idf_path + '/components/protocomm/python')
    sys.path.insert(1, idf_path + '/tools/esp_prov')

    import esp_prov
    import security


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
    return (prop["flags"] & PROP_FLAG_READONLY) != 0


def on_except(err):
    if config_throw_except:
        raise RuntimeError(err)
    else:
        print(err)


def get_security(secver, pop=None, verbose=False):
    if secver == 1:
        return security.Security1(pop, verbose)
    elif secver == 0:
        return security.Security0(verbose)
    return None


def get_transport(sel_transport, service_name, check_hostname):
    try:
        tp = None
        if (sel_transport == 'http'):
            example_path = os.environ['IDF_PATH'] + "/examples/protocols/esp_local_ctrl"
            cert_path = example_path + "/main/certs/rootCA.pem"
            ssl_ctx = ssl.create_default_context(cafile=cert_path)
            ssl_ctx.check_hostname = check_hostname
            tp = esp_prov.transport.Transport_HTTP(service_name, ssl_ctx)
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


def version_match(tp, protover, verbose=False):
    try:
        response = tp.send_data('proto-ver', protover)

        if verbose:
            print('proto-ver response : ', response)

        # First assume this to be a simple version string
        if response.lower() == protover.lower():
            return True

        try:
            # Else interpret this as JSON structure containing
            # information with versions and capabilities of both
            # provisioning service and application
            info = json.loads(response)
            if info['prov']['ver'].lower() == protover.lower():
                return True

        except ValueError:
            # If decoding as JSON fails, it means that capabilities
            # are not supported
            return False

    except Exception as e:
        on_except(e)
        return None


def has_capability(tp, capability='none', verbose=False):
    # Note : default value of `capability` argument cannot be empty string
    # because protocomm_httpd expects non zero content lengths
    try:
        response = tp.send_data('proto-ver', capability)

        if verbose:
            print('proto-ver response : ', response)

        try:
            # Interpret this as JSON structure containing
            # information with versions and capabilities of both
            # provisioning service and application
            info = json.loads(response)
            supported_capabilities = info['prov']['cap']
            if capability.lower() == 'none':
                # No specific capability to check, but capabilities
                # feature is present so return True
                return True
            elif capability in supported_capabilities:
                return True
            return False

        except ValueError:
            # If decoding as JSON fails, it means that capabilities
            # are not supported
            return False

    except RuntimeError as e:
        on_except(e)

    return False


def establish_session(tp, sec):
    try:
        response = None
        while True:
            request = sec.security_session(response)
            if request is None:
                break
            response = tp.send_data('esp_local_ctrl/session', request)
            if (response is None):
                return False
        return True
    except RuntimeError as e:
        on_except(e)
        return None


def get_all_property_values(tp, security_ctx):
    try:
        props = []
        message = proto_lc.get_prop_count_request(security_ctx)
        response = tp.send_data('esp_local_ctrl/control', message)
        count = proto_lc.get_prop_count_response(security_ctx, response)
        if count == 0:
            raise RuntimeError("No properties found!")
        indices = [i for i in range(count)]
        message = proto_lc.get_prop_vals_request(security_ctx, indices)
        response = tp.send_data('esp_local_ctrl/control', message)
        props = proto_lc.get_prop_vals_response(security_ctx, response)
        if len(props) != count:
            raise RuntimeError('Incorrect count of properties!', len(props), count)
        for p in props:
            p["value"] = decode_prop_value(p, p["value"])
        return props
    except RuntimeError as e:
        on_except(e)
        return []


def set_property_values(tp, security_ctx, props, indices, values, check_readonly=False):
    try:
        if check_readonly:
            for index in indices:
                if prop_is_readonly(props[index]):
                    raise RuntimeError('Cannot set value of Read-Only property')
        message = proto_lc.set_prop_vals_request(security_ctx, indices, values)
        response = tp.send_data('esp_local_ctrl/control', message)
        return proto_lc.set_prop_vals_response(security_ctx, response)
    except RuntimeError as e:
        on_except(e)
        return False


def desc_format(*args):
    desc = ''
    for arg in args:
        desc += textwrap.fill(replace_whitespace=False, text=arg) + '\n'
    return desc


if __name__ == '__main__':
    parser = argparse.ArgumentParser(add_help=False)

    parser = argparse.ArgumentParser(description="Control an ESP32 running esp_local_ctrl service")

    parser.add_argument("--version", dest='version', type=str,
                        help="Protocol version", default='')

    parser.add_argument("--transport", dest='transport', type=str,
                        help="transport i.e http or ble", default='http')

    parser.add_argument("--name", dest='service_name', type=str,
                        help="BLE Device Name / HTTP Server hostname or IP", default='')

    parser.add_argument('--sec_ver', dest='secver', type=int, default=None,
                        help=desc_format(
                            'Protocomm security scheme used by the provisioning service for secure '
                            'session establishment. Accepted values are :',
                            '\t- 0 : No security',
                            '\t- 1 : X25519 key exchange + AES-CTR encryption',
                            '\t      + Authentication using Proof of Possession (PoP)',
                            'In case device side application uses IDF\'s provisioning manager, '
                            'the compatible security version is automatically determined from '
                            'capabilities retrieved via the version endpoint'))

    parser.add_argument('--pop', dest='pop', type=str, default='',
                        help=desc_format(
                            'This specifies the Proof of possession (PoP) when security scheme 1 '
                            'is used'))

    parser.add_argument('--dont-check-hostname', action='store_true',
                        # If enabled, the certificate won't be rejected for hostname mismatch.
                        # This option is hidden because it should be used only for testing purposes.
                        help=argparse.SUPPRESS)

    parser.add_argument("-v", "--verbose", dest='verbose', help="increase output verbosity", action="store_true")
    args = parser.parse_args()

    if args.version != '':
        print("==== Esp_Ctrl Version: " + args.version + " ====")

    if args.service_name == '':
        args.service_name = 'my_esp_ctrl_device'
        if args.transport == 'http':
            args.service_name += '.local'

    obj_transport = get_transport(args.transport, args.service_name, not args.dont_check_hostname)
    if obj_transport is None:
        print("---- Invalid transport ----")
        exit(1)

    # If security version not specified check in capabilities
    if args.secver is None:
        # First check if capabilities are supported or not
        if not has_capability(obj_transport):
            print('Security capabilities could not be determined. Please specify \'--sec_ver\' explicitly')
            print('---- Invalid Security Version ----')
            exit(2)

        # When no_sec is present, use security 0, else security 1
        args.secver = int(not has_capability(obj_transport, 'no_sec'))
        print('Security scheme determined to be :', args.secver)

        if (args.secver != 0) and not has_capability(obj_transport, 'no_pop'):
            if len(args.pop) == 0:
                print('---- Proof of Possession argument not provided ----')
                exit(2)
        elif len(args.pop) != 0:
            print('---- Proof of Possession will be ignored ----')
            args.pop = ''

    obj_security = get_security(args.secver, args.pop, False)
    if obj_security is None:
        print('---- Invalid Security Version ----')
        exit(2)

    if args.version != '':
        print("\n==== Verifying protocol version ====")
        if not version_match(obj_transport, args.version, args.verbose):
            print("---- Error in protocol version matching ----")
            exit(2)
        print("==== Verified protocol version successfully ====")

    print('\n==== Starting Session ====')
    if not establish_session(obj_transport, obj_security):
        print('Failed to establish session. Ensure that security scheme and proof of possession are correct')
        print('---- Error in establishing session ----')
        exit(3)
    print('==== Session Established ====')

    while True:
        properties = get_all_property_values(obj_transport, obj_security)
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
                inval = input('\nSelect properties to set (0 to re-read, \'q\' to quit) : ')
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

        if not set_property_values(obj_transport, obj_security, properties, set_indices, set_values):
            print('Failed to set values!')
