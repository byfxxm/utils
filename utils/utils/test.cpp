#include "array_nd.h"

int main()
{
	array_nd<char, 3> a(9, 9, 9);

	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			for (int k = 0; k < 9; ++k)
				a(i, j, k) = (char)i * j * k;

	//a.get(1, 2, 1) = 2;
	auto b = a(4, 2, 8);

	return 0;
}