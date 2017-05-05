#!/usr/bin/python

from socket import *

server_name = 'localhost'
server_port =  30020
max_msg_len = 500

client_socket = socket(AF_INET, SOCK_STREAM)
client_socket.connect((server_name, server_port))
handle = raw_input('Input your handle: ')


message = raw_input(str(handle) +'> ')
client_socket.send(message)
response = client_socket.recv(max_msg_len)
print(str(response))
client_socket.close()