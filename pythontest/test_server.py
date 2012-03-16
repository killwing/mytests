#!/usr/bin/python
# -*- coding: utf-8 -*-

# SCTP bindings for Python - auto-test routine "server"
# 
# Elvis Pfützenreuter (elvis.pfutzenreuter@{gmail.com,indt.org.br})
# Copyright (c) 2005 Instituto Nokia de Tecnologia


import socket
import select
import time
import sys
import os
import sctp
from sctp import *
import random

baseport = 0x1415

tcp = sctpsocket_tcp(socket.AF_INET)
udp = sctpsocket_udp(socket.AF_INET)
tcp2 = sctpsocket_tcp(socket.AF_INET)
udp2 = sctpsocket_udp(socket.AF_INET)
tcp3 = sctpsocket_tcp(socket.AF_INET)
udp3 = sctpsocket_udp(socket.AF_INET)

socks = [tcp, udp, tcp2, udp2, tcp3, udp3]

for s in socks:
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.bindx([("", baseport)])
	baseport += 1
	s.initparams.max_instreams = 3
	s.initparams.num_ostreams = 3
	s.events.clear()
	s.events.data_io = 1
	s.listen(5)

udp3.events.association = 1

while 1:
	print "SELECTING --------------------------" 
	rd, wr, ex = select.select(socks, [], [], 10000)
	for sk in rd:
		print "READING ---------------------" 
		if sk is tcp:
			client, addr = sk.accept()
			paddrs = client.getpaddrs()
			print "tcp paddrs: ", paddrs
			laddrs = client.getladdrs()
			print "tcp laddrs: ", laddrs
			print "tcp-style client", addr

			x = sk.get_rtoinfo()
			if not x.min:
				raise "get_rtoinfo did not work"
			print "rtoinfo old:", x.__dict__
			x.min = 1001
			sk.set_rtoinfo(x)
			x = sk.get_rtoinfo()
			print "rtoinfo new:", x.__dict__
			if x.min != 1001:
				raise "set_rtoinfo() did not work for TCP socket"

			x = sk.get_assocparams()
			if not x.cookie_life:
				raise "get_assocparams did not work"
			x.cookie_life = 65000
			print "assocparams old:", x.__dict__
			sk.set_assocparams(x)
			x = sk.get_assocparams()
			print "assocparams new", x.__dict__
			if x.cookie_life != 65000:
				raise "set_assocparams() did not work for TCP socket"

			x = sk.get_paddrparams(0, ("",0))
			if not x.hbinterval:
			  	raise "get_paddrparams did not work"
			print "paddrparams:", x.__dict__
			x.hbinterval = 30001
			sk.set_paddrparams(x)
			x = sk.get_paddrparams(0, ("",0))
			if x.hbinterval != 30001:
			 	raise "set_paddrparms() did not work for TCP socket"

			question = client.recv(1000)
			client.send("#%s" % question)
			time.sleep(2)
			client.close()
		elif sk is tcp2 or sk is tcp3:
			client, addr = sk.accept()
			print "tcp paddrs: ", client.getpaddrs()
			print "tcp laddrs: ", client.getladdrs()
			print "tcp-style client", addr
			addr, flags, question, notif = client.sctp_recv(1000)
			client.sctp_send("#%s" % question, flags=MSG_UNORDERED, stream=2)
			time.sleep(2)
			client.close()
		elif sk is udp:
			question, addr = sk.recvfrom(1000)
			print "udp-style client %s, sendto(), msg '%s'" % (addr, question)

			x = sk.get_rtoinfo(0)
			print "rtoinfo old:", x.__dict__
			x.min = 1006
			sk.set_rtoinfo(x)
			x = sk.get_rtoinfo(0)
			print "rtoinfo new:", x.__dict__
			if x.min != 1006:
			 	raise "set_rtoinfo(zero) did not work for UDP socket"

			time.sleep(2)
			sk.sendto("#%s" % question, 0, addr)
			try:
				sk.sctp_send("", flags=MSG_EOF, to=addr)
			except IOError:
				print "Normal IOError problem, since association was already closed"
		elif sk is udp2:
			addr, flags, question, notif = sk.sctp_recv(1000)
			print "udp-style client, sctp_send()", addr
			time.sleep(2)
			sk.sctp_send("#%s" % question, to=addr, flags=MSG_UNORDERED, stream=2)
			sk.sctp_send("", to=addr, flags=MSG_EOF)
		elif sk is udp3:
			addr, flags, question, notif = sk.sctp_recv(1000)
			print ">>>>>>>>>>>>>>>>> udp-style client, peeloff()", addr
			if notif.__class__ != assoc_change:
				raise "Unexpected notification event in udp3"
			if notif.state == assoc_change.state_COMM_UP:

				paddrs = sk.getpaddrs(notif.assoc_id)
				laddrs = sk.getladdrs(notif.assoc_id)
				print "assoc %d udp paddrs: " % notif.assoc_id, paddrs
				print "udp laddrs: ", laddrs

				x = sk.get_status(notif.assoc_id)
				if not x.rwnd:
					raise "get_status() did not work for UDP socket"
				else:
					print " rwnd of UDP socket: %d" % x.rwnd

				x = sk.get_paddrinfo(notif.assoc_id, paddrs[0])
				if not x.cwnd:
					raise "get_paddrinfo() did not work for UDP socket"
				else:
					print "cwnd of UDP socket: %d" % x.cwnd

				sk.set_primary(notif.assoc_id, paddrs[0])
				try:
					sk.set_peer_primary(notif.assoc_id, laddrs[0])
				except IOError:
					raise "set_print_primary failed. Perhaps you have to enable /proc/sys/net/sctp/addip_enable"

				x = sk.get_rtoinfo(notif.assoc_id)
				print "rtoinfo old:", x.__dict__
				x.min = 1009
				sk.set_rtoinfo(x)
				x = sk.get_rtoinfo(notif.assoc_id)
				print "rtoinfo new:", x.__dict__
				if x.min != 1009:
					raise "set_rtoinfo() did not work for UDP socket"

				x = sk.get_assocparams(notif.assoc_id)
				x.cookie_life = 65000
				print "assocparams old:", x.__dict__
				sk.set_assocparams(x)
				x = sk.get_assocparams(notif.assoc_id)
				print "assocparams new", x.__dict__
				if x.cookie_life != 65000:
					raise "set_assocparams() did not work for UDP socket"

				x = sk.get_paddrparams(notif.assoc_id, paddrs[0])
				print "paddrparams:", x.__dict__
				x.hbinterval = 30001
				sk.set_paddrparams(x)
				x = sk.get_paddrparams(notif.assoc_id, paddrs[0])
				if x.hbinterval != 30001:
					raise "set_paddrparms() did not work for UDP socket"

				client = sk.peeloff(notif.assoc_id)
				fromaddr, flags, question, notif = client.sctp_recv(1000)
				time.sleep(2)
				client.sctp_send("#%s" % question, stream=2)
				client.close()

