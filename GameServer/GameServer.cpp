#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include <WinSock2.h>
#include<MSWSock.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	//윈속 라이브러리 초기화 (ws2_32 라이브러리 초기화)
// 관련 정보가 wsaData에 채워짐
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// 블로킹 ( Blocking ) 
	// accept -> 접속한 클라가 있을 때
	// connect - > 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신버퍼에 복사 했을 때 
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고 이를 유저레벨 버퍼에 복사했을 때 

	// 논블로킹 ( Non - Blocking )

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);
	// ACCEPT
	SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)addrLen, &addrLen);
	while (true) {

		if (clientSocket == INVALID_SOCKET)
		{
			// 원래 블록 했어야 하는데 니가 논블록 하라며;
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			break;
		}

		cout << " Client Connected ! " << endl;
	}
	//RECEIVE
	while (true)
	{
		char recvBuffer[1000];
		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (recvLen == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			break;
		}
		else if (recvLen == 0)
		{
			// 연결 끊김 
			break;
		}

		cout << "Recv " << endl;

		// Send 
		while (true)
		{
			if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR) {
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				break;
			}

			cout << "Send Data ! LEN = " << recvLen << endl;
			break;
		}

	}

	// 윈속 종료 
	::WSACleanup();
}