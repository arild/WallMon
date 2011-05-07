/**
 * @file   Dispatcher.cpp
 * @Author Arild Nilsen
 * @date   January, 2011
 *
 * Loads and starts user-defined collectors based on incoming requests.
 */

#include <glog/logging.h>
#include "Config.h"
#include "ClassLoader.h"
#include "stubs/MonitorDispatcher.pb.h"
#include "MonitorDispatcher.h"

#define RECV_BUFFER_SIZE	1024 * 10
#define TERMINATION_TOKEN	"9797AB"

MonitorDispatcher::MonitorDispatcher(IMonitorManager &manager, int multicastListenPort)
{
	_monitorManager = &manager;
	_multicastListenPort = multicastListenPort;
	try {
		_socket = new UDPSocket(multicastListenPort);
	} catch (SocketException &e) {
		LOG(FATAL) << "failed creating udp multicast socket: " << e.what();
	}
	try {
		_socket->joinGroup(MULITCAST_ADDRESS);
	} catch (SocketException &e) {
		LOG(FATAL) << "failed joining multicast group: " << e.what();
	}
}

MonitorDispatcher::~MonitorDispatcher()
{
}

void MonitorDispatcher::Start()
{
	_running = true;
	_thread = boost::thread(&MonitorDispatcher::_ListenForever, this);
}

void MonitorDispatcher::Stop()
{
	LOG(INFO) << "stopping Dispatcher...";
	_running = false;
	_socket->sendTo("", 0, "localhost", _multicastListenPort);
	_thread.join();
	LOG(INFO) << "Dispatcher stopped";
}

void MonitorDispatcher::_ListenForever()
{
	char recvBuf[RECV_BUFFER_SIZE];
	string sourceAddress; // Address of datagram source
	unsigned short sourcePort; // Port of datagram source
	IBase *monitor = NULL;
	Wallmon::MonitorDispatcherMessage msg;
	LOG(INFO) << "Dispatcher started and serving requests";
	while (true) {
		try {
			int bytesRcvd = _socket->recvFrom(recvBuf, RECV_BUFFER_SIZE, sourceAddress, sourcePort);
			if (_running == false)
				break;

			if (msg.ParseFromArray(recvBuf, bytesRcvd) == false) {
				LOG(ERROR) << "failed parsing monitor message";
				continue;
			}

			if (msg.type() == msg.INIT) {
				try {
					ClassLoader loader(msg.filepath());
					if (_multicastListenPort == SERVER_MULTICAST_LISTEN_PORT)
						monitor = loader.LoadAndInstantiateHandler();
					else
						monitor = loader.LoadAndInstantiateCollector();
					LOG(INFO) << "user-defined collector successfully loaded";
				} catch (exception &e) {
					LOG(ERROR) << "failed loading user-defined collector: " << e.what();
					continue;
				}
				Context *ctx = new Context();
				try {
					// Adhere to interface contract
					monitor->OnInit(ctx);
					if (_multicastListenPort == DAEMON_MULTICAST_LISTEN_PORT && ctx->servers->size() == 0)
						// Server not defined, use source
						ctx->AddServer(sourceAddress);
				} catch (exception &e) {
					LOG(ERROR) << "user-defined OnStart() failed: " << e.what();
				}
				_monitorManager->Register(*monitor, *ctx);
				_monitors[msg.filepath()] = monitor;
				LOG(INFO) << "monitor successfully registered";
			} else if (msg.type() == msg.STOP) {
				monitor = _monitors[msg.filepath()];
				_monitorManager->Remove(*monitor);
				monitor->OnStop();
				_monitors.erase(msg.filepath());
			} else if (msg.type() == msg.EVENT) {
				LOG(INFO) << "Event: " << msg.eventdata();
			}
			else {}
		} catch (exception &e) {
			LOG(ERROR) << "dispatcher failed: " << e.what();
			break;
		}
	}
	_socket->leaveGroup(MULITCAST_ADDRESS);
	_socket->disconnect();
	delete _socket; // calls close() on socket
}
