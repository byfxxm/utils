#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

/*
* ��ά����
* usage:
*		array_nd<char, 3> arr(7, 8, 9); // ��������Ԫ������Ϊ char��ά���� 3���ڴ��ʼ��Ϊ 0xff����һ���ڶ�������ά�ȷֱ��� 7��8��9��
*		char c = arr(1, 2, 3); // ����������飬������������ά��ʱ����������
*		char* d = arr(5); // ��������С��ά��ʱ������ָ��
*		int e = arr[0]; // ��ȡ��һά��
*		arr.reset(0xff); // �ڴ��ʼ����Ĭ��ÿ���ֽڳ�ʼ��Ϊ 0
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
		_copy_ele(arr._ele);
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
	T* operator()(Ts... ts) const {
		assert(_ele);
		return _get(0, _ele, ts...);
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

		delete[] _ele;
		_ele = new T[_ele_cnt];
		_copy_ele(arr._ele);

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

	void _copy_ele(T* p) {
		if constexpr (std::is_pod_v<T>)
			memcpy(_ele, p, _ele_cnt * sizeof(T));
		else
			for (int i = 0; i < _ele_cnt; ++i)
				_ele[i] = p[i];
	}

private:
	T* _ele{ nullptr };
	int _ele_cnt{ 1 };
	int _dim[N]{};
	int _factor[N]{};
};