#pragma once
#include<mutex>

template<class T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();

	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		// empty => top -> pop
		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false; });
		value = std::move(_stack.top());
		_stack.pop();
	}

	//bool Empty()
	//{
	//	lock_guard<mutex> lock(_mutex);
	//	return _stack.empty();
	//}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;

};

template<class T>
class LockFreeStack
{

	struct Node
	{
		Node(const T& value) : data(value), next(nullptr) {}
		T data;
		Node* next;
	};

public:

	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		while (_head.compare_exchange_weak(node->next, node) == false) {

		}
	}


	bool TryPop(T& value)
	{
		++_popCount;
		Node* oldHead = _head;

		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{

		}
		if (oldHead == nullptr) {
			--_popCount;
			return false;
		}

		value = oldHead->data;

		return  true;
	}
	// 1) ������ �и�
	// 2) Count üũ
	// 3) �� ȥ�ڸ� üũ 

	void TryDelete(Node* oldHead)
	{
		// �� �ܿ� ���� �ִ°�?
		if (_popCount == 1)
		{
			// �� ȥ�ڳ�?

			// �̿� ȥ���ΰ�, ���� ����� �ٸ� �����͵鵵 ���� �غ��� 
			Node* node = _pendingList.exchange(nullptr);

			if (--_popCount == 0)
			{
				// ����� �ְ� ���� -> ���� ���� 
				// �����ͼ� ����� , ������ �����ʹ� �и��ص� ���� ~!
				DeleteNodes(node);
			}
			else if(node)
			{
				// ���� ���������� �ٽ� ���� ���� 
				ChainPendingNodeList(node);
			}

			// �� �����ʹ� ���� 
			delete oldHead;
		}
		else
		{
			// ���� �ֳ� ? �׷� ���� ���� ���� �ʰ� ���� ���ุ 
			ChainPendingNode(oldHead);
			--_popCount;
		}
	}

	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{

		}
	}
	
	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingNodeList(node, last);
	}
	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}
private:
	atomic<Node*> _head;

	atomic<uint32> _popCount = 0; // pop�� �������� ������ ���� 
	atomic<Node*> _pendingList; // ���� �Ǿ�� �� ����(ù��° ���)
};