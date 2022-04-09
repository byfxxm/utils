#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

template<typename T, int N>
struct pointer_iterator {
	using type = typename pointer_iterator<T*, N - 1>::type;
};

template<typename T>
struct pointer_iterator<T, 0> {
	using type = T;
};

/*
* 多级指针
* 数组有类型退化的特性，比如: 有五维数组 int a[1][2][3][4][5]，则 auto b = a[0][0] 中 b 的类型退化为 int***。
* 所以，pointer_iterator_t 用于把类型转换为多级指针，例如: 
*		int a = 0;
*		auto b = pointer_iterator_t<int, 5>(a);	// b 类型为 int*****
*/
template<typename T, int N>
using pointer_iterator_t = typename pointer_iterator<T, N>::type;

/*
* 多维数组
* usage:
*		array_nd<char, 3> arr(7, 8, 9); // 构建对象（元素类型为 char，维数是 3，内存初始化为 0xff，第一、第二、第三维度分别是 7、8、9）
*		char c = arr(1, 2, 3); // 随机访问数组
*		auto d = arr(5); // 参数个数小于维数时，返回类型为指定类型的 N 级指针，N = 维数 - 参数个数，此处 d 类型为 char**
*		int e = arr[0]; // 获取第一维度
*		arr.reset(0xff); // 内存初始化，默认每个字节初始化为 0
*/
template<typename T, int N>
class array_nd {
public:
	template<typename... Ts>
	array_nd(Ts... ts) {
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

	array_nd(const array_nd& arr) {
		assert(arr._ele);
		memcpy(this, &arr, sizeof(arr));
		_ele = new T[_ele_cnt];
		memcpy(_ele, arr._ele, _ele_cnt * sizeof(T));
	}

	array_nd(array_nd&& arr) noexcept {
		assert(arr._ele);
		memcpy(this, &arr, sizeof(arr));
		arr._ele = nullptr;
	}

	~array_nd() {
		delete[] _ele;
	}

	void reset(unsigned char val = 0) {
		assert(_ele);
		memset(_ele, val, _ele_cnt * sizeof(T));
	}

	template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == N>>
	T& operator()(Ts... ts) const {
		assert(_ele);
		return *_get(0, _ele, ts...);
	}

	template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) < N>>
	auto operator()(Ts... ts) const {
		assert(_ele);
		return pointer_iterator_t<T, N - sizeof...(ts)>(_get(0, _ele, ts...));
	}

	int operator[](int idx) const {
		assert(idx >= 0 && idx < N);
		assert(_ele);
		return _dim[idx];
	}

	array_nd& operator=(const array_nd& arr) {
		assert(arr._ele);
		_ele_cnt = arr._ele_cnt;
		memcpy(_dim, arr._dim, sizeof(_dim));
		memcpy(_factor, arr._factor, sizeof(_factor));

		delete _ele;
		_ele = new T[_ele_cnt];
		memcpy(_ele, arr._ele, _ele_cnt * sizeof(T));

		return *this;
	}

private:
	template<typename T1, typename... Ts>
	void _array_nd(int idx, T1 t1, Ts... ts) {
		static_assert(std::is_integral_v<T1> || std::is_enum_v<T1>);
		_dim[idx] = t1;
		if constexpr (sizeof...(ts) > 0)
			_array_nd(++idx, ts...);
	}

	template<typename T1, typename... Ts>
	T* _get(int idx, T* p, T1 t1, Ts... ts) const {
		static_assert(std::is_integral_v<T1> || std::is_enum_v<T1>);
		assert(t1 >= 0 && t1 < _dim[idx]);
		return _get(idx + 1, &p[_factor[idx] * t1], ts...);
	}

	T* _get(int, T* p) const {
		return p;
	}

private:
	T* _ele{ nullptr };
	int _ele_cnt{ 1 };
	int _dim[N]{};
	int _factor[N]{};
};