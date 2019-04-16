#pragma once
class CHeightMapImage;

struct Event {
	int id;
	int type;
	unsigned int start_time;
	int target;
};
class Comp {
public:
	bool operator() (const Event& left, const Event& right) {
		return (left.start_time > right.start_time);
	}
};
class ServerFramework
{
	WSADATA wsa;
	HANDLE iocp_handle;
	SOCKET listen_socket;
	SOCKADDR_IN server_addr;

	BOOL mode_selector;	// 

	Client clients[MAXIMUM_PLAYER];
	bool player_entered[4] = { 0 };
	bool player_ready[4] = { 0 };		// Player_Ready ��Ŷ �����ϸ� �ش� 
										// Client_ID�� �´� �迭 true
										// ��� true�� �Ǹ� ���� ���� �Լ� ����
	CHeightMapImage* height_map;
	time_point<system_clock> prev_time = system_clock::now();
	float sender_time = 0;
	mutex client_lock;

	// Timer���� OverlappedExtensionSetd
	// 4�� �÷��̾� ��ġ ������Ʈ ����
	// 5�� �浹üũ����
	// 6�� �÷��̾� �Ѿ� ����
	// 7�� �Ѿ� ������Ʈ
	OverlappedExtensionSet ol_ex[10];

	Bullet bullets[4][MAX_BULLET_SIZE] = { 0 };
	mutex bullet_lock;
	// �÷��̾ �� ��° �Ѿ˱��� �߻��ߴ��� �����ϴ� ����
	int bullet_counter[4] = { 0 };


	// �÷��̾�� bullet �ð��� ������ �ִ�. 
	float bullet_times[4];


	// Ÿ�̸� �籸��
	priority_queue < Event, vector<Event>, Comp> timer_queue;
public:
	void TimerThread();
	void AddTimer(int id, int type, unsigned int start_time);


	void InitServer();
	void AcceptPlayer();
	void WorkerThread();
	void SendPacket(int cl_id, void* packet);		//
	void ProcessPacket(int cl_id, char* packet);	// ��Ŷ ������ �����ؼ� �۽�
	void DisconnectPlayer(int cl_id);				// �÷��̾� ���� ����
	bool IsStartGame();

	// �� �Լ��� ElaspsedTime�� �����ϴ� ������ �Լ��̴�.
	//void TimerFunc();
	// ElapsedTime�� �޾ƿͼ� ������Ʈ �ϴ� �Լ��̴�. 
	void Update(duration<float>& elapsed_time);
	ServerFramework();
	~ServerFramework();
};

