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

	memset(this, 0, sizeof(WSAOVERLAPPED));

	dataBuffer.buf = messageBuffer;
	dataBuffer.len = maxBuffer;
	flag = 0;
	recvByte = 0;
}
