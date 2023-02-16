#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include<thread>
#include<atomic>
#include<vector>
#include<mutex>
#include<windows.h>
#include<future>
#include<random>

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
#include"CoreMacro.h"

int main()
{
	int32 a = 0; 
	ASSERT_CRASH(a != 3);
}