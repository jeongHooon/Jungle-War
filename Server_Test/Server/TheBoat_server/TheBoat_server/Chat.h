#pragma once

// 채팅용 솔루션을 위한 헤더 파일

#pragma comment(lib,"Ws2_32.lib")

#include <algorithm>
#include <iostream>
#include <WinSock2.h>
#include <string.h>
using namespace std;


// 서버에 접속할 ip와 port
const char serverIP[] = "127.0.0.1";
//const int serverPort = 12345;

const int maxUserIDLen = 20;
const int maxPasswdLen = 20;
const int maxChatSize = 256;

// 모든 프로토콜에서 공통되는 부분
struct ProtoCommand {
	WORD command;
	BYTE data[0]; // ProtoCommand의 끝부분을 알려주는 포인터

};

// 로그인을 위한 프로토콜
const WORD ComLoginREQ = 1; //id와 패스워드로 로그인 ㅇ청
struct StrLoginREQ{  // 로그인을 요청하는 패킷
	BYTE userid[maxUserIDLen];
	BYTE passwd[maxPasswdLen];
};

const WORD ComLoginACK = 2;  //로그인결과 응답
struct StrLoginACK {  // REQ에 대한 응답을 하는 패킷
	BYTE result;  // 0이면 로그인 성공, 아니면 실패
};

enum EnumLoginACK {
	LoginACKConnectAllow = 0,  	    // 접속 허용
	LoginACKInvalidPasswd = 1,      // 패스워드 불일치
	LoginACKDuplicateConnect =2,    // 한 아이디로 중복접속
};

