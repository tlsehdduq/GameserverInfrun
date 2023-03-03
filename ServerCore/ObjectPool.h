#pragma once
#include"Types.h"
#include"MemoryPool.h"

template<typename Type>

class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
		Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
		new(memory)Type(forward<Args>(args)...); // placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
		s_pool.Push(MemoryHeader::DetachHeader(obj));

	}

	static shared_ptr<Type> MakeShared()
	{
		shared_ptr<Type>ptr = { Pop(),Push };
		return ptr;
	}
private:
	static int32 s_allocSize;
	static MemoryPool s_pool;

};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };