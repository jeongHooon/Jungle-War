#pragma once
#include "../Chat.h"
#include <list>
#include <mutex>
using namespace std;

const int maxBuffer = 1024;
class ClientInfo : public WSAOVERLAPPED
{
public:
	// 모든 클라이언트의 정보를 관리해야 하므로
	// 각 클라이언트를 구분할 정보(핸들)가 필요
	static int UserHandle;
	int userHandle; // 이 유저핸들은 모두가 달라야 한다.
	char id[maxUserIDLen];  // 접속한 클라이언트의 id

	WSABUF dataBuffer; // IOCP에서 패킷을 채울 메모리
	SOCKET socket;
	char messageBuffer[maxBuffer];
	unsigned long recvByte;// 읽은 패킷 길이
	unsigned long flag;  // WSARecv의 작동 조정

	bool loginComplete;
	//	 LoginREQ가 성공하면 true로 바뀌며, 이것이 true일
	//   동안에만 채팅을 할 수 있다.
	ClientInfo();
	void Reset();
};

// 아래 변수들은 이미 ChattingServer.cpp에서
// 만들어져 있으므로 다른 파일에서 다시 만들면
// 동일한 변수가 여러개 있다는 에러가 나게 된다.
// 그러므로 여기서는 [이런 변수들은 다른 곳에서
// 만들어져 있으니 직접 만들지는 말고 참조만
// 해라]라는 것을 컴파일러에게 알려주어야 한다.
// 이럴때 사용하는 키워드가 extern이다.
extern list<ClientInfo *> clientList;
extern mutex mutexList;
