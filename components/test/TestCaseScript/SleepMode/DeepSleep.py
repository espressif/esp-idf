import random
import os
import time

from TCAction import TCActionBase, PerformanceTCBase
from Utility import MakeFolder
from Utility import MultimeterUtil
from NativeLog import NativeLog

LOG_PATH = os.path.join("AT_LOG", "SLEEP")

DEEP_SLEEP_OPTION_LIST = ["up_to_bin", "normal", "no_rf_calibrate", "rf_off"]
DEEP_SLEEP_OPTION = {
    "up_to_bin": 0,
    "normal": 1,
    "no_rf_calibrate": 2,
    "rf_off": 4,
}

SAMPLE_RATE = 0.001
SAMPLE_NUM = 512
MAX_VALUE = 0.1
Y_AXIS_LABEL = "Current (mA)"


MEASURE_FREQ = 3600

GPIO_WAKE_UP = 15
GPIO_CHIP_RESET = 14
GPIO_EDGE_DELAY = 100  # 20 ms


class DeepSleep(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.test_mode = "mode_change"
        self.test_count = 100
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.log_folder = MakeFolder.make_folder(os.path.join(LOG_PATH,
                                                              "DEEP_SLEEP_%s_%s" % (self.test_mode,
                                                                                    time.strftime("%d%H%M%S",
                                                                                                  time.localtime()))))
        self.sleep_time_log = os.path.join(self.log_folder, "deep_sleep_wakeup_time.log")
        self.multimeter = MultimeterUtil.MultimeterUtil(self.log_folder)

    def deep_sleep_stable(self):
        result = True
        RandomTime = random.randint(1, 100)
        self.serial_write_line("SSC1", "dsleep -S -t %s" % RandomTime)
        if self.check_response("SSC1", "+DSLEEP:OK") is False:
            result = False
        if self.check_response("SSC1", "ready!!!") is False:
            result = False
            NativeLog.add_trace_critical("[DeepSleep][Stable] wait ready err")
        else:
            NativeLog.add_trace_critical("[DeepSleep][Stable] SleepTime:%d" % RandomTime) 
        time.sleep(1)
        
        RandomTime = random.randint(100000, 1000000)
        self.serial_write_line("SSC1", "dsleep -S -t %s" % RandomTime)
        if self.check_response("SSC1", "+DSLEEP:OK") is False:
            result = False
        if self.check_response("SSC1", "ready!!!") is False:
            result = False
            NativeLog.add_trace_critical("[DeepSleep][Stable] wait ready err")
        else:
            NativeLog.add_trace_critical("[DeepSleep][Stable] SleepTime:%d" % RandomTime) 
        time.sleep(1)        
        return result

    def deep_sleep_current_measure(self):
        result = True
        self.serial_write_line("SSC1", "")
        self.serial_write_line("SSC1", "dsleep -S -t 10000000")
        if self.check_response("SSC1", "+DSLEEP:OK") is False:
            result = False
        time.sleep(3)
        # measure current
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        average_current = float(0)
        for current in current_line:
            average_current += current
        average_current /= SAMPLE_NUM

        self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                   "deep_sleep_current", Y_AXIS_LABEL)

        if average_current > 1:
            NativeLog.add_trace_critical("[DeepSleep][CurrentMeasure] average current %f > 1mA" % average_current)
        else:
            NativeLog.add_trace_critical("[DeepSleep][CurrentMeasure] dsleep current ok, %f" % average_current)

        if self.check_response("SSC1", "ready!!!") is False:
            NativeLog.add_trace_critical("[DeepSleep][CurrentMeasure] CurrentMeasure wait ready err %f"
                                         % average_current)
            result = False
            
        NativeLog.add_trace_critical("[DeepSleep][CurrentMeasure] wait ready ok")
        
        return result

    ##########################################
    # gpio wake up
    ##########################################
    def deep_sleep_wakeup(self):
        result = True
        
        self.serial_write_line("SSC1", "dsleep -S -t 0")
        if self.check_response("SSC1", "+DSLEEP:OK") is False:
            result = False

        time.sleep(2)

        # measure current
        current_line = self.multimeter.measure_current(sample_rate=SAMPLE_RATE,
                                                       sample_num=SAMPLE_NUM,
                                                       max_value=MAX_VALUE)
        average_current = float(0)
        for current in current_line:
            average_current += current
        average_current /= SAMPLE_NUM

        if average_current > 1:
            NativeLog.add_trace_critical("[DeepSleep][Wakeup] average current %f > 1mA" % average_current)
            self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                       "deep_sleep_current", Y_AXIS_LABEL)

        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET, GPIO_EDGE_DELAY))
        self.check_response("SSC2", "+GPIO_SET:OK")
        if self.check_response("SSC1", "ready!!!") is False:
            NativeLog.add_trace_critical("[DeepSleep][Wakeup] target did not wakeup")
            result = False
        else:
            NativeLog.add_trace_critical("[DeepSleep][Wakeup] target wakeup")
        
        time.sleep(1)            
        return result

    #########################################
    #test one hour, Verify RTC Clock timer
    #########################################
    def deep_sleep_timeout(self):
        result = True
        Timeout = 3600 

        start_sleep_time = time.time()
        self.serial_write_line("SSC1", "")
        self.serial_write_line("SSC1", "dsleep -S -t %d" % (Timeout*1000000))
        if self.check_response("SSC1", "+DSLEEP:OK") is False:
            result = False
        self.check_response("SSC1", "ready!!!", timeout = Timeout*2)
        time_escaped = time.time() - start_sleep_time
        NativeLog.add_trace_critical("[DeepSleep][timeout] desired sleep timeout is %s, actual sleep timeout is %s" % (Timeout, time_escaped))
        with open(self.sleep_time_log, "ab+") as f:
            f.write("[DeepSleep] desired sleep timeout is %s, actual sleep timeout is %s" % (Timeout, time_escaped))
        return result

    ############################################
    # Capture current map, verify the process of power on
    # notice: option = "up_to_bin" up to byte108 in init.bin,
    ############################################
    def wake_option(self):
        result = True
        for option in DEEP_SLEEP_OPTION_LIST:
            for i in range(8):
                self.serial_write_line("SSC1", "dsleep -O -m %s" % DEEP_SLEEP_OPTION[option])
                if self.check_response("SSC1", "+DSLEEP:OK") is False:
                    result = False
                self.serial_write_line("SSC1", "dsleep -S -t 1200000")
                if self.check_response("SSC1", "+DSLEEP:OK") is False:
                    result = False

                # measure current
                current_line = self.multimeter.measure_current(sample_rate=0.002,
                                                               sample_num=SAMPLE_NUM,
                                                               max_value=1)
                self.multimeter.draw_graph(current_line, SAMPLE_RATE,
                                           "deep_sleep_wakeup_option_%s_%d"
                                           % (option, DEEP_SLEEP_OPTION[option]), Y_AXIS_LABEL)

                NativeLog.add_trace_critical("[DeepSleep][wake_option] target wakeup option:%d"
                                             % DEEP_SLEEP_OPTION[option])
                time.sleep(3)
                
        return result
    
    def deep_sleep_wakeup_flash_gpio_status(self):
        result = True
        RandomTime = random.randint(2000000, 2000000)
        self.serial_write_line("SSC1", "dsleep -S -t %s" % RandomTime)
        if self.check_response("SSC1", "+DSLEEP:OK") is False:
            result = False
        if self.check_response("SSC1", "ready!!!") is False:
            result = False
            NativeLog.add_trace_critical("[DeepSleep][Stable] wait ready err")
        else:
            NativeLog.add_trace_critical("[DeepSleep][Stable] SleepTime:%d" % RandomTime) 
        
        self.serial_write_line("SSC1", "gpio -E -p %d -t 0 -d %d" % (GPIO_WAKE_UP, GPIO_EDGE_DELAY))
        self.check_response("SSC1", "+GPIO_SET:OK")        
        
        time.sleep(1)        
        return result

    def cleanup(self):
        self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET, GPIO_EDGE_DELAY))
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
        except StandardError, e:
            return

        # self.serial_write_line("SSC2", "gpio -E -p %d -t 0 -d %d" % (GPIO_CHIP_RESET, GPIO_EDGE_DELAY))
        # self.check_response("SSC2", "+GPIO_SET:OK")
        # time.sleep(1)

        if "stable" in test_mode:
            for i in range(test_count):
                # result = self.deep_sleep_wakeup_flash_gpio_status()
                result = self.deep_sleep_stable()
        elif "measure_current" in test_mode:
            for i in range(test_count):
                result = self.deep_sleep_current_measure()
        elif "timeout" in test_mode:
            for i in range(test_count):
                result = self.deep_sleep_timeout()
        elif "wakeup" in test_mode:
            for i in range(test_count):
                result = self.deep_sleep_wakeup()
        elif "wake_option" in test_mode:
            for i in range(test_count):
                result = self.wake_option()

        self.set_result("Succeed")
        pass


def main():
    pass


if __name__ == '__main__':
    main()
