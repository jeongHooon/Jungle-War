#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "stdafx.h"
#include "Chat.h"
#include <process.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS


void SendWisper(SOCKET toServer);
void SendLoginREQ(SOCKET toServer);
void SendChatREQ(SOCKET toServer, char *chatBuffer);
void SendFeelingREQ(SOCKET toServer, char *buf);


bool loginComplete = false;
// 로긴이 완료되었는지를 확인하기 위한 전역변수
// 서버에서 로긴 인정 패킷이 와야 이 값이 true로 바뀐다.

unsigned int __stdcall RecieveThread(void *arg);
int _tmain(int argc, _TCHAR* argv[])
{
	// 1. 윈속 초기화
	WSADATA wsaData;
	int startupResult =
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0)
	{
		cout << "윈속 초기화 실패" << endl;
		return 1;
	}

	//2. 서버로 접속할 소켓
	SOCKET toServer
		= WSASocket(
			AF_INET,	// 4바이트 주소
			SOCK_STREAM,// 양방향 전송
			0,			// 프로토콜 타입
			nullptr,	// 프로토콜 정보
			0,			// 소켓 그룹
			WSA_FLAG_OVERLAPPED);
	if (toServer == INVALID_SOCKET)
	{
		cout << "소켓 생성 실패" << endl;
		WSACleanup();
		return 1;
	}

	// 3. 서버로의 접속 시도
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	// 필요한 데이터 설정
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_addr.S_un.S_addr =
		inet_addr(serverIP); //문자열을 주소형식으로 바꾼다.
	serverAddr.sin_port = serverPort;
	int connectResult =
		connect(toServer, (SOCKADDR *)&serverAddr,
			sizeof(serverAddr));
	if (connectResult == SOCKET_ERROR)
	{
		cout << "접속 실패" << endl;
		WSACleanup();
		return 1;
	}
	cout << "접속성공" << endl;

	// recv를 전담할 스레드 생성
	_beginthreadex(
		nullptr, // 보안관련정보
		0,    // 기본 스택 사이즈
		RecieveThread,
		&toServer,	// 소켓이 있어야 리시브 가능
		0,
		nullptr);

	// 인증 프로토콜 보내기
	SendLoginREQ(toServer); // ComLogin프로토콜이므로 SendLogin으로 이름 붙인다.

	// 인증이 완료될 때까지는 더 진행해서는 안된다.
	while (loginComplete == false)
		Sleep(100); // 0.1초에 한번씩 깨어남

	while (true)
	{
		char buffer[1024];
		cout << "전송할 문자열 : ";
		cin >> buffer;
		/* 기존의, 입력문자열을 그대로 서버로 보내는 부분
		int sendByte =
			send(toServer, buffer, strlen(buffer) + 1, 0);
		cout << "Send " << sendByte << "bytes" << endl;
		*/
		if (strcmp(buffer, "/w") == 0)  // 입력 문자열이 "/w"라면 귓속말
			SendWisper(toServer);

		else if (strcmp(buffer, "/웃기") == 0)
			SendFeelingREQ(toServer, buffer);
		else if (strcmp(buffer, "/울기") == 0)
			SendFeelingREQ(toServer, buffer);
		else
			SendChatREQ(toServer, buffer);
	}

	return 0;
}

// Jungle-War
void SendWisper(SOCKET toServer)
{
	char toWhom[1024];  // 누구에게 귓속말을 보낼 것인가
	char chat[1024];    // 어떤 내용을 보낼 것인가

	cout << "누구에게 귓속말을 보낼까요? ";
	cin >> toWhom;
	cout << "보낼 말을 입력하세요 ";
	cin >> chat;

	// toWhom에게 chat내용을 보내달라고 요청
	char buffer[sizeof(ProtoCommand) + sizeof(StrWisperREQ)
		+ maxChatSize];// StrWisperREQ::chat의 크기가 0이 되었으므로
						//  채팅크기를 추가한다.
	// 버퍼의 앞쪽을 ProtoCommand로 만든다
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// 그 뒤쪽을 StrWisperREQ로
	StrWisperREQ *wisperREQ = (StrWisperREQ *)cmd->data;

	// 내용 채우기
	cmd->command = ComWisperREQ;
	strncpy_s((char *)wisperREQ->toWhom, maxUserIDLen,
		toWhom, maxUserIDLen);
	wisperREQ->toWhom[maxUserIDLen - 1] = '\0';// 문자열의 끝을 표시
	strncpy_s((char *)wisperREQ->chat, maxChatSize,
		chat, maxChatSize);
	wisperREQ->chat[maxChatSize - 1] = '\0';// 문자열의 끝을 표시

	send(toServer, buffer,
		sizeof(ProtoCommand) + sizeof(StrWisperREQ)
		// chat길이가 0인 구조체 크기
		+ strlen((char *)wisperREQ->chat) + 1,
		// 거기에 채팅의 길이를 추가
		0);
}

void SendFeelingREQ(SOCKET toServer, char *buf)
{
	char buffer[sizeof(ProtoCommand) + sizeof(StrFeelingREQ)];

	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrFeelingREQ *feelingREQ = (StrFeelingREQ *)cmd->data;

	cmd->command = ComFeelingREQ;

	//

	if (strcmp(buf, "/웃기") == 0)
		strncpy_s((char *)feelingREQ->chat, maxChatSize,
			" 님이 웃고 있습니다.", maxChatSize);
	else if (strcmp(buf, "/울기") == 0)
		strncpy_s((char *)feelingREQ->chat, maxChatSize,
			" 님이 울고 있습니다.", maxChatSize);
	feelingREQ->chat[maxChatSize - 1] = '\0';// 문자열의 끝을 표시

	send(toServer, buffer,
		sizeof(ProtoCommand) + sizeof(StrFeelingREQ), 0);
}

void SendLoginREQ(SOCKET toServer)
{	//ComLogin 프로토콜을 보낸다.
	// id와 passwd를 입력받는다.
	char userid[256];
	char passwd[256];

	cout << "id를 입력해 주세요 ";
	cin >> userid;
	cout << "암호를 입력해 주세요 ";
	cin >> passwd;

	// 명령어구조체(ProtoCommand)와 StrLogin 생성
	char protoBuffer[1024];
	ProtoCommand *cmd = (ProtoCommand *)protoBuffer;
	StrLoginREQ *login = (StrLoginREQ *)cmd->data;
	// 이것으로 *login 구조체는 *cmd 바로 뒤에 붙어있는
	// 구조체가 되며, cmd는 이미 할당된 충분히 큰 메모리
	// protoBuffer 시작부분에 위치하므로
	// 두 구조체는 할당된 메모리에 연속으로 자리하게 된다.

	// 명령어 세팅
	cmd->command = ComLoginREQ; // 로긴을 한다는 명령어
	// 데이터 세팅
	strncpy_s((char *)login->userid, maxUserIDLen, userid, maxUserIDLen);
	// strcpy는 문제가 생기면 한없이 복사하므로
	// 제한된 길이만큼만 복하사느 strncpy가 안전
	// 단, 지정된 문자보다 길어진다면 지정된 글자만큼만
	// 복사하며 끝에 '\0'을 붙이지 않으므로 수동으로
	// 붙여줘야 한다.
	login->userid[maxUserIDLen - 1] = '\0';// 가장 끝자리에 '\0'을 붙여준다.

	strncpy_s((char *)login->passwd, maxPasswdLen, passwd, maxPasswdLen);
	login->passwd[maxPasswdLen - 1] = '\0';

	// 이 프로토콜을 서버로 보낸다.
	// 서버로 보낼 위치 : protoBuffer
	// 서버로 보낼 길이 : sizeof(ProtoCommand) + sizeof(StrLogin)
	send(toServer, protoBuffer,
		sizeof(ProtoCommand) + sizeof(StrLoginREQ), 0);
}

void SendChatREQ(SOCKET toServer, char *chatBuffer)
{	// 이 프로토콜을 ComChatREQ 프로토콜로 가공해서 보낸다.
	char buffer[1024];
	// 버퍼 앞부분을 ProtoCommand로 만듦
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// cmd 뒷부분에 StrChatREQ를 연결
	StrChatREQ *chatREQ = (StrChatREQ *)cmd->data;

	//  프로토콜의 내용을 채운다.
	cmd->command = ComChatREQ;
	strncpy_s((char *)chatREQ->chat, maxChatSize, chatBuffer, maxChatSize);
	chatREQ->chat[maxChatSize - 1] = '\0'; // 문자열의 끝을 의마

	// 서버로 보낸다.
	send(toServer, buffer,
		sizeof(*cmd) + // 포인터 cmd가 가리키는 객체의 크기 == ProtoCommand의 크기
		sizeof(*chatREQ), // chatREQ가 가리키는 객체의 크기 == StrChatReq의 크기
		0);
}

/////////////////////////////////////////////////////

void PacketProcess(char *buffer);
void ActionLoginACK(StrLoginACK *loginACK);
void ActionChatACK(StrChatACK *chatACK);
void ActionChatCMD(StrChatCMD *chatCMD);
void ActionWisperACK(StrWisperACK *wisperACK);
void ActionWisperCMD(StrWisperCMD *wisperCMD);
void ActionFeelingACK(StrFeelingACK *feelingACK);
void ActionFeelingCMD(StrFeelingCMD * feelingCMD);

unsigned int __stdcall RecieveThread(void *arg)
{
	SOCKET *sockPtr = (SOCKET *)arg;
	SOCKET toServer = *sockPtr;
	char buffer[1024]; //패킷을 받을 버퍼
	while (true)
	{
		recv(toServer, buffer, 1024, 0);
		/* 예전 무조건 출력하던 부분
		cout << "받은 내용 : " << buffer << endl;
		*/
		// 패킷을 분석해서 프로토콜대로 실행
		PacketProcess(buffer);
	}
	return 0;
}

void PacketProcess(char *buffer)
{
	// 어떤 프로토콜인지는 아직 모르지만
	// 가장 앞이 ProtoCommand라는 사실을 알 수 있다
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	switch (cmd->command)
	{
	case ComLoginACK:
		ActionLoginACK((StrLoginACK *)cmd->data);
		break;
	case ComChatACK:
		ActionChatACK((StrChatACK *)cmd->data);
		break;
	case ComChatCMD:
		ActionChatCMD((StrChatCMD *)cmd->data);
		break;
	case ComWisperACK:
		ActionWisperACK((StrWisperACK *)cmd->data);
		break;
	case ComWisperCMD:
		ActionWisperCMD((StrWisperCMD *)cmd->data);
		break;
	case ComFeelinACK:
		ActionFeelingACK((StrFeelingACK*)cmd->data);
		break;
	case ComFeelingCMD:
		ActionFeelingCMD((StrFeelingCMD *)cmd->data);
		break;
	}
}

void ActionLoginACK(StrLoginACK *loginACK)
{
	if (loginACK->result == LoginACKConnectAllow) // 성공
	{
		cout << "인증완료" << endl;
		loginComplete = true;
	}
	else
	{
		switch (loginACK->result)
		{
		case LoginACKDuplicateConnect:
			cout << "인증 실패 - 중복접속" << endl;
			break;
		case LoginACKInvalidPasswd:
			cout << "인증 실패 - 패스워드 불일치" << endl;
			break;
		}

		// TODO : 서버와의 소켓 끊고 프로그램 종료
	}
}

void ActionChatACK(StrChatACK *chatACK)
{
	if (chatACK->result != 0)
		cout << "채팅이 거부되었습니다" << endl;
}

void ActionChatCMD(StrChatCMD *chatCMD)
{
	cout << chatCMD->userid << " : " << chatCMD->chat << endl;
}

void ActionWisperACK(StrWisperACK *wisperACK)
{
	switch (wisperACK->result)
	{
	case WisperACKSuccess:
		break;
	case WisperACKNotLogin:
		cout << "아직 로긴 안되어 있습니다." << endl;
		break;
	case WisperACKNotFound:
		cout << "대상을 찾을 수 없습니다." << endl;
		break;
	}
}

void ActionWisperCMD(StrWisperCMD *wisperCMD)
{
	cout << wisperCMD->userID << "님으로부터의 귓말 : "
		<< wisperCMD->chat << endl;
}

void ActionFeelingACK(StrFeelingACK *feelingACK)
{
	switch (feelingACK->result)
	{
	case FeelingACKSuccess:
		break;
	case FeelingACKNotLogin:
		cout << "채팅이 거부되었습니다" << endl;
		break;
	}
}

void ActionFeelingCMD(StrFeelingCMD * feelingCMD)
{
	cout << "[" << feelingCMD->userid << "]" << feelingCMD->chat << endl;
}