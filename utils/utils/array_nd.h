#pragma once
#include <type_traits>
#include <cassert>
#include <array>
#include <memory>

namespace array_nd {
	/*
	* 多维数组
	* usage:
	*		ArrayNd<char, 3> arr(7, 8, 9); // 构建对象（元素类型为 char，维数是 3，第一、第二、第三维度分别是 7、8、9）
	*		char c = arr[1][2][3]; // 随机访问数组，参数个数等于维数时，返回引用
	*		arr.Memset('z'); // 内存初始化
	*/
	template <typename T, size_t N, typename = std::enable_if_t<(N > 0)>>
		class ArrayNd final {
		private:
			template <size_t N>
			class BasePtr final {
			public:
				BasePtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dims_(d), factors_(f) {
					assert(ptr_);
				}

				BasePtr<N - 1> operator[](size_t idx) const&& {
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
				BasePtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dims_(d) {}

				T& operator[](size_t idx) const&& {
					assert(idx >= 0 && idx < dims_[0]);
					return ptr_[idx];
				}

			private:
				T* ptr_{ nullptr };
				const size_t* dims_{ nullptr };
			};

		public:
			template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == N>>
			ArrayNd(Args&&... args) : dims_{ static_cast<size_t>(args)... }, len_((... * args)) {
				for (size_t i = 0; i < N; ++i) {
					factors_[i] = 1;
					for (size_t j = i + 1; j < N; ++j)
						factors_[i] *= dims_[j];
				}

				mem_ = std::make_shared<T[]>(len_);
				Memset(0);
			}

			ArrayNd(const ArrayNd& arr) = delete;
			ArrayNd(ArrayNd&&) noexcept = default;
			ArrayNd& operator=(const ArrayNd&) = delete;
			ArrayNd& operator=(ArrayNd&&) noexcept = default;

			decltype(auto) operator[](size_t idx) {
				return BasePtr<N>(mem_.get(), &dims_.front(), &factors_.front())[idx];
			}

			void Memset(T val) {
				assert(mem_);
				for (size_t i = 0; i < len_; ++i)
					mem_[i] = val;
			}

		private:
			std::shared_ptr<T[]> mem_;
			size_t len_{ 1 };
			std::array<size_t, N> dims_;
			std::array<size_t, N> factors_;
	};
}
