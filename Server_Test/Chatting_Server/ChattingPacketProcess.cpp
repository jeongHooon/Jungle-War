#include "ClientInfo.h"
#include "PacketManager.h"

// ä�� ��Ŷ�� �����ϴ� ������ - IOCP�� ��ŷ������
unsigned int __stdcall ChattingPacketProcess(void *arg)
{
	// arg : iocp�ڵ��� ������
	HANDLE *hndPnt = (HANDLE *)arg;
	HANDLE iocpHandle = *hndPnt;
	// ����� ó�� ť�� ���� ���� ������
	unsigned long recvBytes;
	ULONG_PTR completionKey;
	WSAOVERLAPPED *cliInfo;
	unsigned long flag;

	while (true)
	{

		BOOL result =
			GetQueuedCompletionStatus(
				iocpHandle,
				&recvBytes,		// ���� ��Ŷ ����
				&completionKey,
				&cliInfo,		//WSARecv���� ������ ���ϰ��ð�ü
				INFINITE		// ���Ѵ��
			);
		ClientInfo *clientInfo = (ClientInfo *)cliInfo;
		if (result == FALSE) // Ŭ���̾�Ʈ ���� ����
		{
			cout << "��������" << endl;
			// ���� �ݱ�
			closesocket(clientInfo->socket);

			// ����Ʈ���� ����
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
		// ����� �� ��Ŷ�� ���Դ�.
		cout << "���� ��Ŷ " << recvBytes << "bytes" << endl;

		PacketManager pacMan(clientInfo);
		pacMan.PacketProcess();// ��Ŷ �۾��� �ض�.

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
