#include "array_nd.h"

int main()
{
	using type = __int64;
	array_nd<type, 3> a(7, 8, 9);

	for (int i = 0; i < a[0]; ++i)
		for (int j = 0; j < a[1]; ++j)
			for (int k = 0; k < a[2]; ++k)
				a(i, j, k) = (type)i * j * k;

	assert(8 == a[1]);
	assert(&a(2, 3, 4) - &a(1, 2, 3) == 82);
	assert(&a(6, 7, 8) - &a(0, 0, 0) == 7 * 8 * 9 - 1);
	assert((char*)&a(6, 7, 8) - (char*)&a(0, 0, 0) == (7 * 8 * 9 - 1) * sizeof(type));

	array_nd<type, 5> b(7, 8, 9, 10, 11);
	assert(&b(6, 7, 8, 9, 10) - &b(0, 0, 0, 0, 0) == 7 * 8 * 9 * 10 * 11 - 1);
	assert(&b(2, 3, 4, 5, 6) - &b(2, 1, 2, 1, 2) == 82);

	return 0;
}