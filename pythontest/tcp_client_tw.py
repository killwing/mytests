#!/usr/bin/python
from twisted.internet.protocol import Protocol, ClientFactory
from twisted.internet import reactor
from twisted.protocols import basic

class Echo(basic.LineReceiver):
    def lineReceived(self, data):
        print '==== line recved: ', data

    #def dataReceived(self, data):
    #    print '==== data recved: ', data

    def connectionMade(self):
        print '==== connection made'
        self.transport.write("Hello server, I am the client!\r\n")

    def connectionLost(self, reason):
        print '==== connection lost'

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

reactor.connectTCP('localhost', 8888, EchoClientFactory())
reactor.run()

