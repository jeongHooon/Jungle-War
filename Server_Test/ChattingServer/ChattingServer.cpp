// ChattingServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

// 채팅시스템을 위한 서버 프로그램

#include "stdafx.h"
#include "../Chat.h"
#include <process.h>
#include <list>
#include <mutex>

unsigned int __stdcall ChattingPacketProcess(void *arg);
const int maxWorkingThread = 8;

// 접속한 유저와 소켓의 정보를 가지고 있는 객체 필요
// 1. RSARecv 함수에서 패킷이 들어왔을때 워킹스레드로
//    전달해야 하는 객체
// 2. 채팅 프로그램은 한 사람의 말을 전체 사람들에게
//    전달해야 하므로 접속중인 모든 사람들의 정보를
//    가지고 있어야 한다.
// 그러므로 소켓과 그 소켓에 연결된 유저의 정보가 필요

#include "ClientInfo.h"

// 채팅서버는 한사람의 채팅을 모든 사람에게 전달해야 하므로
// 클라이언트 정보(ClientInfo)를 모아서 관리해야 한다.
list<ClientInfo *> clientList;
mutex mutexList;

int _tmain(int argc, _TCHAR* argv[])
{
	//1. winsock 초기화
	WSADATA wsaData;
	int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (startupResult != 0) //0이 아니면 에러
	{
		cout << "윈속 초기화 실패!" << endl;
		return 1;
	}

	// 2. 리슨소켓 생성
	// 리슨소켓은 포트를 감시하며 클라이언트 접속이
	// 있는지를 확인한다.
	SOCKET listenSocket =
		WSASocket(
		AF_INET, // 4바이트 IP 주소
		SOCK_STREAM, // 믿을수 있는 양방향 통신
		0,   // 프로토콜 특성, 0
		nullptr, // 프로토콜 정보, nullptr이면 기본값
		0,  // 소켓 그룹
		WSA_FLAG_OVERLAPPED);
	// 입출력 여러개가 동시에 중첩되어 진행될
	// 수 있다.
	if (listenSocket == INVALID_SOCKET)
	{// 소켓이 잘못 만들어졌다.
		cout << "리슨소켓 생성 실패" << endl;
		WSACleanup();
		// 위에서 윈속 초기화를 했으므로
		// 끝내기 전에 윈속 해제를 해야 한다.
		return 1;
	}

	//3. listenSocket에게 감시해야 할 포트
	// 등 필요한 속성을 bind해 주어야 한다.
	// 필요한 정보를 SOCKADDR_IN이라는
	// 구조체에 넣어 bind해야 함
	SOCKADDR_IN sockInfo;
	// 구조체 클리어 - 구조체 전체를 0으로 채운다.
	memset(&sockInfo, 0, sizeof(sockInfo));
	// 필요한 데이터 세팅
	sockInfo.sin_family = PF_INET; // 4바이트 주소 체계
	sockInfo.sin_port = serverPort;
	sockInfo.sin_addr.S_un.S_addr
		= htonl(INADDR_ANY); // 모든 클라이언트에서 접속 가능

	// 이 구조체를 이용해서 listenSocket에 바인드
	/*
		bind는 원래 소켓을 사용하는 함수.
		하지만 #include <list>에서 std라는 네임스페이스 안의
		bind라는 함수가 존재하므로 이 bind라는 함수를 제대로
		찾지 못한다.
		만약 std라는 네임스페이스 안의 bind라면 std::bind로
		확정할 수 있으나, namespace를 가지지 않은 bind라면
		::bind()로 찾을 수 있다. 즉 namespace 없이 ::으로
		시작하는 함수나 변수는 namespace를 가지지 않는 것을
		의미한다.
	*/
	int bindResult = 
		::bind(listenSocket,// 바인드할 소켓(std안에 있는 bind가 아니라
						//namespace에 속하지 않은 bind를 의미)
			(SOCKADDR *)&sockInfo, // 소켓에 연결할 구조체
			sizeof(sockInfo)	// 구조체의 크기
		);
	if (bindResult == SOCKET_ERROR)
	{
		cout << "listenSocket 바인드 에러" << endl;
		WSACleanup();
		return 1;
	}

	//4. bind로 소켓에 정보를 추가하였으므로
	// 리슨소켓이 포트를 감시하도록 해야 한다.
	int listenResult =
		listen(listenSocket, 5);
	// listen함수에 의해, listenSocket은 해당
	// 포트를 감시하며, 클라이언트 접속이 있으면
	// 그 클라이언트와 연결된 소켓을 만들어
	// 내부에 보관한다.
	// (최대 두번째 인수에 정해진 만큼)
	if (listenResult == SOCKET_ERROR)
	{
		cout << "listen() 호출 실패" << endl;
		WSACleanup();
		return 1;
	}
	////// 리슨소켓 완료 //////
	////// IOCP 객체 만들기/////
	//5. IOCP 객체 만들기
	HANDLE iocpHandle =
		CreateIoCompletionPort(
			INVALID_HANDLE_VALUE, // 최초 만들때 소켓이 없으므로
			nullptr, // 역시 최초에 만들때 IOCP 핸들이 없으므로
			0, // 역시 최초로 만듦으로
			maxWorkingThread); // 최대 워킹스레드 갯수
	// IOCP 객체를 새로 만들었음.
	if (iocpHandle == INVALID_HANDLE_VALUE)
	{
		cout << "iocp 객체 생성 실패" << endl;
		WSACleanup();
		return 1;
	}



	//6. IOCP용 Working Thread 생성
	for (int cnt = 0; cnt < maxWorkingThread; ++cnt)
	{
		_beginthreadex(
			nullptr, // 보안관련이므로 기본 nullptr
			0, // 스택크기, 0이면 기본크기로 세팅
			ChattingPacketProcess, // 함수 이름
			&iocpHandle,	// 스레드로 iocp핸들을 전달한다.
			0,	// 스레드의 초기상태 - 스레드 생성과 함께 실행
			nullptr //스레드 ID를 가져올 필요가 없다.
			);
	}

	/////// 리슨소켓과 iocp 모두 준비 끝 //////
	/////// 채팅 프로그램 시작 ///////
	while (true)
	{
		//7. 클라이언트에서의 접속 확인
		// accept함수로서 listenSocket에 모여있는
		// 클라이언트로의 소켓을 하나씩 꺼내 온다.
		SOCKADDR_IN clientInfo;
		int size = sizeof(clientInfo);
		SOCKET sockToCli = //클라이언트로의 소켓
			accept(listenSocket, // 리슨소켓에서 받아온다.
				(SOCKADDR *)&clientInfo,// 클라이언트 정보
				&size); // 구조체 크기
		if (sockToCli == INVALID_SOCKET)
		{
			cout << "소켓 억셉트 실패" << endl;
			// 여기서는 이미 많은 사람들이 채팅을
			// 하고 있는 상황. 그러므로 여기서 서버를
			// 종료해 버린다면 큰 문제.
			// 잘못된 소켓은 버리고 반복문 다시 실행
			continue;
		}
		// 소켓 확인
		cout << "클라이언트 접속 확인" << endl;

		// 8. 접속된 소켓을 IOCP에 등록
		ClientInfo *cliInfo = new ClientInfo();
		// 새로 접속한 클라이언트의 정보
		cliInfo->socket = sockToCli;

		// 여기서 만든 소켓정보를 리스트에 저장한다.
		mutexList.lock();
		clientList.push_back(cliInfo);
		mutexList.unlock();


		iocpHandle =
			CreateIoCompletionPort(
				(HANDLE)sockToCli, //등록할 소켓
				iocpHandle, // 이전에 만들어 놓은 핸들
				(ULONG_PTR)cliInfo,  // 입력을 구분하기 위한 키값
				maxWorkingThread
				);

		// 9. WorkingThread와 연결시키기 위해
		// 패킷 접수가 끝났을 경우 워킹스레드를
		// 실행시키기 위한 함수
		int recvResult =
			WSARecv(cliInfo->socket,
				&cliInfo->dataBuffer,
				1,
				&cliInfo->recvByte,
				&cliInfo->flag,
				cliInfo, //이 객체정보가 워킹스레드로 전달됨
				nullptr
				);
		if (recvResult != 0) // 뭔가 에러
		{
			// pending은 데이터 송수신 중이므로 에러가 아님
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				cout << "Pending Error" << endl;
				continue;
			}
		}



	}
	return 0;
}


