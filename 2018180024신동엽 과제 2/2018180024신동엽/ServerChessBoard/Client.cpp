#include"Client.h"

void Client::ConnectServer(const char* address)
{
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, address, &server_addr.sin_addr);
	connect(clientSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

}

void Client::SendData(char key)
{
	char send_data[1];
	send_data[0] = key;
	DWORD sent_byte;

	WSABUF mybuf;
	//mybuf.buf = buf; mybuf.len = static_cast<ULONG>(strlen(buf)) + 1;
	mybuf.buf = send_data;
	mybuf.len = static_cast<ULONG>(strlen(send_data)) + 1;

	WSASend(clientSocket, &mybuf,1, &sent_byte, 0, 0, 0);


}

void Client::RecvData(Player& playerInfo)
{
	char recv_data[BUFSIZE];

	WSABUF mybuf;
	mybuf.buf = recv_data;
	mybuf.len = BUFSIZE;
	DWORD recv_byte;
	DWORD recv_flag = 0;
	WSARecv(clientSocket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
	int x = 0; 
	int y = 0;
	::memcpy(&x, mybuf.buf, 4);
	::memcpy(&y, mybuf.buf + 4, 4);

	_buf.buf = mybuf.buf;
	_buf.len = mybuf.len;

	playerInfo.setPosX(x);
	playerInfo.setPosY(y);

	cout << " RECV X : " << playerInfo.getPosX() << " Y : " << playerInfo.getPosY() << endl;

}

void Client::recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
}

void Client::send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{

}


