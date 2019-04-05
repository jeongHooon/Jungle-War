// HW1.cpp: 응용 프로그램의 진입점을 정의합니다.
//
#pragma warning(disable : 4996)

#include "stdafx.h"
#include "HW2 cilent.h"

#define MAX_LOADSTRING 100

SOCKET sock; // 소켓
char IPAdd[30] = "127.0.0.1";

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", (LPCSTR)msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}
DWORD WINAPI ClientMain(LPVOID arg);

CImage character[MAX_PLAYER];
CImage character2;
POINT characterPos1 = { 400,0 }; //캐릭터 초기 위치 값
POINT characterPos2 = { 400,700 };
BOOL KeyBuffer[256];
int receiveBytes;
int sendBytes;

int retval;

struct SOCKETINFO
{
	bool connected;
	int client_id;
	POINT pos;
};

#pragma pack(1)
struct Client_Send_Struct {
	BOOL KeyBuffer[256];
	int client_id;
};
#pragma pack()

#pragma pack(1)
struct Client_Recv_Struct {
	int client_id;
	bool connected[MAX_PLAYER];
	POINT pos[MAX_PLAYER];
};
#pragma pack()

Client_Send_Struct CSS;
Client_Recv_Struct CRS;

SOCKETINFO clients[MAX_PLAYER];
WSABUF dataBuffer;
SOCKET listenSocket;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	printf("IP INPUT : ");
	scanf_s("%s", IPAdd, sizeof(IPAdd));

	

	// 윈속 초기화
	WSADATA  wsa;
	if (WSAStartup(MAKEWORD(2, 0), &wsa) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}


	// 1. 소켓생성
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");
		return 1;
	}


	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serverAddr.sin_port = htons(SERVERPORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(IPAdd);


	// 2. 연결요청
	if (connect(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("connect error\n");
		// 4. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return 1;
	}
	else
	{
		printf("connect!\n");
	}

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_HW2CILENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HW2CILENT));

	MSG msg;

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HW2CILENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HW1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT rcWindow = { 0, 0, 800, 800 };

	AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, false);
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		0, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DWORD black = RGB(0, 0, 0);
	DWORD white = RGB(255, 255, 255);

	HBRUSH BackBrush = CreateSolidBrush(RGB(255, 255, 255));

	HBRUSH BlackBrush = CreateSolidBrush(black);
	HBRUSH WhiteBrush = CreateSolidBrush(white);

	static RECT rectView;

	RECT backGround{ 0, 0, BOARDSIZE, BOARDSIZE };

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rectView);
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (clients[i].connected)
				character[i].Load(TEXT("no1.png"));
		}

		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_KEYDOWN:
		printf("키다운\n");
		if (wParam == VK_RIGHT)
		{
			KeyBuffer[VK_RIGHT] = true;
		}
		else if (wParam == VK_LEFT)
		{
			KeyBuffer[VK_LEFT] = true;
		}
		else if (wParam == VK_UP)
		{
			KeyBuffer[VK_UP] = true;
		}
		else if (wParam == VK_DOWN)
		{
			KeyBuffer[VK_DOWN] = true;
		}
		
		printf("키다운 recv전\n");
		receiveBytes = recv(listenSocket, (char*)&CRS, sizeof(CRS), 0);
		printf("키다운 recv후\n");
		if (receiveBytes > 0)
		{
			// 받은 소켓 값 넣기
			clients[CRS.client_id].client_id = CRS.client_id;

			for (int i = 0; i < 10; ++i)
			{
				cout << "Recv " << clients[i].pos.x << ", " << clients[i].pos.y << endl;
				clients[i].connected = CRS.connected[i];
				clients[i].pos = CRS.pos[i];
			}
		}
		else
		{
			if (CRS.client_id == -1)
			{
				closesocket(listenSocket);
			}
		}

		CSS.client_id = CRS.client_id;
		memcpy(&CSS.KeyBuffer, &KeyBuffer, sizeof(&KeyBuffer));

		if (CSS.client_id == -1)
		{
			cout << "수용 인원을 초과하였습니다." << endl;
			closesocket(listenSocket);
		}

		sendBytes = send(listenSocket, (char*)&CSS, sizeof(CSS), 0);
		if (sendBytes > 0)
		{
			/*cout << "Recv " << Move_char.points.x << ", " << Move_char.points.y << endl;
			receiveBytes = recv(listenSocket, (char*)&Move_char.points, sizeof(Move_char.points), 0);
			if (receiveBytes > 0)
			{
			}*/
			cout << "Send " << KeyBuffer << endl;
			memset(&CSS.KeyBuffer, 0, sizeof(&KeyBuffer));
		}
		else
		{
			err_quit((char*)"send()");
		}
		
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_KEYUP:
		KeyBuffer[wParam] = FALSE;
		//InvalidateRect(hWnd, NULL, TRUE);
		break;


	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.

		CImage img;
		img.Create(rectView.right, rectView.bottom, 24);

		HDC memDC = img.GetDC();
		RECT rc;
		FillRect(memDC, &backGround, BackBrush);

		for (int i = 0; i < BOARDNUM; ++i) {
			for (int j = 0; j < BOARDNUM; ++j) {

				rc.left = j * (BOARDSIZE / BOARDNUM);
				rc.top = i * (BOARDSIZE / BOARDNUM);

				rc.right = (j + 1)*(BOARDSIZE / BOARDNUM);
				rc.bottom = (i + 1)*(BOARDSIZE / BOARDNUM);

				if ((i + j) % 2 == 0)
					FillRect(memDC, &rc, BlackBrush);

			}
		}
		//보정 값 25
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (clients[i].connected)
			{
				character[i].Draw(memDC, 0 + clients[i].pos.x + 25, 0 + clients[i].pos.y + 25, CHARACTERSIZE, CHARACTERSIZE);
			}
		}
		img.Draw(hdc, 0, 0);
		img.ReleaseDC();
		//InvalidateRect(hWnd, NULL, TRUE);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


