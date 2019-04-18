#pragma once

#define SERVER_IP			127.0.0.1
#define SERVER_PORT			4000
#define MAX_BUFFER_SIZE		4000
#define MAX_PACKET_SIZE		256
#define MAXIMUM_PLAYER		2
#define	WM_SOCKET			WM_USER + 1
#define CLIENT_BUF_SIZE		1024
#define MAX_BULLET_SIZE			30

// ���� Ŭ���̾�Ʈ �� �������� ���
#define RUN_SPEED				2.78f
#define METER_PER_PIXEL			20
#define WALK_SPEED				1.67f


//Player Height // ������ �߹ٴ��� �����Ƿ� �ʿ� ��?
#define PLAYER_HEIGHT		0.f

// Server To Client
#define SC_ENTER_PLAYER			1
#define SC_POS					2
#define SC_REMOVE_PLAYER		3
#define SC_PLAYER_MOVE			4
#define SC_PLAYER_LOOKVEC		5
#define SC_BULLET_POS			6	// Bullet Position
#define SC_COLLSION_PB			7	// Collsion Player to Bullet

// Server To Server
#define SS_COLLISION			8
#define SS_PLAYER_POS_UPDATE	9
#define SS_BULLET_GENERATE		10
#define SS_BULLET_UPDATE		11
#define SS_PLAYER_READY			12
#define SS_PLAYER_MOVE			13
#define SS_ITEM_GEN				14


// Client To Server
#define CS_KEY_PRESS_UP			1
#define CS_KEY_PRESS_DOWN		2
#define CS_KEY_PRESS_LEFT		3
#define CS_KEY_PRESS_RIGHT		4
#define CS_KEY_PRESS_SPACE		5
#define CS_KEY_PRESS_SHIFT		6
#define CS_KEY_PRESS_1			7
#define CS_KEY_PRESS_2			8
#define CS_LEFT_BUTTON_DOWN		9
#define CS_RIGHT_BUTTON_DOWN	10


#define CS_KEY_RELEASE_UP			11
#define CS_KEY_RELEASE_DOWN			12
#define CS_KEY_RELEASE_LEFT			13
#define CS_KEY_RELEASE_RIGHT		14
#define CS_KEY_RELEASE_SPACE		15
#define CS_KEY_RELEASE_SHIFT		16
#define CS_KEY_RELEASE_1			17
#define CS_KEY_RELEASE_2			18
#define CS_LEFT_BUTTON_UP			19
#define CS_RIGHT_BUTTON_UP			20
#define CS_MOUSE_MOVE				21

#define CS_PLAYER_READY		100
#define CS_PLAYER_READY_CANCLE 101
#define CS_PLAYER_TEAM_SELECT	102

enum GameMode {
	TEAM_MODE, MELEE
};
enum Team {
	NON_TEAM = 0, TEAM_1, TEAM_2, TEAM_3, TEAM_4
};
enum ARWeapons {
	NON_AR = 0
};
enum SubWeapons {
	NON_SUB = 0
};

// ����->Ŭ��
struct SC_PACKET_ENTER_PLAYER {
	BYTE size;
	BYTE type;
	WORD id;
	float x, y, z;
};

struct SC_PACKET_LOOCVEC {
	BYTE size;
	BYTE type;
	WORD id;
	DirectX::XMFLOAT3 look_vec;
};

struct SC_PACKET_POS {
	BYTE size;
	BYTE type;
	WORD id;
	float x, y, z;
};

struct SC_PACKET_COLLISION {
	BYTE size;
	BYTE type;
	WORD client_id;
	float x, y, z;
	float hp;
};


// Ŭ��->����
struct CS_PACKET_BIGGEST {
	BYTE size;
	BYTE type;
	WORD id;
	bool player_in[4];
};

struct CS_PACKET_KEYUP {
	BYTE size;
	BYTE type;
	DirectX::XMFLOAT3 look_vec;
};
struct CS_PACKET_KEYDOWN {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYLEFT {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYRIGHT {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEY1 {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEY2 {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYSPACE {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_KEYSHIFT {
	BYTE size;
	BYTE type;
};

struct CS_PACKET_MOUSEMOVE {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_LEFTBUTTON {
	BYTE size;
	BYTE type;
};

struct CS_PACKET_READY {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_MODE_SELECT {
	BYTE size;
	BYTE type;
	GameMode game_mode;	// false - Melee
						// true	- Team
};
struct CS_PACKET_TEAM_SELECT {
	BYTE size;
	BYTE type;
	Team team;
};

struct CS_PACKET_LOOK_VECTOR {
	BYTE size;
	BYTE type;
	DirectX::XMVECTOR look_vector;
};

struct SC_PACKET_REMOVE_PLAYER {
	BYTE size;
	BYTE type;
	WORD client_id;
};

struct SC_PACKET_BULLET {
	BYTE size;
	BYTE type;
	WORD id;
	WORD bullet_id;
	DirectX::XMFLOAT3 pos;

	float x, y, z;
};