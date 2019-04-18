#pragma once
#include "stdafx.h"

void err_quit(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags) {
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


// ------------------------------------------------------------------------------


struct StcPacket {

};

// �ش� Ŭ���̾�Ʈ���� ����Ǿ� ���� ������� �ʴ��̻� �״�� ����
// �Ǿ��ִ�. 
struct CtsPacket {
	UINT keyboard_click = 0x00000000;
	UINT mouse_click = 0x00000000;
	Point2D mouse_direction = { 0 };

	// Ŭ���̾�Ʈ���� ���� �� Ű������ Ŭ���� ���� ��
	// OR ���� -> �� �ϳ��� 1�� ������ TRUE�� ��ȯ�Ѵٴ� ��.
	// �ߺ� Ű �Է��� ���� ��� 1�� �̸� ���� �� �ִµ� �� ��쵵 Ŀ�� ����
	void PushArrowKeyUp() {
		UINT input_buffer = 0x10000000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyDown() {
		UINT input_buffer = 0x01000000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyRight() {
		UINT input_buffer = 0x00100000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyLeft() {
		UINT input_buffer = 0x00010000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyShift() {
		UINT input_buffer = 0x00001000;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeySpaceBar() {
		UINT input_buffer = 0x00000100;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyOne() {
		UINT input_buffer = 0x00000010;
		keyboard_click = keyboard_click | input_buffer;
	}
	void PushArrowKeyTwo() {
		UINT input_buffer = 0x00000001;
		keyboard_click = keyboard_click | input_buffer;
	}


	// Release�� 1�� 0���� ����� �ִ� �۾��� �ؾ��Ѵ�. 
	// &������ ���ؼ� Ȯ��
	void ReleaseArrowKeyUp() {
		UINT input_buffer = 0x01111111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyDown() {
		UINT input_buffer = 0x10111111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyRight() {
		UINT input_buffer = 0x11011111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyLeft() {
		UINT input_buffer = 0x11101111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyShift() {
		UINT input_buffer = 0x11110111;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeySpaceBar() {
		UINT input_buffer = 0x11111011;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyOne() {
		UINT input_buffer = 0x11111101;
		keyboard_click = keyboard_click & input_buffer;
	}
	void ReleaseArrowKeyTwo() {
		UINT input_buffer = 0x11111110;
		keyboard_click = keyboard_click & input_buffer;
	}
};