#pragma once

template<typename T, int N>
class array_nd
{
public:
	template<typename... Ts>
	array_nd(Ts... ts)
	{
		static_assert(sizeof...(Ts) == N);
		_array_nd(0, ts...);

		for (auto i : dim)
			ele_cnt *= i;

		ele = new T[ele_cnt];
	}

	~array_nd()
	{
		delete[] ele;
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
		dim[n] = t1;
		_array_nd(++n, ts...);
	}

	void _array_nd(int) {}

	template<typename T1, typename... Ts>
	T& _get(T* p, T1 t1, Ts... ts)
	{
		int product = 1;
		for (auto i = N - sizeof...(Ts); i < N; ++i)
			product *= dim[i];

		return _get(&p[product * t1], ts...);
	}

	T& _get(T* p)
	{
		return *p;
	}

private:
	T* ele{ nullptr };
	int ele_cnt{ 1 };
	int dim[N];
};