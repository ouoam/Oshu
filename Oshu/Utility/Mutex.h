#pragma once

#include <Windows.h>
#include <SFML/System/NonCopyable.hpp>


class myMutex : sf::NonCopyable {
public:
	myMutex() {
		InitializeCriticalSection(&m_mutex);
	}
	~myMutex() {
		DeleteCriticalSection(&m_mutex);
	}
	void lock() {
		EnterCriticalSection(&m_mutex);
	}

	void unlock() {
		LeaveCriticalSection(&m_mutex);
	}

	bool tryLock() {
		// https://docs.microsoft.com/th-th/windows/desktop/api/synchapi/nf-synchapi-tryentercriticalsection
		return TryEnterCriticalSection(&m_mutex);
	}

private:
	CRITICAL_SECTION m_mutex;
};