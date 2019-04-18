#pragma once

// ä�ÿ� �ַ���� ���� ��� ����

#pragma comment(lib,"Ws2_32.lib")

#include <algorithm>
#include <iostream>
#include <WinSock2.h>
#include <string.h>
using namespace std;


// ������ ������ ip�� port
const char serverIP[] = "127.0.0.1";
//const int serverPort = 12345;

const int maxUserIDLen = 20;
const int maxPasswdLen = 20;
const int maxChatSize = 256;

// ��� �������ݿ��� ����Ǵ� �κ�
struct ProtoCommand {
	WORD command;
	BYTE data[0]; // ProtoCommand�� ���κ��� �˷��ִ� ������

};

// �α����� ���� ��������
const WORD ComLoginREQ = 1; //id�� �н������ �α��� ��û
struct StrLoginREQ{  // �α����� ��û�ϴ� ��Ŷ
	BYTE userid[maxUserIDLen];
	BYTE passwd[maxPasswdLen];
};

const WORD ComLoginACK = 2;  //�α��ΰ�� ����
struct StrLoginACK {  // REQ�� ���� ������ �ϴ� ��Ŷ
	BYTE result;  // 0�̸� �α��� ����, �ƴϸ� ����
};

enum EnumLoginACK {
	LoginACKConnectAllow = 0,  	    // ���� ���
	LoginACKInvalidPasswd = 1,      // �н����� ����ġ
	LoginACKDuplicateConnect =2,    // �� ���̵�� �ߺ�����
};

