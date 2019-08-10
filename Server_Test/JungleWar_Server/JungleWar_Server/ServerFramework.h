#pragma once

class CHeightMapImage;
class Building;

struct Event {
	int id;
	int type;
	float time;
	int target;
};

class Comp {
public:
	bool operator() (const Event& left, const Event& right) {
		return (left.time > right.time);
	}
};

class ServerFramework
{
	WSADATA wsa;
	HANDLE iocp_handle;
	SOCKET listen_socket;
	SOCKADDR_IN server_addr;

	BOOL mode_selector;	// 

	Client clients[MAX_PLAYER_SIZE];
	bool player_entered[MAX_PLAYER_SIZE] = { 0 };
	bool player_ready[MAX_PLAYER_SIZE] = { 0 };		// Player_Ready 패킷 도착하면 해당 
													// Client_ID에 맞는 배열 true
													// 모두 true가 되면 게임 시작 함수 실행
	CHeightMapImage* height_map;
	time_point<system_clock> prev_time = system_clock::now();
	float sender_time = 0;
	float item_gen_timer = 0;
	bool is_item_gen = false;
	mutex client_lock;

	// Timer전용 OverlappedExtensionSetd
	// MAX_PLAYER_SIZE  플레이어 위치 업데이트 전용
	// MAX_PLAYER_SIZE + 1 플레이어 총알 충돌체크전용
	// MAX_PLAYER_SIZE + 2 플레이어 총알 생성
	// MAX_PLAYER_SIZE + 3 총알 업데이트
	// MAX_PLAYER_SIZE + 4 박스 업데이트
	// MAX_PLAYER_SIZE + 5 박스 총알 충돌체크전용
	// MAX_PLAYER_SIZE + 6 자기장 업데이트
	// MAX_PLAYER_SIZE + 7 오브젝트 총알 충돌체크 전용
	OverlappedExtensionSet ol_ex[OX_SIZE];

	Bullet bullets[MAX_PLAYER_SIZE * MAX_BULLET_SIZE];
	//Box boxes[MAX_PLAYER_SIZE][MAX_BOX_SIZE] = { 0 };
	Box boxes[MAX_PLAYER_SIZE * MAX_BOX_SIZE];
	mutex bullet_lock;
	// 플레이어별 몇 번째 총알까지 발사했는지 저장하는 변수
	int bullet_counter[MAX_PLAYER_SIZE] = { 0 };
	int box_counter[MAX_PLAYER_SIZE] = { 0 };

	Map_Object obj[MAX_OBJECT_SIZE] = { 0 };

	int box_count = 0;

	// 플레이어마다 bullet 시간을 가지고 있다. 
	float bullet_times[MAX_PLAYER_SIZE];
	float jumpAcc = 70.0f;

	int elecCount = 0;

	// Building obejct는 총 10개
	//Object* object_mother;
	Building* building[OBJECT_BUILDING];

public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// 패킷 수신후 정리해서 송신
	void DisconnectPlayer(int cl_id);				// 플레이어 접속 해지
	void magnetic();

													//void TimerFunc();
	ServerFramework();
	~ServerFramework();


	// 이 함수는 ElaspsedTime을 측정하는 스레드 함수이다.
	void TimerSend(duration<float>& elapsed_time);
	// ElapsedTime을 받아와서 업데이트 하는 함수이다. 
	void Update(duration<float>& elapsed_time);

	void GameStart();
};

