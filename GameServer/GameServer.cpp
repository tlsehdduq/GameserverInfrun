﻿#include "pch.h"
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

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	//int32 sendBytes = 0;
	WSAOVERLAPPED overlapped = {};
};

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	// 블록킹(Blocking)소켓
	// accept -> 접속한 클라가 있을 때
	// connet -> 서버 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 때

	// 논블록킹(Non-Blocking)

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

	// Overlapped IO (비동기 + 논블로킹) 
	// - Overlapped 함수를 건다 ( WSARecv , WSASend)
	// - Overlapped 함수가 성공했는지 처리
	// -> 성공했으면 결과 얻어서 처리
	// -> 실패했으면 사유를 확인 
	
	//char sendBuffer[100];

	//WSABUF wsaBuf;
	//wsaBuf.buf = sendBuffer;
	//wsaBuf.len = 100;


	// WSASend
	// 1 ) 비동기 입출력 소캣
	// 2 ) WSABUF 배열의 시작 주소 + 개수 
 	// Scatter - Gather 
	// 패킷을 보낼때 이리저리 분산이 되어있을탠데 연동을해줘서 한번에 Send를 해줄 수 있기 떄문에 성능적으로 우아함
	// 나중에배울것 알고만있자
	// 3 ) 보내고/받은 바이트 수 
	// 4 ) 상세 옵션인데 0
	// 5 ) WSAOVERLAPPED 구조체 주솟값
	// 6 ) 입출력이 완료되면 OS가 호출할 콜백함수 
	
	// WSARecv
	// AcceptEx
	// ConnectEx
	
	// Overlapped 모델 (이벤트 기반)
	// - 비동기 입출력 지원하는 소켓생성 + 통지 받기 위한 이벤트 객체 생성
	// - 비동기 입출력 함수 호출 ( 1에서 만든 이벤트 객체를 같이 넘겨줌 ) 
	// - 비동기 작업이 바로 완료되지 않으면 , WSA_IO_PENDING 오류 코드
	// 운영체제는 이벤트 객체를 통해 signaled 상태로 만들어서 완료 상태 알려줌
	// - WSAWaitForMultipleEvents 함수 호출해서 이벤트 객체의 signal 판별 
	// - WSAGetOverlappedResult 호출해서 비동기 입출력 결과 확인 및 데이터 처리

	// 1) 비동기소켓
	// 2) 넘겨준 overlapped 구조체
	// 3) 전송된 바이트 수 
	// 4) 비동기 입출력 작업이 끝날때까지 대기할지? 
	// false
	// 5) 비동기 입출력 작업 관련 부가 정보, 거의 사용 안함 
	// WSAGetOverlappedResult
	
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket;

		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			return 0;
		}
		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << " Client Connected ! " << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;

			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{


			}
			cout << "Data Recv Len = " << recvLen << endl;
		}

	}

	// 윈속 종료
	::WSACleanup();
}