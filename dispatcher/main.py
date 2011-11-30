import socket
import os
import sys
import getpass
import MonitorDispatcher_pb2

STREAMER_ENTRY_PORT = 5577
MULITCAST_ADDRESS = "224.0.0.127"
DAEMON_MULTICAST_LISTEN_PORT = 9955
SERVER_MULTICAST_LISTEN_PORT = 9956
#MONITORS_PATH = os.path.expanduser("~/WallMon/modules/bin/")
MONITORS_PATH = "/home/arild/WallMon/modules/bin/"

def multicast_monitor_message(msg):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
    sock.sendto(msg, (MULITCAST_ADDRESS, SERVER_MULTICAST_LISTEN_PORT))
    sock.sendto(msg, (MULITCAST_ADDRESS, DAEMON_MULTICAST_LISTEN_PORT))
    sock.close()

def listdir_fullpath(d):
    return [os.path.join(d, f) for f in os.listdir(d)]

if __name__ == '__main__':
    
    msg = MonitorDispatcher_pb2.MonitorDispatcherMessage()
    if len(sys.argv) == 0 or sys.argv[1].startswith('init'):
        msg.type = msg.INIT
        for path in listdir_fullpath(MONITORS_PATH):
            msg.filePath = path
            print 'Init path: ' + path
            multicast_monitor_message(msg.SerializeToString())
    
    elif sys.argv[1].startswith('stop'):
        msg.type = msg.STOP
        for path in listdir_fullpath(MONITORS_PATH):
            msg.filePath = path
    
    elif sys.argv[1].startswith('event'):
        msg.type = msg.EVENT
        msg.eventData = 'Hello'
    
    else:
        print 'Unknown command'
        
