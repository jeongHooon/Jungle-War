#pragma once
#include "Chat.h"
#include <list>
#include <mutex>
using namespace std;

const int maxBuffer = 1024;
class ClientInfo : public WSAOVERLAPPED
{
public:

	static int UserHandle;
	int userHandle; // 이 유저핸들은 모두가 달라야 한다.
	char id[maxUserIDLen];  // 접속한 클라이언트의 id

	WSABUF dataBuffer; // IOCP에서 패킷을 채울 메모리
	SOCKET socket;
	char messageBuffer[maxBuffer];
	unsigned long recvByte;// 읽은 패킷 길이
	unsigned long flag;  // WSARecv의 작동 조정

	bool loginComplete;

	ClientInfo();
	void Reset();
};

extern list<ClientInfo *> clientList;
extern mutex mutexList;
