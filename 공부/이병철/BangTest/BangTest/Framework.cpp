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
		printf("WSAStartup() ����\n");

	iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);  // ����� �Ϸ� ��Ʈ (�ؿ��� ���ڵ�)
								// CP�������ϰ����ϴ� Overlapped���� �ڵ�(INVALID_HANDLE_VALUE ���ο� IOCP����)
								// �̹� ������ CP�ڵ�
								// ����Ǵ� ���Ͽ� ���� ������ �����忡 �����ϴ� �뵵 
								// ���� ���� ������ ������ ��(0 �Է��ϸ� CPU���� ��ġ)
	if (iocp_handle == NULL)
		printf("����: CreateIoCompletionPort() ����\n");

	// �񵿱� ����� Listen ���� ����
	listen_socket = WSASocketW(AF_INET, SOCK_STREAM,      // Socket�� �ƴ� WSASocketW�� ���� ������ ������ ���� ����
			IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	int opt_val = TRUE;
	setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	// ���� �ɼ� => ����������ȣ, level ���� SOL_SOCKET or IPPROTO_TCP, 
	//             optname(���Ͽɼ��ǹ�ȣ),optval(����������), optlen(����ũ��)
	//             TCP_NODELAY Nagle ������� �ʰڴ�!
	if (listen_socket == INVALID_SOCKET)
		printf("listen_socket ���� ����\n");

	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);         // 9000�� ��Ʈ
	retval = ::bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR)
		printf("bind ����\n");

	retval = listen(listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		printf("listen ����\n");
}
