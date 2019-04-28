#include "ClientInfo.h"

int ClientInfo::UserHandle = 0;
ClientInfo::ClientInfo()
{
	userHandle = UserHandle;
	++UserHandle;
	// ���� ������ ���� ���� �ٸ� �ڵ鰪�� ������ �ȴ�.
	Reset();
	loginComplete = false;
}
void ClientInfo::Reset()
{
	// ���� ������ �ִ� �޸� �� WSAOVERLAPPED �κи� �����.
	// �� �κ��� ������ ������ WSARecv ȣ��� pending error�� �����.
	memset(this, 0, sizeof(WSAOVERLAPPED));

	dataBuffer.buf = messageBuffer;
	dataBuffer.len = maxBuffer;
	flag = 0;
	recvByte = 0;
}
