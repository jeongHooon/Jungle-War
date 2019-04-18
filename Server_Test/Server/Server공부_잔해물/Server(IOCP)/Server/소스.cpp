#include "NetworkDeclare.h"

#define SERVERPORT 9000

// ���� ���� ������ ���� ����ü�� ����
struct SOCKETINFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[sizeof(CtsPacket)];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};

// �񵿱� ����� ó�� �Լ�
DWORD WINAPI WorkerThread(LPVOID arg);

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;


	// ����� �Ϸ���Ʈ ����
	// (���⼭ ���ο� ����� ��Ʈ�� ����Ƿ�, ȿ�� ���� �ڵ鰪�� �־��ش�.
	// �׸��� �� ��° ���ڷ� NULL�� �־ ���ο� ����� ��Ʈ���� ����Ѵ�.
	// 
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;


	// CPU ���� Ȯ�� 
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("Processor : %d\n", (int)si.dwNumberOfProcessors);

	// Processor * 2��ŭ �۾��� ������ ����
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i) {
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hThread == NULL) return 1;
		CloseHandle(hThread);
	}
	printf("������ ���� �Ϸ� \n");
	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	DWORD recvbytes, flags;


	while (1) {
		printf("���� �����\n");


		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()"); 
			break;
		}
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// listen �������κ��� �޾ƿ� Ŭ���̾�Ʈ ���ϰ� 
		// ����� �Ϸ���Ʈ�� �������ش�.
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);



		// -----------------------------------------------------------------------
		// �� �κ� �־�� �Ǵ��� �ǹ�
		// �� �κ� ������ �־���Ѵ�.
		// �ֳ��ϸ� �ϴ� WSARecv�� Ŭ���̾�Ʈ �ϳ� ������ ������ 
		// �ɾ��ְ� ƴƴ�� GetCompletionStatus�� Ȯ��

		// ���� ���� ����ü �Ҵ�
		SOCKETINFO* ptr = new SOCKETINFO;
		if (ptr == NULL)break;
		CtsPacket client_buffer;
		ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvbytes = ptr->sendbytes = 0;
		ptr->wsabuf.buf = ptr->buf;

		// ����
		// Client���� Server�� ���� �� �� ��Ŷ��
		// Server���� Client�� ������ ��Ŷ�� ������ �ٸ�.
		// buffer 2�� ����ؾ� �� �ʿ� ����.
		ptr->wsabuf.len = sizeof(CtsPacket);

		flags = 0;
		// �񵿱� ����� ����
		retval = WSARecv(client_sock, &ptr->wsabuf, 1, &recvbytes,
			&flags, &ptr->overlapped, NULL);
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				err_display((char*)"WSARecv()");
			}
			continue;
		}

		//// ���� ȥ���� ƴŸ�� �����ϴ°� ������ �̰� ��� �� ��.
		memcpy(&client_buffer, ptr->buf, sizeof(ptr->buf));
		printf("[TCP/%s:%d] %x\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), client_buffer.keyboard_click);
		//// -----------------------------------------------------------------------

	}

	// ���� ����
	WSACleanup();
	return 0;
}

// �񵿱� ����� ó�� �Լ�
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;
	CtsPacket cts_buffer;
	ZeroMemory(&cts_buffer, sizeof(cts_buffer));

	while (true) {
		// �񵿱� ����� �Ϸ� ��ٸ��� 
		DWORD cbTransferred;  // CbTransferred�� �񵿱� ����� �۾����� ���۵� ����Ʈ ��.
		SOCKET client_sock;
		SOCKETINFO* ptr;

		//retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
		//	(LPDWORD)&client_sock, (LPOVERLAPPED*)&ptr, INFINITE);
		
		printf("������ ����� \n");
		// ���� �ȵǼ� PULONG_PTR�� �����
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,		
			(PULONG_PTR)&client_sock, (LPOVERLAPPED*)&ptr, INFINITE);
		printf("ť�� �Ϸ�� ��Ʈ �߰�\n");

		// Ŭ���̾�Ʈ ���� �ҷ�����
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);


		// �񵿱� ����� ��� Ȯ��
		if (retval == 0 || cbTransferred == 0) {
			if (retval == 0) {
				DWORD temp1, temp2;
				WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
					&temp1, FALSE, &temp2);
				err_display((char*)"WSAGetOverlappedResult()");
			}
			closesocket(ptr->sock);
			printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			delete ptr;
			continue;
		}


		// ������ ���۷� ����
		// but ������ Ŭ���̾�Ʈ���� ���� ������
		// ���Ÿ� �ϹǷ� ������������ recv�� 
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes == 0;
			// ���� ������ ���
			memcpy(&cts_buffer, ptr->buf, sizeof(ptr->buf));
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), cts_buffer.keyboard_click);
		}
		else {	// ���� �����Ͱ� �ִ� ���
			ptr->sendbytes += cbTransferred;
		}



		////----------------------------------------------------------------------------
		//// ���ʿ� �� �۾��� �ʿ� ���°Ű���.


		//// ������ ������
		//if (ptr->recvbytes > ptr->sendbytes) {
		//	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		//	ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
		//	// ���⼭ sendbyte�� �� ���ϴ°���?
		//	ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

		//	DWORD sendbytes;
		//	retval = WSASend(ptr->sock, &ptr->wsabuf, 1,
		//		&sendbytes, 0, &ptr->overlapped, NULL);
		//	if (retval == SOCKET_ERROR) {
		//		if (WSAGetLastError() != WSA_IO_PENDING) {
		//			err_display((char*)"WSASend()");
		//		}
		//		continue;
		//	}
		//}

		//// ������ �ޱ�
		//// �� �ְŴ� �ްŴϰ� �ʿ��� ��Ȳ�̸�
		//// �̷������� ���α׷� �ؾ�������
		//// ���� ���� ���������� �̷��� ������ �� �ʿ䰡
		//// ����. 
		//else {
		//	ptr->recvbytes = 0;

		//	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		//	ptr->wsabuf.buf = ptr->buf;
		//	ptr->wsabuf.len = sizeof(CtsPacket);

		//	DWORD recvbytes;
		//	DWORD flags = 0;
		//	retval = WSARecv(ptr->sock, &ptr->wsabuf, 1,
		//		&recvbytes, &flags, &ptr->overlapped, NULL);
		//	if (retval == SOCKET_ERROR) {
		//		if (WSAGetLastError() != WSA_IO_PENDING) {
		//			err_display((char*)"WSARecv()");
		//		}
		//		continue;
		//	}
		//}


		////----------------------------------------------------------------------------




	}




	return retval; 
}

