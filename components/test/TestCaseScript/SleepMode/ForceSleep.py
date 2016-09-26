import random
import os
import time

from TCAction import TCActionBase, PerformanceTCBase
from Utility import MakeFolder
from Utility import MultimeterUtil
from NativeLog import NativeLog

LOG_PATH = os.path.join("AT_LOG", "SLEEP")

SLEEP_MODE_LIST = ["none_sleep", "light_sleep", "modem_sleep"]
SLEEP_MODE = dict(zip(SLEEP_MODE_LIST, range(len(SLEEP_MODE_LIST))))

SAMPLE_RATE = 0.002
SAMPLE_NUM = 512
MAX_VALUE = 1
Y_AXIS_LABEL = "Current (mA)"

MEASURE_FREQ_HOUR = 3600

GPIO_WAKE_UP = 15
GPIO_EDGE_DELAY = 120  # 20 ms
GPIO_CHIP_RESET = 14
GPIO_CHIP_RESET_DELAY = 100

NONE_SLEEP_MIN_CUR = 30
LIGHT_SLEEP_MIN_CUR = 1.5
MODEM_SLEEP_MIN_CUR = 20

LIGHT_SLEEP_WAKEUP_DELAY = 0.01


class ForceSleep(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.test_mode = "mode_change"
        self.test_count = 100
        self.sleep_mode = SLEEP_MODE_LIST
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.log_folder = MakeFolder.make_folder(os.path.join(LOG_PATH,
                                                              "FORCE_SLEEP_%s_%s" % (self.test_mode,
                                                                                     time.strftime("%d%H%M%S",
                                                                                                   time.localtime()))))
        self.multimeter = MultimeterUtil.MultimeterUtil(self.log_folder)

    @staticmethod
    def find_min_items(item_list, count):
        assert count < len(item_list)
        min_items = []
        for i in range(count):
            min_val = min(item_list)
            min_items.append(min_val)
            item_list.remove(min_val)
        return min_items   
    
    def sleep_time_boundary_test(self):
        result = True
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 0")
        self.check_response("SSC2", "+GPIO_SET:OK")
        if self.check_response("SSC1", "+MODE:OK") is False:
            result = False
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "fsleep -S -t 1")
        self.check_response("SSC2", "+GPIO_SET:OK")
        if self.check_response("SSC1", "+FSLEEP_MODE:OK") is False:
            result = False
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "fsleep -D -d 0")
        self.check_response("SSC2", "+GPIO_SET:OK")
        # if self.check_response("SSC1", "+FSLEEP_MODE:OK") is False:
        #     result = False
        time.sleep(1)
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val > LIGHT_SLEEP_MIN_CUR:
            NativeLog.add_trace_critical("[ForceSleep][Boundary] did not enter light sleep %d" % average_val)
            result = False 
            return result
        else:
            NativeLog.add_trace_critical("[ForceSleep][Boundary] enter light sleep")

        for i in range(3):
            time.sleep(MEASURE_FREQ_HOUR)
            for j in range(3):
                time.sleep(10)
                current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                               sample_num=SAMPLE_NUM,
                                                               max_value=MAX_VALUE)
                self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                           "light_sleep_boundary_%s_%s" % (i, j), Y_AXIS_LABEL)
                pass

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        self.check_response("SSC2", "+GPIO_SET:OK")
        time.sleep(1)
        self.serial_write_line("SSC1", "reboot")
        self.check_response("SSC1", "ready!!!")
        self.serial_write_line("SSC1", "fsleep -S -t 1")
        if self.check_response("SSC1", "+FSLEEP_MODE:OK") is False:
            result = False
        self.serial_write_line("SSC1", "")
        self.serial_write_line("SSC1", "fsleep -B -t 1")
        if self.check_response("SSC1", "+FSLEEP_MODE:OK") is False:
            result = False
        time.sleep(MEASURE_FREQ_HOUR)
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET,GPIO_CHIP_RESET_DELAY))
        return result

    def force_sleep_current_measure(self, sleep_mode):
        result = True
        # choose sleep mode
        sleep_mode_enum = SLEEP_MODE[sleep_mode]

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 0")
        if self.check_response("SSC1", "+MODE:OK") is False:
            result = False
        self.check_response("SSC2", "+GPIO_SET:OK")

        # set sleep mode
        self.serial_write_line("SSC1", "fsleep -S -t %s" % sleep_mode_enum)
        if self.check_response("SSC1", "+FSLEEP_MODE:OK") is False:
            result = False
        self.serial_write_line("SSC1", "fsleep -D -d 0")
        # if self.check_response("SSC1", "+FSLEEP_MODE:OK") is False:
        #     result = False

        time.sleep(3)

        for i in range(10):
            time.sleep(10)
            # measure current
            current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                           sample_num=SAMPLE_NUM,
                                                           max_value=MAX_VALUE)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "force_%s_sleep_current_%s" % (sleep_mode, i), Y_AXIS_LABEL)
            NativeLog.add_trace_critical("[ForceSleep][current_measure] force_%s_%d"% (sleep_mode,i))

        # self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP,GPIO_EDGE_DELAY))
        # self.check_response("SSC2", "+GPIO_SET:OK")
        # self.serial_write_line("SSC1", "reboot")
        # self.check_response("SSC1", "ready!!!")
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET, GPIO_CHIP_RESET_DELAY))
        self.check_response("SSC2", "+GPIO_SET:OK")
        if self.check_response("SSC1", "ready!!!") is False:
            result = False
        time.sleep(1)
        return result
    
    def force_sleep_illegal_enter(self):
        result = True
        # choose sleep mode

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 2")
        if self.check_response("SSC1", "+MODE:OK") is False:
            result = False
        self.check_response("SSC2", "+GPIO_SET:OK")

        # set sleep mode
        self.serial_write_line("SSC1", "fsleep -D -d 0")
        if self.check_response("SSC1", "ready!!!", timeout=10) is False:
            result = False
        time.sleep(5)
        return result 
    
    def force_sleep_stable_test(self):
        result = True
        # choose sleep mode

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "fsleep -L")
        if self.check_response("SSC1", "+MODE:OK") is False:
            result = False
        self.check_response("SSC2", "+GPIO_SET:OK")

        time.sleep(3600)
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET, GPIO_CHIP_RESET_DELAY))
        time.sleep(5)
        return result 

    def cleanup(self):
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(LIGHT_SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "reboot")
        self.check_response("SSC1", "ready!!!")
        self.check_response("SSC2", "+GPIO_SET:OK")

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.serial_write_line("SSC2", "sta -D")
        self.check_response("SSC2", "+QAP")
        self.serial_write_line("SSC1", "sta -D")
        self.check_response("SSC1", "+QAP")        
        try:
            test_mode = self.test_mode
            test_count = self.test_count
            sleep_mode = self.sleep_mode
        except StandardError, e:
            return

        # set gpio to input on sleep target
        self.serial_write_line("SSC1", "gpio -G -p %d" % GPIO_WAKE_UP)
        self.check_response("SSC1", "+GPIO_GET")
        self.serial_write_line("SSC1", "gpio -G -p %d" % GPIO_CHIP_RESET)
        self.check_response("SSC1", "+GPIO_GET")

        if test_mode == "boundary_test":
            for i in range(test_count):
                result = self.sleep_time_boundary_test()
                pass
        elif test_mode == "measure_current":
            for j in range(test_count):
                for mode in sleep_mode:
                    result = self.force_sleep_current_measure(mode)
            pass
        elif test_mode == "illegal_enter":
            for i in range(test_count):
                result = self.force_sleep_illegal_enter()
            pass   
        elif test_mode == "stable_test":
            for i in range(test_count):
                result = self.force_sleep_stable_test()
            pass         
        pass


def main():
    pass


if __name__ == '__main__':
    main()
