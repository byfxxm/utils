#pragma once
#include <type_traits>
#include <cstring>
#include <cassert>

namespace array_nd {
	/*
	* ��ά����
	* usage:
	*		ArrayNd<char, 3> arr(7, 8, 9); // ��������Ԫ������Ϊ char��ά���� 3���ڴ��ʼ��Ϊ 0xff����һ���ڶ�������ά�ȷֱ��� 7��8��9��
	*		char c = arr[1][2][3]; // ����������飬������������ά��ʱ����������
	*		arr.Memset(0xff); // �ڴ��ʼ����Ĭ��ÿ���ֽڳ�ʼ��Ϊ 0
	*/
	template <typename T, size_t N, typename = std::enable_if_t<(N > 0)>>
		class ArrayNd final {
		private:
			template <size_t N>
			class BasePtr {
			public:
				BasePtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dims_(d), factors_(f) {}

				BasePtr<N - 1> operator[](size_t idx)&& {
					assert(idx >= 0 && idx < dims_[0]);
					return BasePtr<N - 1>(ptr_ + idx * factors_[0], dims_ + 1, factors_ + 1);
				}

			private:
				T* ptr_{ nullptr };
				const size_t* dims_{ nullptr };
				const size_t* factors_{ nullptr };
			};

			template <>
			class BasePtr<1> {
			public:
				BasePtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dims_(d), factors_(f) {}

				T& operator[](size_t idx)&& {
					assert(idx >= 0 && idx < dims_[0]);
					return ptr_[idx];
				}

			private:
				T* ptr_{ nullptr };
				const size_t* dims_{ nullptr };
				const size_t* factors_{ nullptr };
			};

		public:
			template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
			ArrayNd(Args&&... args) {
				SetDims(0, std::forward<Args>(args)...);
				for (size_t i = 0; i < N; ++i) {
					len_ *= dims_[i];
					factors_[i] = 1;
					for (size_t j = i + 1; j < N; ++j)
						factors_[i] *= dims_[j];
				}

				mem_ = new T[len_];
				Memset(0);
			}

			ArrayNd(const ArrayNd& arr) {
				assert(arr.mem_);
				memcpy(this, &arr, sizeof(arr));
				mem_ = new T[len_];
				CopyMem(arr.mem_);
			}

			ArrayNd(ArrayNd&& arr) noexcept {
				assert(arr.mem_);
				memcpy(this, &arr, sizeof(arr));
				arr.mem_ = nullptr;
			}

			~ArrayNd() {
				delete[] mem_;
			}

			template <typename T>
			void Memset(T&& val) {
				assert(mem_);
				for (size_t i = 0; i < len_; ++i) {
					mem_[i] = std::forward<T>(val);
				}
			}

			decltype(auto) operator[](size_t idx) {
				return BasePtr<N>(mem_, dims_, factors_)[idx];
			}

			ArrayNd& operator=(const ArrayNd& arr) {
				if (this == &arr)
					return *this;

				assert(arr.mem_);
				len_ = arr.len_;
				memcpy(dims_, arr.dims_, sizeof(dims_));
				memcpy(factors_, arr.factors_, sizeof(factors_));

				delete[] mem_;
				mem_ = new T[len_];
				CopyMem(arr.mem_);
				return *this;
			}

		private:
			template <typename... Args>
			void SetDims(size_t idx, size_t first, Args... args) {
				dims_[idx] = first;
				if constexpr (sizeof...(args) > 0)
					SetDims(idx + 1, args...);
			}

			void CopyMem(T* p) {
				if constexpr (std::is_trivial_v<T>)
					memcpy(mem_, p, len_ * sizeof(T));
				else
					for (size_t i = 0; i < len_; ++i)
						mem_[i] = p[i];
			}

		private:
			T* mem_{ nullptr };
			size_t len_{ 1 };
			size_t dims_[N]{};
			size_t factors_[N]{};
	};
}