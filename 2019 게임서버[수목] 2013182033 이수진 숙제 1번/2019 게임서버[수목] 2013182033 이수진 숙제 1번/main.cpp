#include <Windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = "2013182033 이수진";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct MAP {
	POINT min;
	POINT max;
	int color;
}map;

typedef struct PLAYER {
	POINT mid;
}player;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevlnstance, LPSTR IpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);

	//윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush, oldBrush;
	RECT rect;

	static RECT rectview;
	static MAP map[8][8];
	static PLAYER player;

	//메시지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 500, NULL);
		static int M_L, M_R;

		GetClientRect(hWnd, &rectview);

		for (M_L = 0; M_L < 8; M_L++) {	//맵
			for (M_R = 0; M_R < 8; M_R++) {
				map[M_L][M_R].min.x = 60 * M_L;
				map[M_L][M_R].min.y = 60 * M_R;
				map[M_L][M_R].max.x = 60 + (60 * M_L);
				map[M_L][M_R].max.y = 60 + (60 * M_R);
			}
		}
		player.mid.x = 30;
		player.mid.y = 30;
		break;
		
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);


		for (M_L = 0; M_L < 8; M_L++) {
			for (M_R = 0; M_R < 8; M_R++) {
				if ((M_L + M_R) % 2 == 0)
					map[M_L][M_R].color = 0;
				else
					map[M_L][M_R].color = 255;
				hBrush = CreateSolidBrush(RGB(map[M_L][M_R].color, map[M_L][M_R].color, map[M_L][M_R].color));
				oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				Rectangle(hdc, map[M_L][M_R].min.x, map[M_L][M_R].min.y, map[M_L][M_R].max.x, map[M_L][M_R].max.y);	//체스판 그리기
				SelectObject(hdc, oldBrush);
				DeleteObject(hBrush);
			}
		}

		hBrush = CreateSolidBrush(RGB(255, 255, 0));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Ellipse(hdc, player.mid.x-30, player.mid.y - 30, player.mid.x + 30, player.mid.y + 30);	//체스 말
		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
	
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_RIGHT) {
			player.mid.x += 60;
			if (player.mid.x + 30 >= 540)
				player.mid.x -= 60;
		}
		else if (wParam == VK_LEFT) {
			player.mid.x -= 60;
			if (player.mid.x <= 0)
				player.mid.x += 60;
		}
		else if (wParam == VK_UP) {
			player.mid.y -= 60;
			if (player.mid.y <= 0)
				player.mid.y += 60;
		}
		else if (wParam == VK_DOWN) {
			player.mid.y += 60;
			if (player.mid.y + 30 >= 540)
				player.mid.y -= 60;
		}
		break;

	case WM_TIMER:
		GetClientRect(hWnd, &rect);
	
		InvalidateRgn(hWnd, NULL, TRUE);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}




