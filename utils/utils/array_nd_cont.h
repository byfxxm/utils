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
*		size_t e = arr[0]; // 获取第一维度
*		arr.Memset(0xff); // 内存初始化，默认每个字节初始化为 0
*/
template <typename T, size_t N, typename = std::enable_if_t<(N > 0)>>
class ArrayNd final {
private:
	template <size_t N>
	class BassPtr {
	private:
		using _Base = BassPtr<N - 1>;

	public:
		BassPtr(T* p, size_t(&d)[N]) : ptr_(p), dim_(d) {}

		_Base& operator[](size_t idx) {
			return _Base(&T[dim_[0]], static_cast<size_t[N - 1]>(dim_[1]));
		}

	private:
		T ptr_{ nullptr };
		size_t dim_[N]{};
	};

	template <>
	class BassPtr<1> {
	public:
		BassPtr(T* p, size_t(&d)[1]) : ptr_(p), dim_(d[0]) {}

		T& operator[](size_t idx) {
			return T[dim_];
		}

	private:
		T ptr_{ nullptr };
		size_t dim_{ 0 };
	};

public:
	template <typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == N>>
	ArrayNd(Ts&&... ts) {
		EmplaceDim(0, std::forward<Ts>(ts)...);
		for (auto i = 0; i < N; ++i) {
			ele_cnt_ *= dim_[i];
			factor_[i] = 1;
			for (auto j = i + 1; j < N; ++j)
				factor_[i] *= dim_[j];
		}

		ele_ = new T[ele_cnt_];
		Memset();
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

	void Memset(unsigned char val = 0) {
		assert(ele_);
		memset(ele_, val, ele_cnt_ * sizeof(T));
	}

	template <typename... Ts, typename = std::enable_if_t<sizeof...(Ts) == N>>
	T& operator()(Ts&&... ts) const {
		assert(ele_);
		return *Index(0, ele_, std::forward<Ts>(ts)...);
	}

	template <typename... Ts, typename = std::enable_if_t<sizeof...(Ts) < N>>
	T* operator()(Ts&&... ts) const {
		assert(ele_);
		return Index(0, ele_, std::forward<Ts>(ts)...);
	}

	//size_t operator[](size_t idx) const {
	//	assert(idx >= 0 && idx < N);
	//	assert(ele_);
	//	return dim_[idx];
	//}

	BassPtr<N - 1>& operator[](size_t idx) {
		return BassPtr<N - 1>(ele_, static_cast<size_t[N - 1]>(dim_[1]));
	}

	ArrayNd& operator=(const ArrayNd& arr) {
		if (this == &arr)
			return *this;

		assert(arr.ele_);
		ele_cnt_ = arr.ele_cnt_;
		memcpy(dim_, arr.dim_, sizeof(dim_));
		memcpy(factor_, arr.factor_, sizeof(factor_));

		delete[] ele_;
		ele_ = new T[ele_cnt_];
		CopyEle(arr.ele_);
		return *this;
	}

	T* begin() {
		return ele_;
	}

	T* end() {
		return ele_ + ele_cnt_;
	}

private:
	template <typename T1, typename... Ts, typename = std::enable_if_t<std::is_integral_v<std::remove_reference_t<T1>>>>
	void EmplaceDim(size_t idx, T1&& t1, Ts&&... ts) {
		dim_[idx] = t1;
		if constexpr (sizeof...(ts) > 0)
			EmplaceDim(idx + 1, std::forward<Ts>(ts)...);
	}

	template <typename T1, typename... Ts, typename = std::enable_if_t<std::is_integral_v<std::remove_reference_t<T1>>>>
	T* Index(size_t idx, T* p, T1&& t1, Ts&&... ts) const {
		assert((size_t)t1 < dim_[idx]);
		return Index(idx + 1, &p[factor_[idx] * t1], std::forward<Ts>(ts)...);
	}

	T* Index(size_t, T* p) const {
		return p;
	}

	void CopyEle(T* p) {
		if constexpr (std::is_trivial_v<T>)
			memcpy(ele_, p, ele_cnt_ * sizeof(T));
		else
			for (size_t i = 0; i < ele_cnt_; ++i)
				ele_[i] = p[i];
	}

private:
	T* ele_{ nullptr };
	size_t ele_cnt_{ 1 };
	size_t dim_[N]{};
	size_t factor_[N]{};
};