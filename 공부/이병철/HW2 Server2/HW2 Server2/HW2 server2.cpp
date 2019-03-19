// HW2 server.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <atlimage.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#define SERVERPORT 9000
#define BUFSIZE    30000
#define BOARDNUM 8
#define BOARDSIZE 800
#define CHARACTERSIZE 50
#define MAX_CLIENT 2

BOOL P1KeyBuffer[256];
BOOL P2KeyBuffer[256];

int ClientCount = 0;
bool isstart = false;

CImage character1;
CImage character2;

POINT character1Pos = { 400,0 }; //캐릭터 초기 위치 값
POINT character2Pos = { 400,700 }; //캐릭터 초기 위치 값

#pragma pack(1)
struct Init {
	int id;
	POINT PlayerPos;
};
#pragma pack()

#pragma pack(1)
struct Client{

	int id; // 0 ,1
	int x,y;
	SOCKET s;
	bool keyBuffer[256];

};
#pragma pack()
Client cl[MAX_CLIENT];

#pragma pack(1)
struct Server_recv_Struct {
	BOOL KeyBuffer[256];
	int ClientID;
};
#pragma pack()

#pragma pack(1)
struct Server_send_Struct {
	int x1, y1;
	int x2, y2;
};
#pragma pack()

Server_recv_Struct CTS;
Server_send_Struct STC;

int sendCount = 0;

CRITICAL_SECTION cs;

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);
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
	char buf[BUFSIZE + 1];
	int myClientID = 0;


	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(cl[ClientCount-1].s, (SOCKADDR *)&clientaddr, &addrlen);

	while (1)
	{
		if (!isstart)
		{
			myClientID = ClientCount;
			printf("%d", myClientID);
			while (1)
			{
				if (ClientCount < 3)
				{
					
					cout << ClientCount << "보냈다" << endl;
					retval = send(cl[ClientCount - 1].s, (char*)&ClientCount, sizeof(ClientCount), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("send()11");
						exit(1);
					}

					if (ClientCount == 1)
					{
						retval = send(cl[ClientCount - 1].s, (char*)&character1Pos, sizeof(character1Pos), 0);
						if (retval == SOCKET_ERROR)
						{
							err_display("send()Pos1");
							exit(1);
						}
					}
					else
					{
						retval = send(cl[ClientCount - 1].s, (char*)&character2Pos, sizeof(character2Pos), 0);
						if (retval == SOCKET_ERROR)
						{
							err_display("send()Pos1");
							exit(1);
						}
					}
					

					++sendCount;
					break;
				}
			}
			while (1) {
				if (sendCount > 1) {
					cout << "isstart";
					isstart = true;
					break;
				}
			}
		}
		else
		{
			//cout << "recv 진입";
			retval = recvn(client_sock, (char *)&CTS, sizeof(CTS), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()22");
				exit(1);
			}
			//if (CTS.ClientID == 1) memcpy(&P1KeyBuffer, &CTS.KeyBuffer, sizeof(CTS.KeyBuffer));
			//else if (CTS.ClientID == 2) memcpy(&P2KeyBuffer, &CTS.KeyBuffer, sizeof(CTS.KeyBuffer));

			memcpy(&cl[CTS.ClientID].keyBuffer, &CTS.KeyBuffer, sizeof(CTS.KeyBuffer));

			
			//cout << "1번" << P1KeyBuffer[VK_LEFT] << endl;
			//cout << "2번" << P2KeyBuffer[VK_LEFT] << endl;

			/*if (P1KeyBuffer[VK_LEFT])
				if (character1Pos.x > 0)
					character1Pos.x -= 100;
			if (P1KeyBuffer[VK_RIGHT])
				if (character1Pos.x < BOARDSIZE - 100)
					character1Pos.x += 100;
			if (P1KeyBuffer[VK_DOWN])
				if (character1Pos.y < BOARDSIZE - 100)
					character1Pos.y += 100;
			if (P1KeyBuffer[VK_UP])
				if (character1Pos.y > 0)
					character1Pos.y -= 100;

			if (P2KeyBuffer[VK_LEFT])
				if (character2Pos.x > 0)
					character2Pos.x -= 100;
			if (P2KeyBuffer[VK_RIGHT])
				if (character2Pos.x < BOARDSIZE - 100)
					character2Pos.x += 100;
			if (P2KeyBuffer[VK_DOWN])
				if (character2Pos.y < BOARDSIZE - 100)
					character2Pos.y += 100;
			if (P2KeyBuffer[VK_UP])
				if (character2Pos.y > 0)
					character2Pos.y -= 100;*/

			cout << cl[CTS.ClientID].keyBuffer[VK_LEFT] << endl;
			if (cl[CTS.ClientID].keyBuffer[VK_LEFT])
				if (cl[CTS.ClientID].x > 0)
					cl[CTS.ClientID].x -= 100;
			if (cl[CTS.ClientID].keyBuffer[VK_RIGHT])
				if (cl[CTS.ClientID].x < BOARDSIZE - 100)
					cl[CTS.ClientID].x += 100;
			if (cl[CTS.ClientID].keyBuffer[VK_DOWN])
				if (cl[CTS.ClientID].y < BOARDSIZE - 100)
					cl[CTS.ClientID].y += 100;
			if (cl[CTS.ClientID].keyBuffer[VK_UP])
				if (cl[CTS.ClientID].y > 0)
					cl[CTS.ClientID].y -= 100;
				
			for (int i = 0; i < MAX_CLIENT; ++i)
			{
				if (cl[i].id == 0)
				{
					STC.x1 = cl[i].x;
					STC.y1 = cl[i].y;
				}
				else if(cl[i].id == 1)
				{
					STC.x2 = cl[i].x;
					STC.y2 = cl[i].y;
				}
			}
			
			//memcpy(&STC.x1, &character1Pos.x, sizeof(character1Pos.x));
			//memcpy(&STC.y1, &character1Pos.y, sizeof(character1Pos.y));
			//memcpy(&STC.x2, &character2Pos.x, sizeof(character2Pos.x));
			//memcpy(&STC.y2, &character2Pos.y, sizeof(character2Pos.y));

			for (int i = 0; i < MAX_CLIENT; ++i)
			{
				retval = send(cl[i].s, (char *)&STC, sizeof(STC), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()33");
					break;
				}
			}
		


		}
	}
	for (int i = 0; i < MAX_CLIENT; ++i)
	{
		closesocket(cl[i].s);
	}
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

int main()
{
	InitializeCriticalSection(&cs);

	int retval;

	// 윈속초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock; // 클라이언트 저장 소켓
	SOCKADDR_IN clientaddr; // 클라이언트 주소 저장
	int addrlen; // 주소 길이
	HANDLE hThread;

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		cl[ClientCount].s = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (cl[ClientCount].s == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		EnterCriticalSection(&cs);
		++ClientCount;
		LeaveCriticalSection(&cs);

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)cl[ClientCount - 1].s, 0, NULL);
		
	}

	// closesocket()
	closesocket(listen_sock);

	DeleteCriticalSection(&cs);
	CloseHandle(hThread);
	// 윈속 종료
	WSACleanup();
	return 0;

	return 0;
}

