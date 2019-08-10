#pragma once

#include <algorithm>

#include <iostream>
using namespace std;

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <string.h>

// ������ ������ ip�� port
const char serverIP[] = "127.0.0.1";
const int serverPort = 12345;



const int maxUserIDLen = 20;
const int maxPasswdLen = 20;
const int maxChatSize = 256;
// ���� ��� �������ݿ��� ����Ǵ� �κ�
struct ProtoCommand
{
	WORD command;
	BYTE data[0];
};


const WORD ComLoginREQ = 1; // id�� �н������ �α� ��û
struct StrLoginREQ    //�������ݿ� REQ�� ������
{						//  ���� ��û�� �ϴ� ��Ŷ
	BYTE userid[maxUserIDLen];
	BYTE passwd[maxPasswdLen];
};

const WORD ComLoginACK = 2; // �������� �α��� ����
struct StrLoginACK	// �������ݿ� ACK�� ������
{					// REQ�� ���� ������ �ǹ��ϴ� ��Ŷ
	BYTE result;
	// 0�̸� �α� ����, �ƴϸ� ����
};
enum EnumLoginACK
{
	LoginACKConnectAllow = 0,		// ���� ���
	LoginACKInvalidPasswd = 1,		// �н����� ����ġ
	LoginACKDuplicateConnect = 2,	// �� ���̵�� �ߺ�����
};


// ä�ÿ���������
const WORD ComChatREQ = 3;//������ ���ڿ� ������. REQ
struct StrChatREQ
{
	BYTE chat[maxChatSize];
};

const WORD ComChatACK = 4;//���������� ���� ���� ��ȯ ACK
struct StrChatACK
{
	BYTE result; // 0�̸� ����, �ƴϸ� ����
};
enum EnumChatACK
{
	ChatACKSuccess = 0,		// ä�ü���
	ChatACKNotLogin = 1,	// ���� �α���
};

const WORD ComChatCMD = 5;//������ ��� Ŭ���̾�Ʈ�� ���ڿ� ������. CMD
struct StrChatCMD
{
	BYTE userid[maxUserIDLen];
	BYTE chat[maxChatSize];
};

//�ӼӸ��� ���������� �����Ѵ�.
const WORD ComWisperREQ = 6;// ���������� �ӼӸ� �����޶�� ��û
struct StrWisperREQ
{
	BYTE toWhom[maxUserIDLen];  // ���� ��� id
	//BYTE chat[maxChatSize];
	BYTE chat[0];
};


const WORD ComWisperACK = 7;
struct StrWisperACK
{
	BYTE result;
};
enum EnumWisperACK
{
	WisperACKSuccess,
	WisperACKNotLogin, // �α���� ���� ���¿��� �Ӹ��� ������ �� ���
	WisperACKNotFound, // ��븦 ã�� �� ���� ���
};

const WORD ComWisperCMD = 8;
struct StrWisperCMD
{
	BYTE userID[maxUserIDLen];
	BYTE chat[maxChatSize];
};


/* ���� ǥ�� ��Ŷ */
const WORD ComFeelingREQ = 10;
struct StrFeelingREQ
{
	BYTE userID[maxUserIDLen];
	BYTE chat[maxChatSize];
};

const WORD ComFeelinACK = 11;
struct StrFeelingACK
{
	BYTE result;
};

enum EnumFeelingACK
{
	FeelingACKSuccess = 0,
	FeelingACKNotLogin = 1,
};

const WORD ComFeelingCMD = 12;
struct StrFeelingCMD
{
	BYTE userid[maxUserIDLen];
	BYTE chat[maxChatSize];
};
