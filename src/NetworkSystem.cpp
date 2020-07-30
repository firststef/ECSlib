#include "NetworkSystem.h"
#include <iostream>
#include "NetworkEvent.h"
#include "EventManager.h"
#include "External.h"
#ifdef WIN32
#include <WS2tcpip.h>
#elif __linux__
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

struct ClientSocket
{
#ifdef WIN32
	sockaddr_in address;
	int clientSize;
	SOCKET clientSocket;
#elif __linux__
    int cl;
#endif
};

bool INetworkSystem::signal_access(AccessType type, bool value)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(signal_mutex);
#else
	pthread_mutex_lock(&signal_mutex);
#endif

	if (type == WRITE_TYPE)
	{
		stop_thread = value;
	}

#ifdef __linux__
	pthread_mutex_unlock(&signal_mutex);
#endif

	return stop_thread;
}

void* main_thread_f_wrapper(void* ptr)
{
    std::bind(&INetworkSystem::RunMainThread, (INetworkSystem*)ptr)();
    return nullptr;
}

void INetworkSystem::Initialize()
{
	if (type == CLIENT) {
#ifdef WIN32
		// Initialize WinSock
		WSAData data;
		WORD ver = MAKEWORD(2, 2);
		int wsResult = WSAStartup(ver, &data);
		if (wsResult != 0)
		{
			std::cout << "Can't start Winsock, Err #" << wsResult << std::endl;
			signal_access(WRITE_TYPE, true);
			return;
		}

		// Create socket
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			std::cout << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
			WSACleanup();
			signal_access(WRITE_TYPE, true);
			return;
		}

		socket_ptr = sock;

		// Fill in a hint structure
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, server_address.c_str(), &hint.sin_addr);

		// Connect to server
		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
		if (connResult == SOCKET_ERROR)
		{
			std::cout << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			signal_access(WRITE_TYPE, true);
			return;
		}

		u_long iMode = 1;
		auto iResult = ioctlsocket(sock, FIONBIO, &iMode);
		if (iResult != NO_ERROR)
			printf("ioctlsocket failed with error: %ld\n", iResult);

#elif __linux__
		sockaddr_in server;

		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("Error on socket().\n");
			signal_access(WRITE_TYPE, true);
			return;
		}

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(server_address.c_str());
		server.sin_port = htons(port);

		if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
		{
			perror("Error on connect().\n");
			signal_access(WRITE_TYPE, true);
			return;
		}

		int flags = fcntl(sd, F_GETFL);
		fcntl(sd, F_SETFL, flags | O_NONBLOCK);
#endif
	}

#ifdef WIN32
	nt = std::make_shared<std::thread>(&INetworkSystem::RunMainThread, this);
#elif __linux__
	if (pthread_mutex_init(&buffer_mutex, NULL) != 0 || pthread_mutex_init(&signal_mutex, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		signal_access(WRITE_TYPE, true);
		return;
	}
	
    int err = pthread_create(&nt, NULL, &main_thread_f_wrapper, (void*)this);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
#endif
    std::cout << "Thread started" << std::endl;
}

std::vector<std::vector<Packet>> INetworkSystem::gather_packets()
{
    std::vector<std::vector<Packet>> packets;
    if (type == SERVER) {
		int got_one;

		do {
			got_one = false;

			for (auto &client : client_sockets) {
				char buffer[4096];

				int bytesReceived;
				std::vector<Packet> packets_socket;
				packets.push_back(packets_socket);
#ifdef WIN32
				ZeroMemory(buffer, 4096);

				// Wait for client to send data
				bytesReceived = recv(client->clientSocket, buffer, 4096, 0);
				if (bytesReceived <= 0 )
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK) {
						std::cout << "Error in recv(). Quitting" << std::endl;
						signal_access(WRITE_TYPE, true);
						return packets;
					}
					else
					{
						continue;
					}
				}

#elif __linux__
				bytesReceived = read(client->cl, buffer, 4096);

				if (bytesReceived <= 0) {
					if (errno != EWOULDBLOCK){
						perror("Error in read(). Quitting.\n");
						signal_access(WRITE_TYPE, true);
						return packets;
					}
					else {
						continue;
					}
				}

#endif
				if (signal_access(READ_TYPE, false))
					break;

				got_one = true;

				if (bytesReceived > 0) {
					std::vector<char> packet;
					packet.insert(packet.begin(), buffer, buffer + bytesReceived);
					packets.back().push_back(packet);
				}
			}
		} while(got_one == true);
    }
    else {
        char buffer[4096];

        int bytesReceived;
#ifdef WIN32
		ZeroMemory(buffer, 4096);
		bytesReceived = recv(socket_ptr, buffer, 4096, 0);
		if(bytesReceived <= 0)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				std::cout << "Error on recv(). Quitting " << WSAGetLastError() << "\n";
				signal_access(WRITE_TYPE, true);
				return packets;
			}
		}
    	
#elif __linux__
        bytesReceived = read(sd, buffer, 4096);
        if (bytesReceived <= 0) {
			if (errno != EWOULDBLOCK){
				perror("Error in read(). Quitting.\n");
				signal_access(WRITE_TYPE, true);
				return packets;
			}
        }
#endif
		std::vector<char> packet;
    	if (bytesReceived > 0)
			packet.insert(packet.begin(), buffer, buffer + bytesReceived);
		packets.push_back({ packet });
    }

	return packets;
}

void INetworkSystem::send_packets(std::vector<std::vector<Packet>> packets)
{
    if (type == SERVER) {
        if (client_sockets.size() != packets.size()) {
            packets.resize(client_sockets.size());
        }

        for (unsigned int i = 0; i < client_sockets.size(); ++i) {
			for (auto& packet : packets[i]) {
				if (packet.empty())
					continue;
#ifdef WIN32
				if (send(client_sockets[i]->clientSocket, &packet[0], packet.size(), 0) <= 0)
				{
					printf("[Server] Error on send\n");
					signal_access(WRITE_TYPE, true);
				}
#elif __linux__
				if (write(client_sockets[i]->cl, &packet[0], packet.size()) <= 0) {
					perror("Error in write(). Quitting.\n");
					signal_access(WRITE_TYPE, true);
				}
				printf("[Server] Sent message\n");
#endif
			}
        }
    }
    else {
		for (auto& packet : packets[0]){
			if (packet.empty())
				continue;
#ifdef WIN32
			int sendResult = send(socket_ptr, &packet[0], packet.size(), 0);
			if (sendResult <= 0)
			{
				std::cout << "Error on send(). Quitting\n" << WSAGetLastError() << "\n";
				signal_access(WRITE_TYPE, true);
			}
#elif __linux__
			if (write(sd, &packet[0], packet.size()) <= 0) {
				perror("Error in write(). Quitting.\n");
				signal_access(WRITE_TYPE, true);
			}
			printf("[Client] Sent message\n");
#endif
		}
    }
}

//NOTE: this must be closed after the listening socket is closed
void INetworkSystem::Destroy()
{
	signal_access(WRITE_TYPE, true);
	if (type == SERVER) {
#ifdef WIN32
		for (auto &c : client_sockets) {
			closesocket(c->clientSocket);
		}
#elif __linux__
		for (auto &c : client_sockets) {
			shutdown(c->cl, 2);
			close(c->cl);
			c->cl = -1;
		}
#endif
	}
	else {
#ifdef WIN32
		closesocket(socket_ptr);
#elif __linux__
		close(sd);
#endif
	}

#ifdef WIN32
	nt->join();
#elif __linux__
	pthread_join(nt, NULL);
	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&signal_mutex);
#endif
}

void NetworkSystem::RunMainThread()
{
    std::cout << "Running Main Thread" << std::endl;
	if (type == SERVER) {
		while(true)
		{
			if (signal_access(READ_TYPE, false))
				break;

			auto packets = gather_packets();

            if (signal_access(READ_TYPE, false))
                break;

			Packet new_packet;
			for (auto& ps : packets)
			{
			    for (auto& p : ps) {
                    new_packet.insert(new_packet.end(), p.begin(), p.end());
                }
			}
			receive_queue_access(WRITE_TYPE, &new_packet);

			std::vector<std::vector<Packet>> new_packets;
			for (auto& client : client_sockets) {
				Packet pack;
				for (auto& ps : packets)
				{
				    for (auto& packet : ps) {
                        pack.insert(pack.end(), packet.begin(), packet.end());
                    }
				}
				new_packets.push_back({ pack });
			}

			send_packets(new_packets);
		}
	}
	else
	{
	    while(true) {

            if (signal_access(READ_TYPE, false))
                break;

			auto send = send_queue_access(READ_TYPE, nullptr);
			send_packets({ send });

			SleepFunc(10);

            auto p = gather_packets();

            if (signal_access(READ_TYPE, false))
                break;

	    	for(auto& pack : p[0])
	    	{
				receive_queue_access(WRITE_TYPE, &pack);
	    	}
        }
	}
}

void NetworkSystem::Execute()
{
	auto packets = receive_queue_access(READ_TYPE, nullptr);
	if (!packets.empty())
		eventManager->Notify<NetworkEvent>(NetworkEvent::RECEIVE, packets);
}

std::vector<Packet> NetworkSystem::send_queue_access(AccessType type, const Packet* packet)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(buffer_mutex);
#else
	pthread_mutex_lock(&buffer_mutex);
#endif

	std::vector<Packet> packets;
	const Packet& data = *packet;
	
	if (type == WRITE_TYPE)
	{	
		//if (send_queue.is_full()){std::cout << "Warning: Send queue full, dropping data\n";}
		send_queue.push(data);
	}
	else
	{
		while(! send_queue.empty())
		{
			packets.push_back(send_queue.front());
			send_queue.pop();
		}
	}

#ifdef __linux__
	pthread_mutex_unlock(&buffer_mutex);
#endif

	return packets;
}

std::vector<Packet> NetworkSystem::receive_queue_access(AccessType type, const Packet* packet)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(buffer_mutex);
#else
	pthread_mutex_lock(&buffer_mutex);
#endif

	std::vector<Packet> packets;
	const Packet& data = *packet;

	if (type == WRITE_TYPE)
	{
		//if (receive_queue.is_full()){std::cout << "Warning: Receive queue full, dropping data\n";}
		if (!data.empty())
			receive_queue.push(data);
	}
	else
	{
		while (!receive_queue.empty())
		{
			packets.push_back(receive_queue.front());
			receive_queue.pop();
		}
	}

#ifdef __linux__
	pthread_mutex_unlock(&buffer_mutex);
#endif

	return packets;
}

void NetworkSystem::Receive(const NetworkEvent& event)
{
	if (event.type == NetworkEvent::SEND)
	{
		for (auto& p : event.packets)
		{
			send_queue_access(WRITE_TYPE, &p);
		}
	}
}
