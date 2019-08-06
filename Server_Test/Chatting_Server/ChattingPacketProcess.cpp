#include "ClientInfo.h"
#include "PacketManager.h"

// 채팅 패킷을 조작하는 스레드 - IOCP의 워킹스레드
unsigned int __stdcall ChattingPacketProcess(void *arg)
{
	// arg : iocp핸들의 포인터
	HANDLE *hndPnt = (HANDLE *)arg;
	HANDLE iocpHandle = *hndPnt;
	// 입출력 처리 큐로 들어가기 위한 변수들
	unsigned long recvBytes;
	ULONG_PTR completionKey;
	WSAOVERLAPPED *cliInfo;
	unsigned long flag;

	while (true)
	{

		BOOL result =
			GetQueuedCompletionStatus(
				iocpHandle,
				&recvBytes,		// 받은 패킷 길이
				&completionKey,
				&cliInfo,		//WSARecv에서 보내는 소켓관련객체
				INFINITE		// 무한대기
			);
		ClientInfo *clientInfo = (ClientInfo *)cliInfo;
		if (result == FALSE) // 클라이언트 접속 종료
		{
			cout << "접속종료" << endl;
			// 소켓 닫기
			closesocket(clientInfo->socket);

			// 리스트에서 제거
			mutexList.lock();
			clientList.remove_if(
				[clientInfo](ClientInfo *client)->
				bool
			{
				return client->userHandle
					== clientInfo->userHandle;
			}
			);
			mutexList.unlock();

			delete clientInfo;
			continue;
		}
		// 제대로 된 패킷이 들어왔다.
		cout << "받은 패킷 " << recvBytes << "bytes" << endl;

		PacketManager pacMan(clientInfo);
		pacMan.PacketProcess();// 패킷 작업을 해라.

		clientInfo->Reset();
		int recvResult =
			WSARecv(
				clientInfo->socket,
				&clientInfo->dataBuffer,
				1,
				&clientInfo->recvByte,
				&clientInfo->flag,
				clientInfo,
				nullptr);
		if (recvResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				cout << "pending error" << endl;
			}
		}
	}
}
