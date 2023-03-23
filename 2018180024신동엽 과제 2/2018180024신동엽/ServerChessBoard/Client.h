#pragma once
#include"stdafx.h"

class Player;
class Client
{
private:
	WSABUF _buf;


public:
	void ConnectServer(const char* address);
	void SendData(char key);
	void RecvData(Player& playerInfo);

	void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);


	void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);


public:
	const char* SERVER_ADDR = "127.0.0.1";
	const short SERVER_PORT = 7777;

	SOCKET clientSocket;
};

