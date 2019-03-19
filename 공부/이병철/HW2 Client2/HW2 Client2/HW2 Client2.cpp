// HW2 Client2.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "HW2 Client2.h"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

CImage character1;
CImage character2;

POINT character1Pos = { 400,0 }; //ĳ���� �ʱ� ��ġ ��
POINT character2Pos = { 400,700 }; //ĳ���� �ʱ� ��ġ ��

#pragma pack(1)
struct Client_Send_Struct {
	BOOL KeyBuffer[256];
	int ClientID;
};
#pragma pack()

#pragma pack(1)
struct Client_Recv_Struct {
	int x1, y1;
	int x2, y2;
};
#pragma pack()

Client_Send_Struct CTS;
Client_Recv_Struct STC;

BOOL KeyBuffer[256];
bool isstart = false;
int myID = 0;

SOCKET sock; // ����
char buf[BUFSIZE + 1]; // ������ �ۼ��� ����

DWORD WINAPI ClientMain(LPVOID arg);

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

void keysend()
{
	int retval;
	memcpy(&CTS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
	memcpy(&CTS.ClientID, &myID, sizeof(myID));
	// ������ ������
	retval = send(sock, (char *)&CTS, sizeof(CTS), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()22");
	}
}

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

	HANDLE hThread;

	hThread = CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HW2CLIENT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HW2CLIENT2));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

    return (int) msg.wParam;
}

DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	//DWORD FirstTime = timeGetTime();
	DWORD CurTime;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	while (1) {
		if (!isstart)
		{
			int tmp;
			retval = recvn(sock, (char *)&tmp, sizeof(tmp), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()11");
				exit(1);
			}
			if (tmp == 1)
			{
				retval = recvn(sock, (char *)&character1Pos, sizeof(character1Pos), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recv()11");
					exit(1);
				}
			}
			else
			{
				retval = recvn(sock, (char *)&character2Pos, sizeof(character2Pos), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recv()11");
					exit(1);
				}
			}

			myID = tmp;
			if (myID != 0)
			{
				isstart = true;
				cout << myID << "��Ŭ��" << endl;
			}
		}
		else
		{
			//FirstTime = timeGetTime();

				//CurTime = timeGetTime();
				//if (CurTime - FirstTime >= float(1000 / FRAME))
				//{
			//memcpy(&CTS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			//memcpy(&CTS.ClientID, &myID, sizeof(myID));
			//// ������ ������
			//retval = send(sock, (char *)&CTS, sizeof(CTS), 0);
			//if (retval == SOCKET_ERROR) {
			//	err_display("send()22");
			//	break;
			//}
			////}


			// ������ �ޱ�
			retval = recvn(sock, (char *)&STC, sizeof(STC), 0);
			if (retval == SOCKET_ERROR) {
				err_display("recv()33");
				break;
			}

			//memcpy(&character1Pos.x, &STC.x1, sizeof(STC.x1));
			//memcpy(&character1Pos.y, &STC.y1, sizeof(STC.y1));
			//memcpy(&character2Pos.x, &STC.x2, sizeof(STC.x2));
			//memcpy(&character2Pos.y, &STC.y2, sizeof(STC.y2));

			character1Pos.x = STC.x1;
			character1Pos.y = STC.y1;
			character2Pos.x = STC.x2;
			character2Pos.y = STC.y2;
		}
	}
	

	//return 0;
}

//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HW2CLIENT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HW2CLIENT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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
		character1.Load(TEXT("rook.png"));
		character2.Load(TEXT("rook.png"));
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
			//if (character1Pos.x > 0)character1Pos.x -= 100;
			KeyBuffer[VK_LEFT] = true;
			//keysend();
			int retval;
			//memcpy(&CTS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			//memcpy(&CTS.ClientID, &myID, sizeof(myID));
			CTS.KeyBuffer[VK_LEFT] = true;
			CTS.ClientID = myID;
			
			// ������ ������
			retval = send(sock, (char *)&CTS, sizeof(CTS), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()22");
			}
			break;
		}
			break;
		case VK_RIGHT: {
			//if (character1Pos.x < BOARDSIZE - 100)character1Pos.x += 100;
			KeyBuffer[VK_RIGHT] = true;
			//keysend();
			int retval;
			//memcpy(&CTS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			//memcpy(&CTS.ClientID, &myID, sizeof(myID));
			CTS.KeyBuffer[VK_RIGHT] = true;
			CTS.ClientID = myID;
			// ������ ������
			retval = send(sock, (char *)&CTS, sizeof(CTS), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()22");
			}
		}
			break;
		case VK_DOWN: {
			//if (character1Pos.y < BOARDSIZE - 100)character1Pos.y += 100;
			KeyBuffer[VK_DOWN] = true;
			//keysend();
			int retval;
			memcpy(&CTS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			memcpy(&CTS.ClientID, &myID, sizeof(myID));
			// ������ ������
			retval = send(sock, (char *)&CTS, sizeof(CTS), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()22");
			}
		}
					  break;
		case VK_UP: {
			//if (character1Pos.y >0)character1Pos.y -= 100;
			KeyBuffer[VK_UP] = true;
			//keysend();
			int retval;
			memcpy(&CTS.KeyBuffer, &KeyBuffer, sizeof(KeyBuffer));
			memcpy(&CTS.ClientID, &myID, sizeof(myID));
			// ������ ������
			retval = send(sock, (char *)&CTS, sizeof(CTS), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()22");
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
		switch (wParam)
		{
		case VK_LEFT:
		{
			KeyBuffer[VK_LEFT] = false;
			keysend();
		}
		break;
		case VK_RIGHT: {
			KeyBuffer[VK_RIGHT] = false;
			keysend();
		}
					   break;
		case VK_DOWN: {
			KeyBuffer[VK_DOWN] = false;
			keysend();
		}
					  break;
		case VK_UP: {
			KeyBuffer[VK_UP] = false;
			keysend();
		}
					break;
		}
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
		character1.Draw(memDC, 0 + character1Pos.x + 25, 0 + character1Pos.y + 25, CHARACTERSIZE, CHARACTERSIZE);
		character2.Draw(memDC, 0 + character2Pos.x + 25, 0 + character2Pos.y + 25, CHARACTERSIZE, CHARACTERSIZE);
		img.Draw(hdc, 0, 0);
		img.ReleaseDC();

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
