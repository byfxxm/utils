#pragma once

void Memset(int* p, int v, int c) {
	__asm {
		mov ecx, c
		mov edi, p
		mov eax, v
		mov es:[edi], eax
		rep stos dword ptr es:[edi]
	}
}