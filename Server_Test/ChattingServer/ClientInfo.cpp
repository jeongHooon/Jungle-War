#include "ClientInfo.h"

int ClientInfo::UserHandle = 0;
ClientInfo::ClientInfo()
{
	userHandle = UserHandle;
	++UserHandle;
	// 들어온 순서에 따라 각자 다른 핸들값을 가지게 된다.
	Reset();
	loginComplete = false;
}
void ClientInfo::Reset()
{
	// 내가 가지고 있는 메모리 중 WSAOVERLAPPED 부분만 지운다.
	// 이 부분을 지우지 않으면 WSARecv 호출시 pending error가 생긴다.
	memset(this, 0, sizeof(WSAOVERLAPPED));

	dataBuffer.buf = messageBuffer;
	dataBuffer.len = maxBuffer;
	flag = 0;
	recvByte = 0;
}
