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

	// ������� ���ѷ��� �ʿ�
	while (true)
	{
		// ������� �ϴ� GetQueuedCompletionStatus()
		// �Լ��� �� �����·� �ٲ��.
		// ��� ���Ͽ������� ��Ŷ ������ �Ϸ�Ǹ�
		// ������� ������ �ϳ��� ����� ����������
		// GetQueuedCompletionStatus()�� �μ��� �����Ѵ�.
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
		// ���� ��Ŷ�� clientInfo->messageBuffer�� ����
		// �� ������ ��� Ŭ���̾�Ʈ���� ����

		// �� ��Ŷ�� �м��� �������ݿ� �´� �۾��� �ؾ� �Ѵ�.
		// ���������� �м��ϴ� �۾��� ����ġ �����Ƿ�
		// �������� �м��ϰ� ó���ϴ� ��ü�� ������.
		PacketManager pacMan(clientInfo);
		pacMan.PacketProcess();// ��Ŷ �۾��� �ض�.


		/* ������ ��Ŷ�� ������ ������ ��� ����鿡�� �������ִ� �κ�
		mutexList.lock();
		for (list<ClientInfo *>::iterator client = clientList.begin();
			client != clientList.end(); ++client)
		{
			// client�� ���� ������ �ִ� ��ü Ŭ���̾�Ʈ���� ����
			// client�� ������ (*client)->socket�̹Ƿ�
			// �� �������� ��Ŷ������ �����ϸ� ��
			send((*client)->socket, clientInfo->messageBuffer,
				strlen(clientInfo->messageBuffer) + 1, 0);
		}
		mutexList.unlock();
		*/

		// ��Ŷó���� �Ϸ�Ǿ����Ƿ� ���� ��Ŷ�� ó���ϱ�
		// ���ؼ��� �ٽ� WSARect�� �����ؾ� �Ѵ�.
		// �� ���� clientInfo�� �ʱ�ȭ�ؾ� �Ѵ�.
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
