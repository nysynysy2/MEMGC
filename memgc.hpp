#pragma once
#ifndef _GARBAGE_COLLECTOR_
#define _GARBAGE_COLLECTOR_
#include <cstdlib>
#include <cassert>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <type_traits>
class MEMGC
{
	friend class _GC_C;
	template<class T, class... Args> friend auto* alloc(Args... args);
	std::vector<std::pair<int,void*>> ptr_array;
	explicit MEMGC() = default;
	explicit MEMGC(const MEMGC&) = delete;
	static std::recursive_mutex m_lock;
	static volatile std::atomic<MEMGC*> a_instance;
	static std::once_flag flag;
	static void initInst();
	class _GC_C
	{
		friend class MEMGC;
		explicit _GC_C() = default;
		explicit _GC_C(const _GC_C&) = delete;
		~_GC_C() {
			if (MEMGC::getInstance() != nullptr) {
				std::lock_guard<std::recursive_mutex> locker(MEMGC::m_lock);
				if (MEMGC::a_instance.load() != nullptr) {
					delete MEMGC::a_instance.load();
					MEMGC::a_instance.store(nullptr);
				}
			}
		}
	};
public:
	static MEMGC* getInstance();
	~MEMGC();
};
std::recursive_mutex MEMGC::m_lock;
volatile std::atomic<MEMGC*> MEMGC::a_instance;
std::once_flag MEMGC::flag;
MEMGC* MEMGC::getInstance()
{
	static _GC_C gcc;
	std::call_once(MEMGC::flag,MEMGC::initInst);
	return MEMGC::a_instance.load();
}
MEMGC::~MEMGC()
{
	std::lock_guard<std::recursive_mutex> locker(MEMGC::m_lock);
	for (auto& e : this->ptr_array)
	{
		if (e.first == 1) delete e.second;
		else if (e.first == 2) delete[] e.second;
		e.second = nullptr;
	}
}

void MEMGC::initInst()
{
	MEMGC* tmp = nullptr;
	MEMGC::a_instance.compare_exchange_strong(tmp, new MEMGC());
}

template<class T, class... Args>
auto* alloc(Args... args)
{
	using elementT = std::remove_extent_t<T>;
	elementT* ptr = nullptr;
	std::pair<int, void*> ptr_pair;
	if (std::is_array<T>::value) 
	{
		ptr = reinterpret_cast<elementT*>(malloc(sizeof(elementT)*(size_t)std::forward<Args>(args)...));
		assert(("Memory allocate faild", ptr != nullptr));
		ptr_pair.first = 2;
	}
	else 
	{
		ptr = new elementT(std::forward<Args>(args)...);
		assert(("Memory allocate faild", ptr != nullptr));
		ptr_pair.first = 1;
	}
	ptr_pair.second = ptr;
	std::lock_guard<std::recursive_mutex> locker(MEMGC::m_lock);
	MEMGC::getInstance()->ptr_array.push_back(ptr_pair);
	return ptr;
}
#endif
