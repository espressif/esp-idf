#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Utility for testing the web server. Test cases:
# Assume the device supports 'n' simultaneous open sockets
#
# HTTP Server Tests
#
# 0. Firmware Settings:
# - Create a dormant thread whose sole job is to call httpd_stop() when instructed
# - Measure the following before httpd_start() is called:
#     - current free memory
#     - current free sockets
# - Measure the same whenever httpd_stop is called
# - Register maximum possible URI handlers: should be successful
# - Register one more URI handler: should fail
# - Deregister on URI handler: should be successful
# - Register on more URI handler: should succeed
# - Register separate handlers for /hello, /hello/type_html. Also
#   ensure that /hello/type_html is registered BEFORE  /hello. (tests
#   that largest matching URI is picked properly)
# - Create URI handler /adder. Make sure it uses a custom free_ctx
#   structure to free it up

# 1. Using Standard Python HTTP Client
# - simple GET on /hello (returns Hello World. Ensures that basic
#   firmware tests are complete, or returns error)
# - POST on /hello (should fail)
# - PUT on /hello (should fail)
# - simple POST on /echo (returns whatever the POST data)
# - simple PUT on /echo (returns whatever the PUT data)
# - GET on /echo (should fail)
# - simple GET on /hello/type_html (returns Content type as text/html)
# - simple GET on /hello/status_500 (returns HTTP status 500)
# - simple GET on /false_uri (returns HTTP status 404)
# - largest matching URI handler is picked is already verified because
#   of /hello and /hello/type_html tests
#
#
# 2. Session Tests
# - Sessions + Pipelining basics:
#    - Create max supported sessions
#    - On session i,
#          - send 3 back-to-back POST requests with data i on /adder
#          - read back 3 responses. They should be i, 2i and 3i
#    - Tests that
#          - pipelining works
#          - per-session context is maintained for all supported
#            sessions
#    - Close all sessions
#
# - Cleanup leftover data: Tests that the web server properly cleans
#   up leftover data
#    - Create a session
#    - POST on /leftover_data with 52 bytes of data (data includes
#      \r\n)(the handler only
#      reads first 10 bytes and returns them, leaving the rest of the
#      bytes unread)
#    - GET on /hello (should return 'Hello World')
#    - POST on /false_uri with 52  bytes of data (data includes \r\n)
#      (should return HTTP 404)
#    - GET on /hello (should return 'Hello World')
#
# - Test HTTPd Asynchronous response
#   - Create a session
#   - GET on /async_data
#   - returns 'Hello World!' as a response
#   - the handler schedules an async response, which generates a second
#     response 'Hello Double World!'
#
# - Spillover test
#    - Create max supported sessions with the web server
#    - GET /hello on all the sessions (should return Hello World)
#    - Create one more session, this should fail
#    - GET /hello on all the sessions (should return Hello World)
#
# - Timeout test
#    - Create a session and only Send 'GE' on the same (simulates a
#      client that left the network halfway through a request)
#    - Wait for recv-wait-timeout
#    - Server should automatically close the socket


############# TODO TESTS #############

# 3. Stress Tests
#
# - httperf
#     - Run the following httperf command:
#     httperf --server=10.31.130.126 --wsess=8,50,0.5 --rate 8 --burst-length 2
#
#     - The above implies that the test suite will open
#           - 8 simultaneous connections with the server
#           - the rate of opening the sessions will be 8 per sec. So in our
#             case, a new connection will be opened every 0.2 seconds for 1 second
#           - The burst length 2 indicates that 2 requests will be sent
#             simultaneously on the same connection in a single go
#           - 0.5 seconds is the time between sending out 2 bursts
#           - 50 is the total number of requests that will be sent out
#
#     - So in the above example, the test suite will open 8
#       connections, each separated by 0.2 seconds. On each connection
#       it will send 2 requests in a single burst. The bursts on a
#       single connection will be separated by 0.5 seconds. A total of
#       25 bursts (25 x 2 = 50) will be sent out

# 4. Leak Tests
# - Simple Leak test
#    - Simple GET on /hello/restart (returns success, stop web server, measures leaks, restarts webserver)
#    - Simple GET on /hello/restart_results (returns the leak results)
# - Leak test with open sockets
#    - Open 8 sessions
#    - Simple GET on /hello/restart (returns success, stop web server,
#      measures leaks, restarts webserver)
#    - All sockets should get closed
#    - Simple GET on /hello/restart_results (returns the leak results)


import threading
import socket
import time
import argparse
import requests
import signal
import sys
import string
import random

_verbose_ = False

class Session:
    def __init__(self, addr, port):
        self.client = socket.create_connection((addr, int(port)))
        self.client.settimeout(30)
        self.target = addr

    def send_get(self, path, headers=None):
        request = "GET " + path + " HTTP/1.1\r\nHost: " + self.target
        if headers:
            for field, value in headers.iteritems():
                request += "\r\n"+field+": "+value
        request += "\r\n\r\n"
        self.client.send(request);

    def send_put(self, path, data, headers=None):
        request = "PUT " + path +  " HTTP/1.1\r\nHost: " + self.target
        if headers:
            for field, value in headers.iteritems():
                request += "\r\n"+field+": "+value
        request += "\r\nContent-Length: " + str(len(data)) +"\r\n\r\n"
        self.client.send(request)
        self.client.send(data)

    def send_post(self, path, data, headers=None):
        request = "POST " + path +  " HTTP/1.1\r\nHost: " + self.target
        if headers:
            for field, value in headers.iteritems():
                request += "\r\n"+field+": "+value
        request += "\r\nContent-Length: " + str(len(data)) +"\r\n\r\n"
        self.client.send(request)
        self.client.send(data)

    def read_resp_hdrs(self):
        state = 'nothing'
        resp_read = ''
        while True:
            char = self.client.recv(1)
            if char == '\r' and state == 'nothing':
                state = 'first_cr'
            elif char == '\n' and state == 'first_cr':
                state = 'first_lf'
            elif char == '\r' and state == 'first_lf':
                state = 'second_cr'
            elif char == '\n' and state == 'second_cr':
                state = 'second_lf'
            else:
                state = 'nothing'
            resp_read += char
            if state == 'second_lf':
               break;
        # Handle first line
        line_hdrs = resp_read.splitlines()
        line_comp = line_hdrs[0].split()
        self.status = line_comp[1]
        del line_hdrs[0]
        self.encoding = ''
        self.content_type = ''
        headers = dict()
        # Process other headers
        for h in range(len(line_hdrs)):
            line_comp = line_hdrs[h].split(':')
            if line_comp[0] == 'Content-Length':
                self.content_len = int(line_comp[1])
            if line_comp[0] == 'Content-Type':
                self.content_type = line_comp[1].lstrip()
            if line_comp[0] == 'Transfer-Encoding':
                self.encoding = line_comp[1].lstrip()
            if len(line_comp) == 2:
                headers[line_comp[0]] = line_comp[1].lstrip()
        return headers

    def read_resp_data(self):
        read_data = ''
        if self.encoding != 'chunked':
            while len(read_data) != self.content_len:
                read_data += self.client.recv(self.content_len)
            self.content_len = 0
        else:
            chunk_data_buf = ''
            while (True):
                # Read one character into temp  buffer
                read_ch = self.client.recv(1)
                # Check CRLF
                if (read_ch == '\r'):
                    read_ch = self.client.recv(1)
                    if (read_ch == '\n'):
                        # If CRLF decode length of chunk
                        chunk_len = int(chunk_data_buf, 16)
                        # Keep adding to contents
                        self.content_len += chunk_len
                        read_data += self.client.recv(chunk_len)
                        chunk_data_buf = ''
                        # Fetch remaining CRLF
                        if self.client.recv(2) != "\r\n":
                            # Error in packet
                            return None
                        if not chunk_len:
                            # If last chunk
                            break
                        continue
                    chunk_data_buf += '\r'
                # If not CRLF continue appending
                # character to chunked data buffer
                chunk_data_buf += read_ch
        return read_data

    def close(self):
        self.client.close()

def test_val(text, expected, received):
    if expected != received:
        print " Fail!"
        print "  [reason] " + text + ":"
        print "        expected: " + str(expected)
        print "        received: " + str(received)
        return False
    return True

class myThread (threading.Thread):
    def __init__(self, id, dut, port):
        threading.Thread.__init__(self)
        self.id = id
        self.dut = dut
        self.session = Session(dut, port)

    def run(self):
        self.response = []

        # Pipeline 3 requests
        if (_verbose_):
            print "   Thread: Using adder start " + str(self.id)
        self.session.send_post('/adder', str(self.id));
        time.sleep(1)
        self.session.send_post('/adder', str(self.id));
        time.sleep(1)
        self.session.send_post('/adder', str(self.id));
        time.sleep(1)

        self.session.read_resp_hdrs()
        self.response.append(self.session.read_resp_data())
        self.session.read_resp_hdrs()
        self.response.append(self.session.read_resp_data())
        self.session.read_resp_hdrs()
        self.response.append(self.session.read_resp_data())

    def adder_result(self):
        for i in range(len(self.response)):
#            print self.response[i]
            if not test_val("thread" + str(self.id) + ": response[" + str(i) + "]",
                            str(self.id * (i + 1)), str(self.response[i])):
                return False
        return True

    def close(self):
        self.session.close()

def get_hello(dut, port):
    # GET /hello should return 'Hello World!'
    print "[test] GET /hello returns 'Hello World!' =>",
    r = requests.get("http://" + dut + ":" + port + "/hello")
    if not test_val("status_code", 200, r.status_code):
        return False
    if not test_val("data", "Hello World!", r.text):
        return False
    if not test_val("data", "application/json", r.headers['Content-Type']):
        return False
    print "Success"
    return True

def post_hello(dut, port):
    # PUT /hello returns 405'
    print "[test] PUT /hello returns 405' =>",
    r = requests.put("http://" + dut + ":" + port + "/hello", data="Hello")
    if not test_val("status_code", 405, r.status_code):
        return False
    print "Success"
    return True

def put_hello(dut, port):
    # POST /hello returns 405'
    print "[test] POST /hello returns 404' =>",
    r = requests.post("http://" + dut + ":" + port + "/hello", data="Hello")
    if not test_val("status_code", 405, r.status_code):
        return False
    print "Success"
    return True

def post_echo(dut, port):
    # POST /echo echoes data'
    print "[test] POST /echo echoes data' =>",
    r = requests.post("http://" + dut + ":" + port + "/echo", data="Hello")
    if not test_val("status_code", 200, r.status_code):
        return False
    if not test_val("data", "Hello", r.text):
        return False
    print "Success"
    return True

def put_echo(dut, port):
    # POST /echo echoes data'
    print "[test] PUT /echo echoes data' =>",
    r = requests.put("http://" + dut + ":" + port + "/echo", data="Hello")
    if not test_val("status_code", 200, r.status_code):
        return False
    if not test_val("data", "Hello", r.text):
        return False
    print "Success"
    return True

def get_echo(dut, port):
    # GET /echo returns 404'
    print "[test] GET /echo returns 405' =>",
    r = requests.get("http://" + dut + ":" + port + "/echo")
    if not test_val("status_code", 405, r.status_code):
        return False
    print "Success"
    return True

def get_hello_type(dut, port):
    # GET /hello/type_html returns text/html as Content-Type'
    print "[test] GET /hello/type_html has Content-Type of text/html =>",
    r = requests.get("http://" + dut + ":" + port + "/hello/type_html")
    if not test_val("status_code", 200, r.status_code):
        return False
    if not test_val("data", "Hello World!", r.text):
        return False
    if not test_val("data", "text/html", r.headers['Content-Type']):
        return False
    print "Success"
    return True

def get_hello_status(dut, port):
    # GET /hello/status_500 returns status 500'
    print "[test] GET /hello/status_500 returns status 500 =>",
    r = requests.get("http://" + dut + ":" + port + "/hello/status_500")
    if not test_val("status_code", 500, r.status_code):
        return False
    print "Success"
    return True

def get_false_uri(dut, port):
    # GET /false_uri returns status 404'
    print "[test] GET /false_uri returns status 404 =>",
    r = requests.get("http://" + dut + ":" + port + "/false_uri")
    if not test_val("status_code", 404, r.status_code):
        return False
    print "Success"
    return True

def parallel_sessions_adder(dut, port, max_sessions):
    # POSTs on /adder in parallel sessions
    print "[test] POST {pipelined} on /adder in " + str(max_sessions) + " sessions =>",
    t = []
# Create all sessions
    for i in xrange(max_sessions):
        t.append(myThread(i * 2, dut, port))

    for i in xrange(len(t)):
        t[i].start()

    for i in xrange(len(t)):
        t[i].join()

    res = True
    for i in xrange(len(t)):
        if not t[i].adder_result():
            if not test_val("Thread" + str(i) + "Failed", "True", "False"):
                res = False
        t[i].close()
    if (res):
        print "Success"
    return res

def async_response_test(dut, port):
    # Test that an asynchronous work is executed in the HTTPD's context
    # This is tested by reading two responses over the same session
    print "[test] Test HTTPD Work Queue (Async response) =>",
    s = Session(dut, port)

    s.send_get('/async_data')
    s.read_resp_hdrs()
    if not test_val("First Response", "Hello World!", s.read_resp_data()):
        s.close()
        return False
    s.read_resp_hdrs()
    if not test_val("Second Response", "Hello Double World!", s.read_resp_data()):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def leftover_data_test(dut, port):
    # Leftover data in POST is purged (valid and invalid URIs)
    print "[test] Leftover data in POST is purged (valid and invalid URIs) =>",
    s = Session(dut, port)

    s.send_post('/leftover_data',
                "abcdefghijklmnopqrstuvwxyz\r\nabcdefghijklmnopqrstuvwxyz")
    s.read_resp_hdrs()
    if not test_val("Partial data", "abcdefghij", s.read_resp_data()):
        s.close()
        return False

    s.send_get('/hello')
    s.read_resp_hdrs()
    if not test_val("Hello World Data", "Hello World!", s.read_resp_data()):
        s.close()
        return False

    s.send_post('/false_uri',
                "abcdefghijklmnopqrstuvwxyz\r\nabcdefghijklmnopqrstuvwxyz")
    s.read_resp_hdrs()
    if not test_val("False URI Status", str(404), str(s.status)):
        s.close()
        return False
    s.read_resp_data()

    s.send_get('/hello')
    s.read_resp_hdrs()
    if not test_val("Hello World Data", "Hello World!", s.read_resp_data()):
        s.close()
        return False

    s.close()
    print "Success"
    return True

def timeout_handler(signum, frame):
    raise Exception("Timeout")

def spillover_session(dut, port, max):
    # Session max_sessions + 1 is rejected
    print "[test] Session max_sessions (" + str(max) + ") + 1 is rejected =>",
    s = []
    # Register a timeout callback
    signal.signal(signal.SIGALRM, timeout_handler)
    for i in xrange(max + 1):
        if (_verbose_):
            print "Executing " + str(i)
        a = Session(dut, port)
        a.send_get('/hello')

        try:
            # Check for response timeout
            signal.alarm(5)
            a.read_resp_hdrs()
            a.read_resp_data()
            signal.alarm(0)

            # Control reaches here only if connection was established
            s.append(a)
        except Exception, msg:
            if (_verbose_):
                print str(msg) + ": Connection " + str(i) + " rejected"
            break

    # Close open connections
    for a in s:
        a.close()

    # Check if number of connections is equal to max
    print ["Fail","Success"][len(s) == max]
    return (len(s) == max)

def recv_timeout_test(dut, port):
    print "[test] Timeout occurs if partial packet sent =>",
    signal.signal(signal.SIGALRM, timeout_handler)
    s = Session(dut, port)
    s.client.send("GE")
    try:
        signal.alarm(15)
        s.read_resp_hdrs()
        resp = s.read_resp_data()
        signal.alarm(0)
        if not test_val("Request Timeout", "Server closed this connection", resp):
            s.close()
            return False
    except:
        s.close()
        return False
    s.close()
    print "Success"
    return True

def pipeline_test(dut, port, max_sess):
    print "[test] Pipelining test =>",
    s = [Session(dut, port) for _ in range(max_sess)]
    path = "/echo"
    pipeline_depth = 10
    header = "POST " + path + " HTTP/1.1\r\nHost: " + dut + "\r\nContent-Length: "
    s[0].client.send(header)
    for i in range(1, max_sess):
        for j in range(pipeline_depth):
            data = str(i) + ":" + str(j)
            request = header + str(len(data)) + "\r\n\r\n" + data
            s[i].client.send(request)

    s[0].client.send(str(len("0:0")) + "\r\n\r\n" + "0:0")
    for j in range(1, pipeline_depth):
        data = "0:" + str(j)
        request = header + str(len(data)) + "\r\n\r\n" + data
        s[0].client.send(request)

    res = True
    for i in range(max_sess):
        #time.sleep(1)
        for j in range(pipeline_depth):
            s[i].read_resp_hdrs()
            echo_data = s[i].read_resp_data()
            if (_verbose_):
                print "[" + str(i) + "][" + str(j) + "] = " + echo_data
            if not test_val("Echo Data", str(i) + ":" + str(j), echo_data):
                res = False
        s[i].close()

    #for i in range(max_sess):
        #s[i].close()

    if (res):
        print "Success"
    return res

def packet_size_limit_test(dut, port, test_size):
    print "[test] LWIP send size limit test =>",
    s = Session(dut, port)
    random_data = ''.join(string.printable[random.randint(0,len(string.printable))-1] for _ in range(test_size))
    path = "/echo"
    s.send_post(path, random_data)
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    result = (resp == random_data)
    if not result:
        test_val("Data size", str(len(random_data)), str(len(resp)))
        s.close()
        return False
    print "Success"
    s.close()
    return True

def code_500_server_error_test(dut, port):
    print "[test] 500 Server Error test =>",
    s = Session(dut, port)
    s.client.send("abcdefgh\0")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    # Presently server sends back 400 Bad Request
    #if not test_val("Server Error", "500", s.status):
        #s.close()
        #return False
    if not test_val("Server Error", "400", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_501_method_not_impl(dut, port):
    print "[test] 501 Method Not Implemented =>",
    s = Session(dut, port)
    path = "/hello"
    s.client.send("ABC " + path + " HTTP/1.1\r\nHost: " + dut + "\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    # Presently server sends back 400 Bad Request
    #if not test_val("Server Error", "501", s.status):
        #s.close()
        #return False
    if not test_val("Server Error", "400", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_505_version_not_supported(dut, port):
    print "[test] 505 Version Not Supported =>",
    s = Session(dut, port)
    path = "/hello"
    s.client.send("GET " + path + " HTTP/2.0\r\nHost: " + dut + "\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    if not test_val("Server Error", "505", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_400_bad_request(dut, port):
    print "[test] 400 Bad Request =>",
    s = Session(dut, port)
    path = "/hello"
    s.client.send("XYZ " + path + " HTTP/1.1\r\nHost: " + dut + "\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    if not test_val("Client Error", "400", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_404_not_found(dut, port):
    print "[test] 404 Not Found =>",
    s = Session(dut, port)
    path = "/dummy"
    s.client.send("GET " + path + " HTTP/1.1\r\nHost: " + dut + "\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    if not test_val("Client Error", "404", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_405_method_not_allowed(dut, port):
    print "[test] 405 Method Not Allowed =>",
    s = Session(dut, port)
    path = "/hello"
    s.client.send("POST " + path + " HTTP/1.1\r\nHost: " + dut + "\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    if not test_val("Client Error", "405", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_408_req_timeout(dut, port):
    print "[test] 408 Request Timeout =>",
    signal.signal(signal.SIGALRM, timeout_handler)
    s = Session(dut, port)
    s.client.send("POST /echo HTTP/1.1\r\nHost: " + dut + "\r\nContent-Length: 10\r\n\r\nABCD")
    try:
        signal.alarm(15)
        s.read_resp_hdrs()
        resp = s.read_resp_data()
        signal.alarm(0)
        if not test_val("Client Error", "408", s.status):
            s.close()
            return False
    except:
        s.close()
        return False
    s.close()
    print "Success"
    return True

def code_411_length_required(dut, port):
    print "[test] 411 Length Required =>",
    s = Session(dut, port)
    path = "/echo"
    s.client.send("POST " + path + " HTTP/1.1\r\nHost: " + dut + "\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    # Presently server sends back 400 Bad Request
    #if not test_val("Client Error", "411", s.status):
        #s.close()
        #return False
    if not test_val("Client Error", "400", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

def send_getx_uri_len(dut, port, length):
    s = Session(dut, port)
    method = "GET "
    version = " HTTP/1.1\r\n"
    path = "/"+"x"*(length - len(method) - len(version) - len("/"))
    s.client.send(method)
    time.sleep(1)
    s.client.send(path)
    time.sleep(1)
    s.client.send(version + "Host: " + dut + "\r\n\r\n")
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    s.close()
    return s.status

def code_414_uri_too_long(dut, port, max_uri_len):
    print "[test] 414 URI Too Long =>",
    status = send_getx_uri_len(dut, port, max_uri_len)
    if not test_val("Client Error", "404", status):
        return False
    status = send_getx_uri_len(dut, port, max_uri_len + 1)
    if not test_val("Client Error", "414", status):
        return False
    print "Success"
    return True

def send_postx_hdr_len(dut, port, length):
    s = Session(dut, port)
    path = "/echo"
    host = "Host: " + dut
    custom_hdr_field = "\r\nCustom: "
    custom_hdr_val = "x"*(length - len(host) - len(custom_hdr_field) - len("\r\n\r\n") + len("0"))
    request = "POST " + path + " HTTP/1.1\r\n" + host + custom_hdr_field + custom_hdr_val + "\r\n\r\n"
    s.client.send(request[:length/2])
    time.sleep(1)
    s.client.send(request[length/2:])
    hdr = s.read_resp_hdrs()
    resp = s.read_resp_data()
    s.close()
    if "Custom" in hdr:
        return (hdr["Custom"] == custom_hdr_val), resp
    return False, s.status

def code_431_hdr_too_long(dut, port, max_hdr_len):
    print "[test] 431 Header Too Long =>",
    res, status = send_postx_hdr_len(dut, port, max_hdr_len)
    if not res:
        return False
    res, status = send_postx_hdr_len(dut, port, max_hdr_len + 1)
    if not test_val("Client Error", "431", status):
        return False
    print "Success"
    return True

def test_upgrade_not_supported(dut, port):
    print "[test] Upgrade Not Supported =>",
    s = Session(dut, port)
    path = "/hello"
    s.client.send("OPTIONS * HTTP/1.1\r\nHost:" + dut + "\r\nUpgrade: TLS/1.0\r\nConnection: Upgrade\r\n\r\n");
    s.read_resp_hdrs()
    resp = s.read_resp_data()
    if not test_val("Client Error", "200", s.status):
        s.close()
        return False
    s.close()
    print "Success"
    return True

if __name__ == '__main__':
    ########### Execution begins here...
    # Configuration
    # Max number of threads/sessions
    max_sessions = 7
    max_uri_len = 512
    max_hdr_len = 512

    parser = argparse.ArgumentParser(description='Run HTTPD Test')
    parser.add_argument('-4','--ipv4', help='IPv4 address')
    parser.add_argument('-6','--ipv6', help='IPv6 address')
    parser.add_argument('-p','--port', help='Port')
    args = vars(parser.parse_args())

    dut4 = args['ipv4']
    dut6 = args['ipv6']
    port = args['port']
    dut  = dut4

    _verbose_ = True

    print "### Basic HTTP Client Tests"
    get_hello(dut, port)
    post_hello(dut, port)
    put_hello(dut, port)
    post_echo(dut, port)
    get_echo(dut, port)
    put_echo(dut, port)
    get_hello_type(dut, port)
    get_hello_status(dut, port)
    get_false_uri(dut, port)

    print "### Error code tests"
    code_500_server_error_test(dut, port)
    code_501_method_not_impl(dut, port)
    code_505_version_not_supported(dut, port)
    code_400_bad_request(dut, port)
    code_404_not_found(dut, port)
    code_405_method_not_allowed(dut, port)
    code_408_req_timeout(dut, port)
    code_414_uri_too_long(dut, port, max_uri_len)
    code_431_hdr_too_long(dut, port, max_hdr_len)
    test_upgrade_not_supported(dut, port)

    # Not supported yet (Error on chunked request)
    ###code_411_length_required(dut, port)

    print "### Sessions and Context Tests"
    parallel_sessions_adder(dut, port, max_sessions)
    leftover_data_test(dut, port)
    async_response_test(dut, port)
    spillover_session(dut, port, max_sessions)
    recv_timeout_test(dut, port)

    # May timeout in case requests are sent slower than responses are read.
    # Instead use httperf stress test
    pipeline_test(dut, port, max_sessions)
    packet_size_limit_test(dut, port, 50*1024)
    get_hello(dut, port)

    sys.exit()
