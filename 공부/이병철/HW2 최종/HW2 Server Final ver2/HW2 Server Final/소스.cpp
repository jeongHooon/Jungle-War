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

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int myClientID = 0;

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while (1)
	{
		// recv 부분
		retval = recvn(client_sock, (char *)&SRS, sizeof(SRS), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv(22)");
			exit(1);
		}


		memcpy(&P1KeyBuffer, &SRS.KeyBuffer, sizeof(P1KeyBuffer));


		if (P1KeyBuffer[VK_LEFT])
		{
			if (characterPos1.x > 0)
				characterPos1.x -= 100;
		}
		else if (P1KeyBuffer[VK_RIGHT])
		{
			if (characterPos1.x < BOARDSIZE - 100)
				characterPos1.x += 100;
		}
		else if (P1KeyBuffer[VK_DOWN])
		{
			if (characterPos1.y < BOARDSIZE - 100)
				characterPos1.y += 100;
		}
		else if (P1KeyBuffer[VK_UP])
		{
			if (characterPos1.y > 0)
				characterPos1.y -= 100;
		}

		memcpy(&SSS.p1, &characterPos1, sizeof(characterPos1));

		// 고정길이 전송
		retval = send(client_sock, (char *)&SSS, sizeof(SSS), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			return 0;
		}

	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s\t포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

int main()
{
	int retval;

	// 윈속초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock; // 클라이언트 저장 소켓
	SOCKADDR_IN clientaddr; // 클라이언트 주소 저장
	int addrlen; // 주소 길이

	HANDLE hThread;

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		printf("Accept\n");
		if (client_sock == INVALID_SOCKET)
		{
			err_display((char*)"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }

	}

	// closesocket()
	closesocket(listen_sock);

	CloseHandle(hThread);

	// 윈속 종료
	WSACleanup();
	return 0;

	return 0;
}

