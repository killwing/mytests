#!/usr/bin/python
from twisted.internet.protocol import Protocol, ClientFactory, StreamControlProtocol
from twisted.internet import reactor
from twisted.python import log
import sys

class Echo(StreamControlProtocol):
    def msgReceived(self, data, sndrcvinfo):
        print '==== data recved: ', data
        self.transport.setSendingContext(stream=8)
        #self.transport.write("Hello server, I am the client again!\r\n")
        #self.transport.loseConnection()

    def connectionMade(self):
        print '==== connection made'
        #self.transport.setSendingContext(stream=5)
        self.transport.write("Hello server, I am the client!\r\n")

    def connectionLost(self, reason):
        print '==== connection lost'

    def notifAssocChangeReceived(self, state, error, ostreams, istreams, assoc_id):
        print '==== assoc change recved on assoc:', assoc_id

class EchoClientFactory(ClientFactory):

    protocol = Echo

    def startFactory(self):
        print '==== start factory'

    def stopFactory(self):
        print '==== stop factory'

    def startedConnecting(self, connector):
        print '==== Started to connect.'
    
    def clientConnectionLost(self, connector, reason):
        print '==== Lost connection.  Reason:', reason
    
    def clientConnectionFailed(self, connector, reason):
        print '==== Connection failed. Reason:', reason

log.startLogging(sys.stdout)
local = [('192.168.16.1', 8888)]
reactor.connectSCTP([('192.168.16.55', 2905)], EchoClientFactory(), bindAddrs=local)
reactor.run()

