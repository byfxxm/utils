#include <vector>
#include <thread>
#include <iostream>
#include <iomanip>
#include "array_nd.h"
#include "variable_buffer.h"
#include "hash_map.h"

void TestArrayNd() {
	class A {
	public:
		A() {}
		~A() {}
	};

	using type = __int64;
	ArrayNd<type, 3> a(7, 8, 9);
	type a1[7][8][9]{};

	for (size_t i = 0; i < a[0]; ++i)
		for (size_t j = 0; j < a[1]; ++j)
			for (size_t k = 0; k < a[2]; ++k) {
				a(i, j, k) = (type)i * j * k;
				a1[i][j][k] = (type)i * j * k;
			}

	for (auto it : a) {
		std::cout << it << std::endl;
	}

	assert(memcmp(a(), a1, sizeof(a1)) == 0);
	assert(8 == a[1]);
	assert(a(2, 3, 4) == 24);
	assert(&a(2, 3, 4) - &a(1, 2, 3) == 82);
	assert(&a(6, 7, 8) - &a(0, 0, 0) == 7 * 8 * 9 - 1);
	assert((char*)&a(6, 7, 8) - (char*)&a(0, 0, 0) == (7 * 8 * 9 - 1) * sizeof(type));

	ArrayNd<type, 5> b(7, 8, 9, 10, 11);
	assert(&b(6, 7, 8, 9, 10) - &b(0, 0, 0, 0, 0) == 7 * 8 * 9 * 10 * 11 - 1);
	assert(&b(2, 3, 4, 5, 6) - &b(2, 1, 2, 1, 2) == 2 * 9 * 10 * 11 + 2 * 10 * 11 + 4 * 11 + 4);

	ArrayNd<type, 3> c(a);
	assert(&c(6, 7, 8) - &c(0, 0, 0) == 7 * 8 * 9 - 1);

	ArrayNd<type, 3> d(std::move(a));
	assert(&d(6, 7, 8) - &d(0, 0, 0) == 7 * 8 * 9 - 1);
	assert(d(1, 2) - d(0, 0) == 8 * 9 + 2 * 9);

	ArrayNd<type, 5> e(1, 2, 3, 4, 5);
	e = b;
	assert(&e(6, 7, 8, 9, 10) - &e(0, 0, 0, 0, 0) == 7 * 8 * 9 * 10 * 11 - 1);
	assert(&e(2, 3, 4, 5, 6) - &e(2, 1, 2, 1, 2) == 2 * 9 * 10 * 11 + 2 * 10 * 11 + 4 * 11 + 4);

	std::vector<ArrayNd<type, 3>> v;
	v.push_back(d);
	v.push_back(d);
	assert(v[0]() != v[1]());
	assert(v[0](1, 2, 3) == 6);
	v[0].Reset(0xff);
	assert(v[0](1, 2, 3) == -1);

	ArrayNd<A, 2> f(1, 2);
	ArrayNd<A, 2> f1(f);
	ArrayNd<A, 2> f2(3, 4);
	f = f2;
}

void TestVariableBuffer() {
	VariableBuffer<13> vbuf;
	double x = 3.1415926;
	int y = -999;
	short z = 66;
	volatile bool write_finished = false;

	std::thread th1([&]() {
		for (int i = 0; i < 100; ++i) {
			auto remainder = i % 3;
			if (remainder ==0)
				while (!vbuf.Write((char*)&x, sizeof(x)))
					std::this_thread::yield();
			else if (remainder == 1)
				while (!vbuf.Write((char*)&y, sizeof(y)))
					std::this_thread::yield();
			else if (remainder == 2)
				while (!vbuf.Write((char*)&z, sizeof(z)))
					std::this_thread::yield();
		}
		write_finished = true;
		});

	std::thread th2([&]() {
		union Buffer {
			double d;
			int i;
			short s;
		};

		Buffer buf;
		size_t count = 0;

		while (!write_finished) {
			while (!vbuf.Read((char*)&buf, sizeof(buf), count))
				std::this_thread::yield();

			if (count == 8)
				std::cout << buf.d << "\t" << count << std::endl;
			else if (count == 4)
				std::cout << buf.i << "\t" << count << std::endl;
			else if (count == 2)
				std::cout << buf.s << "\t" << count << std::endl;
			else
				assert(0);
		}
		});

	th1.join();
	th2.join();

	VariableBuffer<1000> vbuf1;
	double d[100]{};
	for (auto& i : d)
		i = x;
	vbuf1.Write(d);
	double d1[200]{};
	size_t count = 0;
	vbuf1.Read(d1, count);
	assert(count == sizeof(d));
	for (auto i = 0; i < 100; ++i) {
		assert(d1[i] == x);
	}
}

void TestHashMap() {
	HashMap<int, double> m;
	m[203] = 3.1415926;
	for (size_t i = 0; i < 100; ++i) {
		m.Insert(i, i);
	}

	std::cout << std::setprecision(8) <<m[203] << std::endl;
}

int main() {
	TestArrayNd();
	TestVariableBuffer();
	TestHashMap();
	return 0;
}