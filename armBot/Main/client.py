import socket
import sys
import random
from ctypes import *


""" This class defines a C-like struct """
class Payload(Structure):
    _fields_ = [("id", c_uint32),
                ("counter", c_uint32),
                ("temp", c_float)]


def main():
	server_addr = ('localhost', 2300)
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect(server_addr)
		print("Connected to {:s}".format(repr(server_addr)))
		for i in range(0, 10):
			str = input()
			s.send(str.encode())
	except AttributeError as ae:
		print("Error creating the socket: {}".format(ae))
	except socket.error as se:
		print("Exception on socket: {}".format(se))
	finally:
		print("Closing socket")
		s.close()


if __name__ == "__main__":
    main()
