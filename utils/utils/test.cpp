#include "array_nd.h"

int main()
{
	array_nd<char, 3> a(7, 8, 9);

	for (int i = 0; i < a[0]; ++i)
		for (int j = 0; j < a[1]; ++j)
			for (int k = 0; k < a[2]; ++k)
				a(i, j, k) = (char)i * j * k;

	auto b = a(4, 7, 8);
	auto c = a[1];

	return 0;
}