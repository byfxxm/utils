#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

/*
* 多维数组
* usage:
*		array_nd<char, 3, 0xff> arr(7, 8, 9); // 构建对象（元素类型为char，维数是3，内存初始化为0xff，第一、第二、第三维度分别是7、8、9）
*		char c = arr(1, 2, 3); // 随机访问数组
*		int d = arr[0]; // 获取第一维度
*		arr.reset(); // 内存初始化为0xff
*/
template<typename T, int N, char Init = 0>
class array_nd final
{
public:
	template<typename... Ts>
	array_nd(Ts... ts)
	{
		static_assert(N > 0);
		static_assert(sizeof...(ts) == N);
		_array_nd(0, ts...);

		for (auto i = 0; i < N; ++i) {
			_ele_cnt *= _dim[i];
			_factor[i] = 1;
			for (auto j = i + 1; j < N; ++j)
				_factor[i] *= _dim[j];
		}

		_ele = new T[_ele_cnt];
		reset();
	}

	array_nd(array_nd& arr)
	{
		assert(arr._ele);
		memcpy(this, &arr, sizeof(arr));
		_ele = new T[_ele_cnt];
		memcpy(_ele, arr._ele, _ele_cnt * sizeof(T));
	}

	array_nd(array_nd&& arr) noexcept
	{
		memcpy(this, &arr, sizeof(arr));
		arr._ele = nullptr;
	}

	~array_nd()
	{
		delete[] _ele;
	}

	void reset()
	{
		memset(_ele, Init, _ele_cnt * sizeof(T));
	}

	template<typename... Ts>
	T& operator()(Ts... ts) const
	{
		static_assert(sizeof...(ts) == N);
		return _get(_ele, ts...);
	}

	int operator[](int idx) const
	{
		assert(idx >= 0 && idx < N);
		return _dim[idx];
	}

private:
	template<typename T1, typename... Ts>
	void _array_nd(int idx, T1 t1, Ts... ts)
	{
		static_assert(std::is_integral_v<T1>);
		_dim[idx] = t1;
		if constexpr (sizeof...(ts) > 0)
			_array_nd(++idx, ts...);
	}

	template<typename T1, typename... Ts>
	T& _get(T* p, T1 t1, Ts... ts) const
	{
		static_assert(std::is_integral_v<T1>);
		constexpr int idx = N - sizeof...(ts) - 1;
		assert(t1 >= 0 && t1 < _dim[idx]);
		return _get(&p[_factor[idx] * t1], ts...);
	}

	T& _get(T* p) const
	{
		return *p;
	}

private:
	T* _ele{ nullptr };
	int _ele_cnt{ 1 };
	int _dim[N]{};
	int _factor[N]{};
};