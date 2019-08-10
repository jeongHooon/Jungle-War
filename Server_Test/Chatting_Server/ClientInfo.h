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
	int userHandle; // �� �����ڵ��� ��ΰ� �޶�� �Ѵ�.
	char id[maxUserIDLen];  // ������ Ŭ���̾�Ʈ�� id

	WSABUF dataBuffer; // IOCP���� ��Ŷ�� ä�� �޸�
	SOCKET socket;
	char messageBuffer[maxBuffer];
	unsigned long recvByte;// ���� ��Ŷ ����
	unsigned long flag;  // WSARecv�� �۵� ����

	bool loginComplete;

	ClientInfo();
	void Reset();
};

extern list<ClientInfo *> clientList;
extern mutex mutexList;
