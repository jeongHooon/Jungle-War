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

	// 스레드는 무한루프 필요
	while (true)
	{
		// 스레드는 일단 GetQueuedCompletionStatus()
		// 함수로 들어가 대기상태로 바뀐다.
		// 어느 소켓에서든지 패킷 수신이 완료되면
		// 대기중인 스레드 하나를 깨우고 소켓정보를
		// GetQueuedCompletionStatus()의 인수로 전달한다.
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
		// 받은 패킷은 clientInfo->messageBuffer에 저장
		// 이 내용을 모든 클라이언트에게 전송

		// 이 패킷을 분석해 프로토콜에 맞는 작업을 해야 한다.
		// 프로토콜을 분석하는 작업도 만만치 않으므로
		// 프로토콜 분석하고 처리하는 객체를 만들자.
		PacketManager pacMan(clientInfo);
		pacMan.PacketProcess();// 패킷 작업을 해라.


		/* 기존의 패킷을 받으면 무조건 모든 사람들에게 전달해주는 부분
		mutexList.lock();
		for (list<ClientInfo *>::iterator client = clientList.begin();
			client != clientList.end(); ++client)
		{
			// client는 현재 접속해 있는 전체 클라이언트들의 정보
			// client의 소켓은 (*client)->socket이므로
			// 이 소켓으로 패킷내용을 전달하면 끝
			send((*client)->socket, clientInfo->messageBuffer,
				strlen(clientInfo->messageBuffer) + 1, 0);
		}
		mutexList.unlock();
		*/

		// 패킷처리가 완료되었으므로 다음 패킷을 처리하기
		// 위해서는 다시 WSARect를 실행해야 한다.
		// 그 전에 clientInfo를 초기화해야 한다.
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
