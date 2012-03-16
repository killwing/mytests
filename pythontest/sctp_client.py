#!/usr/bin/python
import socket
import sctpstack
import time


port = 8888
#laddrs = [('192.168.1.21', port), ('192.168.1.22', port), ('192.168.1.23', port)]
saddr = ('150.236.226.167', 7777)

sock = sctpstack.sctpsocket_tcp(socket.AF_INET)

sock.bindx([('', 8888)])

ret = sock.connect_ex(saddr)
if ret != 0:
    exit()

paddrs = sock.getpaddrs()
laddrs = sock.getladdrs()
print "peer addrs: ", paddrs
print "local addrs: ", laddrs

lport =  sock.getsockname()[1]
try:
    sock.set_primary(0, saddr)
    sock.set_peer_primary(0, (laddrs[1][0],lport))
except IOError:
    raise "set_primary or set_peer_primary failed: /proc/sys/net/sctp/addip_enable is probably 0" 


while 1:

    try:
        str = raw_input()
    except EOFError:
        #sock.sctp_send('', to =, flags = MSG_EOF) // for udp style only
        break

    sock.sctp_send(str);

    fromaddr, flags, msgret, notif = sock.sctp_recv(1000)

    if flags & sctpstack.FLAG_NOTIFICATION:
        raise "We did not subscribe to receive notifications!"
    elif msgret == '':
        break
    else:
        print(msgret)


sock.close()



