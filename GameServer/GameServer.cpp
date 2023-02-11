#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include<thread>
#include<atomic>
#include<vector>
#include<mutex>
#include<windows.h>
#include<future>
// Thread local storage

thread_local int32 LThreadID = 0;

void ThreadMain(int32 threadID) {
	LThreadID = threadID;

	while (true)
	{
		cout << " Hi ! I am Thread " << LThreadID << endl;

		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;

	for (int32 i = 0; i < 10; ++i)
	{
		int32 threadID = i + 1;
		threads.push_back(thread(ThreadMain, threadID));
	}
	for (thread& t : threads)
		t.join();
}