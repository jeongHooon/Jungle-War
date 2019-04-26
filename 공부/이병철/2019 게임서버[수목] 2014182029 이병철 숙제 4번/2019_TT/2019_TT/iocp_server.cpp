#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>

using namespace std;

#include <winsock2.h>

#include "protocol.h"

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024

#define VIEW_RARADIUS		3

#define START_X		4
#define START_Y		4

struct OVER_EX
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuffer;
	char			messageBuffer[MAX_BUFFER];
	bool			is_recv;
};

struct SOCKETINFO
{
	mutex vl_lock;
	bool	connected;
	OVER_EX over; 
	SOCKET socket;
	char packet_buf[MAX_BUFFER]; 
	int prev_size;

	int x, y;
	unordered_set<int> viewlist;
};

SOCKETINFO clients[MAX_USER];

HANDLE g_iocp; 

void initialize()
{
	for (auto &cl : clients) {
		cl.connected = false;
		cl.viewlist.clear();
	}
}

char get_new_id()
{	
	while (true)
	{
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected == false)
			{
				clients[i].connected = true;
				return i;
			}
		}
	}
}

bool is_near_object(int a, int b)
{
	if (VIEW_RARADIUS < abs(clients[a].x - clients[b].x)) 
		return false;
	if (VIEW_RARADIUS < abs(clients[a].y - clients[b].y)) 
		return false;
	return true;
}

void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L"에러 " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}

void send_packet(int key, char *packet)
{
	SOCKET client_s = clients[key].socket;

	OVER_EX *over = new OVER_EX;

	over->dataBuffer.len = packet[0];
	over->dataBuffer.buf = over->messageBuffer;
	memcpy(over->messageBuffer, packet, packet[0]); 
	ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	over->is_recv = false;
	if (WSASend(client_s, &over->dataBuffer, 1, NULL, 0, &(over->overlapped), NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "Error - Fail WSASend(error_code : ";
			cout << WSAGetLastError() << ")\n";
		}
	}
}

void send_remove_player_packet(char to, char id)
{
	sc_packet_remove_player packet;
	packet.id = id;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;
	send_packet(to, reinterpret_cast<char *>(&packet));
}

void disconnect(int id)
{
	for (int i = 0; i < MAX_USER; ++i) 
	{
		if (false == clients[i].connected) 
			continue;
		clients[i].vl_lock.lock();
		if (0 != clients[i].viewlist.count(id))
		{
			clients[i].vl_lock.unlock();
			send_remove_player_packet(i, id);
		}
	}
	closesocket(clients[id].socket);
	clients[id].vl_lock.lock();
	clients[id].viewlist.clear();
	clients[id].vl_lock.unlock();
	clients[id].connected = false;
}

void send_login_ok_packet(char to)
{
	sc_packet_login_ok packet;
	packet.id = to;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_OK;
	send_packet(to, reinterpret_cast<char *>(&packet));
}

void send_put_player_packet(char to, char obj)
{
	sc_packet_put_player packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = clients[obj].x;
	packet.y = clients[obj].y;
	send_packet(to, reinterpret_cast<char *>(&packet));
}

void send_pos_packet(char to, char obj)
{
	sc_packet_pos packet;
	packet.id = obj;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = clients[obj].x;
	packet.y = clients[obj].y;
	send_packet(to, reinterpret_cast<char *>(&packet));
}

void process_packet(char id, char * buf)
{
	cs_packet_up *packet = reinterpret_cast<cs_packet_up *>(buf);

	char x = clients[id].x;
	char y = clients[id].y;
	switch (packet->type) 
	{
	case CS_UP:
		--y;
		if (y < 0) 
			y = 0;
		break;
	case CS_DOWN:
		++y;
		if (y >= WORLD_HEIGHT) 
			y = WORLD_HEIGHT - 1;
		break;
	case CS_LEFT: 
		if (0 < x) 
			x--; 
		break;
	case CS_RIGHT:
		if ((WORLD_WIDTH - 1) > x)
			x++; 
		break;
	default:
		cout << "Unknown Packet Type Error\n";
		while (true);
	}
	clients[id].x = x;
	clients[id].y = y;

	clients[id].vl_lock.lock();
	unordered_set<int> old_vl = clients[id].viewlist; // 이동 전
	clients[id].vl_lock.unlock();
	unordered_set<int> new_vl; // 이동 후
	// 이동 후에 플레이어 주변 플레이어들에게 알려준다.
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (true == clients[i].connected && is_near_object(id, i) && (i != id))
			new_vl.insert(i);
	}

	for (auto cl : new_vl)
	{
		if (0 != old_vl.count(cl)) // old, new 동시 존재
			if (0 != clients[cl].viewlist.count(id))
				send_pos_packet(cl, id);
			else
			{
				clients[cl].viewlist.insert(id);
				send_put_player_packet(cl, id);
			}
		else // 새로 시야에 들어옴
		{
			clients[id].viewlist.insert(cl);
			send_put_player_packet(id, cl);
			if (0 != clients[cl].viewlist.count(id))
				send_pos_packet(cl, id);
			else {
				clients[cl].viewlist.insert(id);
				send_put_player_packet(cl, id);
			}
		}
	}
	// remove
	for (auto cl : old_vl) { // 시야에서 사라짐
		if (0 != new_vl.count(cl))
			continue;
		clients[id].viewlist.erase(cl);
		send_remove_player_packet(id, cl);
		if (0 != clients[cl].viewlist.count(id))
		{
			clients[cl].viewlist.erase(id);
			send_remove_player_packet(cl, id);
		}
	}
	send_pos_packet(id, id);
}

void do_recv(char id)
{
	DWORD flags = 0;

	SOCKET client_s = clients[id].socket;
	OVER_EX *over = &clients[id].over;

	over->dataBuffer.len = MAX_BUFFER;
	over->dataBuffer.buf = over->messageBuffer;
	ZeroMemory(&(over->overlapped), sizeof(WSAOVERLAPPED));
	if (WSARecv(client_s, &over->dataBuffer, 1, NULL, &flags, &(over->overlapped), NULL) == SOCKET_ERROR)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)
		{
			error_display("RECV ERROR", err_no);
		}
	}
}

void worker_thread()
{
	while (true) {
		DWORD io_byte;
		ULONG key;
		OVER_EX *lpover_ex;
		BOOL is_error = GetQueuedCompletionStatus(g_iocp, &io_byte, &key,
			reinterpret_cast<LPWSAOVERLAPPED *>(&lpover_ex), INFINITE);

		if (FALSE == is_error)
		{
			int err_no = WSAGetLastError();
			if (64 != err_no)
				error_display("GQCS ", err_no);
			else
			{
				disconnect(key);
				continue;
			}
		}
			
		if (0 == io_byte) 
			disconnect(key);

		// 패킷 재조립
		if (lpover_ex->is_recv) 
		{
			int rest_size = io_byte;
			char *ptr = lpover_ex->messageBuffer;
			char packet_size = 0;
			if (0 < clients[key].prev_size) 
				packet_size = clients[key].packet_buf[0];
			while (rest_size > 0) {
				if (0 == packet_size) 
					packet_size = ptr[0];
				int required = packet_size - clients[key].prev_size;
				if (rest_size >= required) {
					memcpy(clients[key].packet_buf + clients[key].
						prev_size, ptr, required);
					process_packet(key, clients[key].packet_buf);
					rest_size -= required;
					ptr += required;
					packet_size = 0;
				}
				else {
					memcpy(clients[key].packet_buf + clients[key].prev_size,
						ptr, rest_size);
					rest_size = 0;
				}
			}
			do_recv(key);
		}
		else {
			delete lpover_ex;
		}
	}
}

void do_accept()
{
	// Winsock Start - windock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "Error - Can not load 'winsock.dll' file\n";
		return;
	}

	// 1. 소켓생성  
	SOCKET listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket\n";
		return;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Error - Fail bind\n";
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return;
	}

	// 3. 수신대기열생성
	if (listen(listenSocket, 5) == SOCKET_ERROR)
	{
		cout << "Error - Fail listen\n";
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return;
	}

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;
	DWORD flags;

	while (true)
	{
		clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen); // 클라이언트 socket 생성
		if (clientSocket == INVALID_SOCKET) // 클라이언트 socket 오류 발생시
		{
			cout << "Error - Accept Failure\n";
			return;
		}

		char new_id = get_new_id(); // 클라이언트 ID 생성

		//memset(&clients[new_id], 0x00, sizeof(struct SOCKETINFO));
		clients[new_id].socket = clientSocket;
		clients[new_id].over.dataBuffer.len = MAX_BUFFER;
		clients[new_id].over.dataBuffer.buf =
			clients[clientSocket].over.messageBuffer;
		clients[new_id].over.is_recv = true;
		clients[new_id].x = START_X;
		clients[new_id].y = START_Y;
		clients[new_id].viewlist.clear();
		clients[new_id].prev_size = 0;
		ZeroMemory(&clients[new_id].over.overlapped, sizeof(WSAOVERLAPPED));
		flags = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, new_id, 0);
		clients[new_id].connected = true;

		send_login_ok_packet(new_id); // 클라이언트 로그인 패킷
		send_put_player_packet(new_id, new_id);
		for (int i = 0; i < MAX_USER; ++i) {
			if (false == clients[i].connected) 
				continue;
			if (i == new_id)
				continue;
			if (true == is_near_object(i, new_id))
			{
				clients[i].viewlist.insert(new_id);
				send_put_player_packet(i, new_id);
			}
		}
		for (int i = 0; i < MAX_USER; ++i) {
			if (false == clients[i].connected) 
				continue;
			if (i == new_id) 
				continue;
			if (true == is_near_object(i, new_id))
			{
				clients[new_id].viewlist.insert(i);
				send_put_player_packet(new_id, i);
			}	
		}
		do_recv(new_id);
	}

	// 6-2. 리슨 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return;
}

int main()
{
	vector <thread> worker_threads;

	wcout.imbue(locale("korean"));
	initialize(); // 초기화
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // IOCP커널 객체 생성
	for (int i = 0; i < 4; ++i)
		worker_threads.emplace_back(thread{ worker_thread }); // Worker Thread 생성

	thread accept_thread{ do_accept };
	accept_thread.join();
	for (auto &th : worker_threads) 
		th.join();
	CloseHandle(g_iocp);
}

// 이동처리 ? accept 도 ㄱ 뷰리스트 넣어줘야함함함ㄴㅇㅁㄴㅇㅁㄴ 시비ㅏ