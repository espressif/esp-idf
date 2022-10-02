# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import socket

import cv2
import numpy as np

frame_count = 0
stream = bytearray()

print('Connecting to server...')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect(('esp-cam.local', 2222))

    print('Receiving data ')
    while True:
        data = sock.recv(4096)
        if not data:
            break
        stream += data
        print('.', end='', flush=True)

        a = stream.find(b'\xff\xd8')
        b = stream.find(b'\xff\xd9', a)

        if a != -1 and b != -1:
            jpg = stream[a:b + 2]
            stream = stream[b + 2:]
            buffer = np.frombuffer(jpg, dtype=np.uint8)
            image = cv2.imdecode(buffer, cv2.IMREAD_COLOR)
            cv2.imshow('Stream', image)
            if cv2.waitKey(10) == 27:
                exit(0)
            frame_count += 1

print('\nFrames received ', frame_count)
