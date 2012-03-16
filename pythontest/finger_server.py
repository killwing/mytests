#!/usr/bin/python
from twisted.internet import protocol, reactor
from twisted.protocols import basic

class FingerProtocol(basic.LineReceiver):
    def lineReceived(self, user):
        print('== '+user)
        self.transport.write("No such user\r\n")
        self.transport.loseConnection()

    def dataReceived(self, data):
        print('recv: '+data)

class FingerFactory(protocol.ServerFactory):
    protocol = FingerProtocol

reactor.listenTCP(7777, FingerFactory())
reactor.run()
