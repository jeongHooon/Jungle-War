

#include "stdafx.h"
#include "Chat.h"
#include <process.h>
#include <list>
#include <mutex>

unsigned int __stdcall ChattingPacketProcess(void *arg);
const int maxWorkingThread = 8;



#include "ClientInfo.h"

list<ClientInfo *> clientList;
mutex mutexList;

int _tmain(int argc, _TCHAR* argv[])
{
	//1. winsock �ʱ�ȭ
	WSADATA wsaData;
	int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0) //0�� �ƴϸ� ����
	{
		cout << "���� �ʱ�ȭ ����!" << endl;
		return 1;
	}

	// 2. �������� ����
	// ���������� ��Ʈ�� �����ϸ� Ŭ���̾�Ʈ ������
	// �ִ����� Ȯ���Ѵ�.
	SOCKET listenSocket =
		WSASocket(
			AF_INET, // 4����Ʈ IP �ּ�
			SOCK_STREAM, // ������ �ִ� ����� ���
			0,   // �������� Ư��, 0
			nullptr, // �������� ����, nullptr�̸� �⺻��
			0,  // ���� �׷�
			WSA_FLAG_OVERLAPPED);
	// ����� �������� ���ÿ� ��ø�Ǿ� �����
	// �� �ִ�.
	if (listenSocket == INVALID_SOCKET)
	{// ������ �߸� ���������.
		cout << "�������� ���� ����" << endl;
		WSACleanup();
		// ������ ���� �ʱ�ȭ�� �����Ƿ�
		// ������ ���� ���� ������ �ؾ� �Ѵ�.
		return 1;
	}

	SOCKADDR_IN sockInfo;
	// ����ü Ŭ���� - ����ü ��ü�� 0���� ä���.
	memset(&sockInfo, 0, sizeof(sockInfo));
	// �ʿ��� ������ ����
	sockInfo.sin_family = PF_INET; // 4����Ʈ �ּ� ü��
	sockInfo.sin_port = serverPort;
	sockInfo.sin_addr.S_un.S_addr
		= htonl(INADDR_ANY); // ��� Ŭ���̾�Ʈ���� ���� ����

	int bindResult =
		::bind(listenSocket,// ���ε��� ����(std�ȿ� �ִ� bind�� �ƴ϶�
						//namespace�� ������ ���� bind�� �ǹ�)
		(SOCKADDR *)&sockInfo, // ���Ͽ� ������ ����ü
			sizeof(sockInfo)	// ����ü�� ũ��
		);
	if (bindResult == SOCKET_ERROR)
	{
		cout << "listenSocket ���ε� ����" << endl;
		WSACleanup();
		return 1;
	}

	int listenResult =
		listen(listenSocket, 5);

	if (listenResult == SOCKET_ERROR)
	{
		cout << "listen() ȣ�� ����" << endl;
		WSACleanup();
		return 1;
	}

	HANDLE iocpHandle =
		CreateIoCompletionPort(
			INVALID_HANDLE_VALUE, // ���� ���鶧 ������ �����Ƿ�
			nullptr, // ���� ���ʿ� ���鶧 IOCP �ڵ��� �����Ƿ�
			0, // ���� ���ʷ� ��������
			maxWorkingThread); // �ִ� ��ŷ������ ����
	// IOCP ��ü�� ���� �������.
	if (iocpHandle == INVALID_HANDLE_VALUE)
	{
		cout << "iocp ��ü ���� ����" << endl;
		WSACleanup();
		return 1;
	}



	//6. IOCP�� Working Thread ����
	for (int cnt = 0; cnt < maxWorkingThread; ++cnt)
	{
		_beginthreadex(
			nullptr, // ���Ȱ����̹Ƿ� �⺻ nullptr
			0, // ����ũ��, 0�̸� �⺻ũ��� ����
			ChattingPacketProcess, // �Լ� �̸�
			&iocpHandle,	// ������� iocp�ڵ��� �����Ѵ�.
			0,	// �������� �ʱ���� - ������ ������ �Բ� ����
			nullptr //������ ID�� ������ �ʿ䰡 ����.
		);
	}

	/////// ä�� ���α׷� ���� ///////
	while (true)
	{

		SOCKADDR_IN clientInfo;
		int size = sizeof(clientInfo);
		SOCKET sockToCli = //Ŭ���̾�Ʈ���� ����
			accept(listenSocket, // �������Ͽ��� �޾ƿ´�.
			(SOCKADDR *)&clientInfo,// Ŭ���̾�Ʈ ����
				&size); // ����ü ũ��
		if (sockToCli == INVALID_SOCKET)
		{
			cout << "���� ���Ʈ ����" << endl;
			continue;
		}
		// ���� Ȯ��
		cout << "Ŭ���̾�Ʈ ���� Ȯ��" << endl;
		ClientInfo *cliInfo = new ClientInfo();
		// ���� ������ Ŭ���̾�Ʈ�� ����
		cliInfo->socket = sockToCli;

		// ���⼭ ���� ���������� ����Ʈ�� �����Ѵ�.
		mutexList.lock();
		clientList.push_back(cliInfo);
		mutexList.unlock();


		iocpHandle =
			CreateIoCompletionPort(
			(HANDLE)sockToCli, //����� ����
				iocpHandle, // ������ ����� ���� �ڵ�
				(ULONG_PTR)cliInfo,  // �Է��� �����ϱ� ���� Ű��
				maxWorkingThread
			);

		int recvResult =
			WSARecv(cliInfo->socket,
				&cliInfo->dataBuffer,
				1,
				&cliInfo->recvByte,
				&cliInfo->flag,
				cliInfo, //�� ��ü������ ��ŷ������� ���޵�
				nullptr
			);
		if (recvResult != 0) // ���� ����
		{
			// pending�� ������ �ۼ��� ���̹Ƿ� ������ �ƴ�
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				cout << "Pending Error" << endl;
				continue;
			}
		}

	}
	return 0;
}


