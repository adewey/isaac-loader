#include "sockets.h"


void ReceiveMessage(MessageMap messages)
{

};

PWEBSOCKET pInjectorSocket;
void InitSockets()
{
	pInjectorSocket = AddSocket("ws://localhost:9045", "Client");
}

void UnInitSockets()
{
	RemoveSocket(pInjectorSocket);
}


PWEBSOCKET pSocketList = 0;
PWEBSOCKET AddSocket(string url, string origin)
{
	PWEBSOCKET pSocket = new WEBSOCKET();
	pSocket->url = url;
	pSocket->origin = origin;

	pSocket->pLast = 0;
	pSocket->pNext = pSocketList;
	if (pSocketList)
		pSocketList->pLast = pSocket;
	pSocketList = pSocket;

	pSocket->start();

	return pSocket;
}

bool RemoveSocket(PWEBSOCKET pSocket)
{
	if (!pSocket)
		return false;

	pSocket->listen = false;

	if (pSocket->pLast)
		pSocket->pLast->pNext = pSocket->pNext;
	else
		pSocketList = pSocket->pNext;
	if (pSocket->pNext)
		pSocket->pNext->pLast = pSocket->pLast;

	delete pSocket;
	return true;
}

struct MessageHandler : public BaseReaderHandler<UTF8<>, MessageHandler> {
	MessageHandler() : messages_(), state_(kExpectObjectStart), name_() {}

	bool StartObject() {
		switch (state_) {
		case kExpectObjectStart:
			state_ = kExpectNameOrObjectEnd;
			//cout << "Expected start, starting" << endl;
			return true;
		default:
			//cout << "start, not expected. end" <<endl;
			return false;
		}
	}

	bool String(const char* str, SizeType length, bool) {
		switch (state_) {
		case kExpectNameOrObjectEnd:
			name_ = string(str, length);
			//cout << "String, Expected name or obj end" << endl;
			state_ = kExpectValue;
			return true;
		case kExpectValue:
			//cout << "String, Expected value" << endl;
			messages_.insert(MessageMap::value_type(name_, string(str, length)));
			state_ = kExpectNameOrObjectEnd;
			return true;
		default:
			//cout << "String, Not Expected" << endl;
			return false;
		}
	}
	bool Int(int i){
		switch (state_){
		case kExpectValue:
			//cout << "int, expected value " << endl;
			messages_.insert(MessageMap::value_type(name_, to_string(i)));

			state_ = kExpectNameOrObjectEnd;
			return true;
		default:
			//cout << "int, not expected" << endl;
			return false;
		}
	}

	bool EndObject(SizeType) { return state_ == kExpectNameOrObjectEnd; }

	bool Default() { return false; } // All other events are invalid.

	MessageMap messages_;
	enum State {
		kExpectObjectStart,
		kExpectNameOrObjectEnd,
		kExpectValue,
	}state_;
	std::string name_;
};

void ParseMessages(const char* json, MessageMap& messages)
{
	Reader reader;
	MessageHandler handler;
	StringStream ss(json);
	if (reader.Parse(ss, handler))
		messages.swap(handler.messages_);   // Only change it if success.
	else {
		ParseErrorCode e = reader.GetParseErrorCode();
		size_t o = reader.GetErrorOffset();
		//cout << "Error: " << GetParseError_En(e) << endl;;
		//cout << " at offset " << o << " near '" << string(json).substr(o, 10) << "...'" << endl;
	}
}

void handle_message(const std::string & message)
{
	MessageMap messages;
	ParseMessages(message.c_str(), messages);
	ReceiveMessage(messages);
	OnReceiveMessage(messages);
}


void push_message(PWEBSOCKET pSocket, string message) {
	pSocket->push_message(message);
}
void WEBSOCKET::push_message(string message)
{
	while (message_lock == true) Sleep(100);
	message_lock = true;
	try
	{
		message_list.push_back(message);
	}
	catch (int e){}
	message_lock = false;
}

string WEBSOCKET::pop_message()
{
	while (message_lock == true) Sleep(100);
	message_lock = true;
	string message;
	try
	{
		message = message_list.front();
		cout << message.c_str() << endl;
		message_list.pop_front();
	}
	catch (int e){}
	message_lock = false;
	return message;
}

void clear_messages(PWEBSOCKET pSocket) {
	pSocket->clear_messages();
}
void WEBSOCKET::clear_messages()
{
	while (message_lock == true) Sleep(100);
	message_lock = true;
	try
	{
		message_list.clear();
	}
	catch (int e){}
	message_lock = false;
}

static DWORD static_entry(LPVOID* param) {
	WEBSOCKET *pSocket = (WEBSOCKET*)param;
	pSocket->socket_thread();
	return 0;
}

void WEBSOCKET::start()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)static_entry, this, 0, NULL);
}

void WEBSOCKET::socket_thread()
{
	while (gbAttached && listen)
	{
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			cout << "!WSAStartup" << endl;
			//Log("WSAStartup Failed.");
		}
#endif
		websocket = from_url(url, false, origin);
		if (websocket == NULL || websocket->getReadyState() == WebSocket::CLOSED) {
			cout << "!websocket" << endl;
			//Log("Websocket Could not Connect.");
		}
		else {
			//Log("Websocket Connected.");
			while (websocket->getReadyState() != WebSocket::CLOSED && gbAttached && listen) {
				websocket->poll();
				websocket->dispatch(handle_message);
				if (message_list.size() > 0 && send_update)
				{
					websocket->send(pop_message());
					send_update = false;
				}
			}
			websocket->close();

			delete websocket;
		}
#ifdef _WIN32
		WSACleanup();
#endif
		//Log("Web Socket Closed.");
	}
	//Log("No Longer Attached to Isaac");
}