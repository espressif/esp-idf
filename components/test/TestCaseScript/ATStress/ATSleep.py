import random
import os
import time

from TCAction import TCActionBase, PerformanceTCBase
from NativeLog import NativeLog
from Utility import MakeFolder
from Utility import MultimeterUtil

LOG_PATH = os.path.join("AT_LOG", "SLEEP")

SLEEP_MODE_LIST = ["none_sleep", "light_sleep", "modem_sleep"]
SLEEP_MODE = dict(zip(SLEEP_MODE_LIST, range(len(SLEEP_MODE_LIST))))

SAMPLE_RATE_SLEEP_MODE_CHANGE = 0.002
SAMPLE_NUM_SLEEP_MODE_CHANGE = 256

SAMPLE_RATE = 0.002
SAMPLE_NUM = 512
MAX_VALUE = 1
Y_AXIS_LABEL = "Current (mA)"
GPIO_EDGE_DELAY = 120  # 20 ms

NONE_SLEEP_MIN_CUR = 30
LIGHT_SLEEP_MIN_CUR = 1.5
MODEM_SLEEP_MIN_CUR = 20

GPIO_WAKE_UP = 15

AT_WAKE_UP_IND_PIN = 14
AT_WAKE_UP_PIN = 12


class ATSleep(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.test_mode = "mode_change"
        self.test_count = 100
        self.sleep_mode = SLEEP_MODE_LIST
        self.sleep_wake_pin = AT_WAKE_UP_PIN
        self.sleep_wakeup_ind_pin = AT_WAKE_UP_IND_PIN
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.log_folder = MakeFolder.make_folder(os.path.join(LOG_PATH,
                                                              "AT_AUTO_SLEEP_%s_%s" %
                                                              (self.test_mode,
                                                               time.strftime("%d%H%M%S", time.localtime()))))
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

    def sleep_mode_change(self, sleep_mode):
        result = True
        NativeLog.add_prompt_trace("[AutoSleep][ModeChange] %s start" % sleep_mode)
        # choose sleep mode
        sleep_mode_enum = SLEEP_MODE[sleep_mode]
        # change GPIO to make sure target exit sleep mode, so it can process SSC commands
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        # set sleep mode
        self.serial_write_line("AT1", "AT+SLEEP=%d" % sleep_mode_enum)
        self.check_response("AT1", "OK")
        self.check_response("SSC2", "+GPIO_SET:OK")

        NativeLog.add_prompt_trace("[AutoSleep][ModeChange] mode set")
        time.sleep(10)
        # measure current
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE_SLEEP_MODE_CHANGE,
                                                       sample_num=SAMPLE_NUM_SLEEP_MODE_CHANGE,
                                                       max_value=MAX_VALUE)
        # do check measure
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10

        NativeLog.add_prompt_trace("[AutoSleep][ModeChange] measure done, average min current %f" % average_val)

        if sleep_mode == "none_sleep":
            if average_val < NONE_SLEEP_MIN_CUR:
                result = False
        elif sleep_mode == "light_sleep":
            if average_val > LIGHT_SLEEP_MIN_CUR:
                result = False
        elif sleep_mode == "modem_sleep":
            if average_val > MODEM_SLEEP_MIN_CUR or average_val < LIGHT_SLEEP_MIN_CUR:
                result = False
        if result is False:
            NativeLog.add_trace_critical("[AutoSleep][ModeChange] %s failed" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE, "%s_fail" % sleep_mode, Y_AXIS_LABEL)
        
        time.sleep(5)
        return result

    def sleep_current_measure(self, sleep_mode):
        result = True

        NativeLog.add_prompt_trace("[AutoSleep][CurrentMeasure] %s start" % sleep_mode)
        # choose sleep mode
        sleep_mode_enum = SLEEP_MODE[sleep_mode]
        # change GPIO to make sure target exit sleep mode, so it can process SSC commands
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        # set sleep mode
        self.serial_write_line("AT1", "AT+SLEEP=%d" % sleep_mode_enum)
        self.check_response("AT1", "OK")
        self.check_response("SSC2", "+GPIO_SET:OK")

        NativeLog.add_prompt_trace("[AutoSleep][CurrentMeasure] set mode done")
        time.sleep(10)
        # measure current
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        self.multimeter.draw_graph(current_line, SAMPLE_RATE, sleep_mode, Y_AXIS_LABEL)
        NativeLog.add_prompt_trace("[AutoSleep][CurrentMeasure] measure done")
        return result

    def light_sleep_wakeup(self):
        result = True
        NativeLog.add_prompt_trace("[AutoSleep][LightSleepWakeup] start")

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(1)
        self.serial_write_line("AT1", "")
        time.sleep(1)
        self.check_response("SSC2", "+GPIO_SET:OK", timeout=1)

        for i in range(10):
            self.serial_write_line("SSC2", "gpio -G -p %d" % self.sleep_wakeup_ind_pin)
            if self.check_response("SSC2", "+GPIO_GET:0", timeout=0.73) is True:
                break
        else:
            NativeLog.add_prompt_trace("AT Sleep wakeup pin is not correct when in sleep")

        # check if respond to uart
        self.flush_data("AT1")
        for i in range(60):
            self.serial_write("AT1", "a")
            time.sleep(0.43)
        time.sleep(0.1)
        respond_data = self.serial_read_data("AT1")
        if len(respond_data) >= 60:
            NativeLog.add_trace_critical("[AutoSleep][light sleep wakeup] "
                                         "Failed when recving data during sleep, %d" % len(respond_data))
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][LightSleepWakeup] check on sleep mode done")

        # change GPIO to make target wakeup
        self.serial_write_line("SSC2", "gpio -L -p %d -t 0" % GPIO_WAKE_UP)
        self.check_response("SSC2", "+GPIO_SET:OK")
        time.sleep(0.01)

        for i in range(3):
            self.serial_write_line("SSC2", "gpio -G -p %d" % self.sleep_wakeup_ind_pin)
            if self.check_response("SSC2", "+GPIO_GET:1") is False:
                NativeLog.add_prompt_trace("AT Sleep wakeup pin is not correct when wakeup")

        self.serial_write_line("AT1", "")
        time.sleep(1)
        self.flush_data("AT1")
        for i in range(60):
            self.serial_write("AT1", "a")
            time.sleep(0.043)
        time.sleep(0.1)
        respond_data = self.serial_read_data("AT1")
        if len(respond_data) < 60:
            NativeLog.add_trace_critical("[AutoSleep][light sleep wakeup] "
                                         "Failed when recving data during wakeup, %d" % len(respond_data))
            result = False
     
        NativeLog.add_prompt_trace("[AutoSleep][LightSleepWakeup] check on wakeup mode done")
        self.serial_write_line("AT1", "")
        # restore GPIO level
        self.serial_write_line("SSC2", "gpio -L -p %d -t 1" % GPIO_WAKE_UP)
        time.sleep(2)
        return result

    def cleanup(self):
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        self.serial_write_line("AT1", "")
        self.serial_write_line("AT1", "AT+RST")
        self.check_response("SSC2", "+GPIO_SET:OK")
        self.check_response("AT1", "ready")

    def execute(self):
        TCActionBase.TCActionBase.execute(self)

        try:
            test_mode = self.test_mode
            test_count = self.test_count
            sleep_mode = self.sleep_mode
        except StandardError, e:
            return

        # make sure enter modem sleep mode before start test
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        self.serial_write_line("AT1", "AT+RST")
        self.check_response("SSC2", "+GPIO_SET:OK")
        self.check_response("AT1", "ready")
        self.check_response("AT1", "WIFI GOT IP")
        # set AT light sleep wakeup pin
        self.serial_write_line("AT1", "AT+WAKEUPGPIO=1,%d,0" % self.sleep_wake_pin)
        self.check_response("AT1", "OK")

        # start test
        if "mode_change" in test_mode:
            for i in range(test_count):
                result = self.sleep_mode_change(random.choice(SLEEP_MODE_LIST))
                
        elif "measure_current" in test_mode:
            for i in range(test_count):
                for mode in sleep_mode:
                    result = self.sleep_current_measure(mode)
            pass
        elif "gpio_wakeup" in test_mode:
            # change GPIO to make sure target exit sleep mode, so it can process SSC commands
            self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
            # config wakeup gpio
            self.serial_write_line("AT1", "AT+WAKEUPGPIO=1,%d,0,%d,1" % (self.sleep_wake_pin, self.sleep_wakeup_ind_pin))
            self.check_response("AT1", "OK")
            # set sleep mode
            self.serial_write_line("AT1", "AT+SLEEP=%d" % SLEEP_MODE["light_sleep"])
            self.check_response("AT1", "OK")
            self.check_response("SSC2", "+GPIO_SET:OK")

            for i in range(test_count):
                result = self.light_sleep_wakeup()
            pass
        pass


def main():
    pass


if __name__ == '__main__':
    main()
