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
// �α��� �Ϸ�Ǿ������� Ȯ���ϱ� ���� ��������
// �������� �α� ���� ��Ŷ�� �;� �� ���� true�� �ٲ��.

unsigned int __stdcall RecieveThread(void *arg);
int _tmain(int argc, _TCHAR* argv[])
{
	// 1. ���� �ʱ�ȭ
	WSADATA wsaData;
	int startupResult =
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0)
	{
		cout << "���� �ʱ�ȭ ����" << endl;
		return 1;
	}

	//2. ������ ������ ����
	SOCKET toServer
		= WSASocket(
			AF_INET,	// 4����Ʈ �ּ�
			SOCK_STREAM,// ����� ����
			0,			// �������� Ÿ��
			nullptr,	// �������� ����
			0,			// ���� �׷�
			WSA_FLAG_OVERLAPPED);
	if (toServer == INVALID_SOCKET)
	{
		cout << "���� ���� ����" << endl;
		WSACleanup();
		return 1;
	}

	// 3. �������� ���� �õ�
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	// �ʿ��� ������ ����
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_addr.S_un.S_addr =
		inet_addr(serverIP); //���ڿ��� �ּ��������� �ٲ۴�.
	serverAddr.sin_port = serverPort;
	int connectResult =
		connect(toServer, (SOCKADDR *)&serverAddr,
			sizeof(serverAddr));
	if (connectResult == SOCKET_ERROR)
	{
		cout << "���� ����" << endl;
		WSACleanup();
		return 1;
	}
	cout << "���Ӽ���" << endl;

	// recv�� ������ ������ ����
	_beginthreadex(
		nullptr, // ���Ȱ�������
		0,    // �⺻ ���� ������
		RecieveThread,
		&toServer,	// ������ �־�� ���ú� ����
		0,
		nullptr);

	// ���� �������� ������
	SendLoginREQ(toServer); // ComLogin���������̹Ƿ� SendLogin���� �̸� ���δ�.

	// ������ �Ϸ�� �������� �� �����ؼ��� �ȵȴ�.
	while (loginComplete == false)
		Sleep(100); // 0.1�ʿ� �ѹ��� ���

	while (true)
	{
		char buffer[1024];
		cout << "������ ���ڿ� : ";
		cin >> buffer;
		/* ������, �Է¹��ڿ��� �״�� ������ ������ �κ�
		int sendByte =
			send(toServer, buffer, strlen(buffer) + 1, 0);
		cout << "Send " << sendByte << "bytes" << endl;
		*/
		if (strcmp(buffer, "/w") == 0)  // �Է� ���ڿ��� "/w"��� �ӼӸ�
			SendWisper(toServer);

		else if (strcmp(buffer, "/����") == 0)
			SendFeelingREQ(toServer, buffer);
		else if (strcmp(buffer, "/���") == 0)
			SendFeelingREQ(toServer, buffer);
		else
			SendChatREQ(toServer, buffer);
	}

	return 0;
}

// Jungle-War
void SendWisper(SOCKET toServer)
{
	char toWhom[1024];  // �������� �ӼӸ��� ���� ���ΰ�
	char chat[1024];    // � ������ ���� ���ΰ�

	cout << "�������� �ӼӸ��� �������? ";
	cin >> toWhom;
	cout << "���� ���� �Է��ϼ��� ";
	cin >> chat;

	// toWhom���� chat������ �����޶�� ��û
	char buffer[sizeof(ProtoCommand) + sizeof(StrWisperREQ)
		+ maxChatSize];// StrWisperREQ::chat�� ũ�Ⱑ 0�� �Ǿ����Ƿ�
						//  ä��ũ�⸦ �߰��Ѵ�.
	// ������ ������ ProtoCommand�� �����
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// �� ������ StrWisperREQ��
	StrWisperREQ *wisperREQ = (StrWisperREQ *)cmd->data;

	// ���� ä���
	cmd->command = ComWisperREQ;
	strncpy_s((char *)wisperREQ->toWhom, maxUserIDLen,
		toWhom, maxUserIDLen);
	wisperREQ->toWhom[maxUserIDLen - 1] = '\0';// ���ڿ��� ���� ǥ��
	strncpy_s((char *)wisperREQ->chat, maxChatSize,
		chat, maxChatSize);
	wisperREQ->chat[maxChatSize - 1] = '\0';// ���ڿ��� ���� ǥ��

	send(toServer, buffer,
		sizeof(ProtoCommand) + sizeof(StrWisperREQ)
		// chat���̰� 0�� ����ü ũ��
		+ strlen((char *)wisperREQ->chat) + 1,
		// �ű⿡ ä���� ���̸� �߰�
		0);
}

void SendFeelingREQ(SOCKET toServer, char *buf)
{
	char buffer[sizeof(ProtoCommand) + sizeof(StrFeelingREQ)];

	ProtoCommand *cmd = (ProtoCommand *)buffer;
	StrFeelingREQ *feelingREQ = (StrFeelingREQ *)cmd->data;

	cmd->command = ComFeelingREQ;

	//

	if (strcmp(buf, "/����") == 0)
		strncpy_s((char *)feelingREQ->chat, maxChatSize,
			" ���� ���� �ֽ��ϴ�.", maxChatSize);
	else if (strcmp(buf, "/���") == 0)
		strncpy_s((char *)feelingREQ->chat, maxChatSize,
			" ���� ��� �ֽ��ϴ�.", maxChatSize);
	feelingREQ->chat[maxChatSize - 1] = '\0';// ���ڿ��� ���� ǥ��

	send(toServer, buffer,
		sizeof(ProtoCommand) + sizeof(StrFeelingREQ), 0);
}

void SendLoginREQ(SOCKET toServer)
{	//ComLogin ���������� ������.
	// id�� passwd�� �Է¹޴´�.
	char userid[256];
	char passwd[256];

	cout << "id�� �Է��� �ּ��� ";
	cin >> userid;
	cout << "��ȣ�� �Է��� �ּ��� ";
	cin >> passwd;

	// ��ɾ��ü(ProtoCommand)�� StrLogin ����
	char protoBuffer[1024];
	ProtoCommand *cmd = (ProtoCommand *)protoBuffer;
	StrLoginREQ *login = (StrLoginREQ *)cmd->data;
	// �̰����� *login ����ü�� *cmd �ٷ� �ڿ� �پ��ִ�
	// ����ü�� �Ǹ�, cmd�� �̹� �Ҵ�� ����� ū �޸�
	// protoBuffer ���ۺκп� ��ġ�ϹǷ�
	// �� ����ü�� �Ҵ�� �޸𸮿� �������� �ڸ��ϰ� �ȴ�.

	// ��ɾ� ����
	cmd->command = ComLoginREQ; // �α��� �Ѵٴ� ��ɾ�
	// ������ ����
	strncpy_s((char *)login->userid, maxUserIDLen, userid, maxUserIDLen);
	// strcpy�� ������ ����� �Ѿ��� �����ϹǷ�
	// ���ѵ� ���̸�ŭ�� ���ϻ�� strncpy�� ����
	// ��, ������ ���ں��� ������ٸ� ������ ���ڸ�ŭ��
	// �����ϸ� ���� '\0'�� ������ �����Ƿ� ��������
	// �ٿ���� �Ѵ�.
	login->userid[maxUserIDLen - 1] = '\0';// ���� ���ڸ��� '\0'�� �ٿ��ش�.

	strncpy_s((char *)login->passwd, maxPasswdLen, passwd, maxPasswdLen);
	login->passwd[maxPasswdLen - 1] = '\0';

	// �� ���������� ������ ������.
	// ������ ���� ��ġ : protoBuffer
	// ������ ���� ���� : sizeof(ProtoCommand) + sizeof(StrLogin)
	send(toServer, protoBuffer,
		sizeof(ProtoCommand) + sizeof(StrLoginREQ), 0);
}

void SendChatREQ(SOCKET toServer, char *chatBuffer)
{	// �� ���������� ComChatREQ �������ݷ� �����ؼ� ������.
	char buffer[1024];
	// ���� �պκ��� ProtoCommand�� ����
	ProtoCommand *cmd = (ProtoCommand *)buffer;
	// cmd �޺κп� StrChatREQ�� ����
	StrChatREQ *chatREQ = (StrChatREQ *)cmd->data;

	//  ���������� ������ ä���.
	cmd->command = ComChatREQ;
	strncpy_s((char *)chatREQ->chat, maxChatSize, chatBuffer, maxChatSize);
	chatREQ->chat[maxChatSize - 1] = '\0'; // ���ڿ��� ���� �Ǹ�

	// ������ ������.
	send(toServer, buffer,
		sizeof(*cmd) + // ������ cmd�� ����Ű�� ��ü�� ũ�� == ProtoCommand�� ũ��
		sizeof(*chatREQ), // chatREQ�� ����Ű�� ��ü�� ũ�� == StrChatReq�� ũ��
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
	char buffer[1024]; //��Ŷ�� ���� ����
	while (true)
	{
		recv(toServer, buffer, 1024, 0);
		/* ���� ������ ����ϴ� �κ�
		cout << "���� ���� : " << buffer << endl;
		*/
		// ��Ŷ�� �м��ؼ� �������ݴ�� ����
		PacketProcess(buffer);
	}
	return 0;
}

void PacketProcess(char *buffer)
{
	// � �������������� ���� ������
	// ���� ���� ProtoCommand��� ����� �� �� �ִ�
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
	if (loginACK->result == LoginACKConnectAllow) // ����
	{
		cout << "�����Ϸ�" << endl;
		loginComplete = true;
	}
	else
	{
		switch (loginACK->result)
		{
		case LoginACKDuplicateConnect:
			cout << "���� ���� - �ߺ�����" << endl;
			break;
		case LoginACKInvalidPasswd:
			cout << "���� ���� - �н����� ����ġ" << endl;
			break;
		}

		// TODO : �������� ���� ���� ���α׷� ����
	}
}

void ActionChatACK(StrChatACK *chatACK)
{
	if (chatACK->result != 0)
		cout << "ä���� �źεǾ����ϴ�" << endl;
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
		cout << "���� �α� �ȵǾ� �ֽ��ϴ�." << endl;
		break;
	case WisperACKNotFound:
		cout << "����� ã�� �� �����ϴ�." << endl;
		break;
	}
}

void ActionWisperCMD(StrWisperCMD *wisperCMD)
{
	cout << wisperCMD->userID << "�����κ����� �Ӹ� : "
		<< wisperCMD->chat << endl;
}

void ActionFeelingACK(StrFeelingACK *feelingACK)
{
	switch (feelingACK->result)
	{
	case FeelingACKSuccess:
		break;
	case FeelingACKNotLogin:
		cout << "ä���� �źεǾ����ϴ�" << endl;
		break;
	}
}

void ActionFeelingCMD(StrFeelingCMD * feelingCMD)
{
	cout << "[" << feelingCMD->userid << "]" << feelingCMD->chat << endl;
}