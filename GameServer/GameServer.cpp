#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include<thread>
#include<atomic>
#include<vector>
#include<mutex>
#include<windows.h>
#include<future>

//가시성 , 코드 재배치 
int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2= 0;

volatile bool ready;

void Thread_t1()
{
	while (!ready)
		;
	y = 1; //Store y
	r1 = x; // Load x

}

void Thread_t2()
{
	while (!ready)
		;
	x = 1; // Store x
	r2 = y; // Load y
}


int main()
{
	int32 count = 0;
	while (true)
	{
		ready = false;
		count++;

		x = y = r1 = r2 = 0;
		thread t1(Thread_t1);
		thread t2(Thread_t2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}
	cout << count << " 번 만에 빠져나옴 " << endl;


}