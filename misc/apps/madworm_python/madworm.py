
import getopt, sys, os, time
import resource

class CpuMonitor:
    def __init__(self):
        self.last_stime = 0
        self.last_utime = 0 
        self.utime = 0
        self.stime = 0
        self.timestamp = 0
        self.last_timestamp = 0
        self._update()
        
    def get_cpu_load(self):
        diff_timestamp = self.timestamp - self.last_timestamp
        diff_cpu = (self.utime + self.stime) - (self.last_utime + self.last_stime)
        self._update()
        return (diff_cpu / diff_timestamp) * 100
        
    def _update(self):
        o = resource.getrusage(resource.RUSAGE_SELF)
        self.last_utime = self.utime
        self.last_stime = self.stime
        self.utime = o.ru_utime
        self.stime = o.ru_stime
        self.last_timestamp = self.timestamp
        self.timestamp = time.time()

def busy_wait(seconds):
    timestamp = time.time()
    tmp = 0
    while time.time() - timestamp < seconds:
        for i in range(9999):
            tmp += i * 4.3
        
def run_forever(cpu_load):
    m = CpuMonitor()
    spin_time = cpu_load / 100
    sleep_time = 1 - spin_time
    f = 0.01
    while True:
        busy_wait(spin_time)
        if m.get_cpu_load() > cpu_load - 0.5:
            # Slow down
            sleep_time += f
            spin_time -= f
        else:
            # Speed up
            sleep_time -= f
            spin_time += f
        if spin_time < 0: spin_time = 0
        if sleep_time < 0: sleep_time = 0
        time.sleep(sleep_time)
            
if __name__ == '__main__':
    num_instances = 1
    cpu_usage = 1
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'n:c:')
    except getopt.GetoptError, err:
        print str(err) # will print something like "option -a not recognized"
        sys.exit(2)
    for o, a in opts:
        if o == '-n':
            num_instances = int(a)
        elif o in ('-c'):
            cpu_usage = int(a)
        else:
            assert False, 'unhandled option'
            
    for x in range(num_instances - 1):
        if os.fork() == 0:
            # Child must not spawn child
            break
    
    run_forever(cpu_usage)
