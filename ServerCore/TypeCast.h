#pragma once
#include"Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T,typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = U...;
};

#pragma endregion