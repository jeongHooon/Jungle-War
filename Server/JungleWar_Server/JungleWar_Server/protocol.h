#pragma once

#define SERVER_IP			127.0.0.1
#define SERVER_PORT			4000
#define MAX_BUFFER_SIZE		4000
#define MAX_PACKET_SIZE		256
#define MAX_PLAYER_SIZE		4
#define MAX_OBJECT_SIZE		90
#define MAX_OBJECT2_SIZE	20
#define	WM_SOCKET			WM_USER + 1
#define CLIENT_BUF_SIZE		1024
#define MAX_BULLET_SIZE		40
#define MAX_BOX_SIZE		10
#define OX_SIZE				30
#define TERRAIN_SCALE		0.5f
////////////////////////////////
#define maxUserIDLen		10
#define maxChatSize			20

// 본인 클라이언트 및 서버에서 사용
//#define RUN_SPEED				2.78f
// 위치 테스트용
#define RUN_SPEED				5.0f
#define METER_PER_PIXEL			0.243f
#define WALK_SPEED				2.78f
#define BOOST_SPEED				1.3f
#define JUMP_SPEED              40.0f
#define G_S 9.8f
#define MAX_BOX_HP				50.0f
#define MAX_BULLET_DAMAGE		25.0f
#define MAX_OBJECT_HP			75.0f

// Object 갯수 정리 
#define OBJECT_BUILDING			10

// Server To Client
#define SC_LOGIN_PLAYER			0
#define SC_ENTER_PLAYER			1
#define SC_POS					2
#define SC_REMOVE_PLAYER		3
#define SC_PLAYER_MOVE			4
#define SC_PLAYER_LOOKVEC		5
#define SC_BULLET_POS			6	// Bullet Position
#define SC_COLLSION_PB			7	// Collsion Player to Bullet
#define SC_COLLSION_BDP			8	// Building to Player
#define SC_COLLSION_BB			9	// Bullet Building

#define SC_ITEM_GEN				10	// Actually Item gen packet
#define SC_BUILDING_GEN			11
#define SC_COLLSION_OB			12
#define SC_COLLSION_OB2			13
#define SC_IS_DIE				14

#define SC_BOX_POS				21

///////////////////////////////////////
#define SC_PLAYER_LOGIN			51
#define SC_READY				52
#define SC_PLAYER_CHAT			53

// Server To Server
#define SS_COLLISION			12
#define SS_PLAYER_POS_UPDATE	13
#define SS_BULLET_GENERATE		14
#define SS_BULLET_UPDATE		15
#define SS_PLAYER_READY			16
#define SS_PLAYER_MOVE			17
#define SS_ITEM_GEN				18	// Event
#define SS_BOX_GENERATE			19
#define SS_BOX_UPDATE			20
#define SS_COLLISION_BB			21
#define SS_COLLISION_MP			22
#define SS_COLLISION_OB			23



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
#define CS_KEY_PRESS_CROUCH			22
#define CS_KEY_RELEASE_CROUCH		23
#define PlayerDie					26
#define CS_PLAYER_DIE				27
#define CS_ROOT_ITEM				28			
//=============================
#define CS_KEY_PRESS_Q 24
#define CS_KEY_RELEASE_Q 25
//=============================
#define CS_PLAYER_READY		100
#define CS_PLAYER_READY_CANCLE 101
#define CS_PLAYER_TEAM_SELECT	102

///////////////////////////////////////
#define CS_PLAYER_LOGIN 50
#define CS_PLAYER_CHAT	51
///////////////////////////////////////

#define OBJECT_ALIVE		0
#define OBJECT_DEAD			1

#define TYPE_NONE		0
#define TYPE_SPEED		1
#define TYPE_DEFENCE	2
#define TYPE_POWER		3

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

// 서버->클라
struct SC_PACKET_ENTER_PLAYER {
	BYTE size;
	BYTE type;
	WORD id;
	float x, y, z;
	// 건물 크기 보낼 때만 사용

	float elecX, elecY, elecZ;

	float hp;
	float size_x, size_y, size_z;
};

////////////////////////////

struct SC_PACKET_LOGIN_PLAYER {
	BYTE size;
	BYTE type;
	WORD id;
	char userid[10];
};

struct SC_PACKET_CHAT {
	BYTE size;
	BYTE type;
	WORD id;
	char chat[20];
};

struct SC_PACKET_LOOCVEC {
	BYTE size;
	BYTE type;
	WORD id;
	DirectX::XMFLOAT3 look_vec;
	int player_status;

	int elecCount;
};

struct SC_PACKET_POS {
	BYTE size;
	BYTE type;
	WORD id;
	DirectX::XMFLOAT3 look_vec;
	int player_status;
	float x, y, z;
	bool is_die;

	//int elecCount;
};

struct SC_PACKET_IS_DIE {
	BYTE size;
	BYTE type;
	WORD id;
	DirectX::XMFLOAT3 look_vec;
	int player_status;
	bool is_die;
	//int elecCount;
};

struct SC_PACKET_COLLISION {
	BYTE size;
	BYTE type;
	WORD client_id;
	float x, y, z;
	float hp;

	float hp_p1;
};

struct SC_PACKET_COLLISION_BB {
	BYTE size;
	BYTE type;
	WORD client_id;
	WORD box_id;
	float in_use;
	float x, y, z;
	float hp;
};

struct SC_PACKET_COLLISION_OB {
	BYTE size;
	BYTE type;
	WORD client_id;
	WORD obj_id;
	float in_use;
	float x, y, z;
	float hp;
};


struct SC_PAKCET_CLIENT_BUILDING_COLLSION {

};

struct SC_PACKET_ITEM_GEN {
	BYTE size;
	BYTE type;
	float x, y, z;
};

struct SC_PACKET_READY {
	BYTE size;
	BYTE type;
	bool player_ready[MAX_PLAYER_SIZE];
	bool game_start = false;
};


// 클라->서버
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
	bool isPlayerdead[MAX_PLAYER_SIZE] = { false };
};

struct CS_PACKET_LOBBY {
	BYTE size;
	BYTE type;
	char userID[10];
	char chatbuffer[20];

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

struct CS_PACKET_ROOT_ITEM {
	BYTE size;
	BYTE type;
	int skill;
};

//===============================================
struct CS_PACKET_Q_BUTTON {
	BYTE size;
	BYTE type;
};
//===============================================

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

struct SC_PACKET_BOX {
	BYTE size;
	BYTE type;
	WORD id;
	WORD box_id;

	int boxCount[MAX_PLAYER_SIZE];
	bool in_use;
	float hp;
	DirectX::XMFLOAT3 pos;

	float x, y, z;
};

