#!/usr/bin/python
from twisted.internet.protocol import Protocol, Factory
from twisted.internet import reactor

class Echo(Protocol):

    def connectionMade(self):
        print '==== connection made'

    def connectionLost(self, reason):
        print '==== connection lost'

    def dataReceived(self, data):
        print '==== data recved: ', data
        self.transport.write(data)

# Next lines are magic:
factory = Factory()
factory.protocol = Echo

# 8007 is the port you want to run under. Choose something >1024
reactor.listenTCP(8888, factory)
reactor.run()
