// -----------------------------------------------------
// Project. The boat 
// Server Framework 
// Start From 2018. 2. 18 
// Programed by Gunny
// -----------------------------------------------------


#include "stdafx.h"
#include "ServerFramework.h"

ServerFramework server_framework;
UINT player_counter = 0;
void InitServer();
void PlayingSession();
BOOL LoadAndSendInit();


int main() {
	InitServer();
	while (true) {
		// for������ ��ü ����?
		player_counter++;
		if (player_counter <= 4) {
			if (server_framework.AcceptClient(player_counter) == RUOK) {
				cout << player_counter << " player enterd" << endl;
			}
		}
		if (player_counter == 4) {
			break;
		}
	}
	cout << "4���� �÷��̾� ��� ���� ���� �Ϸ�" << endl;
	if (!LoadAndSendInit()) {
		cout << "LoadAndSendInit() ����" << endl;
		return 0;
	}
	while (true) {
		PlayingSession();
	}

	cout << "Server Quit" << endl;
	return 0;
}


void InitServer() {
	server_framework.Initialize();
}

void PlayingSession() {
	// 1. ������Ʈ 
	// 2. �浹üũ 
	// 3. ����->Ŭ���̾�Ʈ ������ �۽�
	server_framework.Update();

	server_framework.CollideCheck();
}

BOOL LoadAndSendInit() {
	// ���⼭ �÷��̾��� ��ġ
	// ���� Ÿ�̸� 
	// 

	return TRUE;
}