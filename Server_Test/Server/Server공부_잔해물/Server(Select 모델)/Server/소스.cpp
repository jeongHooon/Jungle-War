#include "NetworkDeclare.h"

#define SERVERPORT 9000
#define BUFSIZE    512

struct SOCKETINFO {
	SOCKET sock;
	char buf[sizeof(CtsPacket)] = { 0 };
	int recvbytes;
	int sendbytes;
};

int nTotalSockets = 0;
SOCKETINFO* SocketInfoArray[FD_SETSIZE];

BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

int main() 
{
	int retval;


	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// �ͺ��ŷ�������� ��ȯ
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);

	// ���������� �ͺ�ŷ�̸� client ���ϵ� �ڵ����� �ͺ�ŷ�� �ȴ�.


	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;
	FD_SET read_set, write_set;
	char recv_buf[sizeof(CtsPacket)] = { 0 };
	CtsPacket packet_buffer;



	// ���� �Ǻ���
	while (true) {
		FD_ZERO(&read_set);
		FD_ZERO(&write_set);
		FD_SET(listen_sock, &read_set);

		for (int i = 0; i < nTotalSockets; ++i) {
			/*if (SocketInfoArray[i]->recvbytes > SocketInfoArray[i]->sendbytes)
				FD_SET(SocketInfoArray[i]->sock, &write_set);
			else
				FD_SET(SocketInfoArray[i]->sock, &read_set);*/
			FD_SET(SocketInfoArray[i]->sock, &read_set);
		}

		retval = select(0, &read_set, &write_set, NULL, NULL);
		if (retval == SOCKET_ERROR) err_quit("Select()");

		if (FD_ISSET(listen_sock, &read_set)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
			}
			else {
				// �������� �߰��ϱ�
				AddSocketInfo(client_sock);
				printf("Ŭ���̾�Ʈ IN\n");
			}
		}

		for (int i = 0; i < nTotalSockets; ++i) {
			SOCKETINFO* ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock, &read_set)) {
				// ������ �ޱ� ����
				retval = recvn(ptr->sock, ptr->buf, sizeof(CtsPacket), 0);
				if (retval == SOCKET_ERROR) {
					err_display("recvn()");
					RemoveSocketInfo(i);
					continue;
				}
				else if (retval == 0) {
					RemoveSocketInfo(i);
					continue;
				}
				ptr->recvbytes = retval;
				memcpy(&packet_buffer, ptr->buf, sizeof(ptr->buf));

				printf("%x \n", packet_buffer.keyboard_click);

			}
		}
	}


	CtsPacket test1;
	test1.keyboard_click = 0x00010101;
	test1.PushArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	test1.ReleaseArrowKeyUp();
	printf("%x \n", test1.keyboard_click);
	printf("%d\n", sizeof(test1));
}


BOOL AddSocketInfo(SOCKET sock){
	if (nTotalSockets >= FD_SETSIZE) {
		printf("���� ���� �߰� �Ұ�\n");
		return FALSE;
	}

	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("���ĸ� ����\n");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	SocketInfoArray[nTotalSockets++] = ptr;

	return TRUE;
}

void RemoveSocketInfo(int nIndex) {
	SOCKETINFO* ptr = SocketInfoArray[nIndex];

	SOCKADDR_IN clientaddr;

	int addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);

	closesocket(ptr->sock);
	delete ptr;

	if (nIndex != (nTotalSockets - 1))
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];

	--nTotalSockets;
}