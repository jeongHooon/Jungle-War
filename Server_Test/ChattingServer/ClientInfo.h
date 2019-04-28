#pragma once
#include "../Chat.h"
#include <list>
#include <mutex>
using namespace std;

const int maxBuffer = 1024;
class ClientInfo : public WSAOVERLAPPED
{
public:
	// ��� Ŭ���̾�Ʈ�� ������ �����ؾ� �ϹǷ�
	// �� Ŭ���̾�Ʈ�� ������ ����(�ڵ�)�� �ʿ�
	static int UserHandle;
	int userHandle; // �� �����ڵ��� ��ΰ� �޶�� �Ѵ�.
	char id[maxUserIDLen];  // ������ Ŭ���̾�Ʈ�� id

	WSABUF dataBuffer; // IOCP���� ��Ŷ�� ä�� �޸�
	SOCKET socket;
	char messageBuffer[maxBuffer];
	unsigned long recvByte;// ���� ��Ŷ ����
	unsigned long flag;  // WSARecv�� �۵� ����

	bool loginComplete;
	//	 LoginREQ�� �����ϸ� true�� �ٲ��, �̰��� true��
	//   ���ȿ��� ä���� �� �� �ִ�.
	ClientInfo();
	void Reset();
};

// �Ʒ� �������� �̹� ChattingServer.cpp����
// ������� �����Ƿ� �ٸ� ���Ͽ��� �ٽ� �����
// ������ ������ ������ �ִٴ� ������ ���� �ȴ�.
// �׷��Ƿ� ���⼭�� [�̷� �������� �ٸ� ������
// ������� ������ ���� �������� ���� ������
// �ض�]��� ���� �����Ϸ����� �˷��־�� �Ѵ�.
// �̷��� ����ϴ� Ű���尡 extern�̴�.
extern list<ClientInfo *> clientList;
extern mutex mutexList;
