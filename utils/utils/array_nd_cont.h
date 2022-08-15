#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

/*
* ��ά����
* usage:
*		ArrayNd<char, 3> arr(7, 8, 9); // ��������Ԫ������Ϊ char��ά���� 3���ڴ��ʼ��Ϊ 0xff����һ���ڶ�������ά�ȷֱ��� 7��8��9��
*		char c = arr(1, 2, 3); // ����������飬������������ά��ʱ����������
*		char* d = arr(5); // ��������С��ά��ʱ������ָ��
*		size_t e = arr[0]; // ��ȡ��һά��
*		arr.Memset(0xff); // �ڴ��ʼ����Ĭ��ÿ���ֽڳ�ʼ��Ϊ 0
*/
template <typename T, size_t N, typename = std::enable_if_t<(N > 0)>>
class ArrayNd final {
private:
	template <size_t N>
	class BasePtr {
	public:
		BasePtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dim_(d), factor_(f) {}

		BasePtr<N - 1> operator[](size_t idx) {
			return BasePtr<N - 1>(&ptr_[dim_[0]], dim_ + 1, factor_ + 1);
		}

	private:
		T* ptr_{ nullptr };
		const size_t* dim_{ nullptr };
		const size_t* factor_{ nullptr };
	};

	template <>
	class BasePtr<1> {
	public:
		BasePtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dim_(d), factor_(f) {}

		T& operator[](size_t idx) {
			return ptr_[idx];
		}

	private:
		T* ptr_{ nullptr };
		const size_t* dim_{ nullptr };
		const size_t* factor_{ nullptr };
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
		Memset(0);
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

	template <typename T>
	void Memset(T&& val) {
		assert(ele_);
		for (size_t i = 0; i < ele_cnt_; ++i) {
			ele_[i] = std::forward<T>(val);
		}
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

	BasePtr<N - 1> operator[](size_t idx) {
		return BasePtr<N>(ele_, dim_, factor_)[idx];
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