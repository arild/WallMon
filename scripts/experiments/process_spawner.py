
import sys, os, time
from commands import getoutput
import daemonize

def run_child():
    while True:
        time.sleep(5)
                
if __name__ == '__main__':
    
    if len(sys.argv) < 3:
        print 'Usage: <num processes per sec><max num processes>'  
        sys.exit()
    if len(sys.argv) == 3:
        daemonize.daemonize()
    num_processes_per_sec = int(sys.argv[1])
    max_num_processes = int(sys.argv[2])
    
    while True:
        num_processes = int(getoutput('ps ax | wc -l'))
        print num_processes
        if num_processes < max_num_processes and os.fork() == 0:
            # Child must not spawn child
            run_child()
        time.sleep(1 / float(num_processes_per_sec))
        
    