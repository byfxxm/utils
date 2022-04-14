#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

/*
* 多维数组
* usage:
*		ArrayNd<char, 3> arr(7, 8, 9); // 构建对象（元素类型为 char，维数是 3，内存初始化为 0xff，第一、第二、第三维度分别是 7、8、9）
*		char c = arr(1, 2, 3); // 随机访问数组，参数个数等于维数时，返回引用
*		char* d = arr(5); // 参数个数小于维数时，返回指针
*		int e = arr[0]; // 获取第一维度
*		arr.Reset(0xff); // 内存初始化，默认每个字节初始化为 0
*/
template<typename T, int N>
class ArrayNd {
public:
	template<typename... Ts>
	ArrayNd(Ts... ts) {
		static_assert(N > 0);
		static_assert(sizeof...(ts) == N);
		EmplaceDim(0, ts...);

		for (auto i = 0; i < N; ++i) {
			ele_cnt_ *= dim_[i];
			factor_[i] = 1;
			for (auto j = i + 1; j < N; ++j)
				factor_[i] *= dim_[j];
		}

		ele_ = new T[ele_cnt_];
		Reset();
	}

	ArrayNd(const ArrayNd& arr) {
		assert(arr.ele_);
		memcpy(this, &arr, sizeof(arr));
		ele_ = new T[ele_cnt_];
		CopyEle(arr.ele_);
	}

	ArrayNd(ArrayNd&& arr) noexcept {
		assert(arr.ele_);
		memcpy(this, &arr, sizeof(arr));
		arr.ele_ = nullptr;
	}

	~ArrayNd() {
		delete[] ele_;
	}

	void Reset(unsigned char val = 0) {
		assert(ele_);
		memset(ele_, val, ele_cnt_ * sizeof(T));
	}

	template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == N>>
	T& operator()(Ts... ts) const {
		assert(ele_);
		return *Index(0, ele_, ts...);
	}

	template<typename... Ts, typename = std::enable_if_t<sizeof...(Ts) < N>>
	T* operator()(Ts... ts) const {
		assert(ele_);
		return Index(0, ele_, ts...);
	}

	int operator[](int idx) const {
		assert(idx >= 0 && idx < N);
		assert(ele_);
		return dim_[idx];
	}

	ArrayNd& operator=(const ArrayNd& arr) {
		assert(arr.ele_);
		ele_cnt_ = arr.ele_cnt_;
		memcpy(dim_, arr.dim_, sizeof(dim_));
		memcpy(factor_, arr.factor_, sizeof(factor_));

		delete[] ele_;
		ele_ = new T[ele_cnt_];
		CopyEle(arr.ele_);

		return *this;
	}

private:
	template<typename T1, typename... Ts>
	void EmplaceDim(int idx, T1 t1, Ts... ts) {
		static_assert(std::is_integral_v<T1> || std::is_enum_v<T1>);
		dim_[idx] = t1;
		if constexpr (sizeof...(ts) > 0)
			EmplaceDim(++idx, ts...);
	}

	template<typename T1, typename... Ts>
	T* Index(int idx, T* p, T1 t1, Ts... ts) const {
		static_assert(std::is_integral_v<T1> || std::is_enum_v<T1>);
		assert(t1 >= 0 && t1 < dim_[idx]);
		return Index(idx + 1, &p[factor_[idx] * t1], ts...);
	}

	T* Index(int, T* p) const {
		return p;
	}

	void CopyEle(T* p) {
		if constexpr (std::is_trivial_v<T>)
			memcpy(ele_, p, ele_cnt_ * sizeof(T));
		else
			for (int i = 0; i < ele_cnt_; ++i)
				ele_[i] = p[i];
	}

private:
	T* ele_{ nullptr };
	int ele_cnt_{ 1 };
	int dim_[N]{};
	int factor_[N]{};
};