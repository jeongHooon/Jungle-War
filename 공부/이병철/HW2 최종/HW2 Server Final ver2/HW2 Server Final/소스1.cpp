// HW2 server.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#define SERVERPORT 9000
#define BUFSIZE    30000

char IPAdd[30] = "127.0.0.1";
int CalCheck = 0;
#pragma pack(1)
struct Server_Recv_Struct {
	BOOL KeyBuffer[256];
};
#pragma pack()

#pragma pack(1)
struct Server_Send_Struct {
	POINT p1;
};
#pragma pack()

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;

	POINT pos;
	BOOL KeyBuffer[256];

	int receiveBytes;
	int sendBytes;
};

Server_Recv_Struct SRS;
Server_Send_Struct SSS;

CRITICAL_SECTION cs;


void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", (LPCSTR)msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

map <SOCKET, SOCKETINFO> clients;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

int main()
{
	int retval;

	// 윈속초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	//listen()
	retval = listen(listenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;


	while (1)
	{
	
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
		printf("Accept\n");
		if (clientSocket == INVALID_SOCKET)
		{
			err_display((char*)"accept()");
			break;
		}

		clients[clientSocket] = SOCKETINFO{};
		memset(&clients[clientSocket], 0x00, sizeof(struct SOCKETINFO));
		clients[clientSocket].socket = clientSocket;
		clients[clientSocket].dataBuffer.len = sizeof(clients[clientSocket].KeyBuffer);
		clients[clientSocket].dataBuffer.buf = (char*)&clients[clientSocket].KeyBuffer;
		flags = 0;


		// 중첩 소캣을 지정하고 완료시 실행될 함수를 넘겨준다.
		clients[clientSocket].overlapped.hEvent = (HANDLE)clients[clientSocket].socket;

		if (WSARecv(clients[clientSocket].socket, &clients[clientSocket].dataBuffer, 1, NULL, &flags, &(clients[clientSocket].overlapped), recv_callback))
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				cout << "Error - IO pending Failure\n" << endl;;
				return 1;
			}
		}
		else
		{
			cout << "Non Overlapped Recv return.\n" << endl;
			return 1;
		}

	}

	// closesocket()
	closesocket(listenSocket);



	// 윈속 종료
	WSACleanup();
	return 0;
}

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	//int key{};
	DWORD sendBytes = 0;
	DWORD receiveBytes = 0;
	DWORD flags = 0;

	if (dataBytes == 0)
	{
		closesocket(clients[client_s].socket);
		clients.erase(client_s);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	if (clients[client_s].KeyBuffer[VK_LEFT])
	{
		if (clients[client_s].pos.x > 0)
			clients[client_s].pos.x -= 100;
	}
	else if (clients[client_s].KeyBuffer[VK_RIGHT])
	{
		if (clients[client_s].pos.x < BOARDSIZE - 100)
			clients[client_s].pos.x += 100;
	}
	else if (clients[client_s].KeyBuffer[VK_DOWN])
	{
		if (clients[client_s].pos.y < BOARDSIZE - 100)
			clients[client_s].pos.y += 100;
	}
	else if (clients[client_s].KeyBuffer[VK_UP])
	{
		if (clients[client_s].pos.y > 0)
			clients[client_s].pos.y -= 100;
	}

	clients[client_s].dataBuffer.len = sizeof(clients[client_s].pos);
	clients[client_s].dataBuffer.buf = (char*)&clients[client_s].pos;



	memset(&(clients[client_s].overlapped), 0x00, sizeof(WSAOVERLAPPED));
	clients[client_s].overlapped.hEvent = (HANDLE)client_s;


	if (WSASend(client_s, &(clients[client_s].dataBuffer), 1, &dataBytes, 0, &(clients[client_s].overlapped), send_callback) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("Error - Fail WSASend");
		}
	}

}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	DWORD sendBytes = 0;
	DWORD receiveBytes = 0;
	DWORD flags = 0;

	SOCKET client_s = reinterpret_cast<int>(overlapped->hEvent);

	if (Error != 0)
	{
		closesocket(clients[client_s].socket);

		clients.erase(client_s);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	{
		// WSASend(응답에 대한)의 콜백일 경우
		clients[client_s].dataBuffer.len = MAX_BUFFER;
		clients[client_s].dataBuffer.buf = (char*)&clients[client_s].KeyBuffer;



		memset(&(clients[client_s].overlapped), 0x00, sizeof(WSAOVERLAPPED));
		clients[client_s].overlapped.hEvent = (HANDLE)client_s;
		if (WSARecv(client_s, &clients[client_s].dataBuffer, 1, &receiveBytes, &flags, &(clients[client_s].overlapped), recv_callback) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				printf("Error - Fail WSARecv(error_code : %d)\n", WSAGetLastError());
			}
		}
	}
}