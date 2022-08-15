#pragma once
#include <type_traits>

template <class T, class = std::enable_if_t<sizeof(T) == 4>>
inline void Memset(T* p, T v, int c) {
#ifndef _WIN64
	__asm {
		mov ecx, c
		mov edi, p
		mov eax, v
		rep stos dword ptr es:[edi]
	}
#endif
}
