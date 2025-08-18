#ifndef NETSERVER_H
#define NETSERVER_H

#include <unordered_map>
#include "config.h"
#include "network.h"

namespace ygo {

class NetServer {
private:
	static std::unordered_map<bufferevent*, DuelPlayer> users;
	static unsigned short server_port;
#ifndef YGOPRO_SERVER_MODE
	static event_base* net_evbase;
#endif
	static event* broadcast_ev;
	static evconnlistener* listener;
#ifndef YGOPRO_LOG_IN_CHAT
	static DuelMode* duel_mode;
#endif
	static unsigned char net_server_write[SIZE_NETWORK_BUFFER];
	static size_t last_sent;

public:
#ifdef YGOPRO_LOG_IN_CHAT
	static DuelMode* duel_mode;
#endif
#ifdef YGOPRO_SERVER_MODE
	static event_base* net_evbase;
	static void InitDuel();
	static void InitTestCard(int code);
	static unsigned short StartServer(unsigned short port);
	static bool IsCanIncreaseTime(unsigned short gameMsg, void *pdata, unsigned int len);
#else
	static bool StartServer(unsigned short port);
#endif //YGOPRO_SERVER_MODE
	static bool StartBroadcast();
	static void StopServer();
	static void StopBroadcast();
	static void StopListen();
	static void BroadcastEvent(evutil_socket_t fd, short events, void* arg);
	static void ServerAccept(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
	static void ServerAcceptError(evconnlistener *listener, void* ctx);
	static void ServerEchoRead(bufferevent* bev, void* ctx);
	static void ServerEchoEvent(bufferevent* bev, short events, void* ctx);
	static int ServerThread();
	static void DisconnectPlayer(DuelPlayer* dp);
	static void HandleCTOSPacket(DuelPlayer* dp, unsigned char* data, int len);
	static size_t CreateChatPacket(unsigned char* src, int src_size, unsigned char* dst, uint16_t dst_player_type);
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto) {
		auto p = net_server_write;
		BufferIO::Write<uint16_t>(p, 1);
		BufferIO::Write<uint8_t>(p, proto);
		last_sent = 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, 3);
	}
	template<typename ST>
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto, const ST& st) {
		auto p = net_server_write;
		static_assert(sizeof(ST) <= MAX_DATA_SIZE, "Packet size is too large.");
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + sizeof(ST)));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, &st, sizeof(ST));
		last_sent = sizeof(ST) + 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, sizeof(ST) + 3);
	}
	static void SendBufferToPlayer(DuelPlayer* dp, unsigned char proto, void* buffer, size_t len) {
		auto p = net_server_write;
		if (len > MAX_DATA_SIZE)
			len = MAX_DATA_SIZE;
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + len));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, buffer, len);
		last_sent = len + 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, len + 3);
	}
	static void ReSendToPlayer(DuelPlayer* dp) {
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
#ifdef YGOPRO_SERVER_MODE
	static void ReSendToPlayers(DuelPlayer* dp1, DuelPlayer* dp2) {
		if(dp1)
			bufferevent_write(dp1->bev, net_server_write, last_sent);
		if(dp2)
			bufferevent_write(dp2->bev, net_server_write, last_sent);
	}
#endif //YGOPRO_SERVER_MODE
};

}

#endif //NETSERVER_H
