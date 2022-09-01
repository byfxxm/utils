#include <vector>
#include <thread>
#include <iostream>
#include <iomanip>
#include <condition_variable>
#include "array_nd.h"
#include "array_nd_1.h"
#include "variable_buffer.h"
#include "hash_map.h"
#include "ring_buffer.h"
#include "warrior.h"
#include "metaprogramming_practice.h"
#include "n_queens.h"
#include "asm.h"
#include "reflection.h"

void TestArrayNd() {
	auto a = byfxxm::MakeArrayNd<size_t>(7, 8, 9);
	a.Memset(50);
	for (size_t i = 0; i < 7; ++i)
		for (size_t j = 0; j < 8; ++j)
			for (size_t k = 0; k < 9; ++k)
				assert(a[i][j][k] == 50);

	assert(&a[5][4][3] - &a[1][2][3] == 4 * 72 + 2 * 9);

	byfxxm::ArrayNd<size_t, 1> b(5);
	assert(&b[4] - &b[0] == 4);

	byfxxm::ArrayNd<size_t, 3> a2(std::move(a));
	assert(a2[5][5][5] == 50);
	a2[5][5][5] = 10;
	assert(a2[5][5][5] == 10);
	assert(&a2[6][7][8] - &a2[0][0][0] == 7 * 8 * 9 - 1);
}

void TestArrayNd1() {
	array_nd_1::ArrayNd<size_t, 3> a1(7, 8, 9);
	a1.Memset(50);
	for (size_t i = 0; i < 7; ++i)
		for (size_t j = 0; j < 8; ++j)
			for (size_t k = 0; k < 9; ++k)
				assert(a1[i][j][k] == 50);

	decltype(a1) a(std::move(a1));
	auto pa1 = (size_t*)a[1];
	*pa1 = 20;
	assert(a[1][0][0] = 20);
	a[1].Memset(10);
	assert(a[1][2][3] == 10);
	assert(a[2][2][3] == 50);

	a[1][2][3] = 2;
	auto b = std::move(a);
	assert(b[1][2][3] == 2);
	b.Memset(25);
	assert(b[1][2][3] == 25);
	
	array_nd_1::ArrayNd<size_t, 1> c(5);
	assert(c[1] == 0);
}

void TestVariableBuffer() {
	VariableBuffer<20> vbuf;
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
	HashMap<size_t, double> m;
	m[203] = 3.1415926;
	for (size_t i = 0; i < 100; ++i)
		m.Insert(i, (double)i);
	for (size_t i = 0; i < 100; ++i)
		m.Erase(i);

	assert(m.Size() == 1);
	std::cout << std::setprecision(8) << m[203] << std::endl;
	m.Erase(203);
	std::cout << std::setprecision(8) << m[203] << std::endl;
}

void TestRingBuffer() {
	RingBuffer<int, 1024> rb1;
	RingBuffer<int, 1023> rb2;
	int temp;

	auto time1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i) {
		while (rb1.Write(5));
		while (rb1.Read(temp));
	}
	auto time2 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i) {
		while (rb2.Write(5));
		while (rb2.Read(temp));
	}
	auto time3 = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count() << " ms" << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time2).count() << " ms" << std::endl;
}

void TestWarrior() {
	auto battle_ground = BattleGround::Instance();
	auto A = battle_ground->CreateWarrior({ "A", 11, 5, 2, 10 });
	auto B = battle_ground->CreateWarrior({ "B", 11, 3, 3, 20 });
	A->Attack(B);
	B->Attack(A);

	while (!A->IsDead() && !B->IsDead())
		std::this_thread::yield();

	if (!A->IsDead() && B->IsDead())
		std::cout << A->Name() << " won" << std::endl;
	else if (A->IsDead() && !B->IsDead())
		std::cout << B->Name() << " won" << std::endl;
	else
		std::cout << "Even" << std::endl;
}

void TestMetaprogramming() {
	struct A {
		using type = void;
	};

	struct B {};

	static_assert(meta::HasType<A>);
	static_assert(!meta::HasType<B>);

	static_assert(!meta::IsCustomeClass<int>);
	static_assert(meta::IsCustomeClass<A>);

	A a[5];
	static_assert(meta::Countof(a) == 5);

	static_assert(meta::prime::IsPrimeV<97>);
	static_assert(!meta::prime::IsPrimeV<200>);
	static_assert(meta::prime::IsPrimeV<2>);
	static_assert(!meta::prime::IsPrimeV<1>);
	static_assert(!meta::prime::IsPrimeV<0>);

	static_assert(meta::prime::IsPrimeF(97));
	static_assert(!meta::prime::IsPrimeF(200));
	static_assert(meta::prime::IsPrimeF(2));
	static_assert(!meta::prime::IsPrimeF(1));
	static_assert(!meta::prime::IsPrimeF(0));
}

void TestNQueens() {
	NQueens::Queens<8> queens_8;
	for (size_t i = 0; i < queens_8.Count(); ++i) {
		std::cout << "resolve " << i << ":" << std::endl;
		for (auto& q : queens_8[i])
			std::cout << q.GetX() << ", " << q.GetY() << std::endl;
	}
}

void TestAsm() {
	int a[100]{};
	Memset(a, (int)2, 100);
}

void TestReflection() {
	class Type1 : public byfxxm::ISerialize {
	public:
		virtual void Serialize() override {

		}

		virtual void Deserialize() override {

		}

	private:
		int a_;
		double b_;
	};

	byfxxm::ReflectionHelper::Instance().Serialize(Type1());
	byfxxm::ReflectionHelper::Instance().Deserialize(Type1());
}

int main() {
	TestArrayNd();
	TestArrayNd1();
	TestVariableBuffer();
	TestHashMap();
	TestRingBuffer();
	TestWarrior();
	TestMetaprogramming();
	TestNQueens();
	TestAsm();
	TestReflection();
	return 0;
}
