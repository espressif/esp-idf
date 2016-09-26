import Queue

from TCAction import TCActionBase
from NativeLog import NativeLog
from SCUDPServer import *
from TCAction.CmdExecutor import CmdExecutorBasic
from Utility import MakeFolder

TEST_RESULT_CATEGORY = ("AP", "Phone")
TEST_RESULT_PROPERTY = ("model", "total", "succeed", "failed", "total time1", "total time2")
SINGLE_TEST_RESULT = ("AP", "Phone", "result", "time1", "time2")

LOG_FILES = ("by_ap.tmp", "by_phone.tmp", "failed_item.tmp", "disqualified_item.tmp", "total.tmp")

LOG_PATH = os.path.join("AT_LOG", "IOT")


def make_session_id(mac, test_id):
    return mac_to_bytes(mac) + chr((test_id & 0xFF00) >> 8) + chr(test_id & 0xFF)


class TestHandler(threading.Thread):
    def __init__(self, session_id, ap, phone, udp_server, test_action):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.udp_server = udp_server
        self.session_id = session_id
        self.ap = ap
        self.phone = phone
        self.test_action = test_action
        self.recv_queue = Queue.Queue(10)
        self.abort_event = threading.Event()
        self.start_time = time.time()
        self.test_result = None
        udp_server.register_test_handler(session_id, self)
        pass

    def req_receiver(self, msg, address):
        self.recv_queue.put([msg, address])
        pass

    def res_receiver(self, msg, address):
        self.recv_queue.put([msg, address])
        pass

    def abort_handler(self):
        NativeLog.add_prompt_trace("[Test Handler][Debug] test aborted")
        self.abort_event.set()
        self.test_action.remove_from_available_list(self.phone)
        pass

    def wait_result(self, event, timeout=None):
        time_start = time.time()
        while True:
            if self.abort_event.isSet() is True:
                return False

            if time.time() - self.start_time > ABORT_TIMEOUT:
                return False

            if timeout is not None:
                if time.time() - time_start > timeout:
                    return False

            if event == "ACK" or event == "result":
                try:
                    ret = self.recv_queue.get(timeout=0.5)
                except Queue.Empty, e:
                    continue
                else:
                    msg = ret[0]
                    value_list = get_value_from_msg("type", msg)
                    msg_typ = ord(value_list[0])
                    if msg_typ == TYPE_VAL[event]:
                        NativeLog.add_prompt_trace("[Test Handler][Debug] wait message succeed")
                        return msg
                    elif (msg_typ & 0x80) == 0:  # invalid request
                        self.udp_server.send_response([[VALUE_NAME["type"], TYPE_VAL["Not support"]],
                                                       [VALUE_NAME["session id"], self.session_id]],
                                                      ret[1])
                        pass
            else:
                pass
        pass

    def run(self):
        for i in range(1):
            # step1 send broadcast to SP
            msg = [[VALUE_NAME["type"], TYPE_VAL["Init new test"]],
                   [VALUE_NAME["session id"], self.session_id]]
            self.udp_server.send_request(("<broadcast>", self.udp_server.udp_port), self.session_id, msg)
            # wait response
            if self.wait_result("ACK") is False:
                break
            NativeLog.add_prompt_trace("[Step1] Initial new test succeed")

            # step2 start smart config
            checker_stings = ["ATR AT1 L OK"]
            test_action_string = ["ATS AT1 AT+CWSTOPSMART"]
            fail_string = "Fail, Failed to start smart config"
            if self.test_action.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                NativeLog.add_prompt_trace(fail_string)
                break
            checker_stings = ["ATR AT1 L OK"]
            test_action_string = ["ATS AT1 AT+CWSTARTSMART=1"]
            if self.test_action.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                NativeLog.add_prompt_trace(fail_string)
                break
            NativeLog.add_prompt_trace("[Step2] Start smart config succeed")

            # step3 send test request to SP
            msg = [[VALUE_NAME["type"], TYPE_VAL["test request"]],
                   [VALUE_NAME["session id"], self.session_id],
                   [VALUE_NAME["ap ssid"], self.ap["ssid"]],
                   [VALUE_NAME["ap password"], self.ap["password"]],
                   [VALUE_NAME["ap bssid"], mac_to_bytes(self.ap["bssid"])],
                   # [VALUE_NAME["ET version"], 0x20],
                   [VALUE_NAME["ssid hidden"], self.ap["is_hidden"]],
                   [VALUE_NAME["ap encryption"], AP_ENCRYPTION_VAL[self.ap["encryption"]]]
                   ]
            self.udp_server.send_request((self.phone["ip"], self.udp_server.udp_port), self.session_id, msg)
            # wait SP reply
            if self.wait_result("ACK") is False:
                break
            NativeLog.add_prompt_trace("[Step3] Send test request succeed")
            time_base = time.time()

            # step4 wait target smart config succeed
            checker_stings = ["ATR AT1 C get%%20wifi%%20info C %s C %s"
                              % (self.ap["ssid"], self.ap["password"])]
            test_action_string = []
            fail_string = "Fail, Fail to get ap info"
            # if check target get smart config result fail, continue and get result from SP
            ret = self.test_action.load_and_exe_one_step(checker_stings, test_action_string,
                                                         fail_string, check_time=600)
            if ret is False:
                NativeLog.add_prompt_trace("[Step4] Target smart config fail")
                step_4_fail = True
            else:
                NativeLog.add_prompt_trace("[Step4] Target smart config succeed")
                step_4_fail = False
            time_target_succeed = time.time() - time_base

            # step5 wait SP result
            msg = self.wait_result("result")
            if msg is False:
                NativeLog.add_prompt_trace("[Test Handler][Debug] Failed to get result from SP")
                break
            else:
                self.udp_server.send_response([[VALUE_NAME["type"], TYPE_VAL["ACK"]],
                                               [VALUE_NAME["session id"], self.session_id]],
                                              (self.phone["ip"], self.udp_server.udp_port))
                tmp = get_value_from_msg(["result code", "start SC time", "recv UDP time"], msg)
                result_code = ord(tmp[0])
                if result_code == RESULT_CODE_VAL["OK"]:
                    sp_start_time = bytes_to_time(tmp[1])
                    sp_recv_udp_time = bytes_to_time(tmp[2])
                    smart_config_protocol_cost = time_target_succeed - sp_start_time
                    user_experience_time = sp_recv_udp_time - sp_start_time
                    self.test_result = ["Succeed", smart_config_protocol_cost, user_experience_time]
                elif result_code == RESULT_CODE_VAL["recv UDP fail"]:
                    sp_start_time = bytes_to_time(tmp[1])
                    if step_4_fail is True:
                        smart_config_protocol_cost = 0
                    else:
                        smart_config_protocol_cost = time_target_succeed - sp_start_time
                    self.test_result = ["Failed", smart_config_protocol_cost, 0]
                    pass
                else:
                    NativeLog.add_prompt_trace("[Test Handler][Debug] Disqualified message: %s" % tmp)

                for k in range(RETRANSMIT_COUNT - 1):
                    if self.wait_result("result", RETRANSMIT_TIMEOUT) is not False:
                        self.udp_server.send_response([[VALUE_NAME["type"], TYPE_VAL["ACK"]],
                                                       [VALUE_NAME["session id"], self.session_id]],
                                                      (self.phone["ip"], self.udp_server.udp_port))

            NativeLog.add_prompt_trace("[Step5] Receive test result from SP")

        if self.test_result is None:
            self.test_result = ["Disqualified", 0, 0]
        self.udp_server.deregister_test_handler(self.session_id)
        NativeLog.add_prompt_trace("One Test Done")
        pass

    def get_result(self):
        if self.test_result is None:
            NativeLog.add_trace_critical("Get result before test finish")
        return self.test_result
    pass


class SCIOT(TCActionBase.CommonTCActionBase):
    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        self.ap_list = []
        self.test_result = dict.fromkeys(TEST_RESULT_CATEGORY)
        self.test_result["AP"] = []
        self.test_result["Phone"] = []
        self.available_phone_list = []
        self.pc_ip = "<pc_ip>"
        self.udp_port = "<test_udp_port1>"
        self.test_id = 0x00
        self.resource_lock = threading.Lock()
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy" and cmd_set[i][0] != "":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        for i in range(1, len(cmd_set)):
            for j in range(len(cmd_set[i][1])):
                if cmd_set[i][1][j] != "":
                    cmd_string = "self.ap_list.append(dict(zip(AP_PROPERTY, " + cmd_set[i][1][j] + ")))"
                    exec cmd_string
        for ap in self.ap_list:
            self.test_result["AP"].append(dict(zip(TEST_RESULT_PROPERTY, [ap["ssid"], 0, 0, 0, 0, 0])))

        self.log_folder = MakeFolder.make_folder(os.path.join(LOG_PATH, "TEST_%s"
                                                              % (time.strftime("%y%m%d%H%M%S", time.localtime()))))
        self.log_files = dict.fromkeys(LOG_FILES)
        for _file in self.log_files:
            self.log_files[_file] = os.path.join(self.log_folder,
                                                 (time.strftime("%H%M%S", time.localtime())) + _file)
        pass

    def update_phone_list(self, phone):
        with self.resource_lock:
            tmp = filter(lambda x: x["model"] == phone["model"], self.available_phone_list)
            if len(tmp) == 1:
                tmp[0]["ip"] = phone["ip"]
            else:
                self.available_phone_list.append(phone)

            tmp = filter(lambda x: x["model"] == phone["model"], self.test_result["Phone"])
            if len(tmp) == 0:
                self.test_result["Phone"].append(dict(zip(TEST_RESULT_PROPERTY, [phone["model"], 0, 0, 0, 0, 0])))
        pass

    def remove_from_available_list(self, phone):
        with self.resource_lock:
            tmp = filter(lambda x: x["model"] == phone["model"], self.available_phone_list)
            if len(tmp) == 1:
                self.available_phone_list.remove(tmp[0])
        pass

    def allocate_test(self):
        phone = None
        test_count = 0xFFFF
        with self.resource_lock:
            for _phone in self.available_phone_list:
                tmp = filter(lambda x: x["model"] == _phone["model"], self.test_result["Phone"])
                if len(tmp) == 1:
                    _count = tmp[0]["total"]
                    if _count < test_count:
                        test_count = _count
                        phone = _phone
        ap_list = self.ap_list[test_count % len(self.ap_list):]
        return phone, ap_list
        pass

    def output_test_result(self, ap, phone, test_result):
        result_str = "Time stamp" + ":\t" + NativeLog.generate_timestamp() + "\r\n"
        result_str += "AP model" + ":\t" + str(ap["ssid"]) + "\r\n"
        result_str += "AP encryption" + ":\t" + str(ap["encryption"]) + "\r\n"
        result_str += "AP HT" + ":\t" + str(ap["ht"]) + "\r\n"
        result_str += "AP ssid hidden" + ":\t" + str(ap["is_hidden"]) + "\r\n"
        result_str += "Phone model" + ":\t" + str(phone["model"]) + "\r\n"
        result_str += "Result" + ":\t" + str(test_result[0]) + "\r\n"
        result_str += "Time1" + ":\t" + str(test_result[1]) + "\r\n"
        result_str += "Time2" + ":\t" + str(test_result[2]) + "\r\n"

        with self.resource_lock:
            tmp = [filter(lambda x: x["model"] == ap["ssid"], self.test_result["AP"])[0],
                   filter(lambda x: x["model"] == phone["model"], self.test_result["Phone"])[0]]
            if test_result[0] == "Succeed":
                for _tmp in tmp:
                    _tmp["total"] += 1
                    _tmp["succeed"] += 1
                    _tmp["total time1"] += test_result[1]
                    _tmp["total time2"] += test_result[2]
                pass
            elif test_result[0] == "Disqualified":
                for _tmp in tmp:
                    _tmp["total"] += 1
                pass
            else:
                for _tmp in tmp:
                    _tmp["total"] += 1
                    _tmp["failed"] += 1
                pass
            tmp_result = dict(zip(TEST_RESULT_CATEGORY, ["", ""]))
            for category in self.test_result:
                for _result in self.test_result[category]:
                    for n in _result:
                        tmp_result[category] += str(n) + ":\t" + str(_result[n]) + "\r\n"

        # update to log file
        with open(self.log_files["by_ap.tmp"], "wb+") as f:
            f.write(tmp_result["AP"])
        with open(self.log_files["by_phone.tmp"], "wb+") as f:
            f.write(tmp_result["Phone"])

        with open(self.log_files["total.tmp"], "ab+") as f:
            f.write(result_str)
        if test_result[0] == "Failed":
            with open(self.log_files["failed_item.tmp"], "ab+") as f:
                f.write(result_str)
        elif test_result[0] == "Disqualified":
            with open(self.log_files["disqualified_item.tmp"], "ab+") as f:
                f.write(result_str)

        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        pc_ip = CmdExecutorBasic.extract_parameter(self.pc_ip, self.test_env)
        if isinstance(self.udp_port, int) is False:
            udp_port = CmdExecutorBasic.extract_parameter(self.udp_port, self.test_env)
        else:
            udp_port = self.udp_port

        server = UDPServer(pc_ip, udp_port, self.update_phone_list)
        server.start()

        while True:
            phone, ap_list = self.allocate_test()
            if phone is None:
                time.sleep(5)
                continue
            for ap in ap_list:
                NativeLog.add_prompt_trace("AP is %s, Phone is %s" % (ap["ssid"], phone["model"]))
                session_id = make_session_id(phone["mac"], self.test_id)
                self.test_id += 1
                test_handler = TestHandler(session_id, ap, phone, server, self)
                test_handler.start()
                test_handler.join()
                result = test_handler.get_result()
                self.output_test_result(ap, phone, result)

        # finally, execute done
        server.join()
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()
