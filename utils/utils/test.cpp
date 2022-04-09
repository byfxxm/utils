#include <vector>
#include "array_nd.h"

int main() {
	using type = __int64;
	array_nd<type, 3> a(7, 8, 9);

	for (int i = 0; i < a[0]; ++i)
		for (int j = 0; j < a[1]; ++j)
			for (int k = 0; k < a[2]; ++k)
				a(i, j, k) = (type)i * j * k;

	assert(8 == a[1]);
	assert(a(2, 3, 4) == 24);
	assert(&a(2, 3, 4) - &a(1, 2, 3) == 82);
	assert(&a(6, 7, 8) - &a(0, 0, 0) == 7 * 8 * 9 - 1);
	assert((char*)&a(6, 7, 8) - (char*)&a(0, 0, 0) == (7 * 8 * 9 - 1) * sizeof(type));

	array_nd<type, 5> b(7, 8, 9, 10, 11);
	assert(&b(6, 7, 8, 9, 10) - &b(0, 0, 0, 0, 0) == 7 * 8 * 9 * 10 * 11 - 1);
	assert(&b(2, 3, 4, 5, 6) - &b(2, 1, 2, 1, 2) == 2 * 9 * 10 * 11 + 2 * 10 * 11 + 4 * 11 + 4);

	array_nd<type, 3> c(a);
	assert(&c(6, 7, 8) - &c(0, 0, 0) == 7 * 8 * 9 - 1);

	array_nd<type, 3> d(std::move(a));
	assert(&d(6, 7, 8) - &d(0, 0, 0) == 7 * 8 * 9 - 1);
	assert(d(1, 2) - d(0, 0) == 8 * 9 + 2 * 9);

	array_nd<type, 5> e(1, 2, 3, 4, 5);
	e = b;
	assert(&e(6, 7, 8, 9, 10) - &e(0, 0, 0, 0, 0) == 7 * 8 * 9 * 10 * 11 - 1);
	assert(&e(2, 3, 4, 5, 6) - &e(2, 1, 2, 1, 2) == 2 * 9 * 10 * 11 + 2 * 10 * 11 + 4 * 11 + 4);

	std::vector<array_nd<type, 3>> v;
	v.push_back(d);
	v.push_back(d);
	assert(v[0]() != v[1]());
	v[0].reset(0xff);
	assert(v[0](1, 2, 3) == -1);

	return 0;
}