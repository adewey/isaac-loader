#ifndef SOCKETS_H
#define SOCKETS_H

#include "utilities.h"
#include "plugins.h"

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"
#include "easywsclient.hpp"
#include "easywsclient.cpp" // <-- include only if you don't want compile separately

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <deque>
#include <stdio.h>
#include <string>

using namespace rapidjson;
using easywsclient::WebSocket;

typedef struct __declspec(dllexport) _WebSocket
{
	bool listen;
	bool send_update;
	string url;
	string origin;

	WebSocket::pointer websocket;


	deque<string> message_list;
	bool message_lock;
	void push_message(string message);
	string pop_message();
	void clear_messages();

	void start();

	void socket_thread();
	
	struct _WebSocket* pLast;
	struct _WebSocket* pNext;
} WEBSOCKET, *PWEBSOCKET;

void InitSockets();
void UnInitSockets();

GLOBAL PWEBSOCKET AddSocket(string url, string origin);
GLOBAL bool RemoveSocket(PWEBSOCKET pSocket);

#endif