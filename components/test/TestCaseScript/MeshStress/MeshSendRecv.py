from __future__ import division
import time
import threading
import re
import random
import os
import binascii

from TCAction import PerformanceTCBase
from NativeLog import NativeLog
from NativeLog import HTMLGenerator
from comm import MeshPort
from Utility import Encoding

# check frequency in second
CHECK_FREQ = 0.05
# check timeout in seconds
CHECK_TIMEOUT = 30
# multicast group len
MULTICAST_GROUP_LEN = 2


LOG_PATH = os.path.join("..", "log")

def _convert_to_mesh_mac_format(value_in):
    value_out = ""
    match_list = re.findall("([0-9a-fA-F]+)", value_in)
    try:
        for i in range(6):
            value_out += "%02X" % int(match_list[i], base=16)
        pass
    except StandardError, e:
        NativeLog.add_exception_log(e)
        raise e
    return value_out

class SendRecvTime(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.send_time = dict()
        self.recv_time = dict()
        self.send_time_lock = threading.Lock()
        self.recv_time_lock = threading.Lock()

    def add_send_time(self, key, timestamp):
        with self.send_time_lock:
            self.send_time[key] = timestamp

    def add_recv_time(self, key, timestamp):
        with self.recv_time_lock:
            if key in self.recv_time.keys():
                self.recv_time[key].append(timestamp)
            else:
                self.recv_time[key] = [timestamp]

    def calculate(self):
        # add compute delay time code here
        print 'send dict len:', len(self.send_time)
        print 'recv dict len:', len(self.recv_time)
        recv_time_keys = self.recv_time.keys()
        Max_delay_time = 0.0
        Total_delay_time = 0.0
        # for i in range(len(recv_time_keys)):
        #     key = recv_time_keys[i]
        for key in recv_time_keys:
            Total_delay_time_t = 0.0
            if isinstance(self.recv_time[key], list):
                for time1 in self.recv_time[key]:
                    if time1 - self.send_time[key] >= Max_delay_time:
                        Max_delay_time = time1 - self.send_time[key]
                        Total_delay_time_t += (time1 - self.send_time[key])
                    else:
                        pass
            else:
                 if self.recv_time[key] - self.send_time[key] > Max_delay_time:
                     Max_delay_time = self.recv_time[key] - self.send_time[key]
                     Total_delay_time_t += (self.recv_time[key] - self.send_time[key])
            Total_delay_time_t += (Total_delay_time_t / len(self.recv_time[key]))
            Total_delay_time += Total_delay_time_t
        Avg_delay_time = Total_delay_time / len(recv_time_keys)
        loss_rate = (len(self.send_time.keys()) - len(self.recv_time.keys())) / len(self.send_time.keys())
        return [Max_delay_time, Avg_delay_time, loss_rate]
        pass

class EntitySendThread(threading.Thread):
    def __init__(self, port, behavior, unicast_addr, send_delay, typ, device_mac_list, server_addr, send_recv_time):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.recv_data_cache = ""
        self.packets_sent = 0
        self.port = port
        self.behavior = behavior
        self.typ = typ
        self.unicast_addr = unicast_addr
        self.node_num = len(device_mac_list)
        self.device_mac_list = list(device_mac_list)
        self.server_addr = server_addr
        if typ != "SERVER":
            self.device_mac_list.remove(port.device_mac)
        self.send_delay = send_delay
        self.cache_lock = threading.Lock()
        self.exit_event = threading.Event()
        self.send_recv_time = send_recv_time
        pass

    def data_recv_callback(self, data):
        with self.cache_lock:
            self.recv_data_cache += data
            if self.typ == "SSC":
                while True:
                    if self.recv_data_cache is not None:
                        match = re.compile(".+\+MSEND1:\d+:OK", re.DOTALL)
                        res = match.search(self.recv_data_cache)
                        index = re.search("\+MSEND1:(\d+):OK", self.recv_data_cache)
                        if index is not None:
                            time1 = time.time()
                            index1 = int(index.group(1))
                            self.send_recv_time.add_send_time(index1, time1)
                            #print 'send index:', index1
                            process_index = res.group().split("MSEND1")
                            if len(process_index) > 1:
                                process_index_t = len(process_index[0]) + len("MSEND1")
                                self.recv_data_cache = self.recv_data_cache[process_index_t:]
                            else:
                                self.recv_data_cache = self.recv_data_cache[len(res.group()):]
                        else:
                            break
                    else:
                        break
        pass


    def __server_send_packet(self, dst_addr, option_list=None, group_addr=None):
        ver = 0x0
        flags = 0x0
        proto = 0x0
        index = random.randint(10000, 999999999)
        if group_addr is not None:
            len_t = hex(len(group_addr) * 6).split("0x")
            if len(group_addr) <= 2:
                option_list = "070" + len_t[1]
            else:
                option_list = "07" + len_t[1]
            group = ""
            for addr in group_addr:
                group += _convert_to_mesh_mac_format(addr)
            option_list += group
        else:
            option_list = None
            if self.behavior == "broadcast":
                dst_addr = "00:00:00:00:00:00"
            elif self.behavior == "unicast":
                if self.unicast_addr == "random":
                    dst_addr = random.choice(self.device_mac_list)
                else:
                    dst_addr = self.unicast_addr
            elif self.behavior == "p2p":
                proto = 0x2
                if self.unicast_addr == "random":
                    dst_addr = random.choice(self.device_mac_list)
                else:
                    dst_addr = self.unicast_addr
        packet = MeshPort.Packet(ver=ver, flags=flags, proto=proto,
                        dst_addr=dst_addr, src_addr=self.server_addr, option_list=option_list, data="A" * 100, index=index)
        send_data = packet.dumps
        try:
            self.port.socket.send(send_data)
            time2 = time.time()
            self.send_recv_time.add_send_time(index, time2)
        except StandardError, e:
            NativeLog.add_exception_log(e)
            return False

    def __server_do_send(self):
        if self.behavior == "broadcast":
            if self.__server_send_packet(dst_addr="00:00:00:00:00:00", group_addr=None) is True:
                self.packets_sent += self.node_num
        elif self.behavior == "multicast":
            random.shuffle(self.device_mac_list)
            group_addr_list = self.device_mac_list[:MULTICAST_GROUP_LEN]
            if self.__server_send_packet(dst_addr="01:00:5E:00:00:00", group_addr=group_addr_list) is True:
                self.packets_sent += MULTICAST_GROUP_LEN
        elif self.behavior == "unicast":
            if self.__server_send_packet(dst_addr=random.choice(self.device_mac_list), group_addr=None) is True:
                self.packets_sent += 1
        elif self.behavior == "p2p":
            if self.__server_send_packet(dst_addr=random.choice(self.device_mac_list), group_addr=None) is True:
                self.packets_sent += 1
        else:
            NativeLog.add_trace_critical("unsupported behavior [%s]" % self.behavior)
            self.exit()
            return

    def __node_send_packet(self, dst_addr, group_addr=None):
        send_data = ""
        ret = False
        if group_addr is not None:
            len_t = hex(len(group_addr) * 6).split("0x")
            if len(group_addr) <= 2:
                option_list = "070" + len_t[1]
            else:
                option_list = "07" + len_t[1]
            group = ""
            for addr in group_addr:
                group += _convert_to_mesh_mac_format(addr)
            option_list += group
            dst_addr = "01:00:5E:00:00:00"
            send_data = "meshsend -S -d %s -o %s -l 100\r\n" % (dst_addr, option_list)
        else:
            if self.behavior == "broadcast":
                dst_addr = "00:00:00:00:00:00"
                send_data = "meshsend -S -d %s -l 100\r\n" % dst_addr
            elif self.behavior == "unicast":
                if self.unicast_addr == "random":
                    dst_addr = random.choice(self.device_mac_list)
                else:
                    dst_addr = self.unicast_addr
                send_data = "meshsend -S -d %s -l 100\r\n" % dst_addr
            elif self.behavior == "p2p":
                if self.unicast_addr == "random":
                    dst_addr = random.choice(self.device_mac_list)
                else:
                    dst_addr = self.unicast_addr
                send_data = "meshsend -S -d %s -t 1 -l 100\r\n" % dst_addr
        try:
            self.port.write(send_data)
        except StandardError, e:
            NativeLog.add_exception_log(e)
            pass
        for i in range(int(CHECK_TIMEOUT / CHECK_FREQ)):
            time.sleep(CHECK_FREQ)
            with self.cache_lock:
                if self.recv_data_cache.find("+MESHSEND:OK") != -1:
                    ret = True
                    break
                elif self.recv_data_cache.find("+MESHSEND:ERROR") != -1:
                    break
        return ret


    def __node_do_send(self):
        if self.behavior == "broadcast":
            if self.__node_send_packet("00:00:00:00:00:00", group_addr=None) is True:
                self.packets_sent += self.node_num
        elif self.behavior == "multicast":
            random.shuffle(self.device_mac_list)
            group_addr_list = self.device_mac_list[:MULTICAST_GROUP_LEN]
            if self.__node_send_packet("01:00:5E:00:00:00", group_addr_list) is True:
                self.packets_sent += MULTICAST_GROUP_LEN
        elif self.behavior == "unicast":
            if self.__node_send_packet(random.choice(self.device_mac_list), group_addr=None) is True:
                self.packets_sent += 1
        elif self.behavior == "p2p":
            if self.__node_send_packet(random.choice(self.device_mac_list), group_addr=None) is True:
                self.packets_sent += 1
        else:
            NativeLog.add_trace_critical("unsupported behavior [%s]" % self.behavior)
            self.exit()
            return

    def get_sent_packets(self):
        return self.packets_sent

    def exit(self):
        self.exit_event.set()
        pass

    def run(self):
        while self.exit_event.isSet() is False:
            if self.typ == "SSC":
                self.__node_do_send()
            elif self.typ == "SERVER":
                self.__server_do_send()
            else:
                NativeLog.add_trace_critical("type [%s] is neither SSC nor SERVER" % self.typ)
                break
            time.sleep(self.send_delay)

    pass


class EntityRecvThread(threading.Thread):
    def __init__(self, port, typ, send_recv_time):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.recv_data_cache = ""
        self.packets_recv = 0
        self.port = port
        self.typ = typ
        self.cache_lock = threading.Lock()
        self.exit_event = threading.Event()
        self.send_recv_time = send_recv_time
        pass

    def data_recv_callback(self, data):
        # if self.typ == "SERVER":
        #     NativeLog.add_prompt_trace("[data_recv_callback] server recv len %d" % len(data))
        with self.cache_lock:
            self.recv_data_cache += data
        pass

    def __server_do_recv(self):
        while True:
            if self.recv_data_cache:
                data_cache = self.recv_data_cache
                data_cache_hex = binascii.hexlify(data_cache)
                packet_len = int(data_cache_hex[2:6], 16)
                if len(self.recv_data_cache) >= packet_len:
                    time3 = time.time()
                    data_catch_t = self.recv_data_cache[:packet_len]
                    packet = binascii.hexlify(data_catch_t)
                    index3 = int(packet[-8:], 16)
                    self.send_recv_time.add_recv_time(index3, time3)
                    self.recv_data_cache = self.recv_data_cache[packet_len:]
                else:
                    break
                #self.packets_recv += 1
            else:
                break

    def __node_do_recv(self):
        with self.cache_lock:
            while True:
                if self.recv_data_cache:
                    match = re.search("\+MESHRECV:\d+", self.recv_data_cache)
                    index = re.search(",(\d+),OK", self.recv_data_cache)
                    res = re.compile(".+,\d+,OK", re.DOTALL)
                    res_t = res.search(self.recv_data_cache)
                    if match is not None:
                        time4 = time.time()
                        if index is not None:
                            index4 = int(index.group(1))
                            self.send_recv_time.add_recv_time(index4, time4)
                            if len(res_t.group()) > 1:
                                process_index = len(res_t.group(0))
                                self.recv_data_cache = self.recv_data_cache[process_index:]
                            else:
                                process_index = len(res_t.group())
                                self.recv_data_cache = self.recv_data_cache[process_index:]
                        else:
                            break
                    else:
                        break
                        # self.packets_recv += 1
                else:
                    break
        pass

    def get_recv_packets(self):
        return self.packets_recv

    def exit(self):
        self.exit_event.set()
        pass

    def run(self):
        while self.exit_event.isSet() is False:
            if self.typ == "SSC":
                self.__node_do_recv()
            elif self.typ == "SERVER":
                self.__server_do_recv()
            else:
                NativeLog.add_trace_critical("type [%s] is neither SSC nor SERVER" % self.typ)
                break
            time.sleep(CHECK_FREQ)

    pass


class MeshSendRecv(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, name, test_env, cmd_set, timeout, log_path):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.send_config = []
        self.test_time = 0
        self.loss_rate_standard = 0.8
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy" and cmd_set[i][0] != "":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        # load node send config
        for i in range(1, len(cmd_set)):
            for j in range(len(cmd_set[i][1])):
                if cmd_set[i][1][j] != "":
                    cmd_string = "self.send_config.extend([" + cmd_set[i][1][j] + "])"
                    exec cmd_string
        node_num = self.get_parameter("node_num")
        self.recv_cb = dict.fromkeys(["SSC%s" % (x + 1) for x in range(int(node_num))] + ["GSOC1"])
        self.recv_cb_lock = threading.Lock()
        pass

    def register_recv_callback(self, port_name, callback):
        with self.recv_cb_lock:
            if self.recv_cb[port_name] is None:
                self.recv_cb[port_name] = [callback]
            else:
                self.recv_cb[port_name].append(callback)
        pass

    def process(self):
        try:
            test_time = self.test_time * 60
            send_config = self.send_config
            loss_rate_standard = self.loss_rate_standard
            node_num = self.get_parameter("node_num")
            pc_ip_list = self.get_parameter("pc_ip").split(".")
            port = self.get_parameter("test_tcp_port1")
            send_recv_time = SendRecvTime()
        except StandardError:
            return
        #create server_addr
        server_addr = ""
        for i in range(len(pc_ip_list)):
            if pc_ip_list[i] in ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]:
                server_addr = server_addr + "0" + pc_ip_list[i]
            else:
                list_t = hex(int(pc_ip_list[i])).split("0x")
                server_addr += list_t[1]
        port_t = hex(port).split("0x")
        port_t_list = list(port_t[1])
        server_addr = server_addr + port_t_list[2] + port_t_list[3] + port_t_list[0] + port_t_list[1]
        server_port = self.test_env.get_port_by_name("GSOC1")
        if server_port is None:
            return

        # create thread dict
        thread_dict = dict.fromkeys(["SSC%s" % (x + 1) for x in range(int(node_num))] + ["GSOC1"])
        for port_name in thread_dict:
            thread_dict[port_name] = dict(zip(["tx", "rx"], [None, None]))
        device_mac_list = []
        # init recv thread & register port for SSC
        for port_name in ["SSC%s" % (x + 1) for x in range(int(node_num))]:
            port = self.test_env.get_port_by_name(port_name)
            thread_dict[port_name]["rx"] = EntityRecvThread(port, "SSC", send_recv_time)
            self.register_recv_callback(port_name, thread_dict[port_name]["rx"].data_recv_callback)
            device_mac_list.append(port.device_mac)

        thread_dict["GSOC1"]["rx"] = EntityRecvThread(server_port, "SERVER", send_recv_time)
        self.register_recv_callback("GSOC1", thread_dict["GSOC1"]["rx"].data_recv_callback)

        # config[0]: target_name; config[1]: behavior; config[2]: destination; config[3]:send_delay;
        for config in send_config:
            port = self.test_env.get_port_by_name(config[0])
            name = port.name
            if config[2] == "GSOC1":
                dst = server_addr[:2] + ":" + server_addr[2:4] + ":" + server_addr[4:6] + ":" + server_addr[6:8] + \
                    ":" + server_addr[8:10] + ":" + server_addr[10:12]
            elif config[2] == "random":
                dst = "random"
            else:
                dst = self.test_env.get_port_by_name(config[2]).device_mac
            if name != "GSOC1":
                server_addr = None
            if config[1] == "broadcast" or config[1] == "multicast":
                dst = None
            typ = "SSC" if isinstance(port, MeshPort.MeshPort) is False else "SERVER"
            thread_dict[name]["tx"] = EntitySendThread(port, config[1], dst, config[3], typ, device_mac_list,
                                                       server_addr, send_recv_time)
            self.register_recv_callback(name, thread_dict[name]["tx"].data_recv_callback)
            pass

        # start all thread
        for port_name in thread_dict:
            if thread_dict[port_name]["rx"] is not None:
                thread_dict[port_name]["rx"].start()
            if thread_dict[port_name]["tx"] is not None:
                thread_dict[port_name]["tx"].start()

        # wait test time
        time.sleep(test_time)
        # close all send thread
        for port_name in thread_dict:
            if thread_dict[port_name]["tx"] is not None:
                thread_dict[port_name]["tx"].exit()
                thread_dict[port_name]["tx"].join()
        # make sure all packet received before close recv thread
        time.sleep(10)
        # close all recv thread
        for port_name in thread_dict:
            if thread_dict[port_name]["rx"] is not None:
                thread_dict[port_name]["rx"].exit()
                thread_dict[port_name]["rx"].join()

        [max_delay_time, avg_delay_time, loss_rate] = send_recv_time.calculate()

        NativeLog.add_trace_critical("[Mesh Send Recv Test] MAX Delay Time is %.3f" % max_delay_time)
        NativeLog.add_trace_critical("[Mesh Send Recv Test] Avg Delay Time is %.3f" % avg_delay_time)
        NativeLog.add_trace_critical("[Mesh Send Recv Test] loss rate is %.2f%%" % (loss_rate * 100))

        # set succeed if loss rate higher than required
        if loss_rate < loss_rate_standard:
            self.set_result("Succeed")
        pass

    @Encoding.encode_utf8(3)
    def result_check(self, port_name, data):
        if port_name in self.recv_cb:
            # if port_name == "GSOC1":
            #     NativeLog.add_prompt_trace("[result_check] recv GSOC1 data len %s" % len(data))
            with self.recv_cb_lock:
                callback_list = self.recv_cb[port_name]
            if callback_list is not None:
                for callback in callback_list:
                    callback(data)

        # do logging
        timestamp = NativeLog.generate_timestamp()
        with self.sync_lock:
            _formatted_data = HTMLGenerator.process_one_log_item(data, self.log_index, port_name, timestamp)
            self.log_index += 1

        self.append_to_log_file(_formatted_data)

        NativeLog.add_all_tc_log(data, port_name, timestamp)
        pass


def main():
    pass


if __name__ == '__main__':
    main()
