#!/usr/bin/python
import socket
import select
import time
import sys
import os
import sctp
from sctp import *
import random

port = 7777
sock = sctpsocket_udp(socket.AF_INET)

sock.initparams.num_ostreams = 15
sock.initparams.max_instreams = 20

sock.events.clear()
sock.events.data_io = True
sock.events.association = True
sock.events.shutdown = True

sock.bindx([('192.168.1.11', port), ('192.168.1.12', port), ('192.168.1.13', port)])

sock.listen(5)


while 1:
    fromaddr, flags, msgret, notif = sock.sctp_recv(20)

    if flags & FLAG_NOTIFICATION:
        if notif.__class__ == assoc_change:
            if notif.state == assoc_change.state_COMM_UP:
                print('assoc change - comm up on assoc: ' + str(notif.assoc_id))
            elif notif.state == assoc_change.state_COMM_LOST:
                print('assoc change - comm lost on assoc: ' + str(notif.assoc_id))
        elif notif.__class__ == shutdown_event:
            print('shutdown on assoc: '+ str(notif.assoc_id))
    elif msgret == '':
        print('read EOF')
        continue
    else:
        print 'send: ', msgret
        if flags & FLAG_EOR:
            print 'EOR///'

        sock.sctp_send(msgret, to = fromaddr, stream = 3)


sock.close()

