#pragma once
#include <type_traits>
#include <cstring>

template<typename T, int N, char Init = 0>
class array_nd
{
public:
	template<typename... Ts>
	array_nd(Ts... ts)
	{
		static_assert(sizeof...(Ts) == N);
		_array_nd(0, ts...);

		for (auto i = 0; i < N; ++i) {
			ele_cnt *= dim[i];
			factor[i] = 1;
			for (auto j = i + 1; j < N; ++j)
				factor[i] *= dim[j];
		}

		ele = new T[ele_cnt];
		reset();
	}

	~array_nd()
	{
		delete[] ele;
	}

	void reset()
	{
		memset(ele, Init, ele_cnt * sizeof(T));
	}

	template<typename... Ts>
	T& operator()(Ts... ts)
	{
		static_assert(sizeof...(Ts) == N);
		return _get(ele, ts...);
	}

private:
	template<typename T1, typename... Ts>
	void _array_nd(int n, T1 t1, Ts... ts)
	{
		static_assert(std::is_integral_v<T1>);
		dim[n] = t1;
		_array_nd(++n, ts...);
	}

	void _array_nd(int) {}

	template<typename T1, typename... Ts>
	T& _get(T* p, T1 t1, Ts... ts)
	{
		static_assert(std::is_integral_v<T1>);
		constexpr int level = N - sizeof...(Ts) - 1;
		return _get(&p[factor[level] * t1], ts...);
	}

	T& _get(T* p)
	{
		return *p;
	}

private:
	T* ele{ nullptr };
	int ele_cnt{ 1 };
	int dim[N];
	int factor[N];
};