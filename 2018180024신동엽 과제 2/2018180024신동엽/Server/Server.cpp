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
		ZeroMemory(&_recv_over, sizeof(_recv_over)); // 리시브를 하기 위해선 다시 초기화 , Overlapped 재사용 ! 
		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
		WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, recv_callback); // accept를 했으면 recv를 해야하는데 우리의 recv는 overlapped 
		// callback에는 정보가 필요하다 -> recv_callback
		cout << _recv_wsabuf.buf[0] << endl;


	}

	void do_send(int num_bytes) {
		ZeroMemory(&_recv_over, sizeof(_recv_over)); // 콜백을 재사용해야하기 때문에 0으로 초기화  overlapped 재사용 ! 
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

	int s_id = reinterpret_cast<int>(recv_over->hEvent); // cast를 통해서 어떤 소캣인지 끄집어 냄
	cout << "Client Sent [" << num_bytes << "bytes] : " << clients[s_id]._recv_buf << endl;
	
	clients[s_id].do_send(num_bytes); 
	// 클라이언트가 보내려는 그 양만큼 보내야한다. ***
	//// callback함수는 하나만 두고 여러 소캣이 이 하나의 함수를 이용한다. 
	//// 어떤 소캣이 한 리시브인지 알아야한다. 따라서 LPWSAOVERLAPPED 인 overlapped 구조체에 event로 구분한다.  
	//// event가 아닌 callback을 사용한다?  여기다가 소캣을 넣어서 무슨 소캣인지 구분가능 
	//int s_id = reinterpret_cast<int>(recv_over->hEvent); // cast를 통해서 어떤 소캣인지 끄집어 냄

	//cout << "Client Sent [" << num_bytes << "bytes] : " << clients[s_id]._recv_buf << endl;
	//clients[s_id].do_send(num_bytes); // 클라이언트가 보내려는 그 양만큼 보내야한다. ***
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
	std::wcout << L"에러 " << lpMsgBuf << std::endl;
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

