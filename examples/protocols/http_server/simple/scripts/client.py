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

import socket
import argparse

class Session:
    def __init__(self, addr, port):
        self.client = socket.create_connection((addr, int(port)))
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

def verbose_print(verbosity, *args):
    if (verbosity):
        print ''.join(str(elems) for elems in args)

def test_get_handler(ip, port, verbosity = False):
    verbose_print(verbosity, "========  GET HANDLER TEST =============")
    # Establish HTTP connection
    verbose_print(verbosity, "Connecting to => " + ip + ":" + port)
    sess = Session(ip, port)

    uri = "/hello?query1=value1&query2=value2&query3=value3"
    # GET hello response
    test_headers = {"Test-Header-1":"Test-Value-1", "Test-Header-2":"Test-Value-2"}
    verbose_print(verbosity, "Sending GET to URI : ", uri)
    verbose_print(verbosity, "Sending additional headers : ")
    for k, v in test_headers.iteritems():
        verbose_print(verbosity, "\t", k, ": ", v)
    sess.send_get(uri, test_headers)
    resp_hdrs = sess.read_resp_hdrs()
    resp_data = sess.read_resp_data()
    try:
        if resp_hdrs["Custom-Header-1"] != "Custom-Value-1":
            return False
        if resp_hdrs["Custom-Header-2"] != "Custom-Value-2":
            return False
    except:
        return False

    verbose_print(verbosity, "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv")
    verbose_print(verbosity, "Server response to GET /hello")
    verbose_print(verbosity, "Response Headers : ")
    for k, v in resp_hdrs.iteritems():
        verbose_print(verbosity, "\t", k, ": ", v)
    verbose_print(verbosity, "Response Data : " + resp_data)
    verbose_print(verbosity, "========================================\n")

    # Close HTTP connection
    sess.close()
    return (resp_data == "Hello World!")

def test_post_handler(ip, port, msg, verbosity = False):
    verbose_print(verbosity, "========  POST HANDLER TEST ============")
    # Establish HTTP connection
    verbose_print(verbosity, "Connecting to => " + ip + ":" + port)
    sess = Session(ip, port)

    # POST message to /echo and get back response
    sess.send_post("/echo", msg)
    resp_hdrs = sess.read_resp_hdrs()
    resp_data = sess.read_resp_data()
    verbose_print(verbosity, "Server response to POST /echo (" + msg + ")")
    verbose_print(verbosity, "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv")
    verbose_print(verbosity, resp_data)
    verbose_print(verbosity, "========================================\n")

    # Close HTTP connection
    sess.close()
    return (resp_data == msg)

def test_put_handler(ip, port, verbosity = False):
    verbose_print(verbosity, "========  PUT HANDLER TEST =============")
    # Establish HTTP connection
    verbose_print(verbosity, "Connecting to => " + ip + ":" + port)
    sess = Session(ip, port)

    # PUT message to /ctrl to disable /hello URI handler
    verbose_print(verbosity, "Disabling /hello handler")
    sess.send_put("/ctrl", "0")
    sess.read_resp_hdrs()
    if sess.content_len:
        sess.read_resp_data()

    sess.send_get("/hello")
    sess.read_resp_hdrs()
    resp_data1 = sess.read_resp_data()
    verbose_print(verbosity, "Response on GET /hello : " + resp_data1)

    # PUT message to /ctrl to enable /hello URI handler
    verbose_print(verbosity, "Enabling /hello handler")
    sess.send_put("/ctrl", "1")
    sess.read_resp_hdrs()
    if sess.content_len:
        sess.read_resp_data()

    sess.send_get("/hello")
    sess.read_resp_hdrs()
    resp_data2 = sess.read_resp_data()
    verbose_print(verbosity, "Response on GET /hello : " + resp_data2)

    # Close HTTP connection
    sess.close()
    return ((resp_data2 == "Hello World!") and (resp_data1 == "This URI doesn't exist"))

def test_custom_uri_query(ip, port, query, verbosity = False):
    verbose_print(verbosity, "========  GET HANDLER TEST =============")
    # Establish HTTP connection
    verbose_print(verbosity, "Connecting to => " + ip + ":" + port)
    sess = Session(ip, port)

    uri = "/hello?" + query
    # GET hello response
    verbose_print(verbosity, "Sending GET to URI : ", uri)
    sess.send_get(uri, {})
    resp_hdrs = sess.read_resp_hdrs()
    resp_data = sess.read_resp_data()

    verbose_print(verbosity, "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv")
    verbose_print(verbosity, "Server response to GET /hello")
    verbose_print(verbosity, "Response Data : " + resp_data)
    verbose_print(verbosity, "========================================\n")

    # Close HTTP connection
    sess.close()
    return (resp_data == "Hello World!")

if __name__ == '__main__':
    # Configure argument parser
    parser = argparse.ArgumentParser(description='Run HTTPd Test')
    parser.add_argument('IP'  , metavar='IP'  ,    type=str, help='Server IP')
    parser.add_argument('port', metavar='port',    type=str, help='Server port')
    parser.add_argument('msg',  metavar='message', type=str, help='Message to be sent to server')
    args = vars(parser.parse_args())

    # Get arguments
    ip   = args['IP']
    port = args['port']
    msg  = args['msg']

    test_get_handler (ip, port, True)
    test_post_handler(ip, port, msg, True)
    test_put_handler (ip, port, True)
