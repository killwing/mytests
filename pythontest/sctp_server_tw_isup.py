#!/usr/bin/python
from twisted.internet.protocol import Protocol, Factory, StreamControlProtocol
from twisted.internet import reactor
from twisted.python import log
import sys

class Isup(StreamControlProtocol):

    def connectionMade(self):
        print '==== connection made'

    def connectionLost(self, reason):
        print '==== connection lost'

    def msgReceived(self, data, sndrcvinfo):
        print '==== data recved: %s on stream %d' % (data, sndrcvinfo.stream)
        self.transport.write(data)

    def notifShutdownEventReceived(self, assoc_id):
        print '==== shutdown recved on assoc: ', assoc_id

    def notifAssocChangeReceived(self, state, error, ostreams, istreams, assoc_id):
        print '==== assoc change recved on assoc:', assoc_id

    def notifPaddrChangeReceived(self, addr, state, error, assoc_id):
        print '==== paddr change'

    def notifRemoteErrorReceived(self, error, assoc_id, data):
        print '==== remote error'


log.startLogging(sys.stdout)
factory = Factory()
factory.protocol = Isup

local = [('192.168.16.55', 2905)]
reactor.listenSCTP(local, factory)
reactor.run()
