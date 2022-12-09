from __future__ import unicode_literals
from tiny_test_fw import Utility
import os
import serial
import threading
import time
import ttfw_idf


class SerialThread(object):
    '''
    Connect to serial port and fake responses just like from a real modem
    '''

    # Dictionary for transforming received AT command to expected response
    AT_FSM = {b'AT+CGMM': b'0G Dummy Model',
              b'AT+CGSN': b'0123456789',
              b'AT+CIMI': b'ESP',
              b'AT+COPS?': b'+COPS: 0,0,"ESP Network"',
              b'AT+CSQ': b'+CSQ: 4,0',
              b'AT+CBC': b'+CBC: 0,50',
              b'ATD*99***1#': b'CONNECT',
              }

    def run(self, log_path, exit_event):
        with serial.Serial(self.port, 115200) as ser, open(log_path, 'w') as f:
            buff = b''
            while not exit_event.is_set():
                time.sleep(0.1)
                buff += ser.read(ser.in_waiting)
                if not buff.endswith(b'\r'):
                    continue  # read more because the complete command wasn't yet received
                cmd_list = buff.split(b'\r')
                buff = b''
                for cmd in cmd_list:
                    if len(cmd) == 0:
                        continue
                    snd = self.AT_FSM.get(cmd, b'')
                    if snd != b'':
                        snd += b'\n'
                    snd += b'OK\n'
                    f.write('Received: {}\n'.format(repr(cmd.decode())))
                    f.write('Sent: {}\n'.format(repr(snd.decode())))
                    ser.write(snd)

    def __init__(self, port, log_path):
        self.port = port
        self.exit_event = threading.Event()
        self.t = threading.Thread(target=self.run, args=(log_path, self.exit_event,))
        self.t.start()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.exit_event.set()
        self.t.join(60)
        if self.t.is_alive():
            Utility.console_log('The serial thread is still alive', 'O')


@ttfw_idf.idf_example_test(env_tag='Example_PPP')
def test_examples_pppos_client(env, extra_data):

    rel_project_path = 'examples/protocols/pppos_client'
    dut = env.get_dut('pppos_client', rel_project_path)
    project_path = os.path.join(dut.app.get_sdk_path(), rel_project_path)

    modem_port = '/dev/ttyUSB{}'.format(0 if dut.port.endswith('1') else 1)

    with SerialThread(modem_port, os.path.join(project_path, 'serial.log')):
        dut.start_app()

        dut.expect_all('pppos_example: Module: 0G Dummy Model',
                       'pppos_example: Operator: "ESP Network"',
                       'pppos_example: IMEI: 0123456789',
                       'pppos_example: IMSI: ESP',
                       'pppos_example: rssi: 4, ber: 0',
                       'pppos_example: Battery voltage: 0 mV',
                       'pppos_example: Modem PPP Started',
                       timeout=60)

    cmd = ('pppd {} 115200 10.0.0.1:10.0.0.2 logfile {} local noauth debug nocrtscts nodetach +ipv6'
           ''.format(modem_port, os.path.join(project_path, 'ppp.log')))
    with ttfw_idf.CustomProcess(cmd, '/dev/null'):  # Nothing is printed here
        dut.expect_all('pppos_example: Modem Connect to PPP Server',
                       'pppos_example: IP          : 10.0.0.2',
                       'pppos_example: Netmask     : 255.255.255.255',
                       'pppos_example: Gateway     : 10.0.0.1',
                       'pppos_example: Name Server1: 0.0.0.0',
                       'pppos_example: Name Server2: 0.0.0.0',
                       'pppos_example: GOT ip event!!!',
                       'pppos_example: MQTT other event id: 7',
                       # There are no fake DNS server and MQTT server set up so the example fails at this point
                       'TRANS_TCP: DNS lookup failed err=202 res=0x0',
                       'MQTT_CLIENT: Error transport connect',
                       'pppos_example: MQTT_EVENT_ERROR',
                       'pppos_example: MQTT_EVENT_DISCONNECTED')


if __name__ == '__main__':
    test_examples_pppos_client()
