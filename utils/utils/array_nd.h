#pragma once
#include <array>
#include <cassert>
#include <memory>

namespace array_nd {
	/*
	* ��ά����
	* usage:
	*		ArrayNd<char, 3> arr(7, 8, 9); // ��������Ԫ������Ϊ char��ά���� 3����һ���ڶ�������ά�ȷֱ��� 7��8��9��
	*		char c = arr[1][2][3]; // ����������飬������������ά��ʱ����������
	*		arr.Memset('z'); // �ڴ��ʼ��
	*/
	template <class T, size_t N> requires (N > 0)
		class ArrayNd final {
		private:
			template <size_t N>
			class ViewPtr final {
			public:
				ViewPtr(T* p, const size_t* d, const size_t* f) : ptr_(p), dimensions_(d), factors_(f) {
					assert(ptr_);
				}

				ViewPtr<N - 1> operator[](size_t idx) const&& {
					assert(idx >= 0 && idx < dimensions_[0]);
					return ViewPtr<N - 1>(ptr_ + idx * factors_[0], dimensions_ + 1, factors_ + 1);
				}

			private:
				T* ptr_{ nullptr };
				const size_t* dimensions_{ nullptr };
				const size_t* factors_{ nullptr };
			};

			template <>
			class ViewPtr<1> {
			public:
				ViewPtr(T* p, const size_t* d, const size_t*) : ptr_(p), dimensions_(d) {}

				T& operator[](size_t idx) const&& {
					assert(idx >= 0 && idx < dimensions_[0]);
					return ptr_[idx];
				}

			private:
				T* ptr_{ nullptr };
				const size_t* dimensions_{ nullptr };
			};

		public:
			template <class... Args> requires (sizeof...(Args) == N)
			ArrayNd(Args&&... args) : count_((... * args)), dimensions_{ static_cast<size_t>(args)... } {
				element_ = std::make_shared<T[]>(count_);
				Memset(0);
				for (size_t i = 0; i < N; ++i) {
					factors_[i] = 1;
					for (size_t j = i + 1; j < N; ++j)
						factors_[i] *= dimensions_[j];
				}
			}

			ArrayNd(const ArrayNd& arr) = delete;
			ArrayNd(ArrayNd&&) noexcept = default;
			ArrayNd& operator=(const ArrayNd&) = delete;
			ArrayNd& operator=(ArrayNd&&) noexcept = default;

			decltype(auto) operator[](size_t idx) {
				return ViewPtr<N>(element_.get(), &dimensions_.front(), &factors_.front())[idx];
			}

			void Memset(T val) {
				assert(element_);
				for (size_t i = 0; i < count_; ++i)
					element_[i] = val;
			}

		private:
			std::shared_ptr<T[]> element_;
			size_t count_{ 0 };
			std::array<size_t, N> dimensions_;
			std::array<size_t, N> factors_;
	};
}
