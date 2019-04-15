// HW1.cpp: ���� ���α׷��� �������� �����մϴ�.
//
#pragma warning(disable : 4996)

#include "stdafx.h"
#include "HW2 cilent.h"

#define MAX_LOADSTRING 100

SOCKET sock; // ����

char IPAdd[30] = "127.0.0.1";

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.


DWORD WINAPI ClientMain(LPVOID arg);

CImage character1;
CImage character2;
POINT characterPos1 = { 400,0 }; //ĳ���� �ʱ� ��ġ ��
POINT characterPos2 = { 400,700 };
BOOL KeyBuffer[256];

SOCKET listenSocket;

int retval;
int sendBytes;
int receiveBytes;

#pragma pack(1)
struct Client_Send_Struct {
	BOOL KeyBuffer[256];
};
#pragma pack()

#pragma pack(1)
struct Client_Recv_Struct {
	POINT p1;
};
#pragma pack()

struct SOCKETINFO
{
	bool connected;
	int client_id;
	POINT pos;
};

Client_Send_Struct CSS;
Client_Recv_Struct CRS;
SOCKETINFO clients[MAX_PLAYER];


// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	RECT rcWindow = { 0, 0, 800, 800 };


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

	HWND 	 hwnd;
	MSG 	 Msg;
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("Window Class Name");
	RegisterClass(&WndClass);
	hwnd = CreateWindow((LPCWSTR)szWindowClass, 
		(LPCWSTR)szTitle, 
		WS_OVERLAPPEDWINDOW,
		0, 
		0, 
		rcWindow.right - rcWindow.left, 
		rcWindow.bottom - rcWindow.top, 
		nullptr, 
		nullptr, 
		hInstance, 
		nullptr);

	// Winsock Start - winsock.dll �ε�
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	// 1. ���ϻ���
	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET) err_quit((char*)"socket()");

	// �������� ��ü����
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(IPAdd);

	// 2. �����û
	retval = connect(sock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"connect()");


	SOCKETINFO *socketInfo;
	DWORD sendBytes;
	DWORD receiveBytes;
	DWORD flags;

	while (1)
	{
		receiveBytes = recvn(listenSocket, (char *)&CRS, sizeof(CRS), 0);
		memcpy(&characterPos1, &CRS.p1, sizeof(characterPos1));
	}


	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;

	while ((GetMessage(&msg, nullptr, 0, 0)))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return (int)msg.wParam;
}



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
		character1.Load(TEXT("no1.png"));

		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
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
		switch (wParam)
		{
		case VK_LEFT:
		{
			KeyBuffer[VK_LEFT] = true;
			memcpy(&CSS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			retval = send(listenSocket, (char *)&CSS, sizeof(CSS), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display((char*)"send()");
				return 0;
			}
		}
		break;
		case VK_RIGHT:
		{
			KeyBuffer[VK_RIGHT] = true;
			memcpy(&CSS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			retval = send(listenSocket, (char *)&CSS, sizeof(CSS), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display((char*)"send()");
				return 0;
			}
		}
		break;
		case VK_DOWN:
		{
			KeyBuffer[VK_DOWN] = true;
			memcpy(&CSS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			retval = send(listenSocket, (char *)&CSS, sizeof(CSS), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display((char*)"send()");
				return 0;
			}
		}
		break;
		case VK_UP:
		{
			KeyBuffer[VK_UP] = true;
			memcpy(&CSS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			retval = send(listenSocket, (char *)&CSS, sizeof(CSS), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display((char*)"send()");
				return 0;
			}
		}
		break;
		case VK_SPACE:
			break;
		case VK_ESCAPE:
			exit(1);
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
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.

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
		//���� �� 25
		character1.Draw(memDC, 0 + characterPos1.x + 25, 0 + characterPos1.y + 25, CHARACTERSIZE, CHARACTERSIZE);
		img.Draw(hdc, 0, 0);
		img.ReleaseDC();
		//InvalidateRect(hWnd, NULL, TRUE);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		// closesocket()
		closesocket(listenSocket);

		// ���� ����
		WSACleanup();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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


