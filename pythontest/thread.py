#!/usr/bin/python

import time  
import thread  
def timer(no,interval): 
    while True:   
        print 'Thread :(%d) Time:%s'%(no,time.ctime())   
        time.sleep(interval)   

def test(): 
    thread.start_new_thread(timer,(1,1))     
    thread.start_new_thread(timer,(2,3))   

if __name__=='__main__':   
    test()  
    time.sleep(10)   
    print('exit!!')
