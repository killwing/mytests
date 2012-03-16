#!/usr/bin/python
from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
from twisted.protocols.sip import Proxy
from twisted.python import log, util
import sys

log.startLogging(sys.stdout)
log.msg('start...')

p = Proxy()
p.debug = True
reactor.listenUDP(5060, p)
reactor.run()


