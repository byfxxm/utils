#include <vector>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <type_traits>

#include "array_nd.h"
#include "variable_buffer.h"
#include "hash_map.h"
#include "ring_buffer.h"
#include "warrior.h"
#include "metaprogramming_practice.h"
#include "n_queens.h"
#include "asm.h"
#include "reflection.h"
#include "crtp.h"
#include "coroutine.h"
#include "addressing.h"
#include "typelist.h"
#include "coro.h"

void TestArrayNd() {
	byfxxm::ArrayNd a((size_t)7, 8, 9);
	a.Memset(50);
	for (size_t i = 0; i < 7; ++i)
		for (size_t j = 0; j < 8; ++j)
			for (size_t k = 0; k < 9; ++k)
				assert(a[i][j][k] == 50);

	assert(&a[5][4][3] - &a[1][2][3] == 4 * 72 + 2 * 9);

	byfxxm::ArrayNd b(5);
	assert(&b[4] - &b[0] == 4);

	byfxxm::ArrayNd a2(a);
	assert(a2[5][5][5] == 50);
	a2[5][5][5] = 10;
	assert(a2[5][5][5] == 10);
	assert(&a2[6][7][8] - &a2[0][0][0] == 7 * 8 * 9 - 1);

	int x = 22;
	byfxxm::ArrayNd<int, 3> c =
	{
		{
			{ 1, 2, 3 },
			{ 4, 5 }
		},
		{
			{ 13, 14 },
			{ 6, 7, 8, 9 },
			{ x }
		},
		{},
	};

	assert(c[0][0][1] == 2);
	assert(c[0][1][1] == 5);
	assert(c[1][2][0] == 22);
	assert(c[0][2][2] == 0);

	for (size_t i = 0; i < c.Shape<1>(); ++i)
		for (size_t j = 0; j < c.Shape<2>(); ++j)
			assert(c[2][i][j] == 0);

	byfxxm::ArrayNd<int, 2> d{ {0, 1}, {2, 3, 4} };
	assert(d[1][1] == 3);

	byfxxm::ArrayNd<int, 1> e = { 1, 2, 3, 4 };
	assert(e[3] == 4);
	e.ForEach([](auto&& x) { return x * 20; });
	assert(e[2] == 60);

	e.ForEach([](auto&& n) {return n * 20; });
	assert(e[1] == 800);
}

#if 0
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
#endif

void TestVariableBuffer() {
	byfxxm::VariableBuffer<20> vbuf;
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

	byfxxm::VariableBuffer<1000> vbuf1;
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
	byfxxm::RingBuffer<int, 1024> rb1;
	byfxxm::RingBuffer<int, 1023> rb2;
	int temp{ 0 };

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

	struct C : public B {};
	static_assert(meta::IsBaseOf<C, B>);
	static_assert(!meta::IsBaseOf<C, A>);
	static_assert(!meta::IsBaseOf<int, double>);
	static_assert(!meta::IsBaseOf<B, C>);
	static_assert(!meta::IsBaseOf<int, C>);

	[]<size_t... Ns>(meta::Seq<Ns...> seq) {
		static_assert((... + Ns) == 10);
		(std::cout << ... << (std::to_string(Ns) + "\n")) << std::endl;
	}(meta::Make<5>::type{});

	constexpr double a1 = 15.5;
	static_assert(meta::Max(1, 3, 2, a1, 12) == 15.5);
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

void TestCrtp() {
	class Derived1 : public crtp::Base<Derived1> {
	public:
		void Func() {
			std::cout << "this is Derived1" << std::endl;
		}
	};

	class Derived2 : public crtp::Base<Derived2> {
	public:
		void Func() {
			std::cout << "this is Derived2" << std::endl;
		}

		int Func1(int n1, double n2) {
			return n1 + static_cast<int>(n2);
		}
	};

	Derived1 a;
	Derived2 b;
	crtp::Func(a);
	crtp::Func(b);

	crtp::Container<crtp::Base, Derived1, Derived2> cont(a, b);
	cont.Get<0>()->Func();
	cont.Get<1>()->Func();
}

void TestCoroutine() {
	auto f1= [](void* p) -> coroutine::Task {
		while (1) {
			std::cout << p << std::endl;
			co_await std::suspend_always{};
		}
	};

	auto f2 = [](void* p) -> coroutine::Task {
		while (1) {
			std::cout << p << std::endl;
			co_await std::suspend_always{};
		}
	};

	auto co1 = f1(nullptr);
	auto co2 = f2(new int(5));
	auto mf = [&]() {
		while (1) {
			co1.handle_.resume();
			co2.handle_.resume();
		}
	};
	
	mf();
}

void TestAddressing() {
	using Addr1 = byfxxm::Addressing<int, double, std::string>;
	class A {
	public:
		A(std::string name) : name_(name) {
			Addr1::Instance()->Register(name_ + ".a", &a);
			Addr1::Instance()->Register(name_ + ".b", &b);
			Addr1::Instance()->Register(name_ + ".c", &c);
		}

		~A() {
			Addr1::Instance()->Unregister(name_ + ".a");
			Addr1::Instance()->Unregister(name_ + ".b");
			Addr1::Instance()->Unregister(name_ + ".c");
		}

	private:
		std::string name_;
		int a = 0;
		double b = 0;
		std::string c;
	};

	A sa{ "sa" };
	auto inst = Addr1::Instance();
	inst->Set("sa.a", 404);
	inst->Set("sa.c", "hello world");
	byfxxm::Print(inst->Get("sa.a"));
	byfxxm::Print(inst->Get("sa.c"));
	std::cout << byfxxm::Type(inst->Get("sa.a")) << std::endl;
	std::cout << byfxxm::Type(inst->Get("sa.b")) << std::endl;
}

void TestTypelist() {
	namespace typelist = byfxxm::typelist;

	typelist::Typelist<int, double, char> tp0;
	static_assert(std::is_same_v<typelist::Front<decltype(tp0)>::type, int>);
	static_assert(std::is_same_v<typelist::Back<decltype(tp0)>::type, char>);

	typelist::PushFront<decltype(tp0), long*>::type tp1;
	static_assert(std::is_same_v<typelist::Front<decltype(tp1)>::type, long*>);

	typelist::PushBack<decltype(tp1), const char*>::type tp2;
	static_assert(std::is_same_v<typelist::Back<decltype(tp2)>::type, const char*>);
	static_assert(typelist::Size<decltype(tp2)>::value == 5);
	static_assert(std::is_same_v<typelist::PopBack<decltype(tp2)>::type, decltype(tp1)>);

	static_assert(!typelist::Empty<decltype(tp2)>::value);
	static_assert(typelist::Empty<typelist::Typelist<>>::value);

	typelist::PushBack<decltype(tp2), const char*>::type tp3;
	static_assert(typelist::Count<decltype(tp3), const char*>::value == 2);
	static_assert(typelist::Count<decltype(tp3), const char* const>::value == 0);

	typelist::Reverse<decltype(tp3)>::type tp4;
	static_assert(std::is_same_v<typelist::Front<decltype(tp4)>::type, const char*>);
}

void TestCoro() {
	byfxxm::Coro co;
	int x = 0;
	co.SetMain([](byfxxm::CoMainHelper* helper, void* p) {
		while (*(int*)p != 2)
			helper->SwitchToSub(*(int*)p);
		}, &x);

	co.AddSub([](byfxxm::CoSubHelper* helper, void* p) {
		*(int*)p = 1;
		std::cout << *(int*)p << std::endl;
		helper->SwitchToMain();
		}, &x);

	co.AddSub([](byfxxm::CoSubHelper* helper, void* p) {
		*(int*)p = 2;
		std::cout << *(int*)p << std::endl;
		helper->SwitchToMain();
		}, &x);

	co.Run();
}

int main() {
	TestCoro();
	TestAddressing();
	TestArrayNd();
	TestMetaprogramming();
	TestVariableBuffer();
	TestHashMap();
	TestWarrior();
	TestNQueens();
	TestAsm();
	TestReflection();
	TestRingBuffer();
	TestCrtp();
	TestTypelist();
#if 0
	TestCoroutine();
#endif

	return 0;
}
