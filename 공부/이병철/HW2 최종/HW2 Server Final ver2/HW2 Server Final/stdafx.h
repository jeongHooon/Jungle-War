#pragma once

#include <stdio.h>
#include <tchar.h>

#include <stdlib.h>
#include <string.h>

#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include <map>

using namespace std;

#define MAX_BUFFER        1024

int ClientCount = 0;
BOOL isStart = false;

POINT characterPos1 = { 400,0 }; //캐릭터 초기 위치 값
POINT characterPos2 = { 400,700 };

#define PLAYER1 1
#define PLAYER2 2

#define BOARDNUM 8
#define BOARDSIZE 800
#define CHARACTERSIZE 50

BOOL P1KeyBuffer[256];
BOOL P2KeyBuffer[256];


