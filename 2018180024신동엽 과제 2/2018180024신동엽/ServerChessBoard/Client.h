#pragma once
#include"stdafx.h"

class Player;
class Client
{
public:
	void ConnectServer(const char* address);
	void SendData(char key);
	void RecvData(Player& playerInfo);

public:
	const char* SERVER_ADDR = "127.0.0.1";
	const short SERVER_PORT = 7777;

	SOCKET clientSocket;
};

