#include <iostream>
#include <WS2tcpip.h>

using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const short SERVER_PORT = 7777;
const int BUFSIZE = 256;

int posx =0;
int posy =0;

void Move(char key) {
	if (key == 'w')
		posy -= 78;
	if (key == 's')
		posy += 78;
	if (key == 'a')
		posx -= 78;
	if (key == 'd')
		posx += 78;
}

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"¿¡·¯ " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}


int main()
{

	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	if (serverSocket == SOCKET_ERROR) {
		cout << "Connect Failed !!" << endl;
		return 0;
	}
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(serverSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	listen(serverSocket, SOMAXCONN);

	INT addr_size = sizeof(server_addr);
	SOCKET c_socket = WSAAccept(serverSocket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);

	if (c_socket == SOCKET_ERROR) {
		cout << "Cannot Connect " << endl;
		return 0;
	}
	else {
		cout << " Connect" << endl;
	}
	while (true) {

		char recv_buf[BUFSIZE];

		WSABUF mybuf;
		mybuf.buf = recv_buf;
		mybuf.len = BUFSIZE;

		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
		cout << " Recv Data " << (mybuf.buf[0]) << endl;

		char _key = mybuf.buf[0];
		Move(_key);
		
		char send_buf[BUFSIZE];
		::memcpy(send_buf, &posx, 4);
		::memcpy(send_buf+4, &posy, 4);
		mybuf.buf = send_buf;
		mybuf.len = 8;
		DWORD send_byte = 3;
		cout << " Send X : " << int(mybuf.buf[0]) << " Y : " << int(mybuf.buf[1]) << endl;
		WSASend(c_socket, &mybuf, 1, &send_byte, 0, 0, 0);
	}

	WSACleanup();
}

