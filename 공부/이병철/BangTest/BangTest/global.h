#pragma once
#pragma comment(lib,"ws2_32")


using namespace DirectX;
using namespace std;

#define SERVER_IP			127.0.0.1
#define SERVER_PORT			4000
#define MAX_BUFFER_SIZE		4000
#define MAX_PACKET_SIZE		256
#define MAXIMUM_PLAYER		2
#define	WM_SOCKET			WM_USER + 1
#define CLIENT_BUF_SIZE		1024
#define MAX_BULLET_SIZE			30

#include <WinSock2.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <vector>
#include <thread>
#include <chrono>

#include <DirectXMath.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string> 
#include <wrl.h> 
#include <mutex>
#include <stdlib.h>
#include <DirectXCollision.h>
