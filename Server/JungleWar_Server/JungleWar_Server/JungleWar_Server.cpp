// 2018.4.3 FrameWork2 제작시작
//
// 2018.4.5 플레이어 접속하면 다른 클라이언트에서도 
// 확인할 수 있게 해야함.
// 

#include "stdafx.h"
#include "ServerFramework.h"
#define NUM_OF_THREAD	3

ServerFramework server_framework;

// 시간에 관련된 변수
time_point<system_clock> prev_time = system_clock::now();

void InitInstance();
void AcceptPlayer();
void WorkerThread();
void TimerThreadFunc();

int main()
{
	vector<thread*> worker_vector;
	InitInstance();
	for (int i = 0; i < NUM_OF_THREAD; ++i)
		worker_vector.push_back(new thread{ WorkerThread });

	thread accept_thread{ AcceptPlayer };
	thread timer_thread{ TimerThreadFunc };

	for (auto th : worker_vector) {
		th->join();
		delete th;
	}
	accept_thread.join();
	timer_thread.join();
	return 0;
}

void InitInstance() {
	// WSA소켓들 리셋해주기
	server_framework.InitServer();
}

void AcceptPlayer() {
	while (true) {
		server_framework.AcceptPlayer();
	}
}

void WorkerThread() {
	server_framework.WorkerThread();
}

void TimerThreadFunc() {
	while (true) {
		Sleep(1);
		time_point<system_clock> cur_time = system_clock::now();
		duration<float> elapsed_time = cur_time - prev_time;
		server_framework.Update(elapsed_time);
		server_framework.TimerSend(elapsed_time);
		prev_time = cur_time;
	}
}