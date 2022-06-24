# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# APIs for interpreting and creating protobuf packets for Wi-Fi Scanning
import proto
from utils import str_to_bytes


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')


def scan_start_request(security_ctx, blocking=True, passive=False, group_channels=5, period_ms=120):
    # Form protobuf request packet for ScanStart command
    cmd = proto.wifi_scan_pb2.WiFiScanPayload()
    cmd.msg = proto.wifi_scan_pb2.TypeCmdScanStart
    cmd.cmd_scan_start.blocking = blocking
    cmd.cmd_scan_start.passive = passive
    cmd.cmd_scan_start.group_channels = group_channels
    cmd.cmd_scan_start.period_ms = period_ms
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (Encrypted CmdScanStart): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def scan_start_response(security_ctx, response_data):
    # Interpret protobuf response packet from ScanStart command
    dec_resp = security_ctx.decrypt_data(str_to_bytes(response_data))
    resp = proto.wifi_scan_pb2.WiFiScanPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, f'ScanStart status: 0x{str(resp.status)}')
    if resp.status != 0:
        raise RuntimeError


def scan_status_request(security_ctx):
    # Form protobuf request packet for ScanStatus command
    cmd = proto.wifi_scan_pb2.WiFiScanPayload()
    cmd.msg = proto.wifi_scan_pb2.TypeCmdScanStatus
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (Encrypted CmdScanStatus): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def scan_status_response(security_ctx, response_data):
    # Interpret protobuf response packet from ScanStatus command
    dec_resp = security_ctx.decrypt_data(str_to_bytes(response_data))
    resp = proto.wifi_scan_pb2.WiFiScanPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, f'ScanStatus status: 0x{str(resp.status)}')
    if resp.status != 0:
        raise RuntimeError
    return {'finished': resp.resp_scan_status.scan_finished, 'count': resp.resp_scan_status.result_count}


def scan_result_request(security_ctx, index, count):
    # Form protobuf request packet for ScanResult command
    cmd = proto.wifi_scan_pb2.WiFiScanPayload()
    cmd.msg = proto.wifi_scan_pb2.TypeCmdScanResult
    cmd.cmd_scan_result.start_index = index
    cmd.cmd_scan_result.count = count
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (Encrypted CmdScanResult): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def scan_result_response(security_ctx, response_data):
    # Interpret protobuf response packet from ScanResult command
    dec_resp = security_ctx.decrypt_data(str_to_bytes(response_data))
    resp = proto.wifi_scan_pb2.WiFiScanPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, f'ScanResult status: 0x{str(resp.status)}')
    if resp.status != 0:
        raise RuntimeError
    authmode_str = ['Open', 'WEP', 'WPA_PSK', 'WPA2_PSK', 'WPA_WPA2_PSK',
                    'WPA2_ENTERPRISE', 'WPA3_PSK', 'WPA2_WPA3_PSK']
    results = []
    for entry in resp.resp_scan_result.entries:
        results += [{'ssid': entry.ssid.decode('latin-1').rstrip('\x00'),
                     'bssid': entry.bssid.hex(),
                     'channel': entry.channel,
                     'rssi': entry.rssi,
                     'auth': authmode_str[entry.auth]}]
        print_verbose(security_ctx, f"ScanResult SSID    : {str(results[-1]['ssid'])}")
        print_verbose(security_ctx, f"ScanResult BSSID   : {str(results[-1]['bssid'])}")
        print_verbose(security_ctx, f"ScanResult Channel : {str(results[-1]['channel'])}")
        print_verbose(security_ctx, f"ScanResult RSSI    : {str(results[-1]['rssi'])}")
        print_verbose(security_ctx, f"ScanResult AUTH    : {str(results[-1]['auth'])}")
    return results
