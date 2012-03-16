#!/usr/bin/python
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

INVITE = '''INVITE sip:100@127.0.0.1:5060;transport=UDP;user=phone SIP/2.0\r
Via: SIP/2.0/UDP 127.0.0.1:7777\r
From: <sip:+867859471@127.0.0.1:7777;transport=UDP;user=phone>\r
To: <sip:100@127.0.0.1:5060;transport=UDP;user=phone>\r
Call-ID: 2963313058@north.east.isi.edu\r
CSeq: 1 INVITE\r
Subject: SIP will be discussed, too\r
\r
'''

class Callee(DatagramProtocol):

    def startProtocol(self):
        self.transport.write(INVITE, ('127.0.0.1', 5060)) 
        
    def datagramReceived(self, data, (host, port)):
        print "received %r from %s:%d" % (data, host, port)

    # Possibly invoked if there is no server listening on the
    # address to which we are sending.
    def connectionRefused(self):
        print "No one listening"

# 0 means any port, we don't care in this case
reactor.listenUDP(7777, Callee())
reactor.run()
