#include "stdafx.h"
#include "ServerFramework.h"
#include "CHeightMapImage.h"
#include "Building.h"
#include "Object.h"

void ErrorDisplay(const char* msg, int err_no) {
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s", msg);
	wprintf(L"에러%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);  
}

ServerFramework::ServerFramework()
{
}

ServerFramework::~ServerFramework()
{
	for (int i = 0; i < OBJECT_BUILDING; ++i) {
		delete building[i];
	}
	delete height_map;
}

void ServerFramework::InitServer() {
	srand(unsigned(time(NULL)));
	int retval = 0;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		printf("WSAStartup() 에러\n");

	iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (iocp_handle == NULL)
		printf("최초: CreateIoCompletionPort() 에러\n");

	// 비동기 방식의 Listen 소켓 생성
	listen_socket = WSASocketW(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	int opt_val = TRUE;
	setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
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

	XMFLOAT3 xmf3Scale(TERRAIN_SCALE * 4, TERRAIN_SCALE, TERRAIN_SCALE * 4);
	LPCTSTR file_name = _T("TerrainNew1.raw");
	height_map = new CHeightMapImage(file_name, 513, 513, xmf3Scale);

	client_lock.lock();
	clients[0].x = 731.f;
	clients[0].z = 669.f;

	clients[1].x = 600.f;
	clients[1].z = 500.f;

	clients[2].x = 600.f;
	clients[2].z = 950.f;

	clients[3].x = 650.f;
	clients[3].z = 900.f;
	
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		/*clients[i].x = 450.f;
		clients[i].z = 800.f;*/

		clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);
		clients[i].hp = 0;
	}
	client_lock.unlock();

	// PLayer OOBB 셋
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		clients[i].SetOOBB(XMFLOAT3(clients[i].x, clients[i].y, clients[i].z), XMFLOAT3(OBB_SCALE_PLAYER_X, OBB_SCALE_PLAYER_Y, OBB_SCALE_PLAYER_Z), XMFLOAT4(0, 0, 0, 1));
		clients[i].bounding_box.Center;
	}

	// Bullet의 OBB
	for (int j = 0; j < MAX_PLAYER_SIZE; ++j) {
		for (int i = 0; i < MAX_BULLET_SIZE; ++i) {
			bullets[j * MAX_BULLET_SIZE + i].SetOOBB(XMFLOAT3(bullets[j * MAX_BULLET_SIZE + i].x, bullets[j * MAX_BULLET_SIZE + i].y, bullets[j * MAX_BULLET_SIZE + i].z),
				XMFLOAT3(OBB_SCALE_BULLET_X, OBB_SCALE_BULLET_Y, OBB_SCALE_BULLET_Z),
				XMFLOAT4(0, 0, 0, 1));
			bullets[j * MAX_BULLET_SIZE + i].bounding_box.Center;
		}
	}

	// Box OBB
	for (int j = 0; j < MAX_PLAYER_SIZE; ++j) {
		for (int i = 0; i < MAX_BOX_SIZE; ++i) {
			boxes[j * MAX_BOX_SIZE + i].SetOOBB(XMFLOAT3(boxes[j * MAX_BOX_SIZE + i].x, boxes[j * MAX_BOX_SIZE + i].y, boxes[j * MAX_BOX_SIZE + i].z),
				XMFLOAT3(OBB_SCALE_BOX_X, OBB_SCALE_BOX_Y, OBB_SCALE_BOX_Z),
				XMFLOAT4(0, 0, 0, 1));
			boxes[j * MAX_BULLET_SIZE + i].bounding_box.Center;
			boxes[j * MAX_BOX_SIZE + i].hp = MAX_BOX_HP;
		}
	}

	// Map_Object OBB
	for (int i = 0; i < MAX_OBJECT_SIZE; ++i) {
		float xPosition;
		float zPosition;
		bool obj_state;

		if (i == 0) xPosition = 230, zPosition = 280, obj_state = OBJECT_ALIVE;
		else if (i == 1) xPosition = 110, zPosition = 270, obj_state = OBJECT_ALIVE;
		else if (i == 2) xPosition = 135, zPosition = 200, obj_state = OBJECT_ALIVE;
		else if (i == 3) xPosition = 260, zPosition = 340, obj_state = OBJECT_ALIVE;
		else if (i == 4) xPosition = 310, zPosition = 230, obj_state = OBJECT_ALIVE;
		else if (i == 5) xPosition = 360, zPosition = 370, obj_state = OBJECT_ALIVE;
		else if (i == 6) xPosition = 430, zPosition = 150, obj_state = OBJECT_ALIVE;

		else if (i == 7) xPosition = 510, zPosition = 310, obj_state = OBJECT_ALIVE;
		else if (i == 8) xPosition = 560, zPosition = 290, obj_state = OBJECT_ALIVE;
		else if (i == 9) xPosition = 570, zPosition = 190, obj_state = OBJECT_ALIVE;
		else if (i == 10) xPosition = 755, zPosition = 240, obj_state = OBJECT_ALIVE;
		else if (i == 11) xPosition = 665, zPosition = 360, obj_state = OBJECT_ALIVE;
		else if (i == 12) xPosition = 880, zPosition = 335, obj_state = OBJECT_ALIVE;
		else if (i == 13) xPosition = 795, zPosition = 420, obj_state = OBJECT_ALIVE;

		else if (i == 14) xPosition = 325, zPosition = 700, obj_state = OBJECT_ALIVE;
		else if (i == 15) xPosition = 350, zPosition = 550, obj_state = OBJECT_ALIVE;
		else if (i == 16) xPosition = 400, zPosition = 710, obj_state = OBJECT_ALIVE;
		else if (i == 17) xPosition = 420, zPosition = 860, obj_state = OBJECT_ALIVE;
		else if (i == 18) xPosition = 430, zPosition = 645, obj_state = OBJECT_ALIVE;
		else if (i == 19) xPosition = 450, zPosition = 550, obj_state = OBJECT_ALIVE;
		else if (i == 20) xPosition = 460, zPosition = 770, obj_state = OBJECT_ALIVE;

		else if (i == 21) xPosition = 560, zPosition = 840, obj_state = OBJECT_ALIVE;
		else if (i == 22) xPosition = 660, zPosition = 800, obj_state = OBJECT_ALIVE;
		else if (i == 23) xPosition = 590, zPosition = 690, obj_state = OBJECT_ALIVE;
		else if (i == 24) xPosition = 670, zPosition = 580, obj_state = OBJECT_ALIVE;
		else if (i == 25) xPosition = 730, zPosition = 720, obj_state = OBJECT_ALIVE;
		else if (i == 26) xPosition = 750, zPosition = 620, obj_state = OBJECT_ALIVE;
		else if (i == 27) xPosition = 830, zPosition = 620, obj_state = OBJECT_ALIVE;

		else if (i == 28) xPosition = 420, zPosition = 290, obj_state = OBJECT_ALIVE;
		else if (i == 29) xPosition = 424, zPosition = 370, obj_state = OBJECT_ALIVE;
		else if (i == 30) xPosition = 560, zPosition = 500, obj_state = OBJECT_ALIVE;
		else if (i == 31) xPosition = 474, zPosition = 440, obj_state = OBJECT_ALIVE;
		else if (i == 32) xPosition = 630, zPosition = 510, obj_state = OBJECT_ALIVE;
		else if (i == 33) xPosition = 600, zPosition = 440, obj_state = OBJECT_ALIVE;
		else if (i == 34) xPosition = 540, zPosition = 450, obj_state = OBJECT_ALIVE;

		else if (i == 35) xPosition = 520, zPosition = 390, obj_state = OBJECT_ALIVE;
		else if (i == 36) xPosition = 524, zPosition = 470, obj_state = OBJECT_ALIVE;
		else if (i == 37) xPosition = 560, zPosition = 300, obj_state = OBJECT_ALIVE;
		else if (i == 38) xPosition = 574, zPosition = 340, obj_state = OBJECT_ALIVE;
		else if (i == 39) xPosition = 530, zPosition = 410, obj_state = OBJECT_ALIVE;
		else if (i == 40) xPosition = 500, zPosition = 340, obj_state = OBJECT_ALIVE;
		else if (i == 41) xPosition = 540, zPosition = 450, obj_state = OBJECT_ALIVE;
		/*else if (i == 28) xPosition = 602, zPosition = 1122;
		else if (i == 29) xPosition = 3000, zPosition = 3000;*/

		float yPosition = height_map->GetHeight(xPosition, zPosition);

		obj[i].x = xPosition;
		obj[i].y = yPosition;
		obj[i].z = zPosition;
		obj[i].state = obj_state;

		obj[i].SetOOBB(XMFLOAT3(obj[i].x, obj[i].y, obj[i].z),
			XMFLOAT3(OBB_SCALE_TREE_X, OBB_SCALE_TREE_Y, OBB_SCALE_TREE_Z),
			XMFLOAT4(0, 0, 0, 1));
		obj[i].bounding_box.Center;
		obj[i].hp = MAX_OBJECT_HP;
	}

	// Map_Object2 OBB
	for (int i = 0; i < MAX_OBJECT2_SIZE; ++i) {
		float xPosition;
		float zPosition;

		if (i == 0) xPosition = 230, zPosition = 280;
		else if (i == 1) xPosition = 140, zPosition = 270;
		else if (i == 2) xPosition = 200, zPosition = 200;
		else if (i == 3) xPosition = 210, zPosition = 340;
		else if (i == 4) xPosition = 250, zPosition = 230;
		else if (i == 5) xPosition = 170, zPosition = 370;
		else if (i == 6) xPosition = 330, zPosition = 150;

		else if (i == 7) xPosition = 412, zPosition = 310;
		else if (i == 8) xPosition = 526, zPosition = 290;
		else if (i == 9) xPosition = 342, zPosition = 190;
		else if (i == 10) xPosition = 256, zPosition = 240;
		else if (i == 11) xPosition = 332, zPosition = 360;
		else if (i == 12) xPosition = 173, zPosition = 335;
		else if (i == 13) xPosition = 642, zPosition = 420;

		else if (i == 14) xPosition = 627, zPosition = 700;
		else if (i == 15) xPosition = 522, zPosition = 550;
		else if (i == 16) xPosition = 380, zPosition = 710;
		else if (i == 17) xPosition = 270, zPosition = 860;
		else if (i == 18) xPosition = 190, zPosition = 645;
		else if (i == 19) xPosition = 320, zPosition = 550;
		/*else if (i == 28) xPosition = 602, zPosition = 1122;
		else if (i == 29) xPosition = 3000, zPosition = 3000;*/

		float yPosition = height_map->GetHeight(xPosition, zPosition);

		obj2[i].x = xPosition;
		obj2[i].y = yPosition;
		obj2[i].z = zPosition;

		obj2[i].SetOOBB(XMFLOAT3(obj2[i].x, obj2[i].y, obj2[i].z),
			XMFLOAT3(OBB_SCALE_STONE_X, OBB_SCALE_STONE_Y, OBB_SCALE_TREE_Z),
			XMFLOAT4(0, 0, 0, 1));
		obj2[i].bounding_box.Center;
	}

	// building 
	XMFLOAT3 input_buffer[10];
	XMFLOAT3 input_extents[10];

	input_buffer[0] = XMFLOAT3{ 594.f,height_map->GetHeight(594.f,556.f) ,556.f };
	input_buffer[1] = XMFLOAT3{ 922.f,height_map->GetHeight(922.f,519.f) ,519.f };
	input_buffer[2] = XMFLOAT3{ 1152.f,height_map->GetHeight(1152.f,911.f) ,911.f };
	input_buffer[3] = XMFLOAT3{ 2168.f,height_map->GetHeight(2168.f,741.f) ,741.f };
	input_buffer[4] = XMFLOAT3{ 594.f,height_map->GetHeight(594.f,556.f) ,556.f };
	input_buffer[5] = XMFLOAT3{ 739.f,height_map->GetHeight(739.f,3526.f) ,3526.f };

	input_buffer[6] = XMFLOAT3{ 2516.f,height_map->GetHeight(2516.f,1589.f) ,1589.f };
	input_buffer[7] = XMFLOAT3{ 3071.f,height_map->GetHeight(3071.f,1906.f) ,1906.f };
	input_buffer[8] = XMFLOAT3{ 3251.f,height_map->GetHeight(3251.f,2721.f) ,2721.f };
	input_buffer[9] = XMFLOAT3{ 2106.f,height_map->GetHeight(2106.f,3322.f) ,3322.f };

	input_extents[0] = XMFLOAT3{ 100,100,100 };
	input_extents[1] = XMFLOAT3{ 100,100,100 };
	input_extents[2] = XMFLOAT3{ 100,100,100 };
	input_extents[3] = XMFLOAT3{ 100,100,100 };
	input_extents[4] = XMFLOAT3{ 100,100,100 };
	input_extents[5] = XMFLOAT3{ 100,100,100 };
	input_extents[6] = XMFLOAT3{ 100,100,100 };
	input_extents[7] = XMFLOAT3{ 100,100,100 };
	input_extents[8] = XMFLOAT3{ 100,100,100 };
	input_extents[9] = XMFLOAT3{ 100,100,100 };


	for (int i = 0; i < OBJECT_BUILDING; ++i) {
		input_extents[0] = XMFLOAT3{ 100,100,100 };
		building[i] = new Building;
		building[i]->SetPosition(input_buffer[i], input_extents[i]);
	}

	//for (int i = 0; i < OBJECT_BUILDING; ++i) {
	//   building[i] = new Building;
	//   XMFLOAT3 input_buffer = XMFLOAT3{ static_cast<float>(rand() % 4000), 0.f, static_cast<float>(rand() % 4000) };
	//   XMFLOAT3 input_extents = XMFLOAT3{ static_cast<float>(rand() % 50 + 10),static_cast<float>(rand() % 30 + 200), static_cast<float>(rand() % 50 + 10) };
	//   input_buffer.y = height_map->GetHeight(input_buffer.x, input_buffer.z);
	//   building[i]->SetPosition(input_buffer, input_extents);
	//   //building[i]->SetObbExtents(i)
	//   //printf("[%d]건물 위치 [%f, %f, %f] \n", i,
	//   //   building[i]->GetPosition().x,
	//   //   building[i]->GetPosition().y,
	//   //   building[i]->GetPosition().z);
	//}
}

void ServerFramework::AcceptPlayer() {
	SOCKADDR_IN c_addr;
	ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(SERVER_PORT);
	c_addr.sin_addr.s_addr = INADDR_ANY;
	int addr_len = sizeof(SOCKADDR_IN);

	int new_key = -1;
	auto client_socket = WSAAccept(listen_socket, reinterpret_cast<SOCKADDR*>(&c_addr), &addr_len, NULL, NULL);
	// Nagle알고리즘
	int opt_val = TRUE;
	setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	//
	printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

	int client_id = -1;

	client_lock.lock();
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		if (clients[i].in_use == false) {
			client_id = i;
			break;
		}
	}
	client_lock.unlock();
	
	if (client_id == -1) {
		printf("최대 유저 초과\n");
	}
	printf("[%d] 플레이어 입장\n", client_id);
	client_lock.lock();
	clients[client_id].s = client_socket;
	clients[client_id].ar_mag = 0;
	clients[client_id].sub_mag = 0;
	clients[client_id].ar_weapons = ARWeapons::NON_AR;
	clients[client_id].sub_weapons = SubWeapons::NON_SUB;
	client_lock.unlock();
	clients[client_id].is_ready = false;
	clients[client_id].is_running = false;
	clients[client_id].is_crouch = false;
	clients[client_id].is_q = false;
	clients[client_id].is_jump = false;
	clients[client_id].is_die = false;
	ZeroMemory(&clients[client_id].overlapped_ex.wsa_over, sizeof(WSAOVERLAPPED));
	clients[client_id].overlapped_ex.is_recv = true;
	clients[client_id].overlapped_ex.wsabuf.buf = clients[client_id].overlapped_ex.io_buffer;
	clients[client_id].overlapped_ex.wsabuf.len = sizeof(clients[client_id].overlapped_ex.io_buffer);
	clients[client_id].packet_size = 0;
	clients[client_id].prev_packet_size = 0;
	clients[client_id].team = Team::NON_TEAM;

	clients[client_id].boxCount = 10;

	clients[client_id].hp = 100.f;
	printf("%d번 플레이어 체력 %f\n",client_id, clients[client_id].hp);

	clients[client_id].elecX = 500.f;
	clients[client_id].elecY = 1000.f;
	clients[client_id].elecZ = 500.f;


	CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
		iocp_handle, client_id, 0);
	// 플레이어 입장 표시
	player_entered[client_id] = true;
	clients[client_id].in_use = true;
	unsigned long flag = 0;
	WSARecv(client_socket, &clients[client_id].overlapped_ex.wsabuf, 1, NULL,
		&flag, &clients[client_id].overlapped_ex.wsa_over, NULL);

	// 플레이어 입장했다고 패킷 보내줘야함.
	// 이 정보에는 플레이어의 초기 위치정보도 포함되어야 한다. 
	SC_PACKET_ENTER_PLAYER packet;
	packet.id = client_id;
	packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
	packet.type = SC_ENTER_PLAYER;
	packet.hp = clients[client_id].hp;
	packet.x = clients[client_id].x;
	packet.y = clients[client_id].y;
	packet.z = clients[client_id].z;
	packet.elecX = 500.f;
	packet.elecY = 1000.f;
	packet.elecZ = 500.f;

	
	SendPacket(client_id, &packet);
	//printf("%d 자기장 중심 %f \n", client_id, clients[client_id].elecX);
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		if (clients[i].in_use && (client_id != i)) {
			printf("%d 플레이어 입장 정보 전송\n", i);
			//printf("밑 %d 자기장 중심 %f \n", i, clients[i].elecX);
			SendPacket(i, &packet);
		}
	}

	// 건물 정보 보내주기
	/*for (int j = 0; j < OBJECT_BUILDING; ++j) {
		SC_PACKET_ENTER_PLAYER packet;
		packet.id = j;
		packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
		packet.type = SC_BUILDING_GEN;
		packet.x = building[j]->GetPosition().x;
		packet.y = building[j]->GetPosition().y;
		packet.z = building[j]->GetPosition().z;
		packet.size_x = building[j]->GetExtents().x;
		packet.size_y = building[j]->GetExtents().y;
		packet.size_z = building[j]->GetExtents().z;
		for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
			if (clients[i].in_use) {
				SendPacket(i, &packet);
			}
		}
	}*/

	// 해당 클라이언트에게도 다른 클라이언트의 위치를 보내줘야한당!~
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		ZeroMemory(&packet, sizeof(packet));
		if (i != client_id) {
			if (clients[i].in_use == true) {
				packet.id = i;
				packet.size = sizeof(SC_PACKET_ENTER_PLAYER);
				packet.type = SC_ENTER_PLAYER;
				clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);
				packet.x = clients[i].x;
				packet.y = clients[i].y;
				packet.z = clients[i].z;
				SendPacket(client_id, &packet);
				printf("%d에게 %d의 정보를 보낸다\n", client_id, i);
			}
		}
	}

}

void ServerFramework::ProcessPacket(int cl_id, char* packet) {
	CS_PACKET_KEYUP* packet_buffer = reinterpret_cast<CS_PACKET_KEYUP*>(packet);

	switch (packet_buffer->type) {
	case CS_KEY_PRESS_UP:
		clients[cl_id].is_move_foward = true;
		break;
	case CS_KEY_PRESS_DOWN:
		clients[cl_id].is_move_backward = true;
		break;
	case CS_KEY_PRESS_LEFT:
		clients[cl_id].is_move_left = true;
		break;
	case CS_KEY_PRESS_RIGHT:
		clients[cl_id].is_move_right = true;
		break;
	case CS_KEY_PRESS_CROUCH:
		clients[cl_id].is_crouch = true;
//		printf("clients[cl_id].is_crouch %d\n", clients[cl_id].is_crouch);
		break;

	case CS_KEY_PRESS_1:
		printf("[ProcessPacket] :: AR 무기 선택\n");
		break;
	case CS_KEY_PRESS_2:
		printf("[ProcessPacket] :: 권총 무기 선택\n");
		break;


	case CS_KEY_PRESS_SHIFT:
		clients[cl_id].is_running = true;
		break;
	case CS_KEY_PRESS_SPACE:
	//	clients[cl_id].is_jump = true;
		break;


	case CS_KEY_RELEASE_UP:
		clients[cl_id].is_move_foward = false;
		break;
	case CS_KEY_RELEASE_DOWN:
		clients[cl_id].is_move_backward = false;
		break;
	case CS_KEY_RELEASE_LEFT:
		clients[cl_id].is_move_left = false;
		break;
	case CS_KEY_RELEASE_RIGHT:
		clients[cl_id].is_move_right = false;
		break;
	case CS_KEY_RELEASE_CROUCH:
		clients[cl_id].is_crouch = false;
		break;
	case CS_KEY_RELEASE_1:
		break;
	case CS_KEY_RELEASE_2:
		break;

	case CS_KEY_RELEASE_SHIFT:
		clients[cl_id].is_running = false;
		break;
	case CS_KEY_RELEASE_SPACE:
		//clients[cl_id].is_jump = false;
		break;

	case CS_RIGHT_BUTTON_DOWN:
		clients[cl_id].is_right_click = true;
		break;
	case CS_RIGHT_BUTTON_UP:
		clients[cl_id].is_right_click = false;
		break;

	case CS_LEFT_BUTTON_DOWN:
		//printf("[%d] 플레이어 좌클릭\n", cl_id);
		clients[cl_id].is_left_click = true;
		ol_ex[6].command = SS_BULLET_GENERATE;
		ol_ex[6].shooter_player_id = cl_id;
		//ol_ex[6].elapsed_time = elapsed_time.count();
		PostQueuedCompletionStatus(iocp_handle, 0, 6, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[6]));
		break;
	case CS_LEFT_BUTTON_UP:
		clients[cl_id].is_left_click = false;
		break;

	case CS_KEY_PRESS_Q:
		printf("[ProcessPacket] :: Q누름 (오브젝트)\n");
		clients[cl_id].is_q = true;
		ol_ex[7].command = SS_BOX_GENERATE;
		ol_ex[7].box_player_id = cl_id;
		//ol_ex[7].elapsed_time = elapsed_time.count();
		PostQueuedCompletionStatus(iocp_handle, 0, 6, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[7]));

		break;
	case CS_KEY_RELEASE_Q:
		clients[cl_id].is_q = false;
		break;
	case PlayerDie:
		clients[cl_id].is_die = true;
		//clients[cl_id].player_status = 17;
		break;
	case CS_MOUSE_MOVE: {
		clients[cl_id].look_vec = packet_buffer->look_vec;
		SC_PACKET_LOOCVEC packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_LOOCVEC);
		packets.type = SC_PLAYER_LOOKVEC;
		packets.look_vec = clients[cl_id].look_vec;

		packets.elecCount = elecCount;

		// 플레이어가 뒤는 상황

		//if (clients[cl_id].is_left_click) {
		//   packets.player_status = 3;
		//}
		//else if (clients[cl_id].is_running) {
		//   packets.player_status = 2;
		//}
		// 걷지도 뛰지도 않는 상황

		if (clients[cl_id].is_left_click) {
			packets.player_status = 2;
		}
		else if (clients[cl_id].is_move_backward == false && clients[cl_id].is_move_foward == false &&
			clients[cl_id].is_move_left == false && clients[cl_id].is_move_right == false ) {
			//clients[cl_id].is_move_left == false && clients[cl_id].is_move_right == false && clients[cl_id].is_jump == false) {
			packets.player_status = 0;
		}
		// 걷는 상황 // 가만 0 총 2 앞런 1 앞 3 뒤 4 오 5 왼 6  크라우치 7 오뒤 8 왼뒤 9 뒤런 10
		else if ((clients[cl_id].is_move_foward)) {
			packets.player_status = 3;
			if (clients[cl_id].is_running)
				packets.player_status = 1;
		}
		else if ((clients[cl_id].is_move_left)) {
			packets.player_status = 6;
		}
		else if ((clients[cl_id].is_move_backward)) {
			packets.player_status = 4;
			if (clients[cl_id].is_running)
				packets.player_status = 10;
		}
		else if ((clients[cl_id].is_move_right)) {
			packets.player_status = 5;
		}
		//////////////////////////
		if ((clients[cl_id].is_crouch)) {
			packets.player_status = 7;
		}
		//if(clients[cl_id].is_die)
			//packets.player_status = 17;
		//////////////////////////
		for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
			if (clients[i].in_use) {
				SendPacket(i, &packets);
			}
		}
		break;
	}
	case CS_PLAYER_READY: {
		int ready_count = 0;
		printf("%d 플레이어 레디\n", cl_id);
		player_ready[cl_id] = true;
		for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
			if (player_ready[i]) {
				ready_count++;
			}
		}
		if (ready_count == MAX_PLAYER_SIZE) {
			GameStart();
		}
		break;
	}
	case CS_PLAYER_READY_CANCLE:
		printf("%d 플레이어 레디취소\n", cl_id);
		player_ready[cl_id] = false;
		break;
	case CS_PLAYER_TEAM_SELECT:
		break;


	case CS_PLAYER_LOGIN:
	//	printf("로그인!!");
		SC_PACKET_LOGIN_PLAYER packets;
		packets.id = cl_id;
		packets.size = sizeof(SC_PACKET_LOGIN_PLAYER);
		packets.type = SC_PLAYER_LOGIN;


//		strncpy_s((char *)login->passwd, maxPasswdLen, passwd, maxPasswdLen);
//		strncmp(packets.userid, (char *)clients[cl_id].id,
//			maxUserIDLen);

	//	packets.userid = clients[cl_id].id;

	//	cout << packets.userid << "로그인" << endl;
		cout << packets.id << "로그인" << endl;
		cout << cl_id << "로그인" << endl;

		cout << clients[cl_id].id << "로그인" << endl;
		break;
	}

}

void ServerFramework::GameStart() {
	// Timer은 
	printf("게임 시작\n");

	// 플레이어  위치 섞기
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		clients[i].x = rand() % 4000;
		clients[i].z = rand() % 4000;
		clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);
		clients[i].hp = 0;
	}
	client_lock.unlock();

	// OOBB 셋
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		//clients[i].SetOOBB(XMFLOAT3(0, 0, 0), XMFLOAT3(10.f, 10.f, 10.f), XMFLOAT4(0, 0, 0, 1));
		clients[i].SetOOBB(XMFLOAT3(clients[i].x, clients[i].y, clients[i].z), 
			XMFLOAT3(OBB_SCALE_PLAYER_X, OBB_SCALE_PLAYER_Y, OBB_SCALE_PLAYER_Z), 
			XMFLOAT4(0, 0, 0, 1));
	}

	// Bullet의 OBB
	for (int j = 0; j < MAX_PLAYER_SIZE; ++j) {
		for (int i = 0; i < MAX_BULLET_SIZE; ++i) {
			bullets[j * MAX_BULLET_SIZE + i].SetOOBB(XMFLOAT3(bullets[j * MAX_BULLET_SIZE + i].x, bullets[j * MAX_BULLET_SIZE + i].y, bullets[j * MAX_BULLET_SIZE + i].z),
				XMFLOAT3(OBB_SCALE_BULLET_X, OBB_SCALE_BULLET_Y, OBB_SCALE_BULLET_Z),
				XMFLOAT4(0, 0, 0, 1));
		}
	}

	// BOX의 OBB
	for (int j = 0; j < MAX_PLAYER_SIZE; ++j) {
		for (int i = 0; i < MAX_BOX_SIZE; ++i) {
			boxes[j * MAX_BOX_SIZE + i].SetOOBB(XMFLOAT3(boxes[j * MAX_BOX_SIZE + i].x, boxes[j * MAX_BOX_SIZE + i].y, boxes[j * MAX_BOX_SIZE + i].z),
				XMFLOAT3(OBB_SCALE_BOX_X, OBB_SCALE_BOX_Y, OBB_SCALE_BOX_Z),
				XMFLOAT4(0, 0, 0, 1));
			boxes[j * MAX_BOX_SIZE + i].hp = MAX_BOX_HP;
		}
	}

	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		ol_ex[i].command = SC_PLAYER_MOVE;
		PostQueuedCompletionStatus(iocp_handle, 0, i, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[i]));
	}
	is_item_gen = true;
}

void ServerFramework::WorkerThread() {
	unsigned long data_size = 0;
	unsigned long long client_id = 0;

	WSAOVERLAPPED* overlapped;

	while (true) {
		bool retval = GetQueuedCompletionStatus(iocp_handle, &data_size,
			&client_id, &overlapped, INFINITE);
		if (retval == FALSE) {
			printf("[WorkerThread::GQCS] 에러 ClientID : %d\n", client_id);
			if (data_size == 0) {
				DisconnectPlayer(client_id);
				continue;
			}
		}
		OverlappedExtensionSet* overlapped_buffer = reinterpret_cast<OverlappedExtensionSet*>(overlapped);
		if (overlapped_buffer->is_recv == true) {
			int recved_size = data_size;
			char* ptr = overlapped_buffer->io_buffer;
			while (recved_size > 0) {
				if (clients[client_id].packet_size == 0) {
					clients[client_id].packet_size = ptr[0];
				}
				int remain = clients[client_id].packet_size - clients[client_id].prev_packet_size;
				if (remain <= recved_size) {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						remain);
					ProcessPacket(static_cast<int>(client_id), clients[client_id].prev_packet);
					recved_size -= remain;
					ptr += remain;
					clients[client_id].packet_size = 0;
					clients[client_id].prev_packet_size = 0;
				}
				else {
					memcpy(clients[client_id].prev_packet + clients[client_id].prev_packet_size,
						ptr,
						recved_size);
					recved_size -= recved_size;
					ptr += recved_size;
				}
			}

			unsigned long rflag = 0;
			ZeroMemory(&overlapped_buffer->wsa_over, sizeof(WSAOVERLAPPED));
			int retval = WSARecv(clients[client_id].s, &overlapped_buffer->wsabuf, 1, NULL, &rflag, &overlapped_buffer->wsa_over, NULL);
			if (retval != 0) {
				int err_no = WSAGetLastError();
				if (err_no != WSA_IO_PENDING) {
					ErrorDisplay("Error in WorkerThread(Recv) : ", err_no);
				}
			}

		}
		// TimerThread에서 호출
		// 1/20 마다 모든 플레이어에게 정보 전송
		else if (overlapped_buffer->command == SS_ITEM_GEN) {
			printf("아이템 생성띠\n");
			SC_PACKET_ITEM_GEN packets;
			packets.size = sizeof(SC_PACKET_ITEM_GEN);
			packets.type = SC_ITEM_GEN;
			packets.x = rand() % 4000;
			packets.z = rand() % 4000;
			packets.y = height_map->GetHeight(packets.x, packets.z);
			for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
				if (clients[i].in_use == true) {
					SendPacket(i, &packets);
				}
			}
		}
		else if (overlapped_buffer->command == SC_PLAYER_MOVE) {
			if (clients[client_id].in_use) {
				SC_PACKET_POS packets;
				packets.id = client_id;
				packets.size = sizeof(SC_PACKET_POS);
				packets.type = SC_POS;

				packets.x = clients[client_id].x;
				packets.y = clients[client_id].y;
				packets.z = clients[client_id].z;
				packets.look_vec = clients[client_id].look_vec;
				packets.is_die = clients[client_id].is_die;

				//packets.elecCount = elecCount;

				if (clients[client_id].is_left_click) {
					packets.player_status = 2;
				}
				else if (clients[client_id].is_move_backward == false && clients[client_id].is_move_foward == false &&
					clients[client_id].is_move_left == false && clients[client_id].is_move_right == false) {
					packets.player_status = 0;
				}
				// 걷는 상황
				else if ((clients[client_id].is_move_foward)) {
					packets.player_status = 3;
					if (clients[client_id].is_running)
						packets.player_status = 1;
				}
				else if ((clients[client_id].is_move_left)) {
					packets.player_status = 6;
				}
				else if ((clients[client_id].is_move_backward)) {
					packets.player_status = 4;
				}
				else if ((clients[client_id].is_move_right)) {
					packets.player_status = 5;
				}
				else if ((clients[client_id].is_crouch)) {
					printf("크라우치 들어왔어\n");
					packets.player_status = 7;
				}
				/*else if (clients[client_id].is_jump) {
					packets.player_status = 15;
				}*/
				else if (clients[client_id].is_die) {
				packets.player_status = 17;
				}
				//packets.player_status = clients[client_id].is_running;
				//printf("높이 : %f\n", clients[client_id].y);
				for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
					if (clients[i].in_use) {
						SendPacket(i, &packets);
					}
				}
				ZeroMemory(overlapped_buffer, sizeof(OverlappedExtensionSet));
			}
		}
		else if (overlapped_buffer->command == SS_COLLISION) {
			for (int j = 0; j < MAX_PLAYER_SIZE; ++j) {
				for (int i = 0; i < MAX_PLAYER_SIZE * MAX_BULLET_SIZE; ++i) {
					if (bullets[i].in_use && clients[j].in_use) {
						ContainmentType containType = clients[j].bounding_box.Contains(bullets[i].bounding_box);
						switch (containType)
						{
						case DISJOINT:
						{
							//printf("충돌 안함ㅠ\n");
							break;
						}
						case INTERSECTS:
						{
							SC_PACKET_COLLISION packets;
							packets.size = sizeof(SC_PACKET_COLLISION);
							packets.type = SC_COLLSION_PB;
							packets.x = clients[j].bounding_box.Center.x;
							// 플레이어의 키 만큼 반영해서
							packets.y = clients[j].bounding_box.Center.y;
							packets.z = clients[j].bounding_box.Center.z;
							packets.client_id = j;
							//
							//clients[j].hp -= MAX_BULLET_DAMAGE;
							//
							//packets.hp = clients[j].hp;
							packets.hp = (-1) * MAX_BULLET_DAMAGE;
							printf("%d번 플레이어 체력 %f\n", j, clients[j].hp);
							if (!(clients[j].is_die))
							{
								clients[j].hp -= MAX_BULLET_DAMAGE;
								if (clients[j].hp <= 0) {
									printf("%d번 플레이어 죽음\n", j);
									clients[j].is_die = true;

									SC_PACKET_IS_DIE die_packet;
									die_packet.size = sizeof(SC_PACKET_IS_DIE);
									die_packet.type = SC_IS_DIE;
									die_packet.id = j;
									die_packet.is_die = clients[j].is_die;
									die_packet.look_vec = clients[j].look_vec;
									die_packet.player_status = 17;

									for (int k = 0; k < MAX_PLAYER_SIZE; ++k) {
										if (clients[k].in_use) {
											SendPacket(k, &die_packet);
										}
									}
								}
							}

							/*if ( clients[j].hp < 0.f) {
							clients[j].is_die = true;
							}*/

							SendPacket(j, &packets);
							printf("플레이어 - 총알 충돌 시작\n");
							bullets[i].in_use = false;
							break;
						}
						case CONTAINS:
							SC_PACKET_COLLISION packets;
							packets.size = sizeof(SC_PACKET_COLLISION);
							packets.type = SC_COLLSION_PB;
							packets.x = clients[j].bounding_box.Center.x;

							packets.z = clients[j].bounding_box.Center.z;
							packets.client_id = j;
							//
							//clients[j].hp -= MAX_BULLET_DAMAGE;
							//
							//packets.hp = clients[j].hp;

							packets.hp = (-1) * MAX_BULLET_DAMAGE;

							if (!(clients[j].is_die))
							{
								clients[j].hp -= MAX_BULLET_DAMAGE;
								if (clients[j].hp <= 0)
									clients[j].is_die = true;
							}

							printf("플레이어 - 총알 충돌\n");
							bullets[i].in_use = false;
							break;
						}
					}
				}
			}
		}
		else if (overlapped_buffer->command == SS_COLLISION_BB) {
			for (int j = 0; j < MAX_PLAYER_SIZE * MAX_BOX_SIZE; ++j) {
				for (int i = 0; i < MAX_PLAYER_SIZE * MAX_BULLET_SIZE; ++i) {
					if (bullets[i].in_use && boxes[j].in_use) {
						ContainmentType containType = boxes[j].bounding_box.Contains(bullets[i].bounding_box);
						switch (containType)
						{
						case DISJOINT:
						{
							//printf("충돌 안함ㅠ\n");
							break;
						}
						case INTERSECTS:
						{
							SC_PACKET_COLLISION_BB packets;
							packets.size = sizeof(SC_PACKET_COLLISION_BB);
							packets.type = SC_COLLSION_BB;
							packets.x = boxes[j].bounding_box.Center.x;
							// 플레이어의 키 만큼 반영해서
							packets.y = boxes[j].bounding_box.Center.y;
							packets.z = boxes[j].bounding_box.Center.z;
							//packets.client_id = j;
							//
							boxes[j].hp -= MAX_BULLET_DAMAGE;
							//
							packets.hp = boxes[j].hp;
							//packets.box_id = boxes[j].boxindex;
							packets.box_id = j;

							printf("맞은 박스는 %d야 index는 %d야\n", j, boxes[j].boxindex);

							if (boxes[j].hp < 1) {
								//packets.box_id = boxes[j].boxindex;
								boxes[j].in_use = false;
							}
							packets.in_use = boxes[j].in_use;

							for (int k = 0; k < MAX_PLAYER_SIZE; ++k)
							{
								if (clients[k].in_use)
									SendPacket(k, &packets);
							}
							//printf("박스 - 총알 충돌 시작\n");
							bullets[i].in_use = false;
							break;
						}
						case CONTAINS:
							SC_PACKET_COLLISION_BB packets;
							packets.size = sizeof(SC_PACKET_COLLISION_BB);
							packets.type = SC_COLLSION_BB;
							packets.x = boxes[j].bounding_box.Center.x;
							// 플레이어의 키 만큼 반영해서
							packets.y = boxes[j].bounding_box.Center.y;
							packets.z = boxes[j].bounding_box.Center.z;
							//packets.client_id = j;
							//
							boxes[j].hp -= MAX_BULLET_DAMAGE;
							//
							packets.hp = boxes[j].hp;
							packets.box_id = boxes[j].boxindex;

							printf("맞은 박스는 %d야 index는 %d야\n", j, boxes[j].boxindex);

							if (boxes[j].hp < 1) {
								packets.box_id = boxes[j].boxindex;
								boxes[j].in_use = false;
							}
							packets.in_use = boxes[boxes[j].boxindex].in_use;
							//packets.in_use = boxes[j].in_use;

							for (int k = 0; k < MAX_PLAYER_SIZE; ++k)
							{
								if (clients[k].in_use)
									SendPacket(k, &packets);
							}
							//printf("박스 - 총알 충돌 시작\n");
							bullets[i].in_use = false;
							break;
						}
					}
				}

			}
		}
		else if (overlapped_buffer->command == SS_PLAYER_POS_UPDATE) {
			for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
				//client_lock.lock();
				clients[i].client_lock.lock();

				if (clients[i].is_move_foward) {
					if (clients[i].is_running) {
						clients[i].z += clients[i].look_vec.z * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += clients[i].look_vec.x * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
					else {
						clients[i].z += clients[i].look_vec.z * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += clients[i].look_vec.x * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
				}
				if (clients[i].is_move_backward) {
					if (clients[i].is_running) {
						clients[i].z += (-1) * clients[i].look_vec.z * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += (-1) * clients[i].look_vec.x * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
					else {
						clients[i].z += (-1) * clients[i].look_vec.z * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += (-1) * clients[i].look_vec.x * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
				}
				if (clients[i].is_move_left) {
					if (clients[i].is_running) {
						clients[i].z += clients[i].look_vec.x * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += (-1) * clients[i].look_vec.z * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
					else {
						clients[i].z += clients[i].look_vec.x * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += (-1) * clients[i].look_vec.z * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
				}
				if (clients[i].is_move_right) {
					if (clients[i].is_running) {
						clients[i].z += (-1) * clients[i].look_vec.x * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += clients[i].look_vec.z * (RUN_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
					else {
						clients[i].z += (-1) * clients[i].look_vec.x * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
						clients[i].x += clients[i].look_vec.z * (WALK_SPEED * overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
					}
				}
				clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);

				//if (!clients[i].is_) {
				//	// 점프 아닐 시 y값 지정
				//	clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);

				//}
				//else if (clients[i].is_jump) {
				//	//	clients[i].y = height_map->GetHeight(clients[i].x, clients[i].z);
				//	clients[i].y += jumpAcc * (overlapped_buffer->elapsed_time) / METER_PER_PIXEL;
				//	//jumpAcc += 10.0f;
				//	jumpAcc -= 1.0f;
				//	if (clients[i].y <= height_map->GetHeight(clients[i].x, clients[i].z)) {
				//		clients[i].is_jump = false;
				//		jumpAcc = 70.0f;
				//	}
				//}

				clients[i].client_lock.unlock();
				clients[i].SetOOBB(XMFLOAT3(clients[i].x, clients[i].y, clients[i].z), XMFLOAT3(OBB_SCALE_PLAYER_X, OBB_SCALE_PLAYER_Y, OBB_SCALE_PLAYER_Z), XMFLOAT4(0, 0, 0, 1));
			}
		}
		else if (overlapped_buffer->command == SS_BULLET_GENERATE) {
			int shooter_id = overlapped_buffer->shooter_player_id;
			if (bullet_counter[shooter_id] > MAX_BULLET_SIZE - 2) {
				for (int d = 0; d < MAX_BULLET_SIZE; ++d) {
					bullets[shooter_id * MAX_BULLET_SIZE + d].in_use = false;
				}
				bullet_counter[shooter_id] = 0;
				printf("총알 초기화\n");
				//break;
			}
			bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].x = clients[shooter_id].x + 10 * clients[shooter_id].look_vec.x;
			bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].y = clients[shooter_id].y;
			bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].z = clients[shooter_id].z + 10 * clients[shooter_id].look_vec.z;
			bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].look_vec = clients[shooter_id].look_vec;
			bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].in_use = true;

			/*printf("%f         %f            %f\n", bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].x,
				bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].y,
				bullets[shooter_id * MAX_BULLET_SIZE + bullet_counter[shooter_id]].z);*/
			bullet_counter[shooter_id]++;
			bullet_times[shooter_id] = 0;
		}
		else if (overlapped_buffer->command == SS_BULLET_UPDATE) {
			// i 가 플레이어
			// j 가 플레이어가 발사한 총알
			for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
				for (int j = 0; j < MAX_BULLET_SIZE; ++j) {
					//bullet_lock.lock();
					if (bullets[i* MAX_BULLET_SIZE + j].in_use) {
						bullets[i* MAX_BULLET_SIZE + j].x += METER_PER_PIXEL * bullets[i* MAX_BULLET_SIZE + j].look_vec.x * (AR_SPEED * overlapped_buffer->elapsed_time);
						bullets[i* MAX_BULLET_SIZE + j].y += METER_PER_PIXEL * bullets[i* MAX_BULLET_SIZE + j].look_vec.y * (AR_SPEED * overlapped_buffer->elapsed_time);
						bullets[i* MAX_BULLET_SIZE + j].z += METER_PER_PIXEL * bullets[i* MAX_BULLET_SIZE + j].look_vec.z * (AR_SPEED * overlapped_buffer->elapsed_time);
						//printf("총알 진행중\n");

						bullets[i* MAX_BULLET_SIZE + j].SetOOBB(
							XMFLOAT3(bullets[i* MAX_BULLET_SIZE + j].x, bullets[i* MAX_BULLET_SIZE + j].y, bullets[i* MAX_BULLET_SIZE + j].z),
							XMFLOAT3(OBB_SCALE_BULLET_X, OBB_SCALE_BULLET_Y, OBB_SCALE_BULLET_Z),
							XMFLOAT4(0, 0, 0, 1));
					}
					if (bullets[i* MAX_BULLET_SIZE + j].x >= 4000.f || bullets[i* MAX_BULLET_SIZE + j].x <= 0) {
						bullets[i* MAX_BULLET_SIZE + j].in_use = false;
						//bullet_lock.unlock();
						continue;
					}
					if (bullets[i* MAX_BULLET_SIZE + j].y >= 4000.f || bullets[i* MAX_BULLET_SIZE + j].y <= 0) {
						bullets[i* MAX_BULLET_SIZE + j].in_use = false;
						//bullet_lock.unlock();
						continue;
					}
					if (bullets[i* MAX_BULLET_SIZE + j].z >= 4000.f || bullets[i* MAX_BULLET_SIZE + j].z <= 0) {
						bullets[i* MAX_BULLET_SIZE + j].in_use = false;
						//bullet_lock.unlock();
						continue;
					}



					//여기서 보내줘야지~
					if (bullets[i* MAX_BULLET_SIZE + j].in_use) {
						SC_PACKET_BULLET packets;
						packets.id = i;
						packets.size = sizeof(SC_PACKET_BULLET);
						packets.type = SC_BULLET_POS;
						packets.bullet_id = j;
						packets.x = bullets[i* MAX_BULLET_SIZE + j].x;
						packets.y = bullets[i* MAX_BULLET_SIZE + j].y;
						packets.z = bullets[i* MAX_BULLET_SIZE + j].z;
						// 해당 플레이어에게만 보내야함
						// 렉 걸려서 안보냄 충돌체크하고 HP만 감소
						//SendPacket(i, &packets);
					}
					//bullet_lock.unlock();
				}
			}
		}
		else if (overlapped_buffer->command == SS_BOX_GENERATE) {
			if (clients[overlapped_buffer->box_player_id].boxCount > 0.5) {

				int box_player_id = overlapped_buffer->box_player_id;

				/*if (box_counter[box_player_id] > MAX_BOX_SIZE - 2) {
					for (int d = 0; d < MAX_PLAYER_SIZE; ++d) {
						for (int f = 0; f < MAX_BOX_SIZE; ++f){
							boxes[d * MAX_BOX_SIZE + f].in_use = false;
						}
					}
					box_counter[box_player_id] = 0;
				}*/


				//clients[client_id].y = height_map->GetHeight(clients[client_id].x, clients[client_id].z);

				boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].x =
					clients[box_player_id].x + 10 * clients[box_player_id].look_vec.x;

				boxes[box_player_id * MAX_BOX_SIZE + box_counter[box_player_id]].z =
					clients[box_player_id].z + 10 * clients[box_player_id].look_vec.z;

				boxes[box_player_id * MAX_BOX_SIZE + box_counter[box_player_id]].y =
					height_map->GetHeight(boxes[box_player_id * MAX_BOX_SIZE + box_counter[box_player_id]].x,
						boxes[box_player_id * MAX_BOX_SIZE + box_counter[box_player_id]].z);

				boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].look_vec = clients[box_player_id].look_vec;
				boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].in_use = true;
				boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].boxindex = box_player_id * MAX_BOX_SIZE + box_counter[box_player_id];
				
				printf("너는 %d번째 박스야 %d번째라고\n", box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id], 
					boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].boxindex);

				boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]]
					.SetOOBB(XMFLOAT3(boxes[box_player_id * MAX_BOX_SIZE + box_counter[box_player_id]].x,
						boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].y,
						boxes[box_player_id  * MAX_BOX_SIZE + box_counter[box_player_id]].z),
						XMFLOAT3(OBB_SCALE_BOX_X, OBB_SCALE_BOX_Y, OBB_SCALE_BOX_Z),
						XMFLOAT4(0, 0, 0, 1));

				box_counter[box_player_id]++;
				//++clients[overlapped_buffer->box_player_id].box_count;
				--clients[overlapped_buffer->box_player_id].boxCount;

				for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
					for (int j = 0; j < MAX_BOX_SIZE; ++j) {
						if (boxes[i * MAX_BOX_SIZE + j].in_use) {
							SC_PACKET_BOX packets;
							packets.id = i;
							packets.size = sizeof(SC_PACKET_BOX);
							packets.type = SC_BOX_POS;
							packets.box_id = i * MAX_BOX_SIZE + j;
							packets.hp = MAX_BOX_HP;
							packets.in_use = true;
							packets.x = boxes[i * MAX_BOX_SIZE + j].x;
							packets.y = boxes[i * MAX_BOX_SIZE + j].y;
							packets.z = boxes[i * MAX_BOX_SIZE + j].z;
							packets.boxCount = clients[i].boxCount;
							printf("%d의 남은 박스는 %d개\n", i, packets.boxCount);


							for (int k = 0; k < MAX_PLAYER_SIZE; ++k)
								if (clients[k].in_use)
									SendPacket(k, &packets);
						}
					}
				}

			}
		}
		else if (overlapped_buffer->command == SS_BOX_UPDATE) {
			/*for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
			for (int j = 0; j < MAX_BOX_SIZE; ++j) {
			if (boxes[i * 10 + j].in_use) {
			SC_PACKET_BOX packets;
			packets.id = i;
			packets.size = sizeof(SC_PACKET_BOX);
			packets.type = SC_BOX_POS;
			packets.box_id = j;
			packets.x = boxes[i * 10 + j].x;
			packets.y = boxes[i * 10 + j].y;
			packets.z = boxes[i * 10 + j].z;

			for (int k = 0; k < MAX_PLAYER_SIZE; ++k)
			if (clients[k].in_use)
			SendPacket(k, &packets);
			}
			}
			}*/
		}
		else if (overlapped_buffer->command == SS_COLLISION_OB) {
			for (int j = 0; j < MAX_OBJECT_SIZE; ++j) {
				for (int i = 0; i < MAX_PLAYER_SIZE * MAX_BULLET_SIZE; ++i) {
					if (bullets[i].in_use && obj[j].in_use) {
						ContainmentType containType = obj[j].bounding_box.Contains(bullets[i].bounding_box);
						switch (containType)
						{
						case DISJOINT:
						{
							//printf("충돌 안함ㅠ\n");
							break;
						}
						case INTERSECTS:
						{
							SC_PACKET_COLLISION_OB packets;
							packets.size = sizeof(SC_PACKET_COLLISION_OB);
							packets.type = SC_COLLSION_OB;
							packets.x = obj[j].bounding_box.Center.x;
							// 플레이어의 키 만큼 반영해서
							packets.y = obj[j].bounding_box.Center.y;
							packets.z = obj[j].bounding_box.Center.z;
							//packets.client_id = j;
							//
							obj[j].hp -= MAX_BULLET_DAMAGE;
							//
							packets.hp = obj[j].hp;
							//packets.box_id = boxes[j].boxindex;
							packets.obj_id = j;

							printf("맞은 나무는 %d야\n", j);

							if (obj[j].hp < 1) {
								//packets.box_id = boxes[j].boxindex;
								obj[j].in_use = false;
							}
							packets.in_use = obj[j].in_use;

							for (int k = 0; k < MAX_PLAYER_SIZE; ++k)
							{
								if (clients[k].in_use)
									SendPacket(k, &packets);
							}
							//printf("박스 - 총알 충돌 시작\n");
							bullets[i].in_use = false;
							break;
						}
						case CONTAINS:
							SC_PACKET_COLLISION_OB packets;
							packets.size = sizeof(SC_PACKET_COLLISION_OB);
							packets.type = SC_COLLSION_OB;
							packets.x = obj[j].bounding_box.Center.x;
							// 플레이어의 키 만큼 반영해서
							packets.y = obj[j].bounding_box.Center.y;
							packets.z = obj[j].bounding_box.Center.z;
							//packets.client_id = j;
							//
							obj[j].hp -= MAX_BULLET_DAMAGE;
							//
							packets.hp = obj[j].hp;
							//packets.box_id = boxes[j].boxindex;
							packets.obj_id = j;

							printf("맞은 나무는 %d야\n", j);

							if (obj[j].hp < 1) {
								//packets.box_id = boxes[j].boxindex;
								obj[j].in_use = false;
							}
							packets.in_use = obj[j].in_use;

							for (int k = 0; k < MAX_PLAYER_SIZE; ++k)
							{
								if (clients[k].in_use)
									SendPacket(k, &packets);
							}
							//printf("박스 - 총알 충돌 시작\n");
							bullets[i].in_use = false;
							break;
						}
					}
				}

			}

			for (int j = 0; j < MAX_OBJECT2_SIZE; ++j) {
				for (int i = 0; i < MAX_PLAYER_SIZE * MAX_BULLET_SIZE; ++i) {
					if (bullets[i].in_use && obj2[j].in_use) {
						ContainmentType containType = obj2[j].bounding_box.Contains(bullets[i].bounding_box);
						switch (containType)
						{
						case DISJOINT:
						{
							//printf("충돌 안함ㅠ\n");
							break;
						}
						case INTERSECTS:
						{
							bullets[i].in_use = false;
							break;
						}
						case CONTAINS:
							bullets[i].in_use = false;
							break;
						}
					}
				}

			}
		}
		// Send로 인해 할당된 영역 반납
		else {
			delete overlapped_buffer;
		}
	}
}

void ServerFramework::SendPacket(int cl_id, void* packet) {
	OverlappedExtensionSet* overlapped = new OverlappedExtensionSet;
	char* send_buffer = reinterpret_cast<char*>(packet);

	memcpy(&overlapped->io_buffer, packet, send_buffer[0]);
	overlapped->is_recv = false;
	overlapped->wsabuf.buf = overlapped->io_buffer;
	overlapped->wsabuf.len = send_buffer[0];
	ZeroMemory(&overlapped->wsa_over, sizeof(WSAOVERLAPPED));
	unsigned long flag = 0;
	int retval = WSASend(clients[cl_id].s, &overlapped->wsabuf, 1, NULL, 0,
		&overlapped->wsa_over, NULL);

	if (retval != 0) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING) {
			ErrorDisplay("SendPacket에서 에러 발생 : ", err_no);
		}
	}
}

void ServerFramework::DisconnectPlayer(int cl_id) {
	// 플레이어 접속 끊기
	closesocket(clients[cl_id].s);
	clients[cl_id].in_use = false;
	printf("[DisconnectPlayer] ClientID : %d\n", cl_id);
	SC_PACKET_REMOVE_PLAYER packet;
	packet.client_id = cl_id;
	packet.size = sizeof(SC_PACKET_REMOVE_PLAYER);
	packet.type = SC_REMOVE_PLAYER;

	// 플레이어가 나갔다는 정보를 모든 클라이언트에 뿌려준다.
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		if (clients[i].in_use == true) {
			SendPacket(i, &packet);
		}
	}

}

void ServerFramework::Update(duration<float>& elapsed_time) {
	// 이유를 모르겠네 도대체;
	ol_ex[MAX_PLAYER_SIZE].command = SS_PLAYER_POS_UPDATE;
	ol_ex[MAX_PLAYER_SIZE].elapsed_time = elapsed_time.count();
	PostQueuedCompletionStatus(iocp_handle, 0, MAX_PLAYER_SIZE, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[MAX_PLAYER_SIZE]));

	ol_ex[MAX_PLAYER_SIZE + 1].command = SS_COLLISION;
	PostQueuedCompletionStatus(iocp_handle, 0, MAX_PLAYER_SIZE + 1, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[MAX_PLAYER_SIZE + 1]));

	// Bullet이 실제로 날아가는건 여기서 관리해야할거같다.
	ol_ex[MAX_PLAYER_SIZE + 3].command = SS_BULLET_UPDATE;
	ol_ex[MAX_PLAYER_SIZE + 3].elapsed_time = elapsed_time.count();
	PostQueuedCompletionStatus(iocp_handle, 0, MAX_PLAYER_SIZE + 3, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[MAX_PLAYER_SIZE + 3]));

	ol_ex[MAX_PLAYER_SIZE + 5].command = SS_COLLISION_BB;
	PostQueuedCompletionStatus(iocp_handle, 0, MAX_PLAYER_SIZE + 4, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[MAX_PLAYER_SIZE + 5]));

	//ol_ex[MAX_PLAYER_SIZE + 6].command = SS_COLLISION_MP;
	//PostQueuedCompletionStatus(iocp_handle, 0, MAX_PLAYER_SIZE + 6, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[MAX_PLAYER_SIZE + 6]));

	ol_ex[MAX_PLAYER_SIZE + 7].command = SS_COLLISION_OB;
	PostQueuedCompletionStatus(iocp_handle, 0, MAX_PLAYER_SIZE + 7, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[MAX_PLAYER_SIZE + 7]));



	// bool 변수 클라이언트마다 배정.
	// 시간값을 overlapped로 넘겨줘서 

	

	//ol_ex[8].command = SS_BOX_UPDATE;
	//ol_ex[8].elapsed_time = elapsed_time.count();
	//PostQueuedCompletionStatus(iocp_handle, 0, 8, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[8]));
}

void ServerFramework::TimerSend(duration<float>& elapsed_time) {
	sender_time += elapsed_time.count();
	if (sender_time >= UPDATE_TIME) {   // 1/60 초마다 데이터 송신
		for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
			if (clients[i].is_move_backward || clients[i].is_move_foward || clients[i].is_move_left || clients[i].is_move_right) {
				ol_ex[i].command = SC_PLAYER_MOVE;
				PostQueuedCompletionStatus(iocp_handle, 0, i, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[i]));
			}
			++elecCount;
		}
		sender_time = 0;
	}
	/*if (is_item_gen) {
	item_gen_timer += elapsed_time.count();
	if (item_gen_timer >= ITEM_GEN_TIME) {
	ol_ex[8].command = SS_ITEM_GEN;
	PostQueuedCompletionStatus(iocp_handle, 0, 0, reinterpret_cast<WSAOVERLAPPED*>(&ol_ex[8]));
	item_gen_timer = 0.f;
	is_item_gen = false;
	}
	}*/
}

void ServerFramework::magnetic()
{
}