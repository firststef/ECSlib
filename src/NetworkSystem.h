#pragma once
#include "System.h"
#include "Utils.h"
#include "NetworkEvent.h"

#include <memory>

#ifdef WIN32
#include <thread>
#include <mutex>
#include <utility>
#elif __linux__
#include<cstdio>
#include<pthread.h>
#include<cstdlib>
#include<unistd.h>
#endif

struct ClientSocket;


using Packet = std::vector<char>;

class INetworkSystem : public ISystem
{
public:

	enum Type
	{
		SERVER,
		CLIENT
	} type;

	enum AccessType
	{
		WRITE_TYPE,
		READ_TYPE
	};

	//SERVER
	std::vector < std::shared_ptr<ClientSocket> > client_sockets;

	//CLIENT
	std::string server_address;
	int port;
	
	INetworkSystem(std::string system_name, std::vector<std::shared_ptr<ClientSocket>> client_sockets = {})
		: ISystem(std::move(system_name)), type(SERVER), client_sockets(std::move(client_sockets))
	{}

	INetworkSystem(std::string system_name, std::string server_address, int port)
		: ISystem(std::move(system_name)), type(CLIENT), server_address(server_address), port(port)
	{}

#ifdef WIN32
	std::shared_ptr<std::thread> nt = nullptr;
	std::mutex buffer_mutex;
	std::mutex signal_mutex;

	unsigned socket_ptr = 0;
#elif __linux__
	pthread_t nt;
	pthread_mutex_t buffer_mutex;
	pthread_mutex_t signal_mutex;

	int sd = 0;
#endif

	bool stop_thread = false;

	bool signal_access(AccessType type, bool value);

	std::vector<std::vector<Packet>> gather_packets();
	void send_packets(std::vector<std::vector<Packet>> packets);
	
	virtual void RunMainThread() = 0;

	void Initialize() override;

	void Destroy() override;
};

//The Network system, an EXAMPLE system for use as a model. Instantiated:
//	as a SERVER: receives client sockets and serves them continuously
//	as a CLIENT: connects to a server continuously
//NOTE: this system does not contain a socket from a server, it only receives its
//connections from somewhere else
//THEREFORE: the closing of the server socket must be done BEFORE this system
//calls Destroy()
class NetworkSystem : public INetworkSystem
{
public:

	NetworkSystem(std::vector<std::shared_ptr<ClientSocket>> client_sockets = {})
		: INetworkSystem("NetworkSystem",client_sockets)
	{
	}

	NetworkSystem(std::string server_address, int port)
		: INetworkSystem("NetworkSystem", server_address, port)
	{
		std::vector<char> conn_msg;
		std::string msg = "connect";
		conn_msg.insert(conn_msg.begin(), msg.begin(), msg.end() + 1);
		send_queue.push(conn_msg);
	}

	FixedQueue<Packet, 4> receive_queue;
	FixedQueue<Packet, 4> send_queue;

	void RunMainThread() override;

	void Execute() override;
	
	std::vector<Packet> send_queue_access(AccessType type, const Packet* packet);
	std::vector<Packet> receive_queue_access(AccessType type, const Packet* packet);

	void Receive(const NetworkEvent& event);
};