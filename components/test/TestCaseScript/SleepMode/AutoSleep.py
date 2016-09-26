import random
import os
import time
import string
import re
import threading

from TCAction import TCActionBase, PerformanceTCBase
from NativeLog import NativeLog
from Utility import MakeFolder
from Utility import MultimeterUtil
from Utility import ShellCmd

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
GPIO_CHIP_RESET = 14

SLEEP_WAKEUP_DELAY = 0.01


class AutoSleep(PerformanceTCBase.PerformanceTCBase):

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
                                                              "AUTO_SLEEP_%s_%s" % (self.test_mode,
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

    def sleep_mode_change(self, sleep_mode):
        result = True
        NativeLog.add_prompt_trace("[AutoSleep][ModeChange] %s start" % sleep_mode)
        # choose sleep mode
        sleep_mode_enum = SLEEP_MODE[sleep_mode]
        # change GPIO to make sure target exit sleep mode, so it can process SSC commands
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        # set sleep mode
        self.serial_write_line("SSC1", "sleep -S -t %d" % sleep_mode_enum)
        self.check_response("SSC1", "+SLEEP_MODE:OK")
        self.check_response("SSC2", "+GPIO_SET:OK")

        NativeLog.add_prompt_trace("[AutoSleep][ModeChange] mode set")
        time.sleep(6)
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
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "sleep -S -t %d" % sleep_mode_enum)
        self.check_response("SSC1", "+SLEEP_MODE:OK")
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

        time.sleep(1)
        self.serial_write_line("SSC1", "")
        time.sleep(1)
        # check if respond to uart
        self.flush_data("SSC1")
        for i in range(60):
            self.serial_write("SSC1", "a")
            time.sleep(0.043)
        time.sleep(0.1)
        respond_data = self.serial_read_data("SSC1")
        if len(respond_data) >= 60:
            NativeLog.add_trace_critical("[AutoSleep][light sleep wakeup] "
                                         "Failed when recving data during sleep, %d" % len(respond_data))
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][LightSleepWakeup] check on sleep mode done")
        
        self.serial_write_line("SSC1", "")
        time.sleep(1)           
        
        # change GPIO to make target wakeup
        self.serial_write_line("SSC2", "gpio -L -p %d -t 0" % GPIO_WAKE_UP)
        self.check_response("SSC2", "+GPIO_SET:OK")
     
        self.serial_write_line("SSC1", "")
        time.sleep(1)
        self.flush_data("SSC1")
        for i in range(60):
            self.serial_write("SSC1", "a")
            time.sleep(0.043)
        time.sleep(0.1)
        respond_data = self.serial_read_data("SSC1")
        if len(respond_data) < 60:
            NativeLog.add_trace_critical("[AutoSleep][light sleep wakeup] "
                                         "Failed when recving data during wakeup, %d" % len(respond_data))
            result = False
     
        NativeLog.add_prompt_trace("[AutoSleep][LightSleepWakeup] check on wakeup mode done")
        self.serial_write_line("SSC1", "")
        # restore GPIO level
        self.serial_write_line("SSC2", "gpio -L -p %d -t 1" % GPIO_WAKE_UP)
        self.check_response("SSC2", "+GPIO_SET:OK")

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET, GPIO_EDGE_DELAY))
        self.check_response("SSC2", "+GPIO_SET:OK")        
        time.sleep(2)
        return result

    def sleep_exit_enter(self, sleep_mode, ssid, password):
        result = True
        if sleep_mode == "modem_sleep":
            max_current_for_sleep = 20
        elif sleep_mode == "light_sleep":
            max_current_for_sleep = 5
        else:
            raise StandardError("Not supported mode %s" % sleep_mode)

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] %s start" % sleep_mode)

        ap_ssid = self.get_parameter("ap_ssid")
        ap_password = self.get_parameter("ap_password")
        
        # step A: no STA connect to SoftAP, enter modem sleep mode
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 1")
        self.check_response("SSC1", "+MODE:OK")
        self.check_response("SSC2", "+GPIO_SET:OK")

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "sta -C -s %s -p %s" % (ap_ssid, ap_password))
        self.check_response("SSC2", "+GPIO_SET:OK")
        self.check_response("SSC1", "+JAP:CONNECTED")
        self.check_response("SSC1", "pm open")
        
        self.serial_write_line("SSC2", "sta -D")
        self.check_response("SSC2", "+QAP")

        time.sleep(5)

        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val > max_current_for_sleep:
            NativeLog.add_trace_critical("[AutoSleep][SleepExitEnter] "
                                         "did not enter %s sleep, %d" % (sleep_mode, average_val))
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_sleep_exit_enter_fail_A" % sleep_mode, Y_AXIS_LABEL)
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] step A done")
        # step B: STA connect to SoftAP, exit modem sleep mode
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 3")
        self.check_response("SSC1", "+MODE:OK")
        self.check_response("SSC2", "+GPIO_SET:OK")
        time.sleep(1)
        self.serial_write_line("SSC2", "sta -C -s %s -p %s" % (ssid, password))
        self.check_response("SSC2", "+JAP:CONNECTED")
        # self.check_response("SSC1", "pm close")
        time.sleep(10)
        
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val < 30:
            NativeLog.add_trace_critical("[AutoSleep][SleepExitEnter] did not exit %s sleep" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_sleep_exit_enter_fail_B" % sleep_mode, Y_AXIS_LABEL)
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] step B done")
        # step C: target set to STA mode, enter modem sleep
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 1")
        self.check_response("SSC1", "+MODE:OK")

        self.check_response("SSC2", "+GPIO_SET:OK")
        # self.check_response("SSC1", "pm open")
        time.sleep(15)

        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val > max_current_for_sleep:
            NativeLog.add_trace_critical("[AutoSleep][SleepExitEnter] did not enter %s sleep" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_sleep_exit_enter_fail_C" % sleep_mode, Y_AXIS_LABEL)
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] step C done")
        # step D: target disconnect, exit modem sleep
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "sta -D")
        self.check_response("SSC1", "+QAP")
        self.check_response("SSC2", "+GPIO_SET:OK")
        # self.check_response("SSC1", "pm close")
        time.sleep(5)
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val < 30:
            NativeLog.add_trace_critical("[AutoSleep][SleepExitEnter] did not exit %s sleep" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_sleep_exit_enter_fail_D" % sleep_mode, Y_AXIS_LABEL)
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] step D done")
        # step E: target connect to AP, enter modem sleep
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "sta -C -s %s -p %s" % (ap_ssid, ap_password))
        self.check_response("SSC2", "+GPIO_SET:OK")
        self.check_response("SSC1", "+JAP:CONNECTED")
        self.check_response("SSC1", "pm open")
        time.sleep(3)
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val > max_current_for_sleep:
            NativeLog.add_trace_critical("[AutoSleep][SleepExitEnter] did not enter %s sleep" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_sleep_exit_enter_fail_E" % sleep_mode, Y_AXIS_LABEL)
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] step E done")
        # step F: target set to AP mode, exit modem sleep
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "op -S -o 2")
        self.check_response("SSC1", "+MODE:OK")
        self.check_response("SSC2", "+GPIO_SET:OK")
        # self.check_response("SSC1", "pm close")
        time.sleep(5)

        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val < 30:
            NativeLog.add_trace_critical("[AutoSleep][SleepExitEnter] did not exit %s sleep" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_sleep_exit_enter_fail_F" % sleep_mode, Y_AXIS_LABEL)
            result = False

        NativeLog.add_prompt_trace("[AutoSleep][EnterExitSleep] step F done")
        return result

    def ping_test(self, sleep_mode):
        result = True
        NativeLog.add_prompt_trace("[AutoSleep][PingTest] %s start" % sleep_mode)
        # choose sleep mode
        sleep_mode_enum = SLEEP_MODE[sleep_mode]
        if sleep_mode == "modem_sleep":
            max_current_for_sleep = MODEM_SLEEP_MIN_CUR
        elif sleep_mode == "light_sleep":
            max_current_for_sleep = LIGHT_SLEEP_MIN_CUR
        else:
            raise StandardError("Not supported mode %s" % sleep_mode)
        
        self.serial_write_line("SSC1", "op -S -o 1")
        self.check_response("SSC1", "+MODE:OK")        
        
        # set sleep mode
        self.serial_write_line("SSC1", "sleep -S -t %d" % sleep_mode_enum)
        self.check_response("SSC1", "+SLEEP_MODE:OK")
        NativeLog.add_prompt_trace("[AutoSleep][PingTest] set mode done")

        # connect to AP
        ap_ssid = self.get_parameter("ap_ssid")
        ap_password = self.get_parameter("ap_password")
        target_ip = self.get_parameter("target_ip")
        
        self.serial_write_line("SSC1", "sta -C -s %s -p %s" % (ap_ssid, ap_password))
        self.check_response("SSC1", "+JAP:CONNECTED")

        time.sleep(10)
        # measure current, should be in sleep mode
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        
        if average_val > max_current_for_sleep:
            NativeLog.add_trace_critical("[AutoSleep][PingTest] step A did not enter %s sleep, %f"
                                         % (sleep_mode, average_val))
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_ping_test_fail_not_enter_sleep" % sleep_mode, Y_AXIS_LABEL)
            result = False
        else:
            NativeLog.add_prompt_trace("[AutoSleep][PingTest] step A enter %s sleep, %f"
                                       % (sleep_mode, average_val))

        class PingThread(threading.Thread):
            def __init__(self, ping_ip):
                threading.Thread.__init__(self)
                self.setDaemon(True)
                self.target_ip = ping_ip
                self.exit_event = threading.Event()

            def run(self):
                while self.exit_event.isSet() is False:
                    ShellCmd.shell_check_output("ping %s -w 500" % self.target_ip)
                    time.sleep(0.1)
                pass

            def exit(self):
                self.exit_event.set()

        NativeLog.add_prompt_trace("[AutoSleep][PingTest] ping start")
        ping_thread = PingThread(target_ip)
        ping_thread.start()
        time.sleep(5)

        # measure current, should not be in sleep mode
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val < 30:
            NativeLog.add_trace_critical("[AutoSleep][PingTest] step B did not exit %s sleep, %f"
                                         % (sleep_mode, average_val))
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_ping_test_fail_not_exit_sleep" % sleep_mode, Y_AXIS_LABEL)
            result = False
        else:
            NativeLog.add_prompt_trace("[AutoSleep][PingTest] step B exit %s sleep, %f"
                                       % (sleep_mode, average_val))

        ping_thread.exit()
        ping_thread.join(20)
        NativeLog.add_prompt_trace("[AutoSleep][PingTest] ping stop")
        time.sleep(10)

        # measure current, should not be in sleep mode
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        min_items = self.find_min_items(current_line, 10)
        average_val = float(0)
        for val in min_items:
            average_val += val
        average_val /= 10
        if average_val > max_current_for_sleep:
            NativeLog.add_trace_critical("[AutoSleep][PingTest] step C did not enter %s" % sleep_mode)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "%s_ping_test_fail_not_enter_sleep" % sleep_mode, Y_AXIS_LABEL)
            result = False
        else:
            NativeLog.add_prompt_trace("[AutoSleep][PingTest] step C enter %s sleep" % sleep_mode)

        return result

    def cleanup(self):
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "sleep -S -t %d" % SLEEP_MODE["modem_sleep"])
        self.check_response("SSC1", "OK")
        self.check_response("SSC2", "+GPIO_SET:OK")

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
        time.sleep(SLEEP_WAKEUP_DELAY)
        self.serial_write_line("SSC1", "sleep -S -t %d" % SLEEP_MODE["modem_sleep"])
        self.check_response("SSC1", "+SLEEP_MODE:OK")
        self.check_response("SSC2", "+GPIO_SET:OK")
        self.check_response("SSC1", "pm open", timeout=10)

        self.serial_write_line("SSC1", "gpio -G -p %d" % GPIO_WAKE_UP)
        self.check_response("SSC1", "+GPIO_GET")
        self.serial_write_line("SSC1", "gpio -G -p %d" % GPIO_CHIP_RESET)
        self.check_response("SSC1", "+GPIO_GET")

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
            time.sleep(SLEEP_WAKEUP_DELAY)
            # set sleep mode
            self.serial_write_line("SSC1", "sleep -S -t %d" % SLEEP_MODE["light_sleep"])
            self.check_response("SSC1", "+SLEEP_MODE:OK")

            self.check_response("SSC2", "+GPIO_SET:OK")
            for i in range(test_count):
                result = self.light_sleep_wakeup()
            pass
        elif "sleep_exit_enter" in test_mode:
            ssid = "".join([random.choice(string.lowercase) for i in range(10)])
            password = "".join([random.choice(string.lowercase) for i in range(10)])
            self.serial_write_line("SSC2", "sta -D")
            self.check_response("SSC2", "+QAP")
            self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
            time.sleep(SLEEP_WAKEUP_DELAY)
            self.serial_write_line("SSC1", "op -S -o 3")
            self.check_response("SSC1", "+MODE:OK")
            self.check_response("SSC2", "+GPIO_SET:OK")
            self.serial_write_line("SSC1", "ap -S -s %s -p %s -t 3" % (ssid, password))
            self.check_response("SSC1", "+SAP:OK")
            self.serial_write_line("SSC2", "op -S -o 1")
            self.check_response("SSC2", "+MODE:OK")

            for mode in sleep_mode:
                self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
                time.sleep(SLEEP_WAKEUP_DELAY)
                self.serial_write_line("SSC1", "sleep -S -t %d" % SLEEP_MODE[mode])
                self.check_response("SSC1", "+SLEEP_MODE:OK")
                self.check_response("SSC2", "+GPIO_SET:OK")

                for i in range(test_count):
                    result = self.sleep_exit_enter(mode, ssid, password)
        elif "ping" in test_mode:
            for mode in sleep_mode:
                for i in range(test_count):
                    result = self.ping_test(mode)
            pass


def main():
    pass


if __name__ == '__main__':
    main()
