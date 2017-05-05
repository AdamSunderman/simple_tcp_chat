#!/usr/bin/python

from socket import *
import sys
import signal

def signal_handler(signal, frame):
	print "\nExiting..."
	sys.exit(0)

if __name__ == '__main__':

	signal.signal(signal.SIGINT, signal_handler)
	handle = 'Server> '
	server_port =  int(sys.argv[1])
	max_msg_len = 501
	quit = 0

	server_socket = socket(AF_INET, SOCK_STREAM)
	server_socket.bind(('', server_port))
	server_socket.listen(1)

	while 1:
		print 'Running...'
		connection_socket, address = server_socket.accept()
		sys.stdout.write("Connection to: {}".format(address))
		print '\n'
		while 1:
			in_message = connection_socket.recv(max_msg_len)
			if in_message == "":
				print 'Connection closed\n'
				break
			sys.stdout.write(str(in_message))
			out_message = raw_input(handle)
			if out_message == '\quit':
				break
			else:
				connection_socket.send(str(handle + out_message).rstrip('\n'))
	connection_socket.close()
