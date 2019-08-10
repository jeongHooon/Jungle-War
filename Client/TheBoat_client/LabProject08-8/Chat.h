#pragma once
/*
	 채팅용 솔루션을 위한 헤더 파일
	 채팅 서버와 채팅 클라이언트에 공용으로
	 사용되므로 솔루션용 폴더에 저장하며
	 양쪽에서 인클루드를 해야 한다.
*/
#include <algorithm>
// Chat.h는 서버와 클라이언트에서 동시에 인클루드하는
// 헤더이므로, 여기에서 algorithm을 인클루드하면
// 서버와 클라이언트 양쪽에서 알고리즘 함수를 사용할 수 있다.

#include <iostream>
using namespace std;

// 양쪽 다 윈속(winsock)을 써야 하므로
// 다음 헤더와 라이브러리가 필요하다.
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <string.h>

// 서버에 접속할 ip와 port
const char serverIP[] = "127.0.0.1";
const int serverPort = 12345;

/*
	문자열을 관리하는 데이터형으로 string이라는 데이터형이
	존재한다. 그러나 이 string은 C++에서 생긴 것으로,
	C에서는 문자열을 관리하는 데이터형이 아예 존재하지 않는다.
	그러므로 C에서는 문자열을 [문자형의 포인터] 또는 [문자의
	배열] 형태로 관리하며, 메모리의 지정된 위치에 문자열을
	나열하는 방식을 사용한다.

		char hello[] = "Hello";
			// 문자 6개짜리 배열을 만든 후,
			// 각각의 문자형변수에 'h', 'e', 'l', 'l',
			// 'o'와 문자열의 끝을 의미하는 '\0'이 들어간다.
		char *hello = "Hello";
			// 메모리 어딘가에 'h', 'e', 'l', 'l', 'o'
			// 그리고 문자열의 끝을 의미하는 '\0'이
			// 나란히 저장된 후, 그 문자열의 시작위치를
			// 포인터변수 hello에 저장한다.
		// C++에서 사용하는 네트워킹용 함수들 거의 모두가
		// C 시절부터 존재하던 것이기 때문에 문자열을
		// 조작하기 위해서는 C 스타일의 문자열 조작을 해야
		// 한다(즉 string 타입을 사용할 수 없다)
		// C에서 문자열을 조작하기 위해서는 함수를 사용해야
		// 하며 주로 다음과 같은 함수들을 많이 사용한다.
		// 1.문자열 복사 - 문자열을 복사하기 위해서는 먼저
		//              - 문자열이 복사될 공간을 확보해야
		//              - 한다.
		//    char buffer[100];
		//    strcpy(buffer, "Test");
		//         buffer라는 배열에 "Test" 문자열 복사
		//	  strncpy(buffer, "Test", 100);
		//         buffer에 "Test"를 복사하는데 최대 100개까지
		//    C++에서는 strcpy나 strncpy보다 보안성이 강화된
		//    strcpy_s, strncpy_s를 사용한다.
		// 2. 문자열 병합 -  문자열 뒤에 새로운 문자열 복사
		//	  char buffer[100];
		//    strcpy(buffer, "hello");
		//    strcat(buffer, ", world");
		//		결과적으로 buffer에는 "hello, world"가
		//      저장된다.
		// 3. 문자열 길이 계산
		//    strlen(문자포인터 또는 문자배열);
		// 이런 문자열 관련 함수들은 모두 <string.h>를
		// 인클루드해야 한다.

	/*
		현재 채팅서버가 하는 일은 클라이언트가 보낸
		문자열을 접속한 모든 사람들에게 전송하는 일 뿐이다.
		하지만 실제 채팅서버가 해야 할 일은 많다.
		- 사용자 관리(id와 passwd 관리)
		- 채팅기능
		- 감정표현  /울기 => ~~님이 울고 있습니다.
				   /웃기 => ~~님이 웃고 있습니다.
		- 귓말기능

		그러므로 서버나 클라이언트나 패킷을 받으면
		그 패킷이 어떤 명령인지를 확인하고 그 명령에 맞는
		작업을 하도록 해야 한다.
		이러한 작업을 PROTOCOl(규약, 약속)이라고 한다.
		- 클라이언트와 서버가 [~~명령은 ~~형태의 패킷을
		보내기로 하자]라는 약속을 하는 일

		미리 알아야 할 점
		  1.int형은 4바이트지만, C컴파일러가 처음 나왔을
			때의 int형은 2바이트였다.
			이렇게 int나 long형 같은 기본 데이터 타입은
			컴퓨터나 컴파일러에 따라 달라질 수 있다.
			이것은 컴퓨터 통신상에서 보내는 쪽의 int와
			받는 쪽의 int가 다르다면 문제가 될 수 있다.
			그러므로 프로토콜을 설계할 때는 int나 long형
			같은 기본 데이터타입이 아니라 다음과 같은
			타입명을 사용한다.
			BYTE  : 1바이트크기
			WORD  : 2바이트크기
			DWORD : 4바이트크기
			시스템마다 int 형의 크기가 달라도 BYTE, WORD
			등의 크기는 일정하므로 이들을 사용하는 것이 좋다.
		  2.컴퓨터 시스템에서는 변수들에의 접근을 빠르게
			하기 위해 변수들을 2바이트 또는 4바이트 간격으로
			배치하는 경우가 있다.
			  struct test
			  {
				  BYTE b;
				  WORD w;
				  DWORD d;
			  };
			  라 한다면 메모리에서 다음과 같이 저장되게 된다.
			  -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
			   | | | |b| | | |w|w| | |d|d|d|d| | | |
			  -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
			  하지만 이렇게 하면 이 구조체를 패킷으로 보낼때
			  패킷 크기가 커지므로 변수들을 빈칸 없이 차곡차곡
			  채워야 한다.

			  #pragram pack(1)
			  struct test
			  {
			  BYTE b;
			  WORD w;
			  DWORD d;
			  };
			  이것은 변수들 사이의 간격을 1바이트단위로(즉 빈틈없이)
			  채우라는 이야기로 다음과 같은 메모리를 차지하게 된다.
			  -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
			   | | | |b|w|w|d|d|d|d| | | | | | | | |
			  -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	*/

	/*
		프로토콜 설계에 대하여
		두 개의 프로토콜을 생각해 보자.
		1. 계정인증
			 계정을 인증하는 프로토콜 구조
			 - 명령어(이 프로토콜은 계정을 인증하는 프로토콜이다)
			 - 유저 id
			 - passwd
				 이 세 개의 데이터가 있어야 사용자인지를 판단 가능

		2. 채팅메시지
			다른 사람들에게 전달되는 채팅 메시지의 구조
			- 명령어(이 프로토콜을 전체 클라이언트에게 보여지는 프로토콜)
			- 채팅 내용
				 이 두 개의 데이터로 전체 사용자에게 내용을 전송할 수 있다.

		이 두 프로토콜에서 명령어 부분은 공통으로 사용할 수 있지만,
		그 이후의 부분은 프로토콜마다 달라질 수밖에 없다.
		그러므로 하나의 프로토콜은 명령어 부분과 데이터 부분으로
		나뉘어지며, 명령어부분은 모든 프로토콜에 공통, 데이터부분은
		프로토콜마다 달라져야 한다.

		명령어 부분(모든 프로토콜에 대해 공통)
		struct ProtoCommand  // 명령어
		{
			WORD command;
		};

		Data부분(각 프로토콜마다 따로)
		struct StrLogin
		{
			BYTE userid[20];
			BYTE passwd[20];
		};
		struct StrChat
		{
			BYTE chat[256];
		};

		이렇게 ProtoCommand와 각 데이터 구조체를 만들어 놓고
		두 구조체를 연속으로 이어서 한번에 보내야 한다.
	*/

//const int maxUserIDLen = 20;
//const int maxPasswdLen = 20;
//const int maxChatSize = 256;
// 먼저 모든 프로토콜에서 공통되는 부분
struct ProtoCommand
{
	WORD command;
	BYTE data[0];//크기가 0이므로 실제로 메모리를 할당하지는
				// 않지만, ProtoCommand의 끝부분을 알려주는
				// 포인터로서 기능한다.
				// 그러므로 이 위치에 데이터를 만든다면
				// ProtoCommand 뒤에 붙어있는 구조체를 만들
				// 수 있다.
};

// 각 프로토콜에 대해 필요한 데이터 구조체
// 로긴을 위한 프로토콜
const WORD ComLoginREQ = 1; // id와 패스워드로 로긴 요청
struct StrLoginREQ    //프로토콜에 REQ가 붙으면
{						//  뭔가 요청을 하는 패킷
	BYTE userid[maxUserIDLen];
	BYTE passwd[maxPasswdLen];
};

const WORD ComLoginACK = 2; // 서버에서 로긴결과 응답
struct StrLoginACK	// 프로토콜에 ACK가 붙으면
{					// REQ에 대한 응답을 의미하는 패킷
	BYTE result;
	// 0이면 로긴 성공, 아니면 실패
};
enum EnumLoginACK
{
	LoginACKConnectAllow = 0,		// 접속 허용
	LoginACKInvalidPasswd = 1,		// 패스워드 불일치
	LoginACKDuplicateConnect = 2,	// 한 아이디로 중복접속
};
// 나중에 다른 프로토콜의 에러메시지가 많아지거나
// 하면 다른 프로토콜의 에러와 헷갈리는 경우가
// 많이 생기므로 이렇게 같은 enum의 값들은
// 같은 형식으로 만드는 것이 좋다.

//-----------------------여기까지

// 채팅용프로토콜
const WORD ComChatREQ = 3;//서버로 문자열 보낸다. REQ
struct StrChatREQ
{
	BYTE chat[maxChatSize];
};

const WORD ComChatACK = 4;//서버에서는 성공 여부 반환 ACK
struct StrChatACK
{
	BYTE result; // 0이면 성공, 아니면 실패
};
enum EnumChatACK
{
	ChatACKSuccess = 0,		// 채팅성공
	ChatACKNotLogin = 1,	// 아직 로긴중
};

const WORD ComChatCMD = 5;//성공시 모든 클라이언트로 문자열 보낸다. CMD
struct StrChatCMD
{
	BYTE userid[maxUserIDLen];
	BYTE chat[maxChatSize];
};

//귓속말용 프로토콜을 설계한다.
const WORD ComWisperREQ = 6;// 누군가에게 귓속말 보내달라고 요청
struct StrWisperREQ
{
	BYTE toWhom[maxUserIDLen];  // 받을 사람 id
	//BYTE chat[maxChatSize];
	BYTE chat[0];
};
// 귓말로 채팅을 단 한글자(1바이트)만 보내더라도 패킷 전체의
//크기(maxChatSize)를 다 보내버리기 때문에 패킷의 낭비가 생길 수
//있다. 그러므로 필요한 만큼만 보낸다면 패킷의 낭비를 막을 수 있다.
// chat의 크기를 0으로 잡는다면 chat 자체는 위치만 가지고 있을 뿐
//크기는 가지지 않게 된다.
// 그러므로 패킷을 보낼 때 이 구조체 크기에 chat의 실제 길이를
//더해서 보낸다면 꼭 필요한 만큼의 패킷을 보내 통신량을 줄일 수
//있다.(참고 : 클라이언트의 SendWisper() 메서드)

const WORD ComWisperACK = 7;
struct StrWisperACK
{
	BYTE result;
};
enum EnumWisperACK
{
	WisperACKSuccess,
	WisperACKNotLogin, // 로긴되지 않은 상태에서 귓말을 보내려 할 경우
	WisperACKNotFound, // 상대를 찾을 수 없을 경우
};

const WORD ComWisperCMD = 8;
struct StrWisperCMD
{
	BYTE userID[maxUserIDLen];
	BYTE chat[maxChatSize];
};


/* 감정 표현 패킷 */
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
