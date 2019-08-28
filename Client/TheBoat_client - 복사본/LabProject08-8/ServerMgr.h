#pragma once

// Server 에서 받아오는 Player의 정보 
struct SPlayer {
	XMFLOAT3 pos;
	float elecX, elecY, elecZ;
	int elecCount;
	int player_status;
	bool is_die;
	char playerID[10];
	char chat[20];
};
/////

class ServerMgr
{
	WSADATA wsa;
	SOCKET sock;
	SOCKADDR_IN server_addr;
	HWND async_handle;
	WSABUF send_wsabuf;
	WSABUF recv_wsabuf;
	int clients_id = 0;

	Bullet bullets[MAX_BULLET_SIZE] = { 0 };
	int recvd_bullet_id = 0;

	Box boxes[MAX_BOX_SIZE*MAX_PLAYER_SIZE] = { 0 };
	int recvd_box_id = 0;

	Map_Object obj[MAX_OBJECT_SIZE] = { 0 };
	Map_Object obj2[MAX_OBJECT2_SIZE] = { 0 };
	int recvd_obj_id = 0;

	bool first_set_id = true;

	char send_buffer[CLIENT_BUF_SIZE] = { 0 };
	//char send_buffer_vector[CLIENT_BUF_SIZE] = { 0 };
	char recv_buffer[CLIENT_BUF_SIZE] = { 0 };

	char packet_buffer[CLIENT_BUF_SIZE] = { 0 };
	DWORD in_packet_size = 0;
	DWORD saved_packet_size = 0;

	SPlayer sc_vec_buff[MAX_PLAYER_SIZE];
	XMFLOAT3 sc_look_vec;

	XMFLOAT3 collision_pos;
	float client_hp[MAX_PLAYER_SIZE] = { 0 };

	XMFLOAT3 collision_box_pos;
	float box_hp[MAX_PLAYER_SIZE * MAX_BOX_SIZE] = { 0 };

	XMFLOAT3 collision_obj_pos;
	float obj_hp[MAX_OBJECT_SIZE] = { 0 };

	int camera_id = 0;
	string server_ip;
	bool isplayerdead[MAX_PLAYER_SIZE] = { 0,0,0,0 };
	bool player_ready[MAX_PLAYER_SIZE] = { 0 };
	bool game_start = false;

	int myBoxCount = 10;

	// 로그인
	char loginID[maxUserIDLen];
	bool newMessage;

	// 아이템 생성 부분
	XMFLOAT3 item_pos;
	bool is_item_gen;

	XMFLOAT3 building_pos[OBJECT_BUILDING];
	XMFLOAT3 building_extents[OBJECT_BUILDING];

	bool s_is_collide = false;
	bool box_is_collide = false;
	bool obj_is_collide = false;
	
public:
	void IPInput();
	void Initialize(HWND& hwnd);
	void ClientError();
	void ReadPacket();
	void SendPacket(int type);
	void SendPacket(int type, XMFLOAT3& xmvector);
	void SendPacket(int type, char* id);
	void SendDeadPacket();



	void SetIsPlayerdead(int index) { isplayerdead[index] = true; }

	void ProcessPacket(char* ptr);
	void ErrorDisplay(const char* msg, int err_no);
	int GetElecCount();
	int GetClientID();
	int ReturnCameraID();
	bool GetGameStart() { return game_start; }
	bool GetPlayerReady(int input) { return player_ready[input]; }
	float GetBoxHp(int index) { return box_hp[index]; }
	bool GetBoxInuse(int index) { return boxes[index].in_use; }
	bool GetTreeInuse(int index) { return obj[index].in_use; }
	bool GetMessageCheck() { return newMessage;	}
	void SetMessageCheck() { !newMessage; }
	int GetBoxCount() { return myBoxCount; }
	Bullet GetBullet();
	Box GetBox(int index);
	SPlayer ReturnPlayerPosStatus(int client_id);
	XMFLOAT3 ReturnLookVector();
	XMFLOAT3 ReturnCollsionPosition(bool* is_collide);
	// 아이템 생성 후 위치 Return
	bool IsItemGen();
	XMFLOAT3 ReturnItemPosition();
	
	// 플레이어 체력
	float GetPlayerHP(int p_n);

	static int elecCount;
	static XMFLOAT3 elecPos;
	static bool damageCheck;

	// 
	void ReturnBuildingPosition(XMFLOAT3* building_pos);
	void ReturnBuildingExtents(XMFLOAT3* building_pos);
};