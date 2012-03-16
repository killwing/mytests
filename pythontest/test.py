#!/usr/bin/python
from twisted.internet import reactor, defer, threads
from twisted.python import failure
from time import sleep

def printData(d):
    print '===', d

def printData2(d):
    print '+++', d

def printError(failure):
    failure.trap(ValueError)
    print '$$$', str(failure)

class Getter:
    _i = 0

    def __init__(self, i):
        self._i = i

    def gotResults(self, x):
        """
        The Deferred mechanism provides a mechanism to signal error
        conditions.  In this case, odd numbers are bad.

        This function demonstrates a more complex way of starting
        the callback chain by checking for expected results and
        choosing whether to fire the callback or errback chain
        """
        print 'getting...'
        # long time to get results
        sleep(3)

        if x % 2 == 0:
            self.d.callback(x*3)
        else:
            self.d.errback(ValueError("You used an odd number!"))

    def _toHTML(self, r):
        """
        This function converts r to HTML.

        It is added to the callback chain by getDummyData in
        order to demonstrate how a callback passes its own result
        to the next callback
        """
        return "Result: %d" % self._i 

    def getDummyData(self, x):
        """
        The Deferred mechanism allows for chained callbacks.
        In this example, the output of gotResults is first
        passed through _toHTML on its way to printData.

        Again this function is a dummy, simulating a delayed result
        using callLater, rather than using a real asynchronous
        setup.
        """
        self.d = defer.Deferred()
        #self.d = threads.deferToThread(self.gotResults, x)
        reactor.callLater(0, self.gotResults, x)
        print '*******************'
        self.d.addCallback(self._toHTML)
        self.d.addCallback(printData)
        return self.d



# this series of callbacks and errbacks will print "Result: 12"
g = Getter(99)
d = g.getDummyData(4)
d.addErrback(printError)

reactor.callLater(60, reactor.stop); 
reactor.run()
