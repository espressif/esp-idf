import socket

TCP_IP = "192.168.7.1"
BUFFER_SIZE = 1024
MESSAGE = "Hello, World!"

print("create socket 1")
s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print("try to connect")
s1.connect((TCP_IP, 1234))
print("connected...")
s1.send("Hello Port: 1234".encode())
data = s1.recv(BUFFER_SIZE)
print("received data: ", data.decode())

print("create socket 2")
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print("try to connect")
s2.connect((TCP_IP, 3333))
print("connected...")
s2.send("Hello Port: 3333".encode())
data = s2.recv(BUFFER_SIZE)
print("received data: ", data.decode())

s1.close()
s2.close()
