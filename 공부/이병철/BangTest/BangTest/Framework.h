#pragma once


class Framework
{
private:
	WSADATA wsa;
	HANDLE iocp_handle;
	SOCKET listen_socket;
	SOCKADDR_IN server_addr;

public:
	Framework();
	~Framework();

	void InitServer();
};