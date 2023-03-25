// ServerChessBoard.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ServerChessBoard.h"
#include"Player.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst = GetModuleHandle(NULL);                                // 현재 인스턴스입니다.
HBITMAP hBmp = (HBITMAP)LoadImage(hInst, L"../Image/board.bmp", IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
HBITMAP hPlayer = (HBITMAP)LoadImage(hInst, L"../Image/player.bmp", IMAGE_BITMAP,0,0,LR_LOADFROMFILE);


WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int Px = 0;
int Py = 0;
int Pwidth = 60;
int Pheight = 60;

struct myOverlapped
{
    WSAOVERLAPPED myoverlapped;
    WSABUF mywbuf;
};

char moveDir;

Player Knight(Px,Py,Pwidth,Pheight);

Client client;

SOCKET clientSocket;

WSABUF mybuf_r;
char recv_buf[BUFSIZE];

WSABUF s_wsabuf;
char send_buf[BUFSIZE];

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag);

void do_Send(char key)
{
    char send_data[1];
    send_data[0] = key;
    DWORD sent_byte;

    WSABUF mybuf;
    //mybuf.buf = buf; mybuf.len = static_cast<ULONG>(strlen(buf)) + 1;
    mybuf.buf = send_data;
    mybuf.len = static_cast<ULONG>(strlen(send_data)) + 1;
    WSAOVERLAPPED* sendoverlapped = new WSAOVERLAPPED;
    ZeroMemory(sendoverlapped, sizeof(WSAOVERLAPPED));

    WSASend(clientSocket, &mybuf, 1, &sent_byte, 0, sendoverlapped, send_callback);
    SleepEx(100, true);
}

void do_Recv(Player& playerInfo)
{
    //char recv_data[BUFSIZE];
    //WSABUF mybuf;

    //mybuf.buf = recv_data;
    //mybuf.len = BUFSIZE;

    DWORD recv_flag = 0;
    WSAOVERLAPPED* r_over = new WSAOVERLAPPED;
    ZeroMemory(r_over, sizeof(WSAOVERLAPPED));
    int ret = WSARecv(clientSocket, &mybuf_r, 1, 0, &recv_flag, r_over, recv_callback);
    if (0 != ret) {
        int err_no = WSAGetLastError();
        if (err_no != WSA_IO_PENDING);
        //err_display("WSARecv  : ", err_no);
    }

    //mybuf_r.buf = mybuf.buf;
    //mybuf_r.len = mybuf.len;


  
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{

    delete send_over;
    return;
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
    myOverlapped* overlapped = (myOverlapped*)recv_over;
    overlapped->mywbuf.buf = mybuf_r.buf;
    overlapped->mywbuf.len = mybuf_r.len;


    int x = int(overlapped->mywbuf.buf[0]);
    int y = int(overlapped->mywbuf.buf[1]);
    Knight.setPosX(x);
    Knight.setPosY(y);
    cout << " RECV X : " << Knight.getPosX() << " Y : " << Knight.getPosY() << endl;
    //cout << "recv _ callback  X: " << x << "  Y : "<< y << endl;
    delete recv_over;
    return;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVERCHESSBOARD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERCHESSBOARD));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVERCHESSBOARD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVERCHESSBOARD);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800,800, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    char address[100] = "127.0.0.1";
    switch (message)
    {
    case WM_CREATE:
        wcout.imbue(locale("korean"));
        WSADATA WSAData;
        WSAStartup(MAKEWORD(2, 0), &WSAData);

        clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
        SOCKADDR_IN server_addr;
        ZeroMemory(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVERPORT);
        inet_pton(AF_INET, address, &server_addr.sin_addr);
        connect(clientSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
        cout << " Connect Server " << endl;

        break;
   
    case WM_PAINT:
        {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hBmp);
        BitBlt(hdc, 0, 0, 800, 800, hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);

        hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hPlayer);
        BitBlt(hdc, Knight.getPosX(), Knight.getPosY(), Knight.getWidth(), Knight.getHeight(), hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN:
    {
        switch (wParam) {
        case VK_LEFT:
            moveDir = 'a';
            do_Send(moveDir);
            do_Recv(Knight);
            break;
        case VK_RIGHT:
            moveDir = 'd';
            do_Send(moveDir);
            do_Recv(Knight);
            break;
        case VK_UP:
            moveDir = 'w';
            do_Send(moveDir);
            do_Recv(Knight);
            break;
        case VK_DOWN:
            moveDir = 's';
            do_Send(moveDir);
            do_Recv(Knight);
            break;
        default:
            break;

        }
        InvalidateRect(hWnd, NULL, TRUE); 
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


