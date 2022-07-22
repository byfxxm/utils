#pragma once

inline void Memset(int* p, int v, int c) {
	__asm {
		mov ecx, c
		mov edi, p
		mov eax, v
		rep stos dword ptr es:[edi]
	}
}