#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

/*
* 多维数组
* usage:
*		array_nd<char, 3, 0xff> arr(7, 8, 9); // 构建对象。（元素类型为char，维度是3，内存初始化为0xff，第一、第二、第三维长度分别是7、8、9）
*		char c = arr(1,2,3); // 随机访问数组
*		int d = arr[0]; // 获取第一维长度
*		arr.reset(); // 内存初始化为0xff
*/
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

	int operator[](int n)
	{
		assert(n >= 0 && n < N);
		return dim[n];
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
		constexpr int idx = N - sizeof...(Ts) - 1;
		assert(t1 >= 0 && t1 < dim[idx]);
		return _get(&p[factor[idx] * t1], ts...);
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