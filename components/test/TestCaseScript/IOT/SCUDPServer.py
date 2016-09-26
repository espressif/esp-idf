import socket
import time
import os
import threading

from NativeLog import NativeLog


RETRANSMIT_COUNT = 5
RETRANSMIT_TIMEOUT = 0.5
ABORT_TIMEOUT = 120
BEACON_SEND_RATE = 30


VALUE_NAME = {"type": 0x00,
              "session id": 0x01,
              "result code": 0x02,
              "ap ssid": 0x03,
              "ap password": 0x04,
              "start SC time": 0x05,
              "recv UDP time": 0x06,
              "SP model": 0x07,
              "SP mac": 0x08,
              "ET version": 0x09,
              "ap bssid": 0x0A,
              "ssid hidden": 0x0B,
              "ap encryption": 0x0C,
              }

TYPE_VAL = {"Init new test": 0x00,
            "test request": 0x01,
            "result": 0x02,
            "query phone": 0x03,
            "ACK": 0x80,
            "phone report": 0x81,
            "Not support": 0xFF,
            "invalid session": 0xFE,
            }

RESULT_CODE_VAL = {"OK": 0x80,
                   "JAP fail": 0x81,  # SP join AP fail, should disqualify this result
                   "recv UDP fail": 0x82,  # SP did not receive UDP sent by target
                   }

AP_ENCRYPTION_VAL = {"OPEN": 0x00,
                     "WEP": 0x01,
                     "WPA": 0x02,
                     }

AP_PROPERTY = ("ssid", "password", "bssid", "is_hidden", "encryption", "ht")
PHONE_PROPERTY = ("ip", "mac", "model")


SERIAL_PORT_NUM = 3
LOG_FILE_PREFIX = "SC_IOT"
LOG_FOLDER = os.path.join("AT_LOG", "TEMP")
LOG_FILE_NAME = os.path.join(LOG_FOLDER, "%s_%s.log" % (LOG_FILE_PREFIX, time.strftime("%d%H%M%S", time.localtime())))


REQUEST_LOCK = threading.Lock()
HANDLER_LOCK = threading.Lock()


def sync_request_list(func):
    def handle_args(*args, **kwargs):
        with REQUEST_LOCK:
            ret = func(*args, **kwargs)
        return ret
    return handle_args


def sync_handler_list(func):
    def handle_args(*args, **kwargs):
        with HANDLER_LOCK:
            ret = func(*args, **kwargs)
        return ret
    return handle_args


def _process_one_tlv_pair(data):
    typ = ord(data[0])
    length = ord(data[1])
    value = data[2:2+length]
    processed_data = data[2+length:]
    return (typ, value), processed_data
    pass


def bytes_to_msg(data):
    data_to_process = data
    msg = []
    while True:
        one_pair, data_to_process = _process_one_tlv_pair(data_to_process)
        msg.append(one_pair)
        if len(data_to_process) == 0:
            break
    return msg
    pass


def msg_to_bytes(msg):
    byte_str = ""
    for pair in msg:
        byte_str += chr(pair[0])
        if isinstance(pair[1], list) is True:
            byte_str += chr(len(pair[1]))
            byte_str.join([chr(m) for m in pair[1]])
        elif isinstance(pair[1], str) is True:
            byte_str += chr(len(pair[1]))
            byte_str += pair[1]
        elif isinstance(pair[1], int) is True:
            byte_str += chr(1)
            byte_str += chr(pair[1])
        else:
            raise TypeError("msg content only support list and string type")
    return byte_str


def get_value_from_msg(type_list, msg):
    if isinstance(type_list, str) is True:
        type_list = [type_list]
    ret = [""] * len(type_list)
    for pair in msg:
        for i in range(len(type_list)):
            if pair[0] == VALUE_NAME[type_list[i]]:
                ret[i] = pair[1]
        if "" not in ret:
            # all type value found
            break
    else:
        NativeLog.add_prompt_trace("missing required type in msg")
    return ret


def bytes_to_time(bytes_in):
    if len(bytes_in) != 4:
        return 0
    t = float(ord(bytes_in[0])*256*256*256 + ord(bytes_in[1])*256*256
              + ord(bytes_in[2])*256 + ord(bytes_in[2]))/1000
    return t
    pass


def mac_to_bytes(mac):
    tmp = mac.split(':')
    return "".join([chr(int(m[:2], base=16)) for m in tmp])
    pass


def bytes_to_mac(bytes_in):
    mac = "".join(["%x:" % ord(m) for m in bytes_in]  )
    return mac[:-1]


class RetransmitHandler(threading.Thread):
    def __init__(self, udp_server):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.udp_server = udp_server
        self.exit_event = threading.Event()
        pass

    @sync_request_list
    def find_required_retransmit_msg(self):
        time_now = time.time()
        aborted_sessions = []
        retransmit_msg = []
        msgs = filter(lambda x: time_now - x[4] >= RETRANSMIT_TIMEOUT, self.udp_server.unconfirmed_request)
        for msg in msgs:
            if msg[3] == 0:
                aborted_sessions.append(msg[0])
                self.udp_server.unconfirmed_request.remove(msg)
            else:
                msg[3] -= 1
                msg[4] = time_now
                retransmit_msg.append(msg)
            pass
        return aborted_sessions, retransmit_msg
        pass

    def run(self):
        while True:
            self.exit_event.wait(0.1)
            if self.exit_event.isSet() is True:
                break
            aborted_sessions, retransmit_msg = self.find_required_retransmit_msg()
            for msg in retransmit_msg:
                self.udp_server.udp_socket.sendto(msg[1], msg[2])
            for session_id in aborted_sessions:
                self.udp_server.session_aborted(session_id)

    def exit(self):
        self.exit_event.set()
    pass


class SendBeacon(threading.Thread):
    def __init__(self, sock, udp_port):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.udp_sock = sock
        self.udp_port = udp_port
        self.exit_event = threading.Event()

    def run(self):
        while True:
            msg = [[VALUE_NAME["type"], TYPE_VAL["query phone"]]]
            data = msg_to_bytes(msg)
            self.udp_sock.sendto(data, ("<broadcast>", self.udp_port))
            for i in range(BEACON_SEND_RATE):
                self.exit_event.wait(1)
                if self.exit_event.isSet() is True:
                    return
        pass

    def exit(self):
        self.exit_event.set()
    pass


class UDPServer(threading.Thread):
    def __init__(self, pc_ip, udp_port, update_phone_handler):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        sock.bind((pc_ip, udp_port))
        sock.settimeout(1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.udp_socket = sock
        self.unconfirmed_request = []
        self.test_handler_list = []
        self.pc_ip = pc_ip
        self.udp_port = udp_port
        self.update_phone_handler = update_phone_handler
        self.retransmit_thread = RetransmitHandler(self)
        self.beacon_thread = SendBeacon(self.udp_socket, self.udp_port)
        self.retransmit_thread.start()
        self.beacon_thread.start()
        self.exit_event = threading.Event()
        pass

    @sync_handler_list
    def register_test_handler(self, session_id, test_handler):
        tmp = filter(lambda x: x[0] == session_id, self.test_handler_list)
        if len(tmp) > 0:
            NativeLog.add_prompt_trace("handler with same session id exist")
        else:
            self.test_handler_list.append([session_id, test_handler])
        pass

    @sync_handler_list
    def deregister_test_handler(self, session_id):
        tmp = filter(lambda x: x[0] == session_id, self.test_handler_list)
        if len(tmp) > 1:
            NativeLog.add_prompt_trace("deregister test handler fail")
        elif len(tmp) == 1:
            self.test_handler_list.remove(tmp[0])
        pass

    @sync_handler_list
    def get_test_handler(self, session_id):
        ret = None
        tmp = filter(lambda x: x[0] == session_id, self.test_handler_list)
        if len(tmp) != 1:
            NativeLog.add_prompt_trace("failed to get test handler, "
                                       "%d handler found, session id %s" % (len(tmp), session_id))
        elif len(tmp) == 1:
            ret = tmp[0][1]
        return ret
        pass

    def session_aborted(self, session_id):
        test_handler = self.get_test_handler(session_id)
        if test_handler is not None:
            test_handler.abort_handler()
        pass

    def confirm_request(self, session_id, msg, address):
        test_handler = self.get_test_handler(session_id)
        if test_handler is not None:
            test_handler.res_receiver(msg, address)
        self.remove_pending_request(session_id)
        pass

    def receive_request(self, msg, address):
        result = get_value_from_msg(["type", "session id"], msg)
        msg_type = ord(result[0])
        session_id = result[1]
        if msg_type != TYPE_VAL["result"]:
            self.send_response([[VALUE_NAME["type"], TYPE_VAL["Not support"]]], address)
        else:
            test_handler = self.get_test_handler(session_id)
            if test_handler is None:
                self.send_response([[VALUE_NAME["type"], TYPE_VAL["invalid session"]],
                                    [VALUE_NAME["session id"], session_id]],
                                   address)
                pass
            else:
                test_handler.req_receiver(msg, address)
        pass

    @sync_request_list
    def add_request_to_queue(self, dest_addr, session_id, data):
        tmp = filter(lambda x: x[0] == session_id, self.unconfirmed_request)
        if len(tmp) != 0:
            NativeLog.add_prompt_trace("One pending request belong to same session id %s" % session_id)
            pass
        else:
            self.unconfirmed_request.append([session_id, data,
                                             dest_addr, RETRANSMIT_COUNT-1, time.time()])

    def send_request(self, dest_addr, session_id, msg):
        data = msg_to_bytes(msg)
        self.add_request_to_queue(dest_addr, session_id, data)
        self.udp_socket.sendto(data, dest_addr)
        pass

    def send_response(self, msg, address):
        self.udp_socket.sendto(msg_to_bytes(msg), address)

    @sync_request_list
    def remove_pending_request(self, session_id):
        tmp = filter(lambda x: x[0] == session_id, self.unconfirmed_request)
        if len(tmp) > 0:
            self.unconfirmed_request.remove(tmp[0])
            pass
        pass

    def handle_response(self, msg, address):
        result = get_value_from_msg(["type", "session id"], msg)
        msg_type = ord(result[0])
        session_id = result[1]
        if msg_type == TYPE_VAL["ACK"]:
            self.confirm_request(session_id, msg, address)
        elif msg_type == TYPE_VAL["phone report"]:
            # add new available phone
            tmp = get_value_from_msg(["SP model", "SP mac"], msg)
            phone = dict(zip(PHONE_PROPERTY, [address[0], bytes_to_mac(tmp[1]), tmp[0]]))
            self.update_phone_handler(phone)
            pass
        elif msg_type == TYPE_VAL["Not support"] or msg_type == TYPE_VAL["invalid session"]:
            self.session_aborted(session_id)
        pass

    def run(self):
        while self.exit_event.isSet() is False:
            try:
                data, address = self.udp_socket.recvfrom(65535)
            except socket.error, e:
                    continue

            if address[0] == self.pc_ip:
                continue

            msg = bytes_to_msg(data)
            msg_type = get_value_from_msg(["type"], msg)[0]

            if msg_type is None:
                NativeLog.add_prompt_trace("invalid incoming msg: %s" % "".join(["0x%X, " % m for m in data]))
            else:
                msg_type = ord(msg_type)
                # check if request or reply
                if (msg_type & 0x80) != 0:
                    self.handle_response(msg, address)
                else:
                    self.receive_request(msg, address)
                    pass

        self.retransmit_thread.exit()
        self.beacon_thread.exit()
        self.retransmit_thread.join()
        self.beacon_thread.join()
        pass

    def exit(self):
        self.exit_event.set()
    pass
