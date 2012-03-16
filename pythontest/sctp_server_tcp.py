#!/usr/bin/python
import socket
import select
import time
import sys
import os
import sctpstack
from sctpstack import *
import random

port = 7777
sock = sctpsocket_tcp(socket.AF_INET)

sock.bindx([('', port)])
#sock.bindx([('192.168.1.11', port), ('192.168.1.12', port), ('192.168.1.13', port)])


sock.listen(5)


while 1:
    client, addr = sock.accept()

    client.events.clear()
    client.events.set_shutdown(1)
    client.events.set_association(1)

    while 1:
        fromaddr, flags, msgret, notif = client.sctp_recv(1000)

        if flags & FLAG_NOTIFICATION:
            #raise "We did not subscribe to receive notifications!"
            if notif.__class__ == shutdown_event:
                print '== shutdown on assoc: ', notif.assoc_id
        elif msgret == '':
            break
        else:
            if notif.__class__ == sndrcvinfo:
                print '== on stream:', notif.stream
            client.sctp_send(msgret)

    client.close()

sock.close()

