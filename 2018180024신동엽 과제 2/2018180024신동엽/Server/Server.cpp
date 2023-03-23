#include<iostream>
#include<unordered_map>
#include <WS2tcpip.h>
#pragma comment (lib, "WS2_32.LIB")

using namespace std;

const char* SERVER_ADDR = "127.0.0.1";
const short SERVER_PORT = 7777;
const int BUFSIZE = 256;



int posx = 0;
int posy = 0;

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag);


class SESSION {

private:

	WSAOVERLAPPED _recv_over;

	int _id;
	WSABUF _recv_wsabuf;
	WSABUF _send_wsabuf;
	SOCKET _socket;

public:
	int posX = 0;
	int posY = 0;
	char _recv_buf[BUFSIZE];
	SESSION() {
		cout << "Unexpected Constructor Call Error!\n";
		exit(-1);
	}
	SESSION(int id, SOCKET s) : _id(id), _socket(s) {
		_recv_wsabuf.buf = _recv_buf; _recv_wsabuf.len = BUFSIZE;
		_send_wsabuf.buf = _recv_buf; _send_wsabuf.len = 0;
	}
	~SESSION() {
		closesocket(_socket);
	}

	void MovePos(char key) {
		if (key == 'w')
			posY += 78;
		if (key == 's')
			posY -= 78;
		if (key == 'a')
			posX += 78;
		if (key == 'd')
			posX -= 78;
	}

	void do_recv() {
		DWORD recv_flag = 0;
		ZeroMemory(&_recv_over, sizeof(_recv_over)); // ���ú긦 �ϱ� ���ؼ� �ٽ� �ʱ�ȭ , Overlapped ���� ! 
		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
		WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, recv_callback); // accept�� ������ recv�� �ؾ��ϴµ� �츮�� recv�� overlapped 
		// callback���� ������ �ʿ��ϴ� -> recv_callback
		cout << _recv_wsabuf.buf[0] << endl;


	}

	void do_send(int num_bytes) {
		ZeroMemory(&_recv_over, sizeof(_recv_over)); // �ݹ��� �����ؾ��ϱ� ������ 0���� �ʱ�ȭ  overlapped ���� ! 
		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
		char send_buf[BUFSIZE];
		::memcpy(send_buf, &posX, 4);
		::memcpy(send_buf + 4, &posY, 4);
		_send_wsabuf.buf = send_buf;
		_send_wsabuf.len = num_bytes;
		WSASend(_socket, &_send_wsabuf, 1, 0, 0, &_recv_over, send_callback);
	}
};

unordered_map <int, SESSION> clients;

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	int s_id = reinterpret_cast<int>(send_over->hEvent);
	clients[s_id].do_recv();
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{

	int s_id = reinterpret_cast<int>(recv_over->hEvent); // cast�� ���ؼ� � ��Ĺ���� ������ ��
	cout << "Client Sent [" << num_bytes << "bytes] : " << clients[s_id]._recv_buf << endl;
	
	clients[s_id].do_send(num_bytes); 
	// Ŭ���̾�Ʈ�� �������� �� �縸ŭ �������Ѵ�. ***
	//// callback�Լ��� �ϳ��� �ΰ� ���� ��Ĺ�� �� �ϳ��� �Լ��� �̿��Ѵ�. 
	//// � ��Ĺ�� �� ���ú����� �˾ƾ��Ѵ�. ���� LPWSAOVERLAPPED �� overlapped ����ü�� event�� �����Ѵ�.  
	//// event�� �ƴ� callback�� ����Ѵ�?  ����ٰ� ��Ĺ�� �־ ���� ��Ĺ���� ���а��� 
	//int s_id = reinterpret_cast<int>(recv_over->hEvent); // cast�� ���ؼ� � ��Ĺ���� ������ ��

	//cout << "Client Sent [" << num_bytes << "bytes] : " << clients[s_id]._recv_buf << endl;
	//clients[s_id].do_send(num_bytes); // Ŭ���̾�Ʈ�� �������� �� �縸ŭ �������Ѵ�. ***
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
	std::wcout << L"���� " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}


int main()
{

	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(serverSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	listen(serverSocket, SOMAXCONN);

	INT addr_size = sizeof(server_addr);

	for (int i = 1; ; ++i) {

		SOCKET c_socket = WSAAccept(serverSocket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		if (c_socket != INVALID_SOCKET)
			cout << " Connect  Client " << endl;
		clients.try_emplace(i, i, c_socket);
		clients[i].do_recv();
	

		//char recv_buf[BUFSIZE];

		//WSABUF mybuf;
		//mybuf.buf = recv_buf;
		//mybuf.len = BUFSIZE;

		//DWORD recv_byte;
		//DWORD recv_flag = 0;
		//WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
		//cout << " Recv Data " << (mybuf.buf[0]) << endl;

		//char _key = mybuf.buf[0];
		//Move(_key);

		//char send_buf[BUFSIZE];
		//::memcpy(send_buf, &posx, 4);
		//::memcpy(send_buf + 4, &posy, 4);
		//mybuf.buf = send_buf;
		//mybuf.len = 8;
		//DWORD send_byte = 3;
		////cout << " Send X : " << int(send_buf) << " Y : " << int(send_buf+ 4) << endl;
		//WSASend(c_socket, &mybuf, 1, &send_byte, 0, 0, 0);
	}

	WSACleanup();
}

