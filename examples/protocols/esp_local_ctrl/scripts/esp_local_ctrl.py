#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import asyncio
import json
import os
import ssl
import struct
import sys
import textwrap
from getpass import getpass

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
    if prop['type'] == PROP_TYPE_TIMESTAMP:
        return 'TIME(us)'
    elif prop['type'] == PROP_TYPE_INT32:
        return 'INT32'
    elif prop['type'] == PROP_TYPE_BOOLEAN:
        return 'BOOLEAN'
    elif prop['type'] == PROP_TYPE_STRING:
        return 'STRING'
    return 'UNKNOWN'


def encode_prop_value(prop, value):
    try:
        if prop['type'] == PROP_TYPE_TIMESTAMP:
            return struct.pack('q', value)
        elif prop['type'] == PROP_TYPE_INT32:
            return struct.pack('i', value)
        elif prop['type'] == PROP_TYPE_BOOLEAN:
            return struct.pack('?', value)
        elif prop['type'] == PROP_TYPE_STRING:
            return bytes(value, encoding='latin-1')
        return value
    except struct.error as e:
        print(e)
    return None


def decode_prop_value(prop, value):
    try:
        if prop['type'] == PROP_TYPE_TIMESTAMP:
            return struct.unpack('q', value)[0]
        elif prop['type'] == PROP_TYPE_INT32:
            return struct.unpack('i', value)[0]
        elif prop['type'] == PROP_TYPE_BOOLEAN:
            return struct.unpack('?', value)[0]
        elif prop['type'] == PROP_TYPE_STRING:
            return value.decode('latin-1')
        return value
    except struct.error as e:
        print(e)
    return None


def str_to_prop_value(prop, strval):
    try:
        if prop['type'] == PROP_TYPE_TIMESTAMP:
            return int(strval)
        elif prop['type'] == PROP_TYPE_INT32:
            return int(strval)
        elif prop['type'] == PROP_TYPE_BOOLEAN:
            return bool(strval)
        elif prop['type'] == PROP_TYPE_STRING:
            return strval
        return strval
    except ValueError as e:
        print(e)
        return None


def prop_is_readonly(prop):
    return (prop['flags'] & PROP_FLAG_READONLY) != 0


def on_except(err):
    if config_throw_except:
        raise RuntimeError(err)
    else:
        print(err)


def get_security(secver, username, password, pop='', verbose=False):
    if secver == 2:
        return security.Security2(username, password, verbose)
    if secver == 1:
        return security.Security1(pop, verbose)
    if secver == 0:
        return security.Security0(verbose)
    return None


async def get_transport(sel_transport, service_name, check_hostname):
    try:
        tp = None
        if (sel_transport == 'http'):
            tp = esp_prov.transport.Transport_HTTP(service_name, None)
        elif (sel_transport == 'https'):
            example_path = os.environ['IDF_PATH'] + '/examples/protocols/esp_local_ctrl'
            cert_path = example_path + '/main/certs/rootCA.pem'
            ssl_ctx = ssl.create_default_context(cafile=cert_path)
            ssl_ctx.check_hostname = check_hostname
            tp = esp_prov.transport.Transport_HTTP(service_name, ssl_ctx)
        elif (sel_transport == 'ble'):
            tp = esp_prov.transport.Transport_BLE(
                service_uuid='3d981e4a-31eb-42b4-8a68-75bd8d3bd521',
                nu_lookup={'esp_local_ctrl/version': '0001',
                           'esp_local_ctrl/session': '0002',
                           'esp_local_ctrl/control': '0003'}
            )
            await tp.connect(devname=service_name)
        return tp
    except RuntimeError as e:
        on_except(e)
        return None


async def version_match(tp, protover, verbose=False):
    try:
        response = await tp.send_data('esp_local_ctrl/version', protover)

        if verbose:
            print('esp_local_ctrl/version response : ', response)

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


async def has_capability(tp, capability='none', verbose=False):
    # Note : default value of `capability` argument cannot be empty string
    # because protocomm_httpd expects non zero content lengths
    try:
        response = await tp.send_data('esp_local_ctrl/version', capability)

        if verbose:
            print('esp_local_ctrl/version response : ', response)

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


async def establish_session(tp, sec):
    try:
        response = None
        while True:
            request = sec.security_session(response)
            if request is None:
                break
            response = await tp.send_data('esp_local_ctrl/session', request)
            if (response is None):
                return False
        return True
    except RuntimeError as e:
        on_except(e)
        return None


async def get_all_property_values(tp, security_ctx):
    try:
        props = []
        message = proto_lc.get_prop_count_request(security_ctx)
        response = await tp.send_data('esp_local_ctrl/control', message)
        count = proto_lc.get_prop_count_response(security_ctx, response)
        if count == 0:
            raise RuntimeError('No properties found!')
        indices = [i for i in range(count)]
        message = proto_lc.get_prop_vals_request(security_ctx, indices)
        response = await tp.send_data('esp_local_ctrl/control', message)
        props = proto_lc.get_prop_vals_response(security_ctx, response)
        if len(props) != count:
            raise RuntimeError('Incorrect count of properties!', len(props), count)
        for p in props:
            p['value'] = decode_prop_value(p, p['value'])
        return props
    except RuntimeError as e:
        on_except(e)
        return []


async def set_property_values(tp, security_ctx, props, indices, values, check_readonly=False):
    try:
        if check_readonly:
            for index in indices:
                if prop_is_readonly(props[index]):
                    raise RuntimeError('Cannot set value of Read-Only property')
        message = proto_lc.set_prop_vals_request(security_ctx, indices, values)
        response = await tp.send_data('esp_local_ctrl/control', message)
        return proto_lc.set_prop_vals_response(security_ctx, response)
    except RuntimeError as e:
        on_except(e)
        return False


def desc_format(*args):
    desc = ''
    for arg in args:
        desc += textwrap.fill(replace_whitespace=False, text=arg) + '\n'
    return desc


async def main():
    parser = argparse.ArgumentParser(add_help=False)

    parser = argparse.ArgumentParser(description='Control an ESP32 running esp_local_ctrl service')

    parser.add_argument('--version', dest='version', type=str,
                        help='Protocol version', default='')

    parser.add_argument('--transport', dest='transport', type=str,
                        help='transport i.e http/https/ble', default='https')

    parser.add_argument('--name', dest='service_name', type=str,
                        help='BLE Device Name / HTTP Server hostname or IP', default='')

    parser.add_argument('--sec_ver', dest='secver', type=int, default=None,
                        help=desc_format(
                            'Protocomm security scheme used for secure '
                            'session establishment. Accepted values are :',
                            '\t- 0 : No security',
                            '\t- 1 : X25519 key exchange + AES-CTR encryption',
                            '\t- 2 : SRP6a + AES-GCM encryption',
                            '\t      + Authentication using Proof of Possession (PoP)'))

    parser.add_argument('--pop', dest='pop', type=str, default='',
                        help=desc_format(
                            'This specifies the Proof of possession (PoP) when security scheme 1 '
                            'is used'))

    parser.add_argument('--sec2_username', dest='sec2_usr', type=str, default='',
                        help=desc_format(
                            'Username for security scheme 2 (SRP6a)'))

    parser.add_argument('--sec2_pwd', dest='sec2_pwd', type=str, default='',
                        help=desc_format(
                            'Password for security scheme 2 (SRP6a)'))

    parser.add_argument('--sec2_gen_cred', help='Generate salt and verifier for security scheme 2 (SRP6a)', action='store_true')

    parser.add_argument('--sec2_salt_len', dest='sec2_salt_len', type=int, default=16,
                        help=desc_format(
                            'Salt length for security scheme 2 (SRP6a)'))

    parser.add_argument('--dont-check-hostname', action='store_true',
                        # If enabled, the certificate won't be rejected for hostname mismatch.
                        # This option is hidden because it should be used only for testing purposes.
                        help=argparse.SUPPRESS)

    parser.add_argument('-v', '--verbose', dest='verbose', help='increase output verbosity', action='store_true')

    args = parser.parse_args()

    if args.secver == 2 and args.sec2_gen_cred:
        if not args.sec2_usr or not args.sec2_pwd:
            raise ValueError('Username/password cannot be empty for security scheme 2 (SRP6a)')

        print('==== Salt-verifier for security scheme 2 (SRP6a) ====')
        security.sec2_gen_salt_verifier(args.sec2_usr, args.sec2_pwd, args.sec2_salt_len)
        sys.exit()

    if args.version != '':
        print(f'==== Esp_Ctrl Version: {args.version} ====')

    if args.service_name == '':
        args.service_name = 'my_esp_ctrl_device'
        if args.transport == 'http' or args.transport == 'https':
            args.service_name += '.local'

    obj_transport = await get_transport(args.transport, args.service_name, not args.dont_check_hostname)
    if obj_transport is None:
        raise RuntimeError('Failed to establish connection')

    # If security version not specified check in capabilities
    if args.secver is None:
        # First check if capabilities are supported or not
        if not await has_capability(obj_transport):
            print('Security capabilities could not be determined, please specify "--sec_ver" explicitly')
            raise ValueError('Invalid Security Version')

        # When no_sec is present, use security 0, else security 1
        args.secver = int(not await has_capability(obj_transport, 'no_sec'))
        print(f'==== Security Scheme: {args.secver} ====')

    if (args.secver == 1):
        if not await has_capability(obj_transport, 'no_pop'):
            if len(args.pop) == 0:
                print('---- Proof of Possession argument not provided ----')
                exit(2)
        elif len(args.pop) != 0:
            print('---- Proof of Possession will be ignored ----')
            args.pop = ''

    if (args.secver == 2):
        if len(args.sec2_usr) == 0:
            args.sec2_usr = input('Security Scheme 2 - SRP6a Username required: ')
        if len(args.sec2_pwd) == 0:
            prompt_str = 'Security Scheme 2 - SRP6a Password required: '
            args.sec2_pwd = getpass(prompt_str)

    obj_security = get_security(args.secver, args.sec2_usr, args.sec2_pwd, args.pop, args.verbose)
    if obj_security is None:
        raise ValueError('Invalid Security Version')

    if args.version != '':
        print('\n==== Verifying protocol version ====')
        if not await version_match(obj_transport, args.version, args.verbose):
            raise RuntimeError('Error in protocol version matching')
        print('==== Verified protocol version successfully ====')

    print('\n==== Starting Session ====')
    if not await establish_session(obj_transport, obj_security):
        print('Failed to establish session. Ensure that security scheme and proof of possession are correct')
        raise RuntimeError('Error in establishing session')
    print('==== Session Established ====')

    while True:
        properties = await get_all_property_values(obj_transport, obj_security)
        if len(properties) == 0:
            raise RuntimeError('Error in reading property value')

        print('\n==== Available Properties ====')
        print('{0: >4} {1: <16} {2: <10} {3: <16} {4: <16}'.format(
            'S.N.', 'Name', 'Type', 'Flags', 'Value'))
        for i in range(len(properties)):
            print('[{0: >2}] {1: <16} {2: <10} {3: <16} {4: <16}'.format(
                i + 1, properties[i]['name'], prop_typestr(properties[i]),
                ['','Read-Only'][prop_is_readonly(properties[i])],
                str(properties[i]['value'])))

        select = 0
        while True:
            try:
                inval = input('\nSelect properties to set (0 to re-read, \'q\' to quit) : ')
                if inval.lower() == 'q':
                    print('Quitting...')
                    exit(0)
                invals = inval.split(',')
                selections = [int(val) for val in invals]
                if min(selections) < 0 or max(selections) > len(properties):
                    raise ValueError('Invalid input')
                break
            except ValueError as e:
                print(str(e) + '! Retry...')

        if len(selections) == 1 and selections[0] == 0:
            continue

        set_values = []
        set_indices = []
        for select in selections:
            while True:
                inval = input('Enter value to set for property (' + properties[select - 1]['name'] + ') : ')
                value = encode_prop_value(properties[select - 1],
                                          str_to_prop_value(properties[select - 1], inval))
                if value is None:
                    print('Invalid input! Retry...')
                    continue
                break
            set_values += [value]
            set_indices += [select - 1]

        if not await set_property_values(obj_transport, obj_security, properties, set_indices, set_values):
            print('Failed to set values!')

if __name__ == '__main__':
    asyncio.run(main())
