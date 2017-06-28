import re
import subprocess
import time

from TCAction import PerformanceTCBase
from TCAction import TCActionBase
from NativeLog import NativeLog
from comm.NIC import Adapter


MIN_PACKETS_TO_CHECK = 10


class TestCase(PerformanceTCBase.PerformanceTCBase):
    INTERVAL_PATTERN = re.compile("(0x[0-9A-Fa-f]+)-(0x[0-9A-Fa-f]+)")

    def __init__(self, test_case, test_env, timeout, log_path):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env, timeout, log_path)
        self.interval_list = []
        self.deviation = 0
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def process_packets(self, packets, interval):

        def convert_ts(ts):
            return float("%s.%s" % (ts["second"], ts["microsecond"]))

        def filter_packets():
            _filtered_packets = []
            mac_address = self.get_parameter("dut1_bt_mac")
            for _packet in packets:
                packet_str = str(_packet)
                if "[le_sub_event_code]: LEAdvReport" in packet_str \
                        and "[address_0]: %s" % mac_address in packet_str:
                    _filtered_packets.append(_packet)
            # sort by time
            _filtered_packets.sort(key=lambda x: convert_ts(x.ts))
            return _filtered_packets

        filtered_packets = filter_packets()

        # add captured packets to log
        for packet in filtered_packets:
            self.result_check("BLENIC", str(packet))
            # flush data cache to free memory
            self.flush_data("BLENIC")

        # scan will switch channel, therefore need to check if there're successive fails
        succeed_packets = 0

        # process packets
        # unit: ms; allow deviation for interval
        allowed_range = [float(interval[0] * (1 - self.deviation) * 0.625) / 1000,
                         float(interval[1] * (1 + self.deviation) * 0.625) / 1000]
        NativeLog.add_trace_info("[BLE][AdvInterval] allowed_interval_range is %s" % allowed_range)
        for i in range(len(filtered_packets) - 1):
            _p1 = filtered_packets[i]
            _p2 = filtered_packets[i+1]
            interval = convert_ts(_p2.ts) - convert_ts(_p1.ts)
            if allowed_range[0] < interval < allowed_range[1]:
                succeed_packets += 1
            else:
                pass
        result = True if succeed_packets >= MIN_PACKETS_TO_CHECK else False
        return result

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        test_result = "Succeed"

        # open capture device
        adapter = Adapter.Adapter(self.get_parameter("bt_capture_nic"), "capture", capture_type="bluetooth")

        ret = adapter.start_capture()
        if ret != "LIBPCAP_SUCCEED":
            NativeLog.add_trace_critical("Can't start capture packets: %s" % ret)
            return

        def run_test_once(interval):
            # flush all packets
            adapter.get_packets()
            # config ble adv data
            self.serial_write_line("SSC1", "bleadv -D -z stop")
            self.check_response("SSC1", "+BLEADV")
            self.serial_write_line("SSC1", "bleadv -L -c 0 -t 3")
            self.check_response("SSC1", "+BLEADV")
            # set adv param and start adv
            self.serial_write_line("SSC1", "bleadv -D -z start -i 0x%04X-0x%04X -h 1" % (interval[0], interval[1]))
            self.check_response("SSC1", "+BLEADV:OK")
            # set scan window = scan interval = 2s, scan for 6s, each channel scan 2 second.
            subprocess.check_output("hcitool cmd 0x08 0x000b 0x00 0x80 0x0c 0x80 0x0c 0x00 0x00\n", shell=True)
            subprocess.check_output("hcitool cmd 0x08 0x000c 0x01 0x00\n", shell=True)
            time.sleep(6)
            subprocess.check_output("hcitool cmd 0x08 0x000c 0x00 0x00\n", shell=True)
            packets = adapter.get_packets()
            return self.process_packets(packets, interval)

        for _interval in self.interval_list:
            match = self.INTERVAL_PATTERN.search(_interval)
            if match is not None:
                if run_test_once([int(match.group(1), base=16), int(match.group(2), base=16)]) is False:
                    NativeLog.add_trace_critical("Test fail for interval: %s." % _interval)
                    test_result = "Fail"
            else:
                NativeLog.add_trace_critical("interval string format not correct: %s." % _interval)
                test_result = "Fail"
            pass

        self.set_result(test_result)


def main():
    pass


if __name__ == '__main__':
    pass
