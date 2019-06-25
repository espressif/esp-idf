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
from builtins import input
import argparse
import time
import os
import sys
import json
from getpass import getpass

try:
    import security
    import transport
    import prov

except ImportError:
    idf_path = os.environ['IDF_PATH']
    sys.path.insert(0, idf_path + "/components/protocomm/python")
    sys.path.insert(1, idf_path + "/tools/esp_prov")

    import security
    import transport
    import prov

# Set this to true to allow exceptions to be thrown
config_throw_except = False


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


def get_transport(sel_transport, softap_endpoint=None, ble_devname=None):
    try:
        tp = None
        if (sel_transport == 'softap'):
            tp = transport.Transport_HTTP(softap_endpoint)
        elif (sel_transport == 'ble'):
            # BLE client is now capable of automatically figuring out
            # the primary service from the advertisement data and the
            # characteristics corresponding to each endpoint.
            # Below, the service_uuid field and 16bit UUIDs in the nu_lookup
            # table are provided only to support devices running older firmware,
            # in which case, the automated discovery will fail and the client
            # will fallback to using the provided UUIDs instead
            nu_lookup = {'prov-session': 'ff51', 'prov-config': 'ff52', 'proto-ver': 'ff53'}
            tp = transport.Transport_BLE(devname=ble_devname,
                                         service_uuid='0000ffff-0000-1000-8000-00805f9b34fb',
                                         nu_lookup=nu_lookup)
        elif (sel_transport == 'console'):
            tp = transport.Transport_Console()
        return tp
    except RuntimeError as e:
        on_except(e)
        return None


def version_match(tp, protover, verbose=False):
    try:
        response = tp.send_data('proto-ver', protover)

        if verbose:
            print("proto-ver response : ", response)

        # First assume this to be a simple version string
        if response.lower() == protover.lower():
            return True

        # Else interpret this as JSON structure containing
        # information with versions and capabilities of both
        # provisioning service and application
        info = json.loads(response)
        if info['prov']['ver'].lower() == protover.lower():
            return True

        return False
    except Exception as e:
        on_except(e)
        return None


def has_capability(tp, capability, verbose=False):
    try:
        response = tp.send_data('proto-ver', capability)

        if verbose:
            print("proto-ver response : ", response)

        info = json.loads(response)
        if capability in info['prov']['cap']:
            return True

    except Exception as e:
        on_except(e)

    return False


def get_version(tp):
    response = None
    try:
        response = tp.send_data('proto-ver', '---')
    except RuntimeError as e:
        on_except(e)
        response = ''
    return response


def establish_session(tp, sec):
    try:
        response = None
        while True:
            request = sec.security_session(response)
            if request is None:
                break
            response = tp.send_data('prov-session', request)
            if (response is None):
                return False
        return True
    except RuntimeError as e:
        on_except(e)
        return None


def custom_config(tp, sec, custom_info, custom_ver):
    try:
        message = prov.custom_config_request(sec, custom_info, custom_ver)
        response = tp.send_data('custom-config', message)
        return (prov.custom_config_response(sec, response) == 0)
    except RuntimeError as e:
        on_except(e)
        return None


def scan_wifi_APs(sel_transport, tp, sec):
    APs = []
    group_channels = 0
    readlen = 100
    if sel_transport == 'softap':
        # In case of softAP we must perform the scan on individual channels, one by one,
        # so that the Wi-Fi controller gets ample time to send out beacons (necessary to
        # maintain connectivity with authenticated stations. As scanning one channel at a
        # time will be slow, we can group more than one channels to be scanned in quick
        # succession, hence speeding up the scan process. Though if too many channels are
        # present in a group, the controller may again miss out on sending beacons. Hence,
        # the application must should use an optimum value. The following value usually
        # works out in most cases
        group_channels = 5
    elif sel_transport == 'ble':
        # Read at most 4 entries at a time. This is because if we are using BLE transport
        # then the response packet size should not exceed the present limit of 256 bytes of
        # characteristic value imposed by protocomm_ble. This limit may be removed in the
        # future
        readlen = 4
    try:
        message = prov.scan_start_request(sec, blocking=True, group_channels=group_channels)
        start_time = time.time()
        response = tp.send_data('prov-scan', message)
        stop_time = time.time()
        print("++++ Scan process executed in " + str(stop_time - start_time) + " sec")
        prov.scan_start_response(sec, response)

        message = prov.scan_status_request(sec)
        response = tp.send_data('prov-scan', message)
        result = prov.scan_status_response(sec, response)
        print("++++ Scan results : " + str(result["count"]))
        if result["count"] != 0:
            index = 0
            remaining = result["count"]
            while remaining:
                count = [remaining, readlen][remaining > readlen]
                message = prov.scan_result_request(sec, index, count)
                response = tp.send_data('prov-scan', message)
                APs += prov.scan_result_response(sec, response)
                remaining -= count
                index += count

    except RuntimeError as e:
        on_except(e)
        return None

    return APs


def send_wifi_config(tp, sec, ssid, passphrase):
    try:
        message = prov.config_set_config_request(sec, ssid, passphrase)
        response = tp.send_data('prov-config', message)
        return (prov.config_set_config_response(sec, response) == 0)
    except RuntimeError as e:
        on_except(e)
        return None


def apply_wifi_config(tp, sec):
    try:
        message = prov.config_apply_config_request(sec)
        response = tp.send_data('prov-config', message)
        return (prov.config_apply_config_response(sec, response) == 0)
    except RuntimeError as e:
        on_except(e)
        return None


def get_wifi_config(tp, sec):
    try:
        message = prov.config_get_status_request(sec)
        response = tp.send_data('prov-config', message)
        return prov.config_get_status_response(sec, response)
    except RuntimeError as e:
        on_except(e)
        return None


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate ESP prov payload")

    parser.add_argument("--ssid", dest='ssid', type=str,
                        help="SSID of Wi-Fi Network", default='')
    parser.add_argument("--passphrase", dest='passphrase', type=str,
                        help="Passphrase of Wi-Fi network", default='')

    parser.add_argument("--sec_ver", dest='secver', type=int,
                        help="Security scheme version", default=None)
    parser.add_argument("--proto_ver", dest='protover', type=str,
                        help="Protocol version", default='')
    parser.add_argument("--pop", dest='pop', type=str,
                        help="Proof of possession", default='')

    parser.add_argument("--softap_endpoint", dest='softap_endpoint', type=str,
                        help="<softap_ip:port>, http(s):// shouldn't be included", default='192.168.4.1:80')

    parser.add_argument("--ble_devname", dest='ble_devname', type=str,
                        help="BLE Device Name", default='')

    parser.add_argument("--transport", dest='provmode', type=str,
                        help="provisioning mode i.e console or softap or ble", default='softap')

    parser.add_argument("--custom_config", help="Provision Custom Configuration",
                        action="store_true")
    parser.add_argument("--custom_info", dest='custom_info', type=str,
                        help="Custom Config Info String", default='<some custom info string>')
    parser.add_argument("--custom_ver", dest='custom_ver', type=int,
                        help="Custom Config Version Number", default=2)

    parser.add_argument("-v","--verbose", help="increase output verbosity", action="store_true")
    args = parser.parse_args()

    if args.protover != '':
        print("==== Esp_Prov Version: " + args.protover + " ====")

    obj_transport = get_transport(args.provmode, args.softap_endpoint, args.ble_devname)
    if obj_transport is None:
        print("---- Invalid provisioning mode ----")
        exit(1)

    # If security version not specified check in capabilities
    if args.secver is None:
        # When no_sec is present, use security 0, else security 1
        args.secver = int(not has_capability(obj_transport, 'no_sec'))

    if (args.secver != 0) and not has_capability(obj_transport, 'no_pop'):
        if len(args.pop) == 0:
            print("---- Proof of Possession argument not provided ----")
            exit(2)
    elif len(args.pop) != 0:
        print("---- Proof of Possession will be ignored ----")
        args.pop = ''

    obj_security = get_security(args.secver, args.pop, args.verbose)
    if obj_security is None:
        print("---- Invalid Security Version ----")
        exit(2)

    if args.protover != '':
        print("\n==== Verifying protocol version ====")
        if not version_match(obj_transport, args.protover, args.verbose):
            print("---- Error in protocol version matching ----")
            exit(3)
        print("==== Verified protocol version successfully ====")

    print("\n==== Starting Session ====")
    if not establish_session(obj_transport, obj_security):
        print("---- Error in establishing session ----")
        exit(4)
    print("==== Session Established ====")

    if args.custom_config:
        print("\n==== Sending Custom config to esp32 ====")
        if not custom_config(obj_transport, obj_security, args.custom_info, args.custom_ver):
            print("---- Error in custom config ----")
            exit(5)
        print("==== Custom config sent successfully ====")

    if args.ssid == '':
        if not has_capability(obj_transport, 'wifi_scan'):
            print("---- Wi-Fi Scan List is not supported by provisioning service ----")
            print("---- Rerun esp_prov with SSID and Passphrase as argument ----")
            exit(3)

        while True:
            print("\n==== Scanning Wi-Fi APs ====")
            start_time = time.time()
            APs = scan_wifi_APs(args.provmode, obj_transport, obj_security)
            end_time = time.time()
            print("\n++++ Scan finished in " + str(end_time - start_time) + " sec")
            if APs is None:
                print("---- Error in scanning Wi-Fi APs ----")
                exit(8)

            if len(APs) == 0:
                print("No APs found!")
                exit(9)

            print("==== Wi-Fi Scan results ====")
            print("{0: >4} {1: <33} {2: <12} {3: >4} {4: <4} {5: <16}".format(
                "S.N.", "SSID", "BSSID", "CHN", "RSSI", "AUTH"))
            for i in range(len(APs)):
                print("[{0: >2}] {1: <33} {2: <12} {3: >4} {4: <4} {5: <16}".format(
                    i + 1, APs[i]["ssid"], APs[i]["bssid"], APs[i]["channel"], APs[i]["rssi"], APs[i]["auth"]))

            while True:
                try:
                    select = int(input("Select AP by number (0 to rescan) : "))
                    if select < 0 or select > len(APs):
                        raise ValueError
                    break
                except ValueError:
                    print("Invalid input! Retry")

            if select != 0:
                break

        args.ssid = APs[select - 1]["ssid"]
        prompt_str = "Enter passphrase for {0} : ".format(args.ssid)
        args.passphrase = getpass(prompt_str)

    print("\n==== Sending Wi-Fi credential to esp32 ====")
    if not send_wifi_config(obj_transport, obj_security, args.ssid, args.passphrase):
        print("---- Error in send Wi-Fi config ----")
        exit(6)
    print("==== Wi-Fi Credentials sent successfully ====")

    print("\n==== Applying config to esp32 ====")
    if not apply_wifi_config(obj_transport, obj_security):
        print("---- Error in apply Wi-Fi config ----")
        exit(7)
    print("==== Apply config sent successfully ====")

    while True:
        time.sleep(5)
        print("\n==== Wi-Fi connection state  ====")
        ret = get_wifi_config(obj_transport, obj_security)
        if (ret == 1):
            continue
        elif (ret == 0):
            print("==== Provisioning was successful ====")
        else:
            print("---- Provisioning failed ----")
        break
