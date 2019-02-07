#include "Framework.h"
#include "global.h"

Framework::Framework()
{
}

Framework::~Framework()
{
}

void Framework::InitServer()
{
	srand(unsigned(time(NULL)));
	int retval = 0;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		printf("WSAStartup() 에러\n");

	iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);  // 입출력 완료 포트 (밑에는 인자들)
								// CP에연결하고자하는 Overlapped소켓 핸들(INVALID_HANDLE_VALUE 새로운 IOCP생성)
								// 이미 생성된 CP핸들
								// 연결되는 소켓에 관한 정보를 쓰데드에 전달하는 용도 
								// 동시 실행 가능한 스레드 수(0 입력하면 CPU수와 일치)
	if (iocp_handle == NULL)
		printf("최초: CreateIoCompletionPort() 에러\n");

	// 비동기 방식의 Listen 소켓 생성
	listen_socket = WSASocketW(AF_INET, SOCK_STREAM,      // Socket이 아닌 WSASocketW를 쓰는 이유는 윈플을 쓰기 떄문
			IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	int opt_val = TRUE;
	setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	// 소켓 옵션 => 소켓지정번호, level 보통 SOL_SOCKET or IPPROTO_TCP, 
	//             optname(소켓옵션의번호),optval(버퍼포인터), optlen(버퍼크기)
	//             TCP_NODELAY Nagle 사용하지 않겠다!
	if (listen_socket == INVALID_SOCKET)
		printf("listen_socket 생성 오류\n");

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);         // 9000번 포트
	retval = ::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR)
		printf("bind 에러\n");

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		printf("listen 에러\n");
}
