#ifndef _GARBAGE_COLLECTOR_
#define _GARBAGE_COLLECTOR_
#include <cstdlib>
#include <cassert>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <type_traits>
class _GC
{
	friend class _GC_C;
	template<class T, class... Args> friend auto* alloc(Args... args);
	std::vector<std::pair<int,void*>> ptr_array;
	explicit _GC() = default;
	explicit _GC(const _GC&) = delete;
	static std::recursive_mutex m_lock;
	static volatile std::atomic<_GC*> a_instance;
	class _GC_C
	{
		friend class _GC;
		explicit _GC_C() = default;
		explicit _GC_C(const _GC_C&) = delete;
		~_GC_C() {
			if (_GC::getInstance() != nullptr) {
				std::lock_guard<std::recursive_mutex> locker(_GC::m_lock);
				if (_GC::a_instance.load() != nullptr) {
					delete _GC::a_instance.load();
					_GC::a_instance.store(nullptr);
				}
			}
		}
	};
public:
	static _GC* getInstance();
	~_GC();
};
std::recursive_mutex _GC::m_lock;
volatile std::atomic<_GC*> _GC::a_instance;
_GC* _GC::getInstance()
{
	static _GC_C gcc;
	_GC* tmp = nullptr;
	if(_GC::a_instance.load()==nullptr)
		_GC::a_instance.compare_exchange_strong(tmp, new _GC());
	return _GC::a_instance.load();
}
_GC::~_GC()
{
	std::lock_guard<std::recursive_mutex> locker(_GC::m_lock);
	for (auto& e : this->ptr_array)
	{
		if (e.first == 1) delete e.second;
		else if (e.first == 2) delete[] e.second;
		e.second = nullptr;
	}
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
	std::lock_guard<std::recursive_mutex> locker(_GC::m_lock);
	_GC::getInstance()->ptr_array.push_back(ptr_pair);
	return ptr;
}
#endif
