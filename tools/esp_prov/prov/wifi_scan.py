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

# APIs for interpreting and creating protobuf packets for Wi-Fi Scanning

from __future__ import print_function
from future.utils import tobytes

import utils
import proto


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print("++++ " + data + " ++++")


def scan_start_request(security_ctx, blocking=True, passive=False, group_channels=5, period_ms=120):
    # Form protobuf request packet for ScanStart command
    cmd = proto.wifi_scan_pb2.WiFiScanPayload()
    cmd.msg = proto.wifi_scan_pb2.TypeCmdScanStart
    cmd.cmd_scan_start.blocking = blocking
    cmd.cmd_scan_start.passive = passive
    cmd.cmd_scan_start.group_channels = group_channels
    cmd.cmd_scan_start.period_ms = period_ms
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString()).decode('latin-1')
    print_verbose(security_ctx, "Client -> Device (Encrypted CmdScanStart) " + utils.str_to_hexstr(enc_cmd))
    return enc_cmd


def scan_start_response(security_ctx, response_data):
    # Interpret protobuf response packet from ScanStart command
    dec_resp = security_ctx.decrypt_data(tobytes(response_data))
    resp = proto.wifi_scan_pb2.WiFiScanPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, "ScanStart status " + str(resp.status))
    if resp.status != 0:
        raise RuntimeError


def scan_status_request(security_ctx):
    # Form protobuf request packet for ScanStatus command
    cmd = proto.wifi_scan_pb2.WiFiScanPayload()
    cmd.msg = proto.wifi_scan_pb2.TypeCmdScanStatus
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString()).decode('latin-1')
    print_verbose(security_ctx, "Client -> Device (Encrypted CmdScanStatus) " + utils.str_to_hexstr(enc_cmd))
    return enc_cmd


def scan_status_response(security_ctx, response_data):
    # Interpret protobuf response packet from ScanStatus command
    dec_resp = security_ctx.decrypt_data(tobytes(response_data))
    resp = proto.wifi_scan_pb2.WiFiScanPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, "ScanStatus status " + str(resp.status))
    if resp.status != 0:
        raise RuntimeError
    return {"finished": resp.resp_scan_status.scan_finished, "count": resp.resp_scan_status.result_count}


def scan_result_request(security_ctx, index, count):
    # Form protobuf request packet for ScanResult command
    cmd = proto.wifi_scan_pb2.WiFiScanPayload()
    cmd.msg = proto.wifi_scan_pb2.TypeCmdScanResult
    cmd.cmd_scan_result.start_index = index
    cmd.cmd_scan_result.count = count
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString()).decode('latin-1')
    print_verbose(security_ctx, "Client -> Device (Encrypted CmdScanResult) " + utils.str_to_hexstr(enc_cmd))
    return enc_cmd


def scan_result_response(security_ctx, response_data):
    # Interpret protobuf response packet from ScanResult command
    dec_resp = security_ctx.decrypt_data(tobytes(response_data))
    resp = proto.wifi_scan_pb2.WiFiScanPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, "ScanResult status " + str(resp.status))
    if resp.status != 0:
        raise RuntimeError
    authmode_str = ["Open", "WEP", "WPA_PSK", "WPA2_PSK", "WPA_WPA2_PSK", "WPA2_ENTERPRISE"]
    results = []
    for entry in resp.resp_scan_result.entries:
        results += [{"ssid": entry.ssid.decode('latin-1').rstrip('\x00'),
                     "bssid": utils.str_to_hexstr(entry.bssid.decode('latin-1')),
                     "channel": entry.channel,
                     "rssi": entry.rssi,
                     "auth": authmode_str[entry.auth]}]
        print_verbose(security_ctx, "ScanResult SSID    : " + str(results[-1]["ssid"]))
        print_verbose(security_ctx, "ScanResult BSSID   : " + str(results[-1]["bssid"]))
        print_verbose(security_ctx, "ScanResult Channel : " + str(results[-1]["channel"]))
        print_verbose(security_ctx, "ScanResult RSSI    : " + str(results[-1]["rssi"]))
        print_verbose(security_ctx, "ScanResult AUTH    : " + str(results[-1]["auth"]))
    return results
