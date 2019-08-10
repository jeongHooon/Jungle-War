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
	bool player_ready[MAX_PLAYER_SIZE] = { 0 };		// Player_Ready ��Ŷ �����ϸ� �ش� 
													// Client_ID�� �´� �迭 true
													// ��� true�� �Ǹ� ���� ���� �Լ� ����
	CHeightMapImage* height_map;
	time_point<system_clock> prev_time = system_clock::now();
	float sender_time = 0;
	float item_gen_timer = 0;
	bool is_item_gen = false;
	mutex client_lock;

	// Timer���� OverlappedExtensionSetd
	// MAX_PLAYER_SIZE  �÷��̾� ��ġ ������Ʈ ����
	// MAX_PLAYER_SIZE + 1 �÷��̾� �Ѿ� �浹üũ����
	// MAX_PLAYER_SIZE + 2 �÷��̾� �Ѿ� ����
	// MAX_PLAYER_SIZE + 3 �Ѿ� ������Ʈ
	// MAX_PLAYER_SIZE + 4 �ڽ� ������Ʈ
	// MAX_PLAYER_SIZE + 5 �ڽ� �Ѿ� �浹üũ����
	// MAX_PLAYER_SIZE + 6 �ڱ��� ������Ʈ
	// MAX_PLAYER_SIZE + 7 ������Ʈ �Ѿ� �浹üũ ����
	OverlappedExtensionSet ol_ex[OX_SIZE];

	Bullet bullets[MAX_PLAYER_SIZE * MAX_BULLET_SIZE];
	//Box boxes[MAX_PLAYER_SIZE][MAX_BOX_SIZE] = { 0 };
	Box boxes[MAX_PLAYER_SIZE * MAX_BOX_SIZE];
	mutex bullet_lock;
	// �÷��̾ �� ��° �Ѿ˱��� �߻��ߴ��� �����ϴ� ����
	int bullet_counter[MAX_PLAYER_SIZE] = { 0 };
	int box_counter[MAX_PLAYER_SIZE] = { 0 };

	Map_Object obj[MAX_OBJECT_SIZE] = { 0 };

	int box_count = 0;

	// �÷��̾�� bullet �ð��� ������ �ִ�. 
	float bullet_times[MAX_PLAYER_SIZE];
	float jumpAcc = 70.0f;

	int elecCount = 0;

	// Building obejct�� �� 10��
	//Object* object_mother;
	Building* building[OBJECT_BUILDING];

public:
	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// ��Ŷ ������ �����ؼ� �۽�
	void DisconnectPlayer(int cl_id);				// �÷��̾� ���� ����
	void magnetic();

													//void TimerFunc();
	ServerFramework();
	~ServerFramework();


	// �� �Լ��� ElaspsedTime�� �����ϴ� ������ �Լ��̴�.
	void TimerSend(duration<float>& elapsed_time);
	// ElapsedTime�� �޾ƿͼ� ������Ʈ �ϴ� �Լ��̴�. 
	void Update(duration<float>& elapsed_time);

	void GameStart();
};

