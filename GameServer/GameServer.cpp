#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include<thread>
#include<atomic>
#include<vector>
#include<mutex>
#include<windows.h>

mutex m;
queue<int32> q;
HANDLE handle;
void Producer()
{
	while (true) {

		{
			unique_lock<mutex> lock(m);
			q.push(100);

		}

		::SetEvent(handle); // signal상태로 바꿔주세요  true를 대입하는 느낌 

		this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE);
		//NON - signal
		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
	}
}

int main()
{
	//커널 오브젝트
	// Usage count
	// Signal ( 파란불 ) / Non_signal ( 빨간불 )  << bool 
	// Auto // Manual << bool 
	handle = ::CreateEvent(NULL/*보안속성*/, FALSE/*bManualReset*/, FALSE/*bInitialState*/, NULL);
	
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);

}